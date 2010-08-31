/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tpinpluginteststep.h"
#include <test/testexecutelog.h>
#include <s32file.h>

_LIT(KProperties,   		"Property");
_LIT(KDefaultData,			"DefaultData");
_LIT(KIdentify,				"Identify");
_LIT(KTrain,   				"Train");
_LIT(KReTrain,   			"ReTrain");
_LIT(KForget,   			"Forget");
_LIT(KSequence,   			"Sequence");
_LIT(KOom,   				"Oom");

_LIT(KInputFile, 	"C:\\t_pinplugindlg_in.dat");
_LIT(KOutputFile, "C:\\t_pinplugindlg_out.dat");

using namespace AuthServer;

CPinPluginTestStep::~CPinPluginTestStep()
	{
	}

CPinPluginTestStep::CPinPluginTestStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KPinPluginTestStep);
	}

TVerdict CPinPluginTestStep::doTestStepPreambleL()
	{
	// get step parameters
	if (!GetStringFromConfig(ConfigSection(), _L("pinprocesstype"), iPinProcessType))
		{
		ERR_PRINTF1(_L("Requested Pin Process Type is missing"));
		SetTestStepResult(EFail);
		}
	else
		{
		if (iPinProcessType.CompareF(KProperties) != 0 
			&& iPinProcessType.CompareF(KOom) != 0)
			{
			TInt id  = 0;
			if (!GetIntFromConfig(ConfigSection(), _L("identityid"), id))
				{
				ERR_PRINTF1(_L("Identity Id Value is missing"));
				SetTestStepResult(EFail);	
				}
			else
				{
				iIdentityId = TIdentityId(id);
				SetTestStepResult(EPass);
				INFO_PRINTF3(_L("\"%S\" the Identity '%d' using PinPlugin System"), &iPinProcessType, id/*iIdentityId*/);
				}
			}
		}
	return TestStepResult();
	}

TVerdict CPinPluginTestStep::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	CActiveScheduler* sched = NULL;
	sched = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);	
	CPinPluginTestActive* activeObj = CPinPluginTestActive::NewL(Logger(), iIdentityId, iPinProcessType, *this);
	CleanupStack::PushL(activeObj);
	
	if (iPinProcessType.CompareF(KProperties) == 0)
		{
		iVerdict = activeObj->DoTestPropertyL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	else if (iPinProcessType.CompareF(KDefaultData) == 0)
		{
		iVerdict = activeObj->DoTestDefaultDataL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	else if (iPinProcessType.CompareF(KIdentify) == 0)
		{
		iVerdict = activeObj->DoTestIdentifyL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	else if (iPinProcessType.CompareF(KTrain) == 0)
		{
		iVerdict = activeObj->DoTestTrainL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	else if (iPinProcessType.CompareF(KReTrain) == 0)
		{
		iVerdict = activeObj->DoTestReTrainL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	else if (iPinProcessType.CompareF(KForget) == 0)
		{
		iVerdict = activeObj->DoTestForgetL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	else if (iPinProcessType.CompareF(KTrainServerCancel) == 0
			|| (iPinProcessType.CompareF(KReTrainServerCancel) == 0)
			|| (iPinProcessType.CompareF(KIdentifyServerCancel) == 0))
		{
		iVerdict = activeObj->DoTestServerCancelL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	else if (iPinProcessType.CompareF(KSequence) == 0)
		{
		iVerdict = activeObj->DoTestSequenceL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	else if (iPinProcessType.CompareF(KOom) == 0)
		{
		iVerdict = activeObj->DoTestOomL();
		INFO_PRINTF1(activeObj->iLogInfo);
		}
	CleanupStack::PopAndDestroy(2, sched);
	SetTestStepResult(iVerdict);
	return TestStepResult();
	}

TVerdict CPinPluginTestStep::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	return TestStepResult();
	}

CCreateFile::~CCreateFile()
	{}

CCreateFile::CCreateFile()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KCreateInputFile);
	}

TVerdict CCreateFile::doTestStepPreambleL()
	{
	TPtrC pinProcessType;
	// get step parameters
	if (!GetStringFromConfig(ConfigSection(), _L("pinprocesstype"), pinProcessType))
		{
		ERR_PRINTF1(_L("Index not present"));
		SetTestStepResult(EFail);
		}
	else
		{
		if (pinProcessType.CompareF(KTrain) == 0)
			{
			iOperation = EPinPluginTraining;
			}
		else if (pinProcessType.CompareF(KIdentify) == 0)
			{
			iOperation = EPinPluginIdentify;
			}
		}
	TPtrC pinValue;
	if (GetStringFromConfig(ConfigSection(), _L("pinvalue"), pinValue))
		{
		if(pinValue == _L("readoutput"))
			{
			iPinValue = GetPinFromOutputFileL();
			}
		else
			{
			iPinValue.Copy(pinValue);
			}
		}
	
	if (!GetIntFromConfig(ConfigSection(), _L("selectedindex"), iIndex))
		{
		ERR_PRINTF1(_L("Index not present"));
		SetTestStepResult(EFail);
		}
	if (!GetIntFromConfig(ConfigSection(), _L("newpinlength"), iNewPinLength))
		{
		ERR_PRINTF1(_L("new pin length missing"));
		SetTestStepResult(EFail);	
		}
	if (!GetIntFromConfig(ConfigSection(), _L("outputatdialog"), iDialogNumber))
		{
		ERR_PRINTF1(_L("dialog number missing"));
		SetTestStepResult(EFail);	
		}
	
	return TestStepResult();
	}

TVerdict CCreateFile::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
	
	RFileWriteStream stream;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TInt err = stream.Replace(fs, KInputFile, EFileWrite | EFileShareExclusive);
	User::LeaveIfError(err);
	stream.PushL();
	
	MStreamBuf* streamBuf = stream.Sink();
	streamBuf->SeekL(MStreamBuf::EWrite, EStreamEnd);
	stream.WriteInt32L(iOperation);
	stream.WriteInt32L(iPinValue.Length());
	stream.WriteL(iPinValue);
	stream.WriteInt32L(iIndex);
	stream.WriteInt32L(iNewPinLength);
	TPinPluginDialogResult cmdResult = EOk;
	stream.WriteInt32L(cmdResult);
	iOperation = EPinPluginInfo;
	stream.WriteInt32L(iOperation);
	TPinPluginDialogResult cmdFinalResult = EOk;
	stream.WriteInt32L(cmdFinalResult);
	stream.WriteInt32L(iDialogNumber);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TPinValue CCreateFile::GetPinFromOutputFileL()
	{
	RFileReadStream stream;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	User::LeaveIfError(stream.Open(fs, KOutputFile, EFileRead | EFileShareExclusive));
	stream.PushL();

	TPinValue pinvalue;
	
	stream.ReadInt32L();
	TInt pinValueLength = stream.ReadInt32L();
	HBufC8* pinValueBuf = HBufC8::NewMaxLC(pinValueLength);
	TPtr8 pinValuePtr(pinValueBuf->Des());
	stream.ReadL(pinValuePtr, pinValueLength);
	pinvalue.Copy(pinValuePtr);
	
	CleanupStack::PopAndDestroy(2, &stream);	

	return pinvalue;
	}

TVerdict CCreateFile::doTestStepPostambleL()
	{
	CActiveScheduler::Install(NULL);
	return TestStepResult();
	}





