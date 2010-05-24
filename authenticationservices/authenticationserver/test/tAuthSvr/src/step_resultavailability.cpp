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
#include <s32file.h>
using namespace AuthServer;
class CAuthActive3 : public CActive
    {
    public:
    CAuthActive3(RAuthMgrClient& aClient, CAuthExpression* aExpr,
				 TInt aFresh, TBool aClientSpecific, TBool aStop = ETrue) : 
         CActive(EPriorityNormal),
         iClient(aClient),
		 iResult(EFail),
         iRunCount(0),
         iAe(aExpr),
         iStop(aStop),
         iFresh(aFresh),
         iIdToStore(0), 
         iClientSpecific(aClientSpecific)
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
    void doDeAuth()
        {
        iClient.DeauthenticateL();
        }
	void DoCancel() 
        {
        }
     void RunL() 
        {
        iErr = iStatus.Int();
		switch (iRunCount)
		  {
		  case 0:
            iStatus = KRequestPending;
			iId = 0;

			iClient.AuthenticateL(*iAe, iFresh, iClientSpecific, EFalse, iId, iStatus);
			SetActive();

			break;
		  case 1:
			if (0 != iId)
				{
				iIdToStore = iId->Id();
				delete iId;
				}
			iStatus = KRequestPending;
			TRequestStatus* status;
			status = &iStatus;				
			User::RequestComplete(status, iErr);				
			SetActive();
			
			break;
   		  case 2:
			iResult = iStatus == KErrNone ? EPass : EFail;
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
    TInt iFresh;
    TIdentityId iIdToStore;
    TBool iClientSpecific;
    };

//====================================================================================================


class CRegActive2 : public CActive
    {
    public:
    CRegActive2(RAuthMgrClient& aClient, CIdentity*& aResult) : 
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
            CActiveScheduler::Stop();
			}
        }
        
    TBool iFirstTime;
    RAuthMgrClient& iClient;
    CIdentity*& iResult;
    TInt iErr;
    };
//====================================================================================================     
CResultAvailability::~CResultAvailability()
/**
  Destructor
 */
	{}

CResultAvailability::CResultAvailability(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	
	SetTestStepName(KTResultAvailability);
	}

TVerdict CResultAvailability::doTestStepPreambleL()
/**
  @return - TVerdict code

 */
	{

	CTStepActSch::doTestStepPreambleL();	
	return TestStepResult();
	}


TVerdict CResultAvailability::doTestStepL()
	{
		SetTestStepResult(EFail);	
		__UHEAP_MARK;		// Check for memory leaks

		//-----------------------------------------------------------------------------------------------------	
		//Get the Identity published by KUidAuthServerLastAuth property .
		
		TLastAuth lastAuth;
		TPckg<TLastAuth> authPkg(lastAuth);
		RProperty::Get(KAuthServerSecureId, KUidAuthServerLastAuth, authPkg);
		
		//Get the Last Authenticated Id from the authsvrpolicy.ini file
		
		TPtrC userIdString;
		GetStringFromConfig(ConfigSection(),_L("LastAuthId"), userIdString);
		
		TLex lex = TLex(userIdString);
		TUint userId(0);
		
		lex.Val(userId);
		INFO_PRINTF3(_L("The UserId published by KUidAuthServerLastAuth property is %x and the UserId from AuthSvrPolicy.ini is %x"), lastAuth.iId, userId );
		if(lastAuth.iId == userId )
			{
			SetTestStepResult(EPass);	

			}

		 __UHEAP_MARKEND;
		 return TestStepResult();
	}
TVerdict CResultAvailability::doTestStepPostambleL()
/**
  @return - TVerdict code

 */
	{
	
	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}
//====================================================================================================
CResultChangeNotify::~CResultChangeNotify()
/**
  Destructor
 */
	{}

CResultChangeNotify::CResultChangeNotify(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	
	SetTestStepName(KTResultChangeNotify);
	}

TVerdict CResultChangeNotify::doTestStepPreambleL()
/**
  @return - TVerdict code

 */
	{

	CTStepActSch::doTestStepPreambleL();	
	return TestStepResult();
	}

