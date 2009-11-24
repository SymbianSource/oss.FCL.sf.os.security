/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tAuthSvrStep.h"

#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"
#include "authrepository.h"
#include <scs/cleanuputils.h>

using namespace AuthServer;
class CAuthOomActive : public CActive
    {
    public:
    CAuthOomActive(RAuthMgrClient& aClient, CAuthExpression* aExpression,
				 TInt aFresh, TBool aClientSpecific, TBool aAuthViaAlias, const TDesC& aClientMessage, TBool aStop = ETrue) : 
         CActive(EPriorityNormal),
         iClient(aClient),
		 iResult(EFail),
         iRunCount(0),
         iAe(aExpression),
         iStop(aStop),
         iFresh(aFresh),
         iIdToStore(0), 
         iClientSpecific(aClientSpecific),
         iAuthViaAlias(aAuthViaAlias),
         iClientMessage(aClientMessage)
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
        
    TInt RunError(TInt aError)
    	{
    	 TRequestStatus* status = &iStatus;
    	  if(iStatus.Int() == KErrNoMemory)
    	 	 {
    	 	 CActiveScheduler::Stop();
    	 	 User::Leave(aError);
    	 	   
    	 	 }
    	  else if (iStatus == KRequestPending)
    	     {   		 
    	      User::RequestComplete(status, aError);
    	      CActiveScheduler::Stop();
    	      User::Leave(aError);
    	     }
    	       	
    	  CActiveScheduler::Stop();
    	  if(aError == KErrNoMemory)
    		  {
    		  User::Leave(aError);
    		  }
    	  return KErrNone;
    	}
    	
    void RunL() 
        {
        iErr = iStatus.Int();
        TUid val = TUid::Uid(0);
		switch (iRunCount)
		  {
		  case 0:
		  	{
		  	iId = 0;

			TUid uid = TUid::Uid(0);
				
	
			iClient.AuthenticateL(*iAe, iFresh, iClientSpecific, val, EFalse, iClientMessage, iId, iStatus);
		
			SetActive();

			break;
		  	}
            
		  case 1:
			if (0 != iId)
				{
				iIdToStore = iId->Id();
				delete iId;
				iId = NULL;
				}
			
			iResult = iStatus == KErrNone ? EPass : EFail;
            iErr = iStatus.Int();
            if(iErr == KErrNoMemory)
            	{
            	User::Leave(KErrNoMemory);
            	}
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

	~CAuthOomActive()
		{
		if(iId != NULL)
			delete iId;
		}
          
   		
	RAuthMgrClient& iClient;
	TVerdict iResult;
	TInt iRunCount;
	CAuthExpression* iAe;
	CIdentity* iId;
    TBool iStop;
    TInt iErr;
    TInt iFresh;
    TIdentityId iIdToStore;
    TBool iClientSpecific;
    TBool iAuthViaAlias;
    const TDesC& iClientMessage;
    };

class CRegOomActive : public CActive
    {
    public:
    CRegOomActive(RAuthMgrClient& aClient, CIdentity*& aResult) : 
         CActive(EPriorityNormal),
         iFirstTime(true),
         iClient(aClient), 
         iResult(aResult),
         iErr(KErrNone)
        {
        CActiveScheduler::Add(this);
        }
    void doReg()
        {
        SetActive();		
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		iFirstTime = ETrue;
        }
        void DoCancel() 
        {
        }
     void RunL() 
        {
        iErr = iStatus.Int();
        if (iFirstTime)
            {
			SetActive();
            iStatus = KRequestPending;
            iClient.RegisterIdentityL(iResult, _L("SOMENAME"), iStatus);
            iFirstTime = false;
            }
        else
            {
            iErr = iStatus.Int();
            
            if (iErr == KErrNoMemory)
            	{
            	User::Leave(KErrNoMemory);
            	}
            CActiveScheduler::Stop();
			}
        }
     TInt RunError(TInt error)
    	 {
    	 
    	 TRequestStatus* status = &iStatus;
    	 if(iStatus.Int() == KErrNoMemory)
    		 {
    		 CActiveScheduler::Stop();
    		 User::Leave(error);
    		   
    		 }
    	 else if (iStatus == KRequestPending)
    		 {   		 
    		 User::RequestComplete(status, error);
    		 CActiveScheduler::Stop();
    		 User::Leave(error);
    		 }
     	
    	 CActiveScheduler::Stop();
         User::Leave(error);
         return KErrNone;
    	 }
     
     ~CRegOomActive()
    	 {
    	 Cancel(); 	 
    	 }
        
    TBool iFirstTime;
    RAuthMgrClient& iClient;
    CIdentity*& iResult;
    TInt iErr;
    };

class CTrainOomActive : public CActive
    {
    public:    
    CTrainOomActive(RAuthMgrClient& aClient, TPluginId plugin1IdValue, TIdentityId iIdFromFile, TBool aStop = ETrue) : 
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
            if (iErr == KErrNoMemory)
                {
                 User::Leave(KErrNoMemory);
                }
			if (iStop)
                {
                CActiveScheduler::Stop();
                }

			break;

     	  default:
			iResult = EFail;
			  if (iErr == KErrNoMemory)
			    {
			    User::Leave(KErrNoMemory);
			    }
			if (iStop)
			    {
			    CActiveScheduler::Stop();
			    }
		    }
		  ++iRunCount;
          }
     TInt RunError(TInt error)
    	 {

    	 TRequestStatus* status = &iStatus;
    	 if(iStatus.Int() == KErrNoMemory)
    		 {
    		 CActiveScheduler::Stop();
    		 User::Leave(error);
    		   
    		 }
    	 else if (iStatus == KRequestPending)
    		 {   		 
    		 User::RequestComplete(status, error);
    		 CActiveScheduler::Stop();
    		 User::Leave(error);
    		 }
     	
    	 CActiveScheduler::Stop();
         User::Leave(error);
         return KErrNone;
    	 
    	 }
	RAuthMgrClient& iClient;
	TVerdict iResult;
	TInt iRunCount;
    TBool iStop;
    TInt iErr;
    TPluginId pluginIdVal;
    TIdentityId idToTrain;
    };

