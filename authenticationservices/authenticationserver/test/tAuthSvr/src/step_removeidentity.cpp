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

using namespace AuthServer;

/*
class CAuthActive : CActive
    {
    public:
    CAuthActive(RAuthMgrClient& aClient, TPluginId plugin1IdValue, TInt aFresh, TBool aStop = ETrue) : 
         CActive(EPriorityNormal),
         iClient(aClient),
		 iResult(EFail),
         iRunCount(0),
         iStop(aStop),
         pluginIdVal(plugin1IdValue),
         iFresh(aFresh)
         
        {
        CActiveScheduler::Add(this);
        }
    void doAuth() 
        {
        SetActive();		
		TRequestStatus* status = &iStatus;		    
		User::RequestComplete(status, KErrNone);
		iRunCount = 0;
        }
	void DoCancel() 
        {
        }
     void RunL() 
        {
        iErr = iStatus.Int();
        if (iStatus.Int() != KErrNone)
            {
            }
		switch (iRunCount)
		  {
		  case 0:
            iStatus = KRequestPending;
			iAe = AuthExpr(pluginIdVal);
			iId = 0;

			iClient.AuthenticateL(*iAe, iFresh, EFalse, EFalse, iId, iStatus);
			SetActive();

			break;
		  case 1:
			delete iAe;
			iStatus = KRequestPending;
			TRequestStatus* status;
			status = &iStatus;				
			User::RequestComplete(status, iErr);				
			SetActive();
			
			break;
   		  case 2:
			iResult = iStatus.Int() == KErrNone ? EPass : EFail;
            iErr = iStatus.Int();
			if (iStop)
                {
                CActiveScheduler::Stop();
                }

			break;
     	  default:
			iResult = EFail;
			
			if (iStop)
			    {
			    CActiveScheduler::Stop();
			    }
		    }
		  ++iRunCount;
          }
	RAuthMgrClient& iClient;
	TVerdict iResult;
	TInt iRunCount;
	CAuthExpression* iAe;
	CIdentity* iId;
    TBool iStop;
    TInt iErr;
    TPluginId pluginIdVal;
    TInt iFresh;
    };
    */
//====================================================================================================    
CTRemoveIdentity::CTRemoveIdentity(CTAuthSvrServer& aParent): iParent(aParent)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTRemoveIdentity);
	}

CTRemoveIdentity::~CTRemoveIdentity()
/**
 * Destructor
 */
	{}
	
TVerdict CTRemoveIdentity::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();
		
	//SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTRemoveIdentity::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * 
 */
	{	
	//INFO_PRINTF1(_L("In CTRemoveIdentity doTestStepL"));
	
	//User::SetJustInTime(ETrue);	//a panic will kill just the thread, not the whole emulator.
	
	//If any test step leaves or panics, the test step thread won't exit, further calls are made.
	// but the html log is updated, so that subsequent failures could be traced here if necessary. 
	if (TestStepResult() != EPass)
		{
		ERR_PRINTF1(_L("There was an error in a previous test step"));
		//return TestStepResult();
		}

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
	/*
	//Examine the authserver and see what's there
	RAuthClient ac;
	User::LeaveIfError(ac.Connect());
	CleanupClosePushL(ac);
	RPluginDescriptions pluginList1;
	TCleanupItem cleanup(CleanupEComArray, &pluginList1);
	CleanupStack::PushL(cleanup);				
	
	ac.PluginsL(pluginList1);	
	TInt numTotalPlugins = pluginList1.Count();
	for(TInt i = 0; i < numTotalPlugins; i++)
		{
		TInt presentPluginIdVal = pluginList1[i]->Id();
		TInt presentPluginStatus = pluginList1[i]->TrainingStatus();
		INFO_PRINTF3(_L("PluginId=%x,status =%i"), presentPluginIdVal,presentPluginStatus);
		}
	CleanupStack::PopAndDestroy(&pluginList1); //infoArray, results in a call to CleanupEComArray	
	
	//Check the username of the second identity and then attempt to set it.
	//SetTestStepResult(checkAndSetUserNameL(ac, 0));
	
	CleanupStack::PopAndDestroy(&ac);	// authClient
	//-----------------------------------------------------------------------------------------------------
	*/

	TBool statusAll = EFalse;
	statusAll = CheckPluginStatusAllL(authMgrClient1);

	// Authenticate by reading the user input
	// Then call the removeIdentityL function
	
	TRAPD(res2, doRemoveIdentityL(authMgrClient1) );
	if(KErrNone != res2)
		{
		ERR_PRINTF2(_L("doRemoveIdentityL() performed a Leave with code %d"), res2 );
		//SetTestStepResult(EPass);
		SetTestStepError(res2);
		}
	
	/*	
	TRAPD(res1, CheckSpecifiedPluginStatusL());
	if(KErrNone != res1)
		{
		ERR_PRINTF2(_L("checkSpecifiedPluginStatusL() performed a Leave with code %d"), res1 );
		}
	*/
	
	statusAll = CheckPluginStatusAllL(authMgrClient1);
	if (!statusAll)
		{
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("Plugin Status OK !!"));
		}

	CleanupStack::PopAndDestroy(&authMgrClient1);	// authClient1	
	//Garbage collect the last previously destroyed implementation 
	// and close the REComSession if no longer in use
	REComSession::FinalClose(); 
__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTRemoveIdentity::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();
	//INFO_PRINTF1(_L("Test Step Postamble"));
	return TestStepResult();
	}


//
//
void CTRemoveIdentity::doRemoveIdentityL (RAuthMgrClient& mgc)
	{	
	
	//If the pluginId is quoted in the ini file, check it's training status
	TInt plugin1IdValue = 0;	
	if (GetHexFromConfig(ConfigSection(),_L("plugin1Id"), plugin1IdValue) != EFalse) // the tag 'pluginId1' was present
		{
	 	//Read the identityId to be trained
	 	TIdentityId idToRemove = getLastAuthId();

		mgc.RemoveIdentityL(idToRemove);
		
		//Check to see if a retry is recommended
		TBool retryRemoveValue = EFalse;
		if (GetBoolFromConfig(ConfigSection(),_L("retryRemove"), retryRemoveValue) != EFalse) 
			{
			if (retryRemoveValue)
				{ mgc.RemoveIdentityL(idToRemove); }
			}
		}
		
	else
		{
		INFO_PRINTF1(_L("plugin1Id was not specified in the ini file"));
		}
		
	}