TVerdict CResultChangeNotify::doTestStepL()
	{
	SetTestStepResult(EPass);	
	__UHEAP_MARK;		// Check for memory leaks
	SetPinPluginStateL();

	//-----------------------------------------------------------------------------------------------------	
	InitAuthServerFromFileL();	// Set things like 'iSupportsDefaultData' and 'DefaultPlugin'
	
	// this method creates the dat file from where the test implementation
	// of pin plugin notifier reads user input.
	// as the pin plugin fails without this data it has been included 
	// in the code as a default step for initializing the pin plugin
	// data.
	TPinValue aPinValue;
	CreatePinPluginInputFileL(EPinPluginTraining,aPinValue);
	CActiveScheduler::Install(iActSchd);
	
	// KUidAuthServerAuthChangeEvent Property retrieves the last authenticated identity
	TInt AuthIdChange = 0;
	RProperty::Get(KAuthServerSecureId, KUidAuthServerAuthChangeEvent, AuthIdChange);
	INFO_PRINTF2(_L("Last Authenticated Identity is %x" ), AuthIdChange );
	
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

	
	//Register a new Identity
	INFO_PRINTF1(_L("Registering a new Identity"));
	CIdentity* identity = 0;
	CRegActive2 regactive(authMgrClient, identity);
	regactive.doReg();
	CActiveScheduler::Start();
	
	if (0 != identity)
	  	{
	  		INFO_PRINTF1(_L("Registration Successful"));
	  		INFO_PRINTF3(_L("Id = %x , KeyLength = %d\n"), identity->Id(), identity->Key().KeyData().Size());
	  	}
			
	

	//Authenticate the new Id.
	
	TPtrC exprString;	
	TInt deAuth;
	if (GetStringFromConfig(ConfigSection(), _L("plugin1Id"), exprString) != EFalse) // the tag 'pluginId1' was present
		{
				 	
			TInt freshnessVal = 0;	//Default value
			//Retrieve the value of 'freshness' from the ini file, but if it's absent use the default value
			 if(GetIntFromConfig(ConfigSection(), _L("freshness"), freshnessVal) == EFalse)
				 {
				 	freshnessVal = 10;
				 }
				 		
			TBool clientSpecificKeyVal = EFalse;	 	
			//Retrieve the value of 'clientSpecificKey' from the ini file, but if it's absent use the default value
			if(GetBoolFromConfig(ConfigSection(), _L("clientSpecificKey"), clientSpecificKeyVal) == EFalse)
				 {
				 	clientSpecificKeyVal = EFalse;
				 }	 	
				
			if(GetIntFromConfig(ConfigSection(), _L("deauth"), deAuth) == EFalse)
				{
					deAuth = 0;
				}
			
			TBool stop;
			if(GetBoolFromConfig(ConfigSection(), _L("stop"), stop ) == EFalse)
							{
								stop = ETrue;
							}
			
			//Create a CResultPropertyWatch object which subscribes to the KUidAuthServerAuthChangeEvent Property
					
			CResultPropertyWatch* watch = CResultPropertyWatch::NewLC();
					
				
			//Train the plugin again with this identity.
			CAuthExpression* expr = CreateAuthExprLC(exprString);
		
			CAuthActive3 authactive(authMgrClient, expr, freshnessVal, clientSpecificKeyVal,stop);
	
			INFO_PRINTF1(_L("Authenticating the new Identity"));
			authactive.doAuth();		
			CActiveScheduler::Start();
		
			//Check if Authentication successful.
			if(authactive.iIdToStore)
				{
					INFO_PRINTF2(_L(" Successful Authentication Id = %x"), authactive.iIdToStore);
				}
			else
				{
					INFO_PRINTF1(_L(" Authentication Failed ."));
				}
			
			if(watch->iNotified)
				{
				TInt AuthIdChange;
					
				//Get the published value
				RProperty::Get(KAuthServerSecureId, KUidAuthServerAuthChangeEvent, AuthIdChange);	
				INFO_PRINTF2(_L("Successfully received Notification. New User Identity is %x " ), AuthIdChange );
				}
			else
				{
				INFO_PRINTF1(_L("No Notification received "));
				}
			CleanupStack::PopAndDestroy(2,watch);
			//Authenticate the same User again to verify that KUidAuthServerAuthChangeEvent 
			//does not get published again
			
			//Create a CResultPropertyWatch object which subscribes to the KUidAuthServerAuthChangeEvent Property
			
			CResultPropertyWatch* watch2 = CResultPropertyWatch::NewLC();	
			
			CAuthExpression* expr2 = CreateAuthExprLC(exprString);
			CAuthActive3 authactive2(authMgrClient, expr, freshnessVal, clientSpecificKeyVal);
			authactive2.doAuth();		
			CActiveScheduler::Start();
		
			//Check if Authentication successful.
			if(authactive.iIdToStore)
				{
					INFO_PRINTF2(_L(" Successful Authentication Id = %x"), authactive.iIdToStore);
				}
			else
				{
					INFO_PRINTF1(_L(" Authentication Failed ."));
					
				}
						
			if(watch2->iNotified)
				{
					TInt AuthIdChange;	
					//Get the published value
					RProperty::Get(KAuthServerSecureId, KUidAuthServerAuthChangeEvent, AuthIdChange);	
					INFO_PRINTF2(_L("Successfully received Notification. New User Identity is %x " ), AuthIdChange );
					SetTestStepResult(EFail);
				}
			else
				{
					INFO_PRINTF1(_L("No Notification received "));
				}
			CleanupStack::PopAndDestroy(2,watch2);
			// If deAuth flag is set ,subscribe to the KUidAuthServerAuthChangeEvent property
			// and deauthenticate 
			if(deAuth)
			{
								
				INFO_PRINTF1(_L(" Trying to DeAuthenticate "));
				//Creating a subscriber active object
				CResultPropertyWatch* watch3 = CResultPropertyWatch::NewLC();
		
				TInt err;
				TRAP(err,authactive.doDeAuth());
				CActiveScheduler::Start();
		
				if(err != KErrNone)
					{
					INFO_PRINTF1(_L(" DeAuthentication Failed "));
					SetTestStepResult(EFail);

					}
				else
					{
					INFO_PRINTF1(_L(" DeAuthentication Succeeded "));
					}
				
				if(watch3->iNotified)
					{
					TInt AuthIdChange;	
					//Get the published value
					RProperty::Get(KAuthServerSecureId, KUidAuthServerAuthChangeEvent, AuthIdChange);	
					INFO_PRINTF2(_L("Successfully received Notification. New User Identity is %x " ), AuthIdChange );
					}
				else
					{
					INFO_PRINTF1(_L(" No Notification of the change in the Authenticated Identity "));
					SetTestStepResult(EFail);
					}
				CleanupStack::PopAndDestroy(watch3);
				INFO_PRINTF1(_L(" Checking the Id published by KUidAuthServerLastAuth upon DeAuthentication"));
				TLastAuth lastAuth;
				TPckg<TLastAuth> authPkg(lastAuth);
				RProperty::Get(KAuthServerSecureId, KUidAuthServerLastAuth, authPkg);
		
				INFO_PRINTF2(_L("Id published by KUidAuthServerLastAuth = %x"), lastAuth.iId);
		
				if(lastAuth.iId != 0)
					{
					SetTestStepResult(EFail);
					}
			
				}
			}
			
		RemovePinPluginFileL();	
		delete identity;
		CleanupStack::PopAndDestroy(&authMgrClient);
		 __UHEAP_MARKEND;
		 return TestStepResult();
	}

	