CAuthSvrOom::~CAuthSvrOom()
/**
  Destructor
 */
	{}

CAuthSvrOom::CAuthSvrOom(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	
	SetTestStepName(KTAuthServerOom);
	}

TVerdict CAuthSvrOom::doTestStepPreambleL()
/**
  @return - TVerdict code

 */
	{
	CTStepActSch::doTestStepPreambleL();	
	CActiveScheduler::Install(iActSchd);
	return TestStepResult();
	}


TVerdict CAuthSvrOom::doTestStepL()
	{
	
	//Client side/Server side OOM test or Performance test??
	
	TInt testType(0);
	GetIntFromConfig(ConfigSection(), _L("TestType"), testType);
	ReadTestConfigurationL();
	switch(testType)
		{
		case 1: doClientOOMTestL();
				break;
		
		case 2: doServerOOMTestL();
				break;
				
		case 3: doPerformanceTestL();
				break;
				
		default:
			//Should not reach here !
			   break;
		}
	
	return TestStepResult();
	}
TVerdict CAuthSvrOom::doClientOOMTestL()
/**
 * 	Runs the test step under OOM Conditions checking that each heap allocation is fail safe
 */
	{	
	// Pre and Post test heap cell allocation counts
 	TInt cellCountAfter = 0;
	TInt cellCountBefore = 0;
	
	/**
	 * The loop tests each heap allocation under out of memory conditions to determine whether
	 * the framework cleans up correctly without leaking memory.
	 * 
	 * The 'for' loop does not have any completion criteria, so the loop breaks out as soon 
	 * as any of the following events occur:
	 * a) The pre and post heap cell counts mismatch signalling a memory leakage
	 * b) An unexpected leave (any leave with an error code other than 'KErrNoMemory')
	 * c) All heap allocations have been tested and the test returns 'KErrNone'
	 */
	for (TInt testCount = 0; ; ++testCount)
 		{
 		__UHEAP_RESET;
 		__UHEAP_MARK;
 		
__UHEAP_SETFAIL(RHeap::EDeterministic, testCount+1);
		cellCountBefore = User::CountAllocCells();
 		AuthServer::RAuthMgrClient authMgrClient;
 		CleanupClosePushL(authMgrClient);
 		authMgrClient.Connect();
 		
 		TRAPD(err, doTestL(authMgrClient));
 		authMgrClient.ShutdownServer();
 		CleanupStack::PopAndDestroy(&authMgrClient);
 		
 		cellCountAfter = User::CountAllocCells();
 __UHEAP_MARKEND;
 		
 		
 		if (err == KErrNone)
 			{
			INFO_PRINTF1(_L("Client OOM Test Finished"));
 			break;
 			}
 		else if(err == KErrNoMemory)
 			{
 			
 			if (cellCountBefore != cellCountAfter)
 				{
 				ERR_PRINTF2(_L("OOM Test Result: Failed - Memory leakage on iteration %d"), testCount);
 				SetTestStepResult(EFail);
 				break;
 				}
 			else
 				{
 				INFO_PRINTF2(_L("OOM Pass %d"), testCount);
 				}
 			}
 		else
 			{
 			User::Leave(err);
 			break;
 			}
		}
	
	
	return TestStepResult();
	}

