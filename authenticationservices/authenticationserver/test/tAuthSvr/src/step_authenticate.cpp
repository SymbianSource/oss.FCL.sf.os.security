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
#include "authrepository.h"
#include "testutilclient.h"

using namespace AuthServer;


class CAuthActive2 : public CActive
    {
    public:
    CAuthActive2(RAuthMgrClient& aClient, CAuthExpression* aExpression,
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
    	iErr = aError;
	   	CActiveScheduler::Stop();
    	return KErrNone;
    	}
    	
    void RunL() 
        {
        iErr = iStatus.Int();
       
		switch (iRunCount)
		  {
		  case 0:
		  	{
		  	iId = 0;
			TUid clientSid = {0};
			iClient.AuthenticateL(*iAe, iFresh, iClientSpecific, clientSid, EFalse, iClientMessage, iId, iStatus);
		
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

	~CAuthActive2()
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
    
//====================================================================================================    
CTAuthenticate::CTAuthenticate(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTAuthenticate);
	}

CTAuthenticate::~CTAuthenticate()
/**
  Destructor
 */
	{}
	
TVerdict CTAuthenticate::doTestStepPreambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();

	return TestStepResult();
	}

TVerdict CTAuthenticate::doTestStepL()
/**
  @return - TVerdict code
  Override of base class pure virtual
  
 */
	{	
	SetTestStepResult(EFail);

__UHEAP_MARK;		// Check for memory leaks
	 	
	//-----------------------------------------------------------------------------------------------------
	TPtrC displayMessage;
	TBool messageSent = (GetStringFromConfig(ConfigSection(),_L("DisplayMessage"), displayMessage) != EFalse);
	TBuf<100> message(displayMessage);
	ClientMessage(messageSent, displayMessage);
	if(!messageSent)
		{
		message = KNullDesC();
		}
	InitAuthServerFromFileL();	// Set things like 'iSupportsDefaultData' and 'DefaultPlugin'
	
	// this method creates the dat file from where the test implementation
	// of pin plugin notifier reads user input.
	// as the pin plugin fails without this data it has been included 
	// in the code as a default step for initializing the pin plugin
	// data.
	// the pin value generate through the reference plugin is a random 
	// value which should be retrieved from the output file and given 
	// as an input back for creation of the input file again.
	// If the ini file specified a pin use that instead
	TPtrC pin;
	if (GetStringFromConfig(ConfigSection(),_L("pinvalue"), pin))
		{
		TPinValue pinValue;
		pinValue.Copy(pin);
		//pinValue.
		CreatePinPluginInputFileL(EPinPluginIdentify, pinValue);
		}
	else
		{
		TPinValue pinValue = GetPinFromOutputFileL();
		CreatePinPluginInputFileL(EPinPluginIdentify, pinValue);
		}

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
	// Authenticate by reading the user input
	
	//Check that alias or pluginId is quoted in the ini file
	TPtrC exprString;
	TBool authViaAlias = GetStringFromConfig(ConfigSection(),_L("alias"), exprString);
	TBool authViaExpr = EFalse;
	if(!authViaAlias)
		{
		authViaExpr = GetStringFromConfig(ConfigSection(),_L("plugin1Id"), exprString);
		}
	
	if(	! authViaAlias && ! authViaExpr)
		{
		INFO_PRINTF1(_L("Neither plugin1Id nor alias was specified in the ini file"));	
		}
	else
		{
	  	TInt freshnessVal = 0;	//Default value
	 	//Retrieve the value of 'freshness' from the ini file, but if it's absent use the default value
	 	if(GetIntFromConfig(ConfigSection(),_L("freshness"), freshnessVal) == EFalse)
	 		{
	 		freshnessVal = 10;
	 		}
	 		
	 	TBool clientSpecificKeyVal = EFalse;	 	
	 	//Retrieve the value of 'clientSpecificKey' from the ini file, but if it's absent use the default value
	 	if(GetBoolFromConfig(ConfigSection(),_L("clientSpecificKey"), clientSpecificKeyVal) == EFalse)
	 		{
	 		clientSpecificKeyVal = EFalse;
	 		}
	 	
	 	TBool defaultpluginusage = EFalse;
	 	if(GetBoolFromConfig(ConfigSection(),_L("defaultpluginusage"), defaultpluginusage ) == EFalse)
	 		 {
	 		defaultpluginusage  = EFalse;
	 		 }
	 		
	 	TBool isSynchronous = EFalse;
	 	//Retrieve the value of 'synchronous' from the ini file, but if it's absent use the default value
	 	if(GetBoolFromConfig(ConfigSection(),_L("synchronous"), isSynchronous) != EFalse)
	 		{
	 		CallSyncAuthenticateL(authMgrClient1, exprString, freshnessVal, clientSpecificKeyVal, authViaAlias, defaultpluginusage, message);
	 		}
	 	else
	 		{
	 		CallAsyncAuthenticateL(authMgrClient1, exprString, freshnessVal, clientSpecificKeyVal, authViaAlias, defaultpluginusage, message);
	 		}

	  	}

	CleanupStack::PopAndDestroy(&authMgrClient1);	// authClient1	
	//Garbage collect the last previously destroyed implementation 
	// and close the REComSession if no longer in use
	REComSession::FinalClose(); 
__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTAuthenticate::doTestStepPostambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}


void CTAuthenticate::setLastAuthIdL(AuthServer::TIdentityId idToStore)
/**
  Utility function that stores the most recently authenticated identity in the GlobalDb file
  This should only be called by the Authenticate test step  class
 */
	{	
	TBuf<100> bufToWrite;
	_LIT16(KFormatLastAuth,"%u");
	
	bufToWrite.Format(KFormatLastAuth, idToStore); //generates:
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> authSvrPolicyFile (sysDrive.Name());
	authSvrPolicyFile.Append(KAuthSvrPolicyFile);

	CIniData* policyFile=NULL;
	TRAP_IGNORE(policyFile=CIniData::NewL(authSvrPolicyFile));
	CleanupStack::PushL(policyFile);

	TInt ret = writeToNamedConfig(policyFile, KLastAuthIdTag, bufToWrite);

	policyFile->WriteToFileL();
	CleanupStack::PopAndDestroy(policyFile);

	if (KErrNotFound == ret)
		{
		ERR_PRINTF3(_L("Error while setting last AuthId. Tag %S not found in %S file"), KLastAuthIdTag, &authSvrPolicyFile);
		}
	else
		{
		INFO_PRINTF2(_L("Setting last AuthId was successful. Id = %S"), &bufToWrite);
		}
	}
	
void CTAuthenticate::CallSyncAuthenticateL(RAuthMgrClient& aAuthMgrClient, const TDesC& aInputString, TInt aFreshness, TBool aClientSpecificKey, TBool aIsAlias, TBool aIsDefault, const TDesC& aClientMessage )
	{
	CIdentity* id = 0;
	TInt error;
	TUid val = TUid::Uid(0);
	
	CAuthExpression* expr = NULL;
	
	if(!aIsAlias)
		{
		if(!aIsDefault)
			{
			CAuthExpression* expr = CreateAuthExprLC(aInputString);			
			TRAP(error, id = aAuthMgrClient.AuthenticateL(*expr, aFreshness, aClientSpecificKey, val, EFalse, aClientMessage));
			CleanupStack::PopAndDestroy(expr);
			}
		else
			{
			CAuthExpression* expr = CreateAuthExprLC(KNullDesC());			
			TRAP(error, id = aAuthMgrClient.AuthenticateL(*expr, aFreshness, aClientSpecificKey, val, EFalse, aClientMessage));
			CleanupStack::PopAndDestroy(expr);
			}
		}
	
	else
		{
		TRAPD(err,expr = aAuthMgrClient.CreateAuthExpressionL(aInputString));
			if(err != KErrNone)
				{
				SetTestStepError(err);
				return;
				}
				
		CleanupStack::PushL(expr);
		TUid clientSid = {0};
		TRAP(error, id = aAuthMgrClient.AuthenticateL(*expr, aFreshness, aClientSpecificKey, clientSid, EFalse, KNullDesC));
		CleanupStack::PopAndDestroy(expr);
		}
		
	if(error != KErrNone)
	 	{
	 	ERR_PRINTF2(_L("Authentication Result error = %d"), error);	
	 	SetTestStepError(error);
	 	return;	
		}
	 	
	//Main check
	if (0 == id->Id())
		{
		ERR_PRINTF1(_L("Authentication failed. No identity returned"));
		delete id;
		}
	
	else
		{
		INFO_PRINTF1(_L("Authentication successful."));
		CleanupStack::PushL(id);
		//Store the id in the Global Db file		
		setLastAuthIdL(id->Id());
		CleanupStack::PopAndDestroy(id);
		SetTestStepResult(EPass);
		}
	}
	
void CTAuthenticate::CallAsyncAuthenticateL(RAuthMgrClient& aAuthMgrClient, TDesC& aInputString, TInt aFreshness, TBool aClientSpecificKey, TBool aIsAlias, TBool aIsDefault, const TDesC& aClientMessage)
	{
	CAuthExpression* expr = 0;
	if(!aIsAlias)
		{
		if(!aIsDefault)
			{	
			expr = CreateAuthExprLC(aInputString);
			}
		else
			{
			expr = CreateAuthExprLC(KNullDesC());
			}
		}
	else
		{
		TRAPD(err,expr = aAuthMgrClient.CreateAuthExpressionL(aInputString));
		if(err != KErrNone)
			{
			SetTestStepError(err);
			return;
			}
		CleanupStack::PushL(expr);
		}
	
	CAuthActive2 active1(aAuthMgrClient, expr, aFreshness, aClientSpecificKey, aIsAlias, aClientMessage);
	active1.doAuth();		
	CActiveScheduler::Start();
		
	if(expr)
		{
		CleanupStack::PopAndDestroy(expr);
		}
	
	 		
	if(EPass != active1.iResult)
		{
		ERR_PRINTF3(_L("Authentication Result error = %d, iResult = %d\n"), active1.iErr, active1.iResult);			
		SetTestStepError(active1.iErr);
		}
	
	else if ((KErrNone == active1.iErr) && (EPass == active1.iResult))
		{
		INFO_PRINTF1(_L("No Authentication Errors."));
		}
		
	else
		{
		ERR_PRINTF3(_L("Unexpected Authentication Result. error = %d, iResult = %d\n"), active1.iErr, active1.iResult);
		}
		
	//Main check
	if (0 == active1.iIdToStore)
		{
		ERR_PRINTF1(_L("Authentication failed. No identity returned"));
		}
	
	else
		{
		INFO_PRINTF1(_L("Authentication successful."));
		//Store the id in the Global Db file		
		setLastAuthIdL(active1.iIdToStore);
		SetTestStepResult(EPass);
		}	
	}
	
//---------------------------------------------------------------------------------------------------------------------------------

CTInitCenRep::CTInitCenRep(CTAuthSvrServer& aParent): iParent(aParent)
/**
 Constructor
*/
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTInitCenRep);
	}


 CTInitCenRep::~CTInitCenRep()
