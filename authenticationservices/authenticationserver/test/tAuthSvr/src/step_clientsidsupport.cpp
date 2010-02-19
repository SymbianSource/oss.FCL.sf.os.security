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
* Description: 
*
*/


#include <bautils.h>
#include <s32file.h>
#include <test/testexecutelog.h>

#include "authserver/authmgrclient.h"
#include "authserver/authclient.h"
#include "authserver/identity.h"
#include "tAuthSvrStep.h"

using namespace AuthServer;

class CActiveObj : public CActive
    {
    public:
    CActiveObj(RAuthMgrClient& aClient, CAuthExpression* aExpression,
				 TInt aFresh, TBool aClientSpecific, TUid aClientSid, TBool aIdentityString, TPluginId aPluginId, TIdentityId aId) : 
         CActive(EPriorityNormal),
         iClient(aClient),
		 iResult(EFail),
         iRunCount(0),
         iAe(aExpression),
         iFresh(aFresh),
         iClientSpecific(aClientSpecific),
         iWithString(aIdentityString),
         iPluginId(aPluginId),
         iId(aId),
         iMode(-1)
        {
        CActiveScheduler::Add(this);
        iClientSid = aClientSid;
        }
   
    void Authenticate() 
        {
        SetActive();		
		TRequestStatus* status = &iStatus;		    
		User::RequestComplete(status, KErrNone);
		iRunCount = 0;
		iMode = 0;
        }
        
   void Retrain() 
        {
        SetActive();		
		TRequestStatus* status = &iStatus;		    
		User::RequestComplete(status, KErrNone);
		iRunCount = 0;
		iMode = 1;
        }
        
    void DoCancel() 
        {
        }
        
    TInt RunError(TInt aError)
    	{
    	iErr = aError;
	   	CActiveScheduler::Stop();
	   	return KErrNone;
    	}
        
    void RunL() 
        {
        switch (iRunCount)
		  {
		  case 0:
		  
		  switch (iMode)
			  {
			  
			  case 0:
				iClient.AuthenticateL(*iAe, iFresh, iClientSpecific, iClientSid, EFalse, KNullDesC(), iIdentity, iStatus);
			    break;
			    
			  case 1:
				iClient.TrainPlugin(iId, iPluginId, iStatus);
				break;
			  }
			  
			SetActive();
			break;
			
		  case 1:
   		  
   		  	iResult = iStatus.Int() == KErrNone ? EPass : EFail;
            iErr = iStatus.Int();
			CActiveScheduler::Stop();
			break;
			
     	  default:
			iResult = EFail;
			CActiveScheduler::Stop();
			 
		    }
		  ++iRunCount;
          }
          
    ~CActiveObj()
		{
		
		}
		
    RAuthMgrClient& iClient;
	TVerdict iResult;
	TInt iRunCount;
	CAuthExpression* iAe;
	CIdentity* iIdentity;
    TInt iErr;
    TInt iFresh;
    TBool iClientSpecific;
    TBool iWithString;
    TPluginId iPluginId;
    TIdentityId iId;
    TInt iMode;
    TUid iClientSid;
    };
    
 
 CTStepClientSidSupport::CTStepClientSidSupport(CTAuthSvrServer& aParent): iParent(aParent)
 	{
 	SetTestStepName(KTStepClientSidSupportCheck);
 	}
 	
 TVerdict CTStepClientSidSupport::doTestStepPreambleL()
 	{
 	SetTestStepResult(EFail);
 	
 	CTStepActSch::doTestStepPreambleL();
 	 	
 	if(GetHexFromConfig(ConfigSection(),_L("plugin1Id"), iPluginId) == EFalse)
		{
		INFO_PRINTF1(_L("Plugin Id not specified in the ini file"));
		return TestStepResult();
		}

	if(GetBoolFromConfig(ConfigSection(),_L("clientSpecificKey"), iClientSpecificKey) == EFalse)
	 	{
	 	INFO_PRINTF1(_L("Client Specific Key not specified in the ini file"));
		return TestStepResult();
	 	} 
		 	
	if(GetBoolFromConfig(ConfigSection(),_L("withString"), iWithString) == EFalse)
	 	{
	 	INFO_PRINTF1(_L("Identity String not specified in the ini file"));
		return TestStepResult();
	 	}
	 	
	TInt clientSid = 0;
   	if(GetHexFromConfig(ConfigSection(),_L("clientSid"), clientSid) == EFalse)
		{
		INFO_PRINTF1(_L("Client Sid not specified in the ini file"));
		return TestStepResult();
		}
	
	iClientSid.iUid = clientSid;
 	
	 	
	SetTestStepResult(EPass);
	return TestStepResult();
 	}
  
 TVerdict CTStepClientSidSupport::doTestStepL()
	{
    SetTestStepResult(EFail);
    
    InitAuthServerFromFileL();	// Set things like 'iSupportsDefaultData' and 'DefaultPlugin'
	
	CActiveScheduler::Install(iActSchd);
    
	//Connect to the AuthServer	
	AuthServer::RAuthMgrClient authMgrClient1;	
	TInt connectVal = authMgrClient1.Connect();
	CleanupClosePushL(authMgrClient1);
	
    TPtrC exprString;
	if(GetStringFromConfig(ConfigSection(),_L("plugin1Id"), exprString) == EFalse)
		{
		INFO_PRINTF1(_L("Plugin Id not specified in the ini file"));
		return TestStepResult();
		}
		
	CAuthExpression* authExpr = CreateAuthExprLC(exprString);
	CActiveObj active1(authMgrClient1, authExpr, 20, iClientSpecificKey, iClientSid, iWithString, iPluginId, 0);
	active1.Authenticate();
	CActiveScheduler::Start();
	
	if(EPass != active1.iResult)
		{
		ERR_PRINTF3(_L("Authentication Result error = %d, iResult = %d\n"), active1.iErr, active1.iResult);			
		SetTestStepError(active1.iErr);
		CleanupStack::PopAndDestroy(2, &authMgrClient1);
		return TestStepResult();
		}
	
	iIdentity = active1.iIdentity;
	CleanupStack::PushL(iIdentity);
	TIdentityId identityId = iIdentity->Id();
	
	// set plugin input train value.
	InitPluginDataFromFileL(ETrue);
	
	CActiveObj active2(authMgrClient1, authExpr, 0, iClientSpecificKey, iClientSid, iWithString, iPluginId, identityId);
	active2.Retrain();		
	CActiveScheduler::Start();
	
	if(EPass != active1.iResult)
		{
		ERR_PRINTF3(_L("Error encountered while training plugin = %d, iResult = %d\n"), active2.iErr, active2.iResult);			
		SetTestStepError(active2.iErr);
		CleanupStack::PopAndDestroy(3, &authMgrClient1);
		return TestStepResult();
		}
	
	// set plugin inputId value.
	InitPluginDataFromFileL(EFalse);
	
	CActiveObj active3(authMgrClient1, authExpr, 20, iClientSpecificKey, iClientSid, iWithString, iPluginId, 0);
	active3.Authenticate();		
	CActiveScheduler::Start();
	
	CleanupStack::Pop(iIdentity);	
	CleanupStack::PopAndDestroy(2, &authMgrClient1);
	
	if(EPass != active1.iResult)
		{
		ERR_PRINTF3(_L("Authentication Result error = %d, iResult = %d\n"), active3.iErr, active3.iResult);			
		SetTestStepError(active3.iErr);
		delete iIdentity;
		return TestStepResult();
		}

	iIdentityAfterRetrain = active3.iIdentity;
	
	TBool success = VerifyObtainedResultsWithExpectedL();
	if(success)
		{
		SetTestStepResult(EPass);
		}
	
	return TestStepResult();
    };
    
 TBool CTStepClientSidSupport::VerifyObtainedResultsWithExpectedL()
 	{
 	// required for ReadUserData capability test(when client sid is not equal
 	// to the calling application's sid or zero, ReadUserData is required for
 	// authentication.)
 	if(iClientSid.iUid == 0)
 		{
 		TInt clientSid;
 		if(GetHexFromConfig(ConfigSection(),_L("callingAppSid"), clientSid) == EFalse)
			{
			INFO_PRINTF1(_L("Sid of calling application not specified in the ini file"));
			return TestStepResult();
			}
			
		iClientSid.iUid = clientSid;
 		}
 		
 	CProtectionKey* protection = iIdentity->Key().ClientKeyL(iClientSid.iUid);
 	 	
 	const CProtectionKey& obtainedKey = iIdentityAfterRetrain->Key();
 	TBool success = EFalse;
 	if(protection->KeyData() == obtainedKey.KeyData())
 		{
 		success = ETrue;
 		}
 	
 	return success;
 	}
 	
 CTStepClientSidSupport::~CTStepClientSidSupport()
 	{
 	delete iIdentity;
 	delete iIdentityAfterRetrain;
 	}