TVerdict CAuthSvrOom::doServerOOMTestL()
	{

	TInt err(0),err2(0);	
	AuthServer::RAuthMgrClient authMgrClient;
	CleanupClosePushL(authMgrClient);
	authMgrClient.Connect();	
	// Pre and Post test heap cell allocation counts
	for (TInt testCount=0; ; ++testCount)
 		{
 		
 		err = authMgrClient.SetServerHeapFail(testCount+1);
 		if(KErrNoMemory == err)
 			{
 			ERR_PRINTF1(_L("ReInitializing..."));
 			authMgrClient.ResetServerHeapFail();
 			continue;
 			}

 		TRAPD(retStepVal, doTestL(authMgrClient));
 		err2 = authMgrClient.ResetServerHeapFail();
 		if((err == KErrServerTerminated) || (err2 == KErrServerTerminated))
 			{
 			INFO_PRINTF1(_L("AuthServer Terminated"));	
 			SetTestStepResult(EFail);
 			break;
 			}
 			
 		if(KErrNone != err)
 			{
 			SetTestStepResult(EFail);
 			break;
 			}
 		 		
 	
 		if(KErrNoMemory == retStepVal)
 			{
 			INFO_PRINTF2(_L("OOM Pass %d"), testCount);
 			}
 		else if (KErrNone == retStepVal)
 			{
 			INFO_PRINTF1(_L("Server OOM Test Finished"));
 			break;
 			}
 		else 
 			{
 			// Propagate all errors apart from KErrNoMemory
 			User::Leave(retStepVal);
 			}
 		
 		
 		}// for
		CleanupStack::PopAndDestroy(&authMgrClient);
		WaitForServerToReleaseDb();
	return TestStepResult();
	}


