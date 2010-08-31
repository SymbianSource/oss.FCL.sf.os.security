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
CTSetAuthPrefs::CTSetAuthPrefs(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTSetPrefs);
	}

CTSetAuthPrefs::~CTSetAuthPrefs()
/**
  Destructor
 */
	{}
	
TVerdict CTSetAuthPrefs::doTestStepPreambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();		
	
	return TestStepResult();
	}

TVerdict CTSetAuthPrefs::doTestStepL()
/**
  @return - TVerdict code
  Override of base class pure virtual
  
 */
	{	
	//If any test step leaves or panics, the test step thread won't exit, further calls are made.
	// but the html log is updatead, so that subsequent failures could be traced here if necessary. 
	if (TestStepResult() != EPass)
		{
		ERR_PRINTF1(_L("There was an error in a previous test step"));
		return TestStepResult();
		}

	SetTestStepResult(EPass);

__UHEAP_MARK;		// Check for memory leaks

	//-----------------------------------------------------------------------------------------------------
	InitAuthServerFromFileL();	// Set things like 'iSupportsDefaultData' and 'DefaultPlugin'
	
	//Connect to the AuthServer	
	AuthServer::RAuthMgrClient authMgrClient;	
	TInt connectVal = authMgrClient.Connect();
	if (KErrNotFound == connectVal)
		{
		//Retry after a delay
		TTimeIntervalMicroSeconds32 timeInterval = 2000;	//2 Milliseconds
		User::After(timeInterval);
		connectVal = authMgrClient.Connect();
		}
	if (KErrNone != connectVal)
		{
		ERR_PRINTF2(_L("Unable to start a session or other connection error. Err = %d"), connectVal);
		User::LeaveIfError(connectVal);		
		}	
	
	CleanupClosePushL(authMgrClient);
	
	AuthServer::RAuthClient authClient;
			
	connectVal = authClient.Connect();
	if (KErrNotFound == connectVal)
		{
		//Retry after a delay
		TTimeIntervalMicroSeconds32 timeInterval = 2000;	//2 Milliseconds
		User::After(timeInterval);
		connectVal = authClient.Connect();
		}
	if (KErrNone != connectVal)
		{
		ERR_PRINTF2(_L("Unable to start a session or other connection error. Err = %d"), connectVal);
		User::LeaveIfError(connectVal);		
		}	
	
	CleanupClosePushL(authClient);
	//-----------------------------------------------------------------------------------------------------
	TRAPD(res2, SetAuthPrefsL(authMgrClient, authClient) );
	if(KErrNone != res2)
		{
		ERR_PRINTF2(_L("SetAuthPrefsL() performed a Leave with code %d"), res2 );
		SetTestStepError(res2);
		SetTestStepResult(EFail);
		}
		
	CleanupStack::PopAndDestroy(2,&authMgrClient);	// authClient, authMgrClient 	
	//Garbage collect the last previously destroyed implementation 
	// and close the REComSession if no longer in use
	REComSession::FinalClose(); 
__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTSetAuthPrefs::doTestStepPostambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}


void CTSetAuthPrefs::SetAuthPrefsL (AuthServer::RAuthMgrClient& aAuthMgrClient, 
									AuthServer::RAuthClient& aAuthClient)
	{	
	TInt plugin = KUnknownPluginId;

	TBool done = EFalse;

	if (GetHexFromConfig(ConfigSection(),KDefauthPrefsTag, plugin))
	  {
	  aAuthMgrClient.SetPreferredTypePluginL(EAuthDefault, plugin);
	  if(aAuthClient.PreferredTypePluginL(EAuthDefault) == plugin)
		  {
		  done = ETrue;
		  }
	  }
	if (GetHexFromConfig(ConfigSection(),KKnowledgePrefsTag, plugin))
	  {
	  aAuthMgrClient.SetPreferredTypePluginL(EAuthKnowledge, plugin);
	  if(aAuthClient.PreferredTypePluginL(EAuthKnowledge) == plugin)
  		  {
  		  done = ETrue;
  		  }
	  }
	if (GetHexFromConfig(ConfigSection(),KBiometricPrefsTag, plugin))
	  {
	  aAuthMgrClient.SetPreferredTypePluginL(EAuthBiometric, plugin);
	  if(aAuthClient.PreferredTypePluginL(EAuthBiometric) == plugin)
  		  {
  		  done = ETrue;
  		  }
	  }
	if (GetHexFromConfig(ConfigSection(),KTokenPrefsTag, plugin))
	  {
	  aAuthMgrClient.SetPreferredTypePluginL(EAuthToken, plugin);
	  if(aAuthClient.PreferredTypePluginL(EAuthToken) == plugin)
  		  {
  		  done = ETrue;
  		  }
	  }

	if (!done)
	  {
		INFO_PRINTF1(_L("missing parameters in the ini file!"));
		SetTestStepResult(EFail);
	  }
	}