/**
 Destructor
*/
	{}
 	
TVerdict CTInitCenRep::doTestStepPreambleL()
/**
 @return - TVerdict code
 Override of base class virtual
*/
	{
	//Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();
	return TestStepResult();
	}

TVerdict CTInitCenRep::doTestStepL()
/**
 @return - TVerdict code
 Override of base class pure virtual
  
 */
	{	

 __UHEAP_MARK;		// Check for memory leaks

	//Initialize the repository
	CRepository* repository = CRepository::NewL(KUidAuthServerRepository);
	CleanupStack::PushL(repository);
	
	//If default plugin is specified, set it
	TPtrC defaultPluginFromFile;
	if(GetStringFromConfig(ConfigSection(),_L("default"), defaultPluginFromFile) != EFalse)
		{
		TLex pluginIdString = TLex(defaultPluginFromFile);
		TPluginId defaultPluginId(0);
		
		pluginIdString.Val(defaultPluginId, EHex );
		SetDefaultPluginIdL(defaultPluginId);
	 		
		//Retrieve the value
		TInt value(0);
		User::LeaveIfError(repository->Get(KAuthDefaultPlugin, value));
		CleanupStack::PopAndDestroy(repository);
		SetTestStepResult(EPass);
		return TestStepResult();
		}
	TInt aliasCount = 0;
	
	// Retrieve the value of 'aliasCount' from the ini file, but if it's absent use the default value.
	// This value is used for validation of cenrep file.
	if(GetIntFromConfig(ConfigSection(),_L("aliasCount"), aliasCount) != EFalse)
		{
		User::LeaveIfError(repository->Set(EAuthAliasesCount, aliasCount));
		CleanupStack::PopAndDestroy(repository);
		SetTestStepResult(EPass);
		return TestStepResult();
		}
	
	User::LeaveIfError(repository->Get(EAuthAliasesCount, aliasCount));
	// since we will only have four aliases in the cenrep file.
	__ASSERT_DEBUG(aliasCount == 4, User::Leave(KErrArgument));
	
	// Retrieve the value of 'unicode' from the ini file, but if it's absent use the default value.
	// For unicode tests we need to set the aliases in addition to thier corresponding alias string.
	TBool isUnicode = EFalse;
	if(GetBoolFromConfig(ConfigSection(),_L("unicode"), isUnicode) != EFalse)
		{
		TBuf<24> alias;
		TPtrC aliasValue;
	
		for(TInt i = 0; i < aliasCount; ++i)
			{
			alias.Format(_L("alias%d"), i);
			if (GetStringFromConfig(ConfigSection(), alias, aliasValue) != EFalse)
				{
				User::LeaveIfError(repository->Set(EAuthAliases+i, aliasValue));
				}
			}
		}
	
	// for each alias ,set the corresponding alias string expression.
	TBuf<24> aliasString;
	TPtrC aliasStringValue;
	
	for(TInt i = 0; i < aliasCount; ++i)
		{
		aliasString.Format(_L("aliasString%d"), i);
		if (GetStringFromConfig(ConfigSection(), aliasString, aliasStringValue) != EFalse)
			{
			User::LeaveIfError(repository->Set(EAuthAliasesExpr+i, aliasStringValue));
			}
		}
	
	CleanupStack::PopAndDestroy(repository);
	SetTestStepResult(EPass);
	
__UHEAP_MARKEND;
	return TestStepResult();
	}
	