TVerdict CAuthSvrOom::doTestL(AuthServer::RAuthMgrClient& ac)
	{
	TInt oomValue;
	GetIntFromConfig(ConfigSection(), _L("OOMTest"), oomValue);
	
	switch(oomValue)
		{
		case 1:
			
			break;
		
		case 2:
			//Registration API check			
			doRegOOMTestL(ac);		
			break;
		
		case 3:
			//Synchronous Authenticate API check	
			doSyncAuthOOMTestL(ac);
			break;
		
		case 4:
			//Asynchronous Authenticate API check
			doAsyncAuthOOMTestL(ac);
			break;
			
		case 5:
			//Set preffered plugin type check
			doSetPreferredTypeL(ac);
			break;
			
		case 6:
			//Check Remove Identity API
			doRemoveIdentityL(ac);
			break;
		case 7:
			//Check forget plugin API
			doForgetPluginL(ac);
			break;
		case 8:
			//Check Retrain Plugin API
			doRetrainPluginL(ac);
			break;
		case 9:
			//Check List PluginsL API
			doListPluginsL(ac);
			break;
		case 10:
			//Check List Active Plugins API
			doListActivePluginsL(ac);
			break;
		case 11:
			//Check List Plugins of type API
			doListPluginsOfTypeL(ac);
			break;
		case 12:
			//Check List Plugins With Training status API
			doListPluginsWithTrainingStatusL(ac);
			break;
		case 13:
			//Check List Authentication aliases API
			doListAuthStrengthAliasesL(ac);
			break;
		case 14:
			// Check the reset API (all plugins)
			doResetAll(ac);
			break;
		case 15:
			// Check the reset API (plugins of specified type)
			doResetType(ac);
			break;
		case 16:
			// Check the reset API (specified plugins)
			doResetList(ac);
			break;
		default:
			//Should never reach here
			break;
		}
	return TestStepResult();
	}

void CAuthSvrOom::doStartupTestL()
	{
	RAuthClient r1;
	CleanupClosePushL(r1);
	r1.Connect();
	CleanupStack::PopAndDestroy(&r1);	
	}

void CAuthSvrOom::doRegOOMTestL(AuthServer::RAuthMgrClient &ac)
	{
	
	CIdentity* identity1 = 0;
    CRegOomActive active(ac, identity1);
    active.doReg();
	CActiveScheduler::Start();
	delete identity1;
	}

void CAuthSvrOom::doSyncAuthOOMTestL(AuthServer::RAuthMgrClient &ac)
	{
	TUid val = TUid::Uid(0);
	CIdentity* id = 0;
	CAuthExpression* expr = CreateAuthExprLC(iExprString);			
	id = ac.AuthenticateL(*expr, iFreshnessVal, iClientSpecificKeyVal, val, EFalse, iMessage);
	
	delete id;
	id = 0;
	CleanupStack::PopAndDestroy(expr);
	}

void CAuthSvrOom::doAsyncAuthOOMTestL(AuthServer::RAuthMgrClient &ac)
	{
	CAuthExpression* expr = ac.CreateAuthExpressionL(iExprString);
	CleanupStack::PushL(expr);
		
	CAuthOomActive active(ac, expr, iFreshnessVal, iClientSpecificKeyVal, EFalse, iMessage);
	active.doAuth();		
	CActiveScheduler::Start();
			
	if(expr)
		{
		CleanupStack::PopAndDestroy(expr);
		}
	}

void CAuthSvrOom::doSetPreferredTypeL(AuthServer::RAuthMgrClient &ac)
	{
	TInt plugin(0);
	GetHexFromConfig(ConfigSection(),KKnowledgePrefsTag, plugin);
	TPluginId id = plugin;
	ac.SetPreferredTypePluginL(EAuthKnowledge, id);
	}

void CAuthSvrOom::doRemoveIdentityL(AuthServer::RAuthMgrClient &ac)
	{
	TInt plugin1IdValue = 0;	
	GetHexFromConfig(ConfigSection(),_L("plugin1Id"), plugin1IdValue);
	
	//Read the identityId to be removed
	TIdentityId idToRemove = getLastAuthId();
	ac.RemoveIdentityL(idToRemove);
	}

void CAuthSvrOom::doForgetPluginL(AuthServer::RAuthMgrClient &ac)
	{
	TInt plugin1IdValue = 0;	
	GetHexFromConfig(ConfigSection(),_L("plugin1Id"), plugin1IdValue);
	
	//Read the identityId to be forgotten
	TIdentityId idToForget = getLastAuthId();
	ac.ForgetPluginL( idToForget , plugin1IdValue);
	}

