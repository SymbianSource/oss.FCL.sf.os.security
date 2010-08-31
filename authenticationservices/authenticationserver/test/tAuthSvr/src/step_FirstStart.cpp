/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CTestStep derived implementation
*
*/

 
#include "tAuthSvrStep.h"

#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"
#include "authrepository.h"
using namespace AuthServer;

CTFirstStart::CTFirstStart()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTFirstStart);
	}

TVerdict CTFirstStart::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTAuthSvrStepBase::doTestStepPreambleL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTFirstStart::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * 
 */
	{
	// remove trained identity 22 for these tests
	CAuthDb2* db = CAuthDb2::NewLC(iFs);
	db->RemoveIdentityL(22);
	CleanupStack::PopAndDestroy(db);
	
	//INFO_PRINTF1(_L("In FirstStart Step"));
	
	//User::SetJustInTime(ETrue);	//a panic will kill just the thread, not the whole emulator.
	
	//If any test step leaves or panics, the test step thread exits and no further calls are made. 
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

__UHEAP_MARK;		// Check for memory leaks

	SetTestStepResult(EPass);

	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<128> fileName (sysDriveName);
	fileName.Append(KAuthSvrPolicyFile);
	CIniData* policyFile=NULL;
	policyFile=CIniData::NewL(fileName);
	CleanupStack::PushL(policyFile);
	
	fileName.Copy(sysDriveName);
	fileName.Append(KPluginIniFile);
	
	CIniData* pluginFile=NULL;
	pluginFile=CIniData::NewL(fileName);
	CleanupStack::PushL(pluginFile);
	//Set a default plugin using an ini file. Read this value from a file if it is specified	
	//Set this to be the default plugin (update the policy file) if the test case specifies it (in the script's ini file).
	TInt defaultPluginFromFile;
	if (GetIntFromConfig(ConfigSection(),_L("defaultPlugin"), defaultPluginFromFile) != EFalse) // the tag 'defaultPlugin' was present
		{
				
		//Write this value to the policy file, Which is picked up by the AuthServer (not implemented yet)
		//writeToNamedConfig(policyFile, KDefaultPluginTag, defaultPluginFromFile);
		SetDefaultPluginIdL(defaultPluginFromFile);
		}
	
	//Set whether a plugin supports default data. Read this value from a file if it is specified
	_LIT(KTrueText,"true");
	_LIT(KFalseText,"false");
	
	TBool iSupportsDefaultDataFromFile(ETrue);	//Default case - the plugin supports default data
	
	TBufC<16> trueBuf1(KTrueText);
	TBufC<16> falseBuf1(KFalseText);
	TPtr valueToWrite = trueBuf1.Des();			//Default case - the plugin supports default data
	
	
	if (GetBoolFromConfig(ConfigSection(),_L("iSupportsDefaultData"), iSupportsDefaultDataFromFile) != EFalse) // the tag 'isTrusted' was present
		{
		//Set the value that will be picked up by the constructor of the plugin
		//writeToNamedConfig(pluginIniFile, KPluginIniSection, _L("iSupportsDefaultData"), iSupportsDefaultDataFromFile);		
		if (iSupportsDefaultDataFromFile)
			{ valueToWrite = trueBuf1.Des();	}
		else
			{ valueToWrite = falseBuf1.Des();	}
			
		writeToNamedConfig(pluginFile, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);		
		}
	else
		{
		INFO_PRINTF1(_L("The iSupportsDefaultData tag was not specified in the testexecute ini file."));
		//Use the default value of false		
		writeToNamedConfig(pluginFile, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		}
		
	//Carry out the other initialisation that is required.	
	// TODO.
	

	//-----------------------------------------------------------------------------------------------------
	//Connect to the AuthServer
	AuthServer::RAuthClient authClient1;
	//TInt connectVal = User::LeaveIfError(authClient1.Connect());
	TInt connectVal = authClient1.Connect();
	if (KErrNotFound == connectVal)
		{
		//Retry after a delay
		TTimeIntervalMicroSeconds32 timeInterval = 2000;	//2 Milliseconds
		User::After(timeInterval);
		connectVal = authClient1.Connect();
		}
	if (KErrNone != connectVal)
		{
		ERR_PRINTF2(_L("Unable to start a session or other connection error. Err = %d"), connectVal);
		User::LeaveIfError(connectVal);		
		}
	CleanupClosePushL(authClient1);


	//Check that the default identity has been created
	//Call the Enumerate Identity API
	//AuthServer::RIdentityIdList idList;
	AuthServer::RIdentityIdArray idArray;
	CleanupClosePushL(idArray);
	//TInt ret =  User::LeaveIfError(authClient1.IdentitiesL(idArray));
	TInt ret = KErrNone;
	TRAP(ret, authClient1.IdentitiesL(idArray));
	if (KErrNone != ret)
		{
		ERR_PRINTF2(_L("IdentitiesL left with errorcode: %d"), ret);
		SetTestStepResult(EFail);
		}
	
	else
		{
		
		//List the present identities by their strings
		for (TInt i = 0; i < idArray.Count(); i++)
			{
			HBufC* idName = 0;
			//TRAP(ret, idName = authClient1.IdentityStringL(idArray[i]));
			idName = authClient1.IdentityStringL(idArray[i]);
			INFO_PRINTF3(_L("Id %d has the name %S"), i, idName);
			delete idName; 
			}
		//Retrieve the expected information from the ini file
		TInt expNumIdentities;
		
		if(GetIntFromConfig(ConfigSection(),_L("NumIdentities"), expNumIdentities) != EFalse) // the tag 'NumIdentities' was present
			{
			//Do the compare
			if ((idArray.Count()) != expNumIdentities)
				{
				ERR_PRINTF2(_L("expNumIdentities differs from expected, Number of Identities = %d"), (idArray.Count()));			
				SetTestStepResult(EFail);
				}
			}
		
		TBool expIsDefaultPlugin = EFalse;	
		if(GetBoolFromConfig(ConfigSection(), _L("isDefaultPlugin"), expIsDefaultPlugin) != EFalse) // the tag 'isDefaultPlugin' was present
			{
			if(!expIsDefaultPlugin && (KErrServerTerminated != ret) )
				{
				//The plugin is not a default plugin as defined in the policy file, and thus the server should have been terminated
				ERR_PRINTF2(_L("connectVal differs from expected, connectVal= %d"), connectVal);			
				SetTestStepResult(EFail);
				}	
			}
	
		}
			
		
		//Maybe check the individual identifier -not predictable though..
	CleanupStack::PopAndDestroy(&idArray); 
	CleanupStack::PopAndDestroy(&authClient1); 
	
	REComSession::FinalClose(); 	//garbage collect the last previously destroyed implementation and close the REComSession if no longer in use
__UHEAP_MARKEND;
	return TestStepResult();
	}

//TVerdict CTFirstStart::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
/*	{
	INFO_PRINTF1(_L("Test Step Postamble"));
	return TestStepResult();
	}
*/
