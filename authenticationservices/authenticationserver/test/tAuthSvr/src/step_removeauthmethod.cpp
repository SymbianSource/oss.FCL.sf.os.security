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
* CTestStep derived implementation
*
*/

 
#include "tAuthSvrStep.h"

#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"

using namespace AuthServer;

//====================================================================================================    
CTRemoveAuthMethod::CTRemoveAuthMethod(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTForgetPlugin);
	}

CTRemoveAuthMethod::~CTRemoveAuthMethod()
/**
  Destructor
 */
	{
	}
	
TVerdict CTRemoveAuthMethod::doTestStepPreambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();
	return TestStepResult();
	}

TVerdict CTRemoveAuthMethod::doTestStepL()
/**
  @return - Test step result (EPass or EFail)    
 */
	{	
	SetTestStepResult(EPass);
__UHEAP_MARK;		// Check for memory leaks

	//-----------------------------------------------------------------------------------------------------
	InitAuthServerFromFileL();	// Set things like 'iSupportsDefaultData' and 'DefaultPlugin'
	
	CActiveScheduler::Install(iActSchd);
	//Connect to the AuthServer	
	AuthServer::RAuthMgrClient authMgrClient1;	
	TInt connectVal = authMgrClient1.Connect();
	if (KErrNotFound == connectVal)
		{
		//Retry after a delay
		TTimeIntervalMicroSeconds32 timeInterval = 2000;	//2 Milliseconds
		User::After(timeInterval);
		connectVal = authMgrClient1.Connect();
		}
	if (KErrNone != connectVal)
		{
		ERR_PRINTF2(_L("Unable to start a session or other connection error. Err = %d"), connectVal);
		User::LeaveIfError(connectVal);		
		}	
	
	CleanupClosePushL(authMgrClient1);
	//-----------------------------------------------------------------------------------------------------
	TBool statusAll = EFalse;
	statusAll = CheckPluginStatusAllL(authMgrClient1);

	TRAPD(res2, RemoveAuthMethodL(authMgrClient1) );
	if(KErrNone != res2)
		{
		ERR_PRINTF2(_L("RemoveAuthMethodL() performed a Leave with code %d"), res2 );
		SetTestStepError(res2);
		SetTestStepResult(EFail);
		}	
	statusAll = CheckPluginStatusAllL(authMgrClient1);
	if (!statusAll)
		{
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(&authMgrClient1);	// authClient1	
	//Garbage collect the last previously destroyed implementation 
	// and close the REComSession if no longer in use
	REComSession::FinalClose(); 
__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTRemoveAuthMethod::doTestStepPostambleL()
/**
  @return - Pass or fail value of test step result
 */
	{
	//Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();	
	return TestStepResult();
	}


void CTRemoveAuthMethod::RemoveAuthMethodL (RAuthMgrClient& mgc)
	{	
	//If the pluginId is quoted in the ini file, check it's training status
	TInt forgetPluginValue = 0;
	TBool forgetUnknownId  = EFalse;


	TBool ok =
		GetHexFromConfig(ConfigSection(),_L("forgetPlugin"), forgetPluginValue);

	GetBoolFromConfig(ConfigSection(),_L("forgetUnknown"), forgetUnknownId);
	
	if (ok)
		{
		// Get the most recently authenticated identity, which we want this plugin to forget.
		TIdentityId identity = 0;

		if (forgetUnknownId)
		  {
		  INFO_PRINTF1(_L("Forgetting for unknown identity!"));
		  identity = 0;
		  }
		else
		  {
		  identity = getLastAuthId();
		  }
		
		// now forget the plugin
		RDebug::Printf("Forget %d, %d", identity, forgetPluginValue);
		mgc.ForgetPluginL(identity, forgetPluginValue);
		RDebug::Printf("DONE: Forget %d, %d", identity, forgetPluginValue);
		}
	else
		{
		INFO_PRINTF1(_L("missing parameters in the ini file!"));
		SetTestStepResult(EFail);
		}
	}
