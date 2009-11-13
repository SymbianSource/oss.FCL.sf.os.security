/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "tScriptSetup.h"
#include "tScriptTests.h"

#include "t_testactionspec.h"
#include "t_input.h"
#include "t_certstoreactionmemfail.h"
#include "tcancel.h"
#include "t_sleep.h"
#include "t_message.h"
#include "t_output.h"
#include "tTestSpec.h"

#include <s32file.h>

#include "t_logger.h"

_LIT8(KStartMemFail, "startmemfailure");
_LIT8(KStopMemFail, "stopmemfailure");
_LIT8(KCancelStart, "startcancellation");
_LIT8(KCancelStop, "stopcancellation");
_LIT8(KMessage, "message");
_LIT8(KSleep, "sleep");

#include "t_policy.h"
_LIT8(KPolicyTest, "policytest");

EXPORT_C CScriptSetup* CScriptSetup::NewLC(CConsoleBase* aConsole)
	{
	CScriptSetup* self = new (ELeave) CScriptSetup(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CScriptSetup::CScriptSetup(CConsoleBase* aConsole) : CTestSetup(aConsole)
	{
	}

CScriptSetup::~CScriptSetup()
	{
	delete iTestInput;
	}

EXPORT_C void CScriptSetup::SetupTestsL(RFs& aFs,
										CTestSpec& aTestSpec, 
										TScriptTests theTestTypes[], 
										const CTestHandlerSettings& aCommandLineSettings)
	{
	TInt pos = 0;
	TInt err = KErrNone;
	for (TInt i = 1 ; ; ++i)
		{
		TPtrC8 actionData = Input::ParseElement(*iTestInput, KActionStart, KActionEnd, pos, err);
		if (err != KErrNone)
			{
			break;
			}
		
		TInt relativePos = 0;
		TInt scriptGroupings= DEFAULTGROUPING;
		TTestActionSpec actionSpec;
		//Parse the file to create action name, type , body and result
		User::LeaveIfError(actionSpec.Init(actionData, relativePos, *iConsole, *iLogFile, scriptGroupings));
			
		//Start off checking Exhaustive and Smoke flags, then calls groupings
		if (!CheckAllFlags(aCommandLineSettings, scriptGroupings))
			//The current test should not be run
			continue;
	
		CTestAction* newAction = 0;
		TRAP(err, newAction = 
			 CreateActionL(aFs, actionSpec, theTestTypes));
				
		if (err != KErrNone)
			{
			iLogFile->write(_L("CScriptSetup::CreateActionL failed: "));
			iLogFile->writeError(err);		
			iLogFile->writeNewLine();
			
			iLogFile->write(_L("Action name: "));
			iLogFile->writeString(actionSpec.iActionName);				
			iLogFile->writeNewLine();

			iLogFile->write(_L("Action type: "));
			iLogFile->writeString(actionSpec.iActionType);				
			iLogFile->writeNewLine();
			
			iLogFile->write(_L("1 tests failed out of 1\r\n"));
			
			User::Leave(err);
			}
		
		if(newAction->ScriptError() != CTestAction::ENone)
			{
			TBuf<KMaxErrorSize> scriptError;
			newAction->ScriptError(scriptError);
			
			iLogFile->write(_L("Script error \"%S\" in test %s\r\n"), &scriptError, i);
				
			delete newAction;
			User::Leave(KErrArgument);
			}
		
		CleanupStack::PushL(newAction);
		User::LeaveIfError(aTestSpec.AddNextTest(newAction));
		CleanupStack::Pop(newAction);
		}
	}

EXPORT_C TBool CScriptSetup::InitialiseL(RFs &aFs, const TDesC& aDefaultScript, const TDesC& aDefaultLog, TBool aUseCommandLine)
	{
	// gets the script file argument
	HBufC* scriptFileName = NULL;

	if (aUseCommandLine)
		{
		scriptFileName = GetArgument();
		CleanupStack::PushL(scriptFileName);
		if(scriptFileName->Length()==0)
			{
			CleanupStack::PopAndDestroy(scriptFileName);
			scriptFileName = NULL;
			}
		}

	if(scriptFileName == NULL)
		{
		if(aDefaultScript.Length() == 0)
			{
			PRINTANDLOG(_L("No script file specified on command line and no default given"));
			return(EFalse);
			}
		else
			{
			scriptFileName = aDefaultScript.AllocLC();
			}
		};

	PRINTANDLOG1(_L("Script file: %S"), scriptFileName);

	// open the script file
	RFile scriptFile;
	TInt err = scriptFile.Open(aFs, *scriptFileName, EFileStream | EFileRead | EFileShareReadersOnly);
	if (err != KErrNone)
		{
		PRINTANDLOG1(_L("Error opening script file: %d"), err);
		CleanupStack::PopAndDestroy();// scriptFileName
		return(EFalse);
		}
	CleanupClosePushL(scriptFile);
	
	TRAP(err, OpenLogFileL(aFs, 1, aDefaultLog, aUseCommandLine));	
	if (err != KErrNone)
		{
		PRINTANDLOG1(_L("Error opening log file: %d"), err);		
		CleanupStack::PopAndDestroy(2);// scripFile, scriptFileName
		return(EFalse);
		}
		
	// gets size of script file
	TInt size;
	scriptFile.Size(size);
	CleanupStack::PopAndDestroy(); // scriptFile
		
	// reads script into iTestInput
	iTestInput = HBufC8::NewL(size);
	TPtr8 pInput(iTestInput->Des());
	pInput.SetLength(size);

	RFileReadStream stream;
	User::LeaveIfError(stream.Open(aFs, *scriptFileName, EFileStream | EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(stream);
	stream.ReadL(pInput, size);

	CleanupStack::PopAndDestroy(2); // stream, scriptFileName
	return(ETrue);
	}

EXPORT_C CTestAction* CScriptSetup::CreateActionL(RFs& aFs,
												  const TTestActionSpec& aTestActionSpec,
												  const TScriptTests theTestTypes[]) const
	{
	// Instantiate built in test actions
	if (aTestActionSpec.iActionType == KStartMemFail)
		{
		return CMemFailStart::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KStopMemFail)
		{
		return CMemFailStop::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KCancelStart)
		{
		return CCancelStart::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KCancelStop)
		{
		return CCancelStop::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KMessage)
		{
		return CTestActionMessage::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KSleep)
		{
		return CSleep::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}
	else if (aTestActionSpec.iActionType == KPolicyTest)
		{
		return CPolicyTest::NewL(*iTestConsole, *iLogFile, aTestActionSpec);
		}

	// Look through the array of tests in order to find the correct class to return
	TInt loop=0;
	while (theTestTypes[loop].action!=NULL)
		{
		if (aTestActionSpec.iActionType == theTestTypes[loop].name)
			{
			return theTestTypes[loop].action(aFs, *iTestConsole, *iLogFile, aTestActionSpec);
			}
		loop++;
		}

	User::Leave(KErrNotSupported);
	return NULL;
	}
