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

class CTrainActive : public CActive
    {
    public:    
    CTrainActive(RAuthMgrClient& aClient, TPluginId plugin1IdValue, TIdentityId iIdFromFile, TBool aStop = ETrue) : 
         CActive(EPriorityNormal),
         iClient(aClient),
		 iResult(EFail),
         iRunCount(0),
         iStop(aStop),
         pluginIdVal(plugin1IdValue),         
         idToTrain (iIdFromFile)
        {
        CActiveScheduler::Add(this);
        }
    void doTrain() 
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
	            iClient.TrainPlugin(idToTrain, pluginIdVal, iStatus);
				SetActive();
			break;

   		  case 1:
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
    TBool iStop;
    TInt iErr;
    TPluginId pluginIdVal;
    TIdentityId idToTrain;
    };
    
//====================================================================================================    
CTRetrainPlugin::CTRetrainPlugin(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTRetrainPlugin);
	}

CTRetrainPlugin::~CTRetrainPlugin()
/**
  Destructor
 */
	{}
	
TVerdict CTRetrainPlugin::doTestStepPreambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();
	return TestStepResult();
	}

TVerdict CTRetrainPlugin::doTestStepL()
/**
  @return - TVerdict code indicating the test step result    
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
	//Check the username of the second identity and then attempt to set it.
	//SetTestStepResult(checkAndSetUserNameL(ac, 0));

	TBool statusAll = EFalse;
	statusAll = CheckPluginStatusAllL(authMgrClient1);
	
	TRAPD(res2, RetrainPlugin(authMgrClient1) );
	if(KErrNone != res2)
		{
		ERR_PRINTF2(_L("RetrainPluginL() performed a Leave with code %d"), res2 );
		}
		
	TRAPD(res1, CheckSpecifiedPluginStatusL());
	if(KErrNone != res1)
		{
		ERR_PRINTF2(_L("checkSpecifiedPluginStatusL() performed a Leave with code %d"), res1 );
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


TVerdict CTRetrainPlugin::doTestStepPostambleL()
/**
  @return - TVerdict code  
 */
	{
	//Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();	
	return TestStepResult();
	}

/**
  Retrain the plugin and set the test step error value as required.
 */
void CTRetrainPlugin::RetrainPlugin (RAuthMgrClient& mgc)
	{	
	
	//If the pluginId is quoted in the ini file, check it's training status
	TInt plugin1IdValue = 0;	
	if (GetHexFromConfig(ConfigSection(),_L("plugin1Id"), plugin1IdValue) != EFalse) // the tag 'pluginId1' was present
		{ 	
	 	//Read the identityId to be trained
	 	TIdentityId idToTrain = getLastAuthId();
	 	INFO_PRINTF2(_L("Id to be (re)trained: %u"), idToTrain);
	 	
	 	//Train the plugin again with this identity. 	    
 	    CTrainActive active(mgc, plugin1IdValue, idToTrain);
	    active.doTrain();		
		CActiveScheduler::Start();		
		
		SetTestStepError(active.iErr);
		
		if (KErrAuthServPluginCancelled == active.iErr)
			{
			INFO_PRINTF1(_L("Training was cancelled."));
			INFO_PRINTF1(_L("User entered trainingInput same as identifyingInput or an existing PIN."));
			INFO_PRINTF1(_L("Or the Cancel code."));			
			SetTestStepError(KErrAuthServPluginCancelled);
			}
		if (KErrAuthServPluginQuit == active.iErr)
			{
			INFO_PRINTF1(_L("Training was quit."));
			INFO_PRINTF1(_L("User entered the Quit code as trainingInput."));						
			SetTestStepError(KErrAuthServPluginQuit);
			}
		else if(EPass != active.iResult)
			{
			ERR_PRINTF3(_L("Training Result error = %d, iResult = %d\n"), active.iErr, active.iResult);			
			SetTestStepError(active.iErr);
			SetTestStepResult (EFail);
			}
		else if ((KErrNone == active.iErr) && (EPass == active.iResult))
			{
			INFO_PRINTF1(_L("(Re)train successful."));
			SetTestStepResult (EPass);
			}
		else
			{
			ERR_PRINTF3(_L("Unexpected Training Result. error = %d, iResult = %d\n"), active.iErr, active.iResult);
			SetTestStepError(active.iErr);
			SetTestStepResult (EFail);
			}		
		}
		
	else
		{
		INFO_PRINTF1(_L("plugin1Id was not specified in the ini file"));
		}
	}