void CResultChangeNotify::SetPinPluginStateL()
	{
	TPtrC activeStateFromFile;
	
	_LIT(KPinPlugin,"pinplugin_inactive.txt");
	
	TFileName filename;
	filename.Copy(KPinPlugin); // convert from 8 -> 16 bit descriptor
		
	TDriveUnit sysDrive = RFs::GetSystemDrive();
	TDriveName sysDriveName (sysDrive.Name());
	filename.Insert(0,sysDriveName);
	filename.Insert(2,_L("\\"));		
	
	RFileWriteStream stream;
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	fs.Delete(filename);
		
	if (GetStringFromConfig(ConfigSection(),_L("Pin200032E5Active"), activeStateFromFile)) 
		{
		if(activeStateFromFile == _L("false"))
			{		
			User::LeaveIfError(stream.Create(fs, filename, EFileWrite | EFileShareExclusive));
			stream.Close();
			}
		}
	CleanupStack::PopAndDestroy(); // fs
	}

void CResultChangeNotify::RemovePinPluginFileL()
	{
	TPtrC activeStateFromFile;
	_LIT(KPinPlugin,"pinplugin_inactive.txt");
	TFileName filename;
	filename.Copy(KPinPlugin); // convert from 8 -> 16 bit descriptor
			
	TDriveUnit sysDrive = RFs::GetSystemDrive();
	TDriveName sysDriveName (sysDrive.Name());
	filename.Insert(0,sysDriveName);
	filename.Insert(2,_L("\\"));
		
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	fs.Delete(filename);
	CleanupStack::PopAndDestroy(); // fs
	}


TVerdict CResultChangeNotify::doTestStepPostambleL()
/**
  @return - TVerdict code

 */
	{
	
	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}

//====================================================================================================     


CResultPropertyWatch* CResultPropertyWatch::NewLC()
{
	CResultPropertyWatch* me=new(ELeave) CResultPropertyWatch;
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
}

CResultPropertyWatch::CResultPropertyWatch() :CActive(EPriority)
	{
		iNotified = EFalse;
	}


void CResultPropertyWatch::ConstructL()
{
	User::LeaveIfError(iProperty.Attach(KAuthServerSecureId,
			 KUidAuthServerAuthChangeEvent));
	CActiveScheduler::Add(this);
	// Subscribe to the KUidAuthServerAuthChangeEvent property
	iProperty.Subscribe(iStatus);
	SetActive();
	
	}
CResultPropertyWatch::~CResultPropertyWatch()
{
	Cancel();
	iProperty.Close();

}
void CResultPropertyWatch::DoCancel()
{
	iProperty.Cancel();
}
void CResultPropertyWatch::RunL()
{	
	TInt AuthIdChange;
	
	//Get the published value
	iProperty.Get(KAuthServerSecureId, KUidAuthServerAuthChangeEvent, AuthIdChange);
	iNotified = ETrue;
	RDebug::Print(_L("Received Notification. New User Identity is %x " ), AuthIdChange );
	CActiveScheduler::Stop();
		
}