void CAuthSvrOom::doRetrainPluginL(AuthServer::RAuthMgrClient &ac)
	{
	TInt plugin1IdValue = 0;
	GetHexFromConfig(ConfigSection(),_L("plugin1Id"), plugin1IdValue);
	
	TIdentityId idToTrain = getLastAuthId();
	//Train the plugin again with this identity. 	    
 	CTrainOomActive active(ac, plugin1IdValue, idToTrain);
	active.doTrain();		
	CActiveScheduler::Start();			
	}

void CAuthSvrOom::doListPluginsL(AuthServer::RAuthMgrClient &ac)
	{
	RPluginDescriptions pluginList1;
	TCleanupItem cleanup(CleanupEComArray, &pluginList1);
	CleanupStack::PushL(cleanup);					
	ac.PluginsL(pluginList1);	
	CleanupStack::PopAndDestroy(&pluginList1);
		
	}

void CAuthSvrOom::doListActivePluginsL(AuthServer::RAuthMgrClient &ac)
	{
	RPluginDescriptions pluginList1;
	TCleanupItem cleanup(CleanupEComArray, &pluginList1);
	CleanupStack::PushL(cleanup);					
	ac.ActivePluginsL(pluginList1);	
	CleanupStack::PopAndDestroy(&pluginList1);
	}

void CAuthSvrOom::doListPluginsOfTypeL(AuthServer::RAuthMgrClient &ac)
	{
	RCPointerArray<const CPluginDesc> pluginsList;
	CleanupClosePushL(pluginsList);
	TAuthPluginType t1 = EAuthKnowledge;				
	ac.PluginsOfTypeL(t1, pluginsList);	
	CleanupStack::PopAndDestroy(&pluginsList);
	}

void CAuthSvrOom::doListPluginsWithTrainingStatusL(AuthServer::RAuthMgrClient &ac)
	{
	RCPointerArray<const CPluginDesc> pluginsList;
	CleanupClosePushL(pluginsList);		
	TAuthTrainingStatus t1 = EAuthTrained;
	ac.PluginsWithTrainingStatusL(t1, pluginsList);	
	CleanupStack::PopAndDestroy(&pluginsList);
	}

void CAuthSvrOom::doListAuthStrengthAliasesL(AuthServer::RAuthMgrClient &ac)
	{
	RPointerArray<HBufC> aliasList;
	CleanupResetAndDestroyPushL(aliasList);
	ac.ListAuthAliasesL(aliasList);
	CleanupStack::PopAndDestroy(&aliasList);
	}

// Check the reset API (all plugins)
void CAuthSvrOom::doResetAll(AuthServer::RAuthMgrClient& ac)
	{
	TIdentityId identity = getLastAuthId(); // Anything but 0 since client does sanity check
	ac.ResetIdentityL(identity, _L("1234"));
	}

// Check the reset API (plugins of specified type)
void CAuthSvrOom::doResetType(AuthServer::RAuthMgrClient& ac)
	{
	TIdentityId identity = getLastAuthId(); // Anything but 0 since client does sanity check
	ac.ResetIdentityL(identity, EAuthKnowledge, _L("2342"));
	}

// Check the reset API (specified plugins)
void CAuthSvrOom::doResetList(AuthServer::RAuthMgrClient& ac)
	{
	TIdentityId identity = getLastAuthId(); // Anything but 0 since client does sanity check
	TInt pluginId(0);
	GetHexFromConfig(ConfigSection(),_L("plugin1Id"), pluginId);
	RArray<TPluginId> pluginList;
	CleanupClosePushL(pluginList);
	RPointerArray<const HBufC> regDataList;
	CleanupResetAndDestroyPushL(regDataList);
	pluginList.AppendL(pluginId);
	regDataList.AppendL(_L("1344").AllocL());
	ac.ResetIdentityL(identity, pluginList, regDataList);
	CleanupStack::PopAndDestroy(2, &pluginList);
	}

