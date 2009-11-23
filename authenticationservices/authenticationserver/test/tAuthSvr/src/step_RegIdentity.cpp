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
#include <s32file.h>
class CAuthActive2 : public CActive
    {
    public:
    CAuthActive2(RAuthMgrClient& aClient, CAuthExpression* aExpr,
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


class CRegActive : public CActive
    {
    public:
    CRegActive(RAuthMgrClient& aClient, HBufC* aIdentityString, CIdentity*& aResult) : 
         CActive(EPriorityNormal),
         iFirstTime(true),
         iClient(aClient), 
         iResult(aResult),
         iIdentityString(aIdentityString),
         iErr(KErrNone)
        {
        CActiveScheduler::Add(this);
        }
    ~CRegActive()
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
            CActiveScheduler::Stop();
			}
        }
        
    TBool iFirstTime;
    RAuthMgrClient& iClient;
    CIdentity*& iResult;
    HBufC* iIdentityString;
    TInt iErr;
    };

//====================================================================================================

CTRegIdentity::~CTRegIdentity()
/**
  Destructor
 */
	{}

CTRegIdentity::CTRegIdentity(CTAuthSvrServer& aParent): iParent(aParent)
/**
  Constructor
 */
	{
	
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTAuthSvrCheck);
	}

TVerdict CTRegIdentity::doTestStepPreambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();	
	return TestStepResult();
	}

TVerdict CTRegIdentity::doTestStepL()
/**
  @return - TVerdict code
  Override of base class pure virtual
  
 */
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
		RemovePinPluginFileL();
		User::LeaveIfError(connectVal);
		}	
	CleanupClosePushL(authMgrClient1);

	//Examine the authserver and see what's there
	ListPluginsL(authMgrClient1);
	
	TBool statusAll = EFalse;
	statusAll = CheckPluginStatusAllL(authMgrClient1);
	
	//Check the username of the second identity and then attempt to set it.
//	SetTestStepResult(checkAndSetUserNameL(authMgrClient1, 0));	
	
	// Get the identity string (if defined)
	TPtrC identityString;
	if (!GetStringFromConfig(ConfigSection(), _L("IdentityString"), identityString))
		{
		identityString.Set(_L("SOMENAME"));
		}
	
	//Register an identity using an active object
	CIdentity* identity1 = 0;
    CRegActive* active = new (ELeave) CRegActive(authMgrClient1, identityString.AllocL(), identity1); // Ownership transferred
    active->doReg();
	CActiveScheduler::Start();
	TInt err = active->iErr;
	delete active;

	SetTestStepError(err);

	if(KErrAuthServPluginQuit == err)
		{
		INFO_PRINTF1(_L("Training was Quit."));
		INFO_PRINTF1(_L("User entered the Quit code as trainingInput."));		
		}
	else if (KErrAuthServPluginCancelled == err)
		{
		INFO_PRINTF1(_L("Training was cancelled."));
		INFO_PRINTF1(_L("User entered trainingInput same as identifyingInput or an existing PIN."));
		INFO_PRINTF1(_L("Or the Cancel code."));		
		}
    else if ((identity1 == 0) && (KErrAuthServPluginQuit != err))
        {
        ERR_PRINTF1(_L("An unexpected error occurred during the registration process."));
        SetTestStepResult(EFail);
        }
    else
  		{
  		if (0 != identity1)
  			{
  			INFO_PRINTF3(_L("Id = %x , KeyLength = %d\n"), identity1->Id(), identity1->Key().KeyData().Size());
  			}	
		//Get a list of all the present identities
		RIdentityIdArray ids;
		authMgrClient1.IdentitiesL(ids);
		CleanupClosePushL(ids);
		TInt actualNumIds = ids.Count();
				
		//If specified in the ini file, check the number of identities now present
		TInt numIdsValue = 0;
		if (GetIntFromConfig(ConfigSection(),_L("NumIdentities"), numIdsValue) != EFalse) // the tag 'numIds' was present
			{
			if (actualNumIds != numIdsValue)
				{
				ERR_PRINTF3(_L("Error. ActualNumIds = %d, numIdsExpected = %d"), actualNumIds, numIdsValue);
				SetTestStepResult(EFail);
				}				
			}
		//List the present identities by their strings
		for (TInt i = 0; i < actualNumIds; i++)
			{
			HBufC* idName = 0;			
			idName = authMgrClient1.IdentityStringL(ids[i]);
			INFO_PRINTF3(_L("Id %d has the name %S"), i, idName);
			delete idName; 
			}		
		CleanupStack::PopAndDestroy(&ids);
		delete identity1;				
	
  		}//End check for when identity1 == 0, i.e 'RegisterIdentity() fails

	//Examine the authserver and see what's there
	ListPluginsL(authMgrClient1);
	
	statusAll = CheckPluginStatusAllL(authMgrClient1);
	if (!statusAll)
		{
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&authMgrClient1);	// authClient1	
	
	RemovePinPluginFileL();
	
	//Garbage collect the last previously destroyed implementation 
	// and close the REComSession if no longer in use
	REComSession::FinalClose(); 	
__UHEAP_MARKEND;
	return TestStepResult();	
	}

void CTRegIdentity::ListPluginsL(AuthServer::RAuthMgrClient& aAuthMgrClient )
	{
	RPluginDescriptions pluginList1;
	TCleanupItem cleanup(CleanupEComArray, &pluginList1);
	CleanupStack::PushL(cleanup);					
	aAuthMgrClient.PluginsL(pluginList1);	
	TInt numTotalPlugins = pluginList1.Count();
	TInt i;
	for(i = 0; i < numTotalPlugins; i++)
		{
		TInt presentPluginIdVal = pluginList1[i]->Id();
		TInt presentPluginStatus = pluginList1[i]->TrainingStatus();
		INFO_PRINTF3(_L("PluginId=%x,status =%i"), presentPluginIdVal,presentPluginStatus);
		}
	CleanupStack::PopAndDestroy(&pluginList1); //infoArray, results in a call to CleanupEComArray	
	
	}

void CTRegIdentity::SetPinPluginStateL()
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

void CTRegIdentity::RemovePinPluginFileL()
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

TVerdict CTRegIdentity::doTestStepPostambleL()
/**
  @return - TVerdict code
  Override of base class virtual
 */
	{
	//Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}
