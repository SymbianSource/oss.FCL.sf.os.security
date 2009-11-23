/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Multi threaded tests on AuthServer.
*
*/


#include "tAuthSvrStep.h"
#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"



using namespace AuthServer;

//Active object for registration
class CMultiThreadedRegisterActive : public CActive
    {
    public:
     CMultiThreadedRegisterActive(RAuthMgrClient& aClient, HBufC* aIdentityString, CIdentity*& aResult) : 
         CActive(EPriorityNormal),
         iFirstTime(true),
         iClient(aClient), 
         iResult(aResult),
         iIdentityString(aIdentityString),
         iErr(KErrNone)
        {
        CActiveScheduler::Add(this);
        }
    ~CMultiThreadedRegisterActive()
        {
        delete iIdentityString; 
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
            iClient.RegisterIdentityL(iResult, *iIdentityString, iStatus);
            iFirstTime = false;
            }
        else
            {
            iErr = iStatus.Int();
            RDebug::Printf("iErr = %d", iErr);
            CActiveScheduler::Stop();
            }
        }
        
    TBool iFirstTime;
    RAuthMgrClient& iClient;
    CIdentity*& iResult;
    HBufC* iIdentityString;
    TInt iErr;
    };

CTMultiThreaded::~CTMultiThreaded()
	{}

CTMultiThreaded::CTMultiThreaded(CTAuthSvrServer& aParent): iParent(aParent)

	{
	
	SetTestStepName(KTMultiThreadedTest);
	}

TVerdict CTMultiThreaded::doTestStepPreambleL()

	{
	CTStepActSch::doTestStepPreambleL();	
	return TestStepResult();
	}

TVerdict CTMultiThreaded::doTestStepPostambleL()

    {
    
    CTStepActSch::doTestStepPostambleL();
    return TestStepResult();
    }

TVerdict CTMultiThreaded::doTestStepL()
	{
	SetTestStepResult(EPass);
	Logger().ShareAuto();
	//Initialize AuthServer.
	InitAuthServerFromFileL();  
	
	
	//Setup two threads which request for registration concurrently.    
	//Create first thread.
	RThread firstThread;
	TInt err(0);
	TThreadParams firstThreadParams;
	err = firstThread.Create( _L("First Thread"), (TThreadFunction)runMultiThreadedTest, 
                                    KDefaultStackSize, KMinHeapSize, 1024*1024 /*Max heap size*/, &firstThreadParams);
	if(err != KErrNone)
	    {
	    ERR_PRINTF2(_L("Couldn't start first thread. Error = %d"), err);
	    SetTestStepResult(EFail);
	    return TestStepResult();
	    }
	    
	// Setup the requeststatus for firstThread completion notification.
	TRequestStatus firstThreadFinishStatus;
	firstThread.Logon(firstThreadFinishStatus);
	
	
	
	//Setup the second thread similarly.
	RThread secondThread;
	TThreadParams secondThreadParams;
    err = secondThread.Create( _L("Second Thread"), (TThreadFunction)runMultiThreadedTest, 
									KDefaultStackSize, KMinHeapSize, 1024*1024 /*Max heap size*/, &secondThreadParams);
    if(err != KErrNone)
        {
        ERR_PRINTF2(_L("Couldn't start second thread. Error = %d"), err);
        SetTestStepResult(EFail);
        return TestStepResult();
        }
        
    // Setup the requeststatus for secondThread completion notification.
    TRequestStatus secondThreadFinishStatus;
    secondThread.Logon(secondThreadFinishStatus);
    
    //Resume both threads.
	firstThread.Resume();   
	secondThread.Resume();   
	
	//Wait for either request to complete.
	User::WaitForRequest(firstThreadFinishStatus, secondThreadFinishStatus );     
	
	//Now, wait for the unfinished thread.
	User::WaitForRequest(firstThreadFinishStatus == KRequestPending? firstThreadFinishStatus:secondThreadFinishStatus);
	
	INFO_PRINTF1(_L("Both Threads completed."));
	INFO_PRINTF2(_L("First thread completed with %d."), firstThreadParams.result);
	INFO_PRINTF2(_L("Second thread completed with %d."), secondThreadParams.result);
	
	if((firstThreadParams.result != KErrServerBusy && secondThreadParams.result != KErrNone) &&
	        (firstThreadParams.result != KErrNone && secondThreadParams.result != KErrServerBusy))
	    {
	    SetTestStepResult(EFail);
	    }
	firstThread.Close();
	secondThread.Close();
	return TestStepResult();
	}



void CTMultiThreaded::runMultiThreadedTest(TThreadParams* aParams)
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    CActiveScheduler* sched(0);
    
    TRAPD(err, sched = new (ELeave) CActiveScheduler);
    if(err != KErrNone)
        {
        delete cleanup;
        User::Exit(err);
        }
    CActiveScheduler::Install(sched); 
   
    
    /* Switch case can be added here to generalize this function to run any method.
     * For now, only registration is called.
     */
    
    TRAPD(ret,doRegisterL(*aParams));
	
	RDebug::Printf("doRegisterL returned %d", ret);
    
    delete cleanup;
    delete sched;
    }


void CTMultiThreaded::doRegisterL(TThreadParams& aParams)
    {
    _LIT(KIdString, "User1");
    
    RAuthMgrClient client;
    User::LeaveIfError(client.Connect());
    CleanupClosePushL(client);
    
    HBufC* identityString = KIdString().AllocLC();
    CIdentity* identity1 = 0;
    
    CMultiThreadedRegisterActive* active = new (ELeave)CMultiThreadedRegisterActive(client, identityString, identity1);
    CleanupStack::PushL(active);
    active->doReg();
    CActiveScheduler::Start();
    
    //Store the error code in aParams.
    aParams.result = active->iErr;
    if(aParams.result == KErrNone)
        {
        RDebug::Printf("Registered new Identity! Id = %x", identity1->Id());
        }
    else
        {
        RDebug::Printf("Registration Failed !");
        }
    
    delete identity1;
    
    CleanupStack::Pop(3, &client);
    }