TVerdict CTInitCenRep::doTestStepPostambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
 	//Call the parent postamble, releasing the file handle, etc
 	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}

//====================================================================================================

CTSysTimeUpdate::CTSysTimeUpdate(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTUpdateSysTime);
	}


CTSysTimeUpdate::~CTSysTimeUpdate()
/**
  Destructor
 */
	{}
	
TVerdict CTSysTimeUpdate::doTestStepPreambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();

	return TestStepResult();
	}

TVerdict CTSysTimeUpdate::doTestStepL()
/**
  @return - TVerdict code
  Override of base class pure virtual
  
 */
	{	
	SetTestStepResult(EFail);

__UHEAP_MARK;		// Check for memory leaks
	
	//Get the "time" which must be added to the system time.
	TInt timeVal(0);
	GetIntFromConfig(ConfigSection(), _L("time"), timeVal);

	TTime currentTime;
	currentTime.UniversalTime();
	
	if(timeVal == 0 )
		{
		//Set the secure clock time.
		RTestUtilSession session;
		session.Connect();
		CleanupClosePushL(session);
		TInt ret = session.SetSecureClock(0);
		INFO_PRINTF2(_L("Set the secure clock --- %d"), ret);
		CleanupStack::PopAndDestroy(&session);
		}
	else
		{
		//Increment the time only if secure clock is present
		TInt err = currentTime.UniversalTimeSecure();
		INFO_PRINTF2(_L("Getting the secure clock time returned %d"), err );
		if(!err)
			{
			INFO_PRINTF1(_L("Secure clock found !"));
			TTimeIntervalSeconds increment(timeVal);
			currentTime += increment;
			User::LeaveIfError(User::SetUTCTime(currentTime));
			}
		else
			{
			INFO_PRINTF1(_L("Secure clock not found !"));
			}
	
		}


	SetTestStepResult(EPass);

__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTSysTimeUpdate::doTestStepPostambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}