void CAuthSvrOom::ReadTestConfigurationL()
	{
	
	TPtrC displayMessage;
	TBool messageSent = (GetStringFromConfig(ConfigSection(),_L("DisplayMessage"), displayMessage) != EFalse);
	iMessage = displayMessage;
	ClientMessage(messageSent, displayMessage);
	if(!messageSent)
		{
		iMessage = KNullDesC();
		}
	
	TPinValue pinValue = GetPinFromOutputFileL();
	CreatePinPluginInputFileL(EPinPluginIdentify, pinValue);
		

	GetStringFromConfig(ConfigSection(),_L("pluginId"), iExprString);
  	
	
	//Retrieve the value of 'freshness' from the ini file, but if it's absent use the default value
	if(GetIntFromConfig(ConfigSection(),_L("freshness"), iFreshnessVal) == EFalse)
		{
		iFreshnessVal = 10;
		}
	 		
	iClientSpecificKeyVal = EFalse;	 	
	//Retrieve the value of 'clientSpecificKey' from the ini file, but if it's absent use the default value
	if(GetBoolFromConfig(ConfigSection(),_L("clientSpecificKey"), iClientSpecificKeyVal) == EFalse)
		{
		iClientSpecificKeyVal = EFalse;
		}
	 	
	iDefaultpluginusage = EFalse;
	if(GetBoolFromConfig(ConfigSection(),_L("defaultpluginusage"), iDefaultpluginusage ) == EFalse)
		 {
		 iDefaultpluginusage  = EFalse;
		 }

	}


TVerdict CAuthSvrOom::doTestStepPostambleL()
/**
  @return - TVerdict code

 */
	{
	
	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}

TVerdict CAuthSvrOom::doPerformanceTestL()
/**
 * 	Runs the test step under OOM Conditions checking that each heap allocation is fail safe
 */
	{
	TInt oomValue;
	GetIntFromConfig(ConfigSection(), _L("OOMTest"), oomValue);
	GetIntFromConfig(ConfigSection(), _L("iterationCount"), iIterationCount);	
	StartTimer();
	
	for (TInt i = 0; i < iIterationCount; ++i)
 		{
 		
 		AuthServer::RAuthMgrClient authMgrClient;
 		CleanupClosePushL(authMgrClient);
 		authMgrClient.Connect();
 	
 		TRAPD(err, doTestL(authMgrClient));
 		authMgrClient.ShutdownServer();
 		CleanupStack::PopAndDestroy(&authMgrClient);
 		if(err != KErrNone)
 			{
 			User::Leave(err);
 			break;
 			}
 
 		ReadTestConfigurationL();
 		if (oomValue == 2)
 			{
 			RemoveExistingDbL();
 			}
 		}
	
	StopTimerAndPrintResultL();
	return TestStepResult();
	}
	
void CAuthSvrOom::PrintPerformanceLog(TTime aTime)
	{
	TDateTime timer = aTime.DateTime();
	INFO_PRINTF6(_L("%S,%d:%d:%d:%d"), &KPerformanceTestInfo(), timer.Hour(), timer.Minute(), timer.Second(), timer.MicroSecond());
	}

void CAuthSvrOom::StartTimer()
	{
	iStartTime.HomeTime();
	PrintPerformanceLog(iStartTime);
	}

void CAuthSvrOom::StopTimerAndPrintResultL()
	{
	TTime endTime;
	endTime.HomeTime();
	PrintPerformanceLog(endTime);
	
	TTimeIntervalMicroSeconds duration = endTime.MicroSecondsFrom(iStartTime);
	TInt actualDuration = (I64INT(duration.Int64())/1000)/iIterationCount; // in millisecond
	
	INFO_PRINTF3(_L("%S,%d"), &KActualTestCaseDuration(), actualDuration);
	}

