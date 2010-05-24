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
*
*/


#include <bautils.h>
#include <s32file.h>
#include <test/testexecutelog.h>

#include "authserver/authmgrclient.h"
#include "authserver/identity.h"
#include "tauthcliservstep.h"

using namespace AuthServer;

class CRegActive : public CActive
    {
    public:
    CRegActive(RAuthMgrClient& aClient, CIdentity*& aResult, TBool aStop = ETrue) : 
         CActive(EPriorityNormal),
         iFirstTime(true),
         iClient(aClient), 
         iResult(aResult),
         iStop(aStop)
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
        if (iFirstTime)
            {
			SetActive();
            iStatus = KRequestPending;
            iClient.RegisterIdentityL(iResult, _L("SOMENAME"), iStatus);
            iFirstTime = false;
            }
        else
            {
            if (iStop)
                {
                CActiveScheduler::Stop();
                }
            iErr = iStatus.Int();
			}
        }
        TBool iFirstTime;
        RAuthMgrClient& iClient;
        CIdentity*& iResult;
        TBool iStop;
        TInt iErr;
    };

class CTrainActive : public CActive
    {
    public:
    CTrainActive(RAuthMgrClient& aClient, TBool aStop = ETrue) : 
         CActive(EPriorityNormal),
         iClient(aClient),
		 iResult(EFail),
         iRunCount(0),
         iStop(aStop)
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
			iAe = AuthExpr(0x10274104);
			iId = 0;

			iClient.AuthenticateL(*iAe, 10, EFalse, EFalse, iId, iStatus);
			SetActive();

			break;
		  case 1:
			delete iAe;
			delete iId;
            iStatus = KRequestPending;
            iClient.TrainPlugin(22, 0x10274104, iStatus);
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
    };

class CTrainActive2 : public CActive
    {
    public:
    CTrainActive2(RAuthMgrClient& aClient, TBool aStop = ETrue) : 
         CActive(EPriorityNormal),
         iClient(aClient),
         iRunCount(0),
         iStop(aStop)
        {
        CActiveScheduler::Add(this);
        }
    void doTrain() 
        {
        SetActive();		
		iClient.TrainPlugin(22, 0x10274105, iStatus);
		}
	void DoCancel() 
        {
        }
	void RunL()
	    {
		iErr = iStatus.Int();
		if (iStop)
		    {
			CActiveScheduler::Stop();
		    }
	    }
	
	RAuthMgrClient& iClient;
	TInt iRunCount;
	CAuthExpression* iAe;
	CIdentity* iId;
    TBool iStop;
    TInt iErr;
    };

class CCancelActive : public CActive
    {
    public:
    CCancelActive(RAuthMgrClient& aClient) : 
         CActive(EPriorityNormal),
         iClient(aClient),
		 iResult(EFail),
         iRunCount(0)
        {
        CActiveScheduler::Add(this);
        }
    void doIdCancel() 
        {
        SetActive();		
		TRequestStatus* status = &iStatus;		    
		User::RequestComplete(status, KErrNone);
		iMode = 1;
		iRunCount = 0;
        }
    void doRegCancel() 
        {
        SetActive();		
		TRequestStatus* status = &iStatus;		    
		User::RequestComplete(status, KErrNone);
		iMode = 2;
		iRunCount = 0;
        }
    void doTrainCancel() 
        {
        SetActive();		
		TRequestStatus* status = &iStatus;		    
		User::RequestComplete(status, KErrNone);
		iMode = 0;
		iRunCount = 0;
        }
	void DoCancel() 
        {
        }
     void RunL() 
        {
        if (iStatus.Int() != KErrNone)
            {
			  //RDebug::Printf("CCancelActive error: %d", iStatus.Int());
            }
		switch (iRunCount)
		  {
		  case 0:
            iStatus = KRequestPending;
			iAe = AuthExpr(0x10274105);
			iId = 0;
			switch (iMode)
			  {
			  case 2:
				iClient.RegisterIdentityL(iId, _L("RegCancelID"), iStatus);
				break;
			  case 1:
				iClient.AuthenticateL(*iAe, 0, EFalse, ETrue, iId, iStatus);
			    break;
			  case 0:
				iClient.TrainPlugin(22, 0x10274105, iStatus);
				break;
			  }
			SetActive();
            User::After(1000000);
            iClient.Cancel();

			break;
		  case 1:
			//iStatus = KRequestPending;

			delete iAe;
			delete iId;
            //SetActive();

		    //break;
   		  case 2:
			iResult = iStatus.Int() == KErrCancel ? EPass : EFail;
            CActiveScheduler::Stop();

			break;
     	  default:
			iResult = EFail;
            CActiveScheduler::Stop();
		  }
		++iRunCount;
        }
	RAuthMgrClient& iClient;
	TVerdict iResult;
	TInt iRunCount;
	CAuthExpression* iAe;
	CIdentity* iId;
	TInt iMode;
    };


CTStepMgrClient::CTStepMgrClient()
	{
	SetTestStepName(KTStepMgrClient);
	}


TBool CTStepMgrClient::TestMultiRegisterL(RAuthMgrClient& client)
    {
    CIdentity* result = 0;
    CRegActive active(client, result);
    
    CIdentity* result2 = 0;
    RAuthMgrClient client2;
    User::LeaveIfError(client2.Connect());
    CleanupClosePushL(client2);
    
    CRegActive active2(client2, result2, EFalse);

    active.doReg();		
    active2.doReg();		
    
	CActiveScheduler::Start();

    TEST(active.iErr == KErrNone);
    TEST(active2.iErr == KErrServerBusy);
    
    delete result;
    delete result2;
    CleanupStack::PopAndDestroy(&client2);
	return ETrue;
    
    }

TBool CTStepMgrClient::TestRegister(RAuthMgrClient& client)
  {
    CIdentity* result = 0;
    CRegActive active(client, result);

    active.doReg();		

	CActiveScheduler::Start();
    if (result == 0)
        {
        return EFalse;
        }
        
	INFO_PRINTF3(_L("Id = %d , KeyLength = %d\n"), result->Id(), result->Key().KeyData().Size());
	delete result;
	result = 0;
    return ETrue;
  }

TBool CTStepMgrClient::TestMultiTrainL(RAuthMgrClient& client)
  {
    CTrainActive2 active(client);
    active.doTrain();		

    RAuthMgrClient client2;
    User::LeaveIfError(client2.Connect());
    CleanupClosePushL(client2);
    
    CTrainActive2 active2(client2, EFalse);
    active2.doTrain();		

	CActiveScheduler::Start();
	
	TEST(active.iErr == KErrAuthServPluginCancelled); // blocking plugin returns
	                                                  // cancelled if left to
	                                                  // complete
    TEST(active2.iErr == KErrServerBusy);
    
	CleanupStack::PopAndDestroy(&client2);	
	INFO_PRINTF3(_L("Training Result (%d,%d)\n"),
	     active.iErr,
	     active2.iErr);
	     
	return true;
  }


TBool CTStepMgrClient::TestTrain(RAuthMgrClient& client)
  {
    CTrainActive active(client);
    active.doTrain();		
	CActiveScheduler::Start();
	INFO_PRINTF2(_L("Training Result = %d\n"), active.iResult == EPass);
	return active.iResult == EPass;
  }

TBool CTStepMgrClient::TestCancellation(RAuthMgrClient& client)
  {
    CCancelActive active(client);
    
    active.doTrainCancel();		
	CActiveScheduler::Start();
	TEST(active.iResult == EPass);
	INFO_PRINTF2(_L("Training Cancel Result = %d\n"), active.iResult == EPass);

    active.doTrainCancel();		
	CActiveScheduler::Start();
	TEST(active.iResult == EPass);
	INFO_PRINTF2(_L("Training Cancel Result = %d\n"), active.iResult == EPass);

    active.doTrainCancel();		
	CActiveScheduler::Start();
	TEST(active.iResult == EPass);
	INFO_PRINTF2(_L("Training Cancel Result = %d\n"), active.iResult == EPass);

    //User::After(5000000);
    active.doIdCancel();		
	CActiveScheduler::Start();
	TEST(active.iResult == EPass);
	INFO_PRINTF2(_L("Id Cancel Result = %d\n"), active.iResult == EPass);

    active.doIdCancel();		
	CActiveScheduler::Start();
	TEST(active.iResult == EPass);
	INFO_PRINTF2(_L("Id Cancel Result = %d\n"), active.iResult == EPass);

	active.doRegCancel();		
	CActiveScheduler::Start();
	TEST(active.iResult == EPass);
	INFO_PRINTF2(_L("Registration Cancel Result = %d\n"), active.iResult == EPass);
		
	return active.iResult == EPass;
  }

TVerdict CTStepMgrClient::doTestStepL()
	{
	TInt r = 0;
	
	if (TestStepResult() != EPass)
		{
		INFO_PRINTF1(_L("Previous step failed"));

		return TestStepResult();
		}

		
	WaitForServerToReleaseDb();
	

	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	
	_LIT(KDbSrcFile ,"\\authdb0.db");
	TBuf<64> dbSrcFileName (sysDriveName);
	dbSrcFileName.Append(KDbSrcFile);

	TBuf<128> dbName(KDbName);
	dbName[0] = 'A' + sysDrive;	

	r = BaflUtils::CopyFile(iFs, dbSrcFileName, dbName);
	
	INFO_PRINTF2(_L("File copy returned %d"), r);

	User::LeaveIfError(r);
	

	
	__UHEAP_MARK;
    SetTestStepResult(EPass);
    
	CActiveScheduler::Install(iActSchd);

    
	RAuthMgrClient mgrclient;
	User::LeaveIfError(mgrclient.Connect());
	CleanupClosePushL(mgrclient);
	
   	mgrclient.RemoveIdentityL(2);


	RIdentityIdArray ids;
	mgrclient.IdentitiesL(ids);
	CleanupClosePushL(ids);

	TESTL(ids.Count() == 3);

	// ensure each expected identity occurs once.
	// (This technique is simple but doesn't generalise.)
	TInt sum = 0;
	for (TInt i = 0; i < 3; ++i)
		{
		TIdentityId id = ids[i];
		TESTL(id == 1 || id == 3 || id == 22);
		sum += id;
		}
	TESTL(sum == 1 + 3 + 22);

	CleanupStack::PopAndDestroy(&ids);

	TEST(TestRegister(mgrclient));

	mgrclient.IdentitiesL(ids);
	CleanupClosePushL(ids);
	TEST(ids.Count() == 4);
	CleanupStack::PopAndDestroy(&ids);

	TEST(TestTrain(mgrclient));

	INFO_PRINTF1(_L("Testing cancellation"));
    TEST(TestCancellation(mgrclient));
    
    INFO_PRINTF1(_L("Testing simultaneous registrations are blocked"));
    TEST(TestMultiRegisterL(mgrclient));
    
    INFO_PRINTF1(_L("Testing simultaneous trainings are blocked"));
    TestMultiTrainL(mgrclient);
    
	// Last plugin can't be removed 
	TRAPD(err, mgrclient.ForgetPluginL(22,0x10274104));
	TEST(err == KErrAuthServCanNotRemoveLastPlugin);
	
	CleanupStack::PopAndDestroy(1, &mgrclient);

	__UHEAP_MARKEND;	
	return EPass;
	}
