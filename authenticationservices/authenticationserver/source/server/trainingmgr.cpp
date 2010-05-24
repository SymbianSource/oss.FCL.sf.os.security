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
* CTrainingMgr - Auth Server helper class
*
*/


/**
 @file 
*/
#include <e32debug.h>
#include <s32mem.h>
#include "authserver_impl.h"
#include "log.h"
#include "authrepository.h"

using namespace AuthServer;


CTrainingMgr::CTrainingMgr(CPluginMgr&      aPluginMgr,
						   CAuthDb2&        aAuthDb,
						   CAuthRepository& aAuthRepository) :
	CActive(EPriorityStandard),
	iPluginMgr(&aPluginMgr),
	iAuthDb(&aAuthDb),
	iAuthRepository(aAuthRepository)
	{
	CActiveScheduler::Add(this);
	}

CTrainingMgr::~CTrainingMgr()
    {
    Cancel();
    Cleanup();
    }

/**
 * @param aMessage
 * @param aId The id number to use for the identity
 * @param aProtKey the protection key will be returned here
 */
void CTrainingMgr::RegisterIdentityL(const RMessage2& aMessage,
									TIdentityId aId,
									CProtectionKey&  aProtKey)
	{
	__ASSERT_ALWAYS(!IsActive(),
					User::Panic(KAuthServerShortName,
								EPanicTrainingMgrBusy));	

	iProtKey = &aProtKey;
	iMessage = new (ELeave) RMessage2(aMessage);
	iCurrentPluginIdx = 0;
	iIdentity = aId;
	iDescription = HBufC::NewL(aMessage.GetDesLength(1));
	TPtr ptr = iDescription->Des();
	aMessage.Read(1,ptr); 

	iState = ERegistrationFirstStep;
	DoRegistrationStepL();
	}

TBool CTrainingMgr::RegisterFirstIdentityL(TIdentityId aId,
										  CProtectionKey& aProtKey)
	{
	__ASSERT_ALWAYS(!IsActive(),
					User::Panic(KAuthServerShortName,
								EPanicTrainingMgrBusy));	

	iIdentity = aId;
	iProtKey  = &aProtKey;
    iDescription = HBufC::NewL(KDefaultUserDescription().Length());
    *iDescription = KDefaultUserDescription;
    
    TBool result = EFalse;
    
    //Get the default plugin id from the configuration file.
    TPluginId defaultPluginId = iAuthRepository.DefaultPluginL();
	
    CAuthPluginInterface* plugin = 0;
	TRAPD(err, plugin = iPluginMgr->PluginL(defaultPluginId));
	
	if ((err == KErrNone) &&
		(plugin->IsActive()) &&
		(plugin->Type() == EAuthKnowledge) && 
	    (plugin->DefaultData(aId, iResult) == KErrNone))
		{
		iCurrentPlugin = plugin->Id(); 
		AddIdentityL(); 
		AddTrainingResultL();
		result = ETrue;
		}	
	
	Cleanup();		    	
	return result;
	}

void CTrainingMgr::TrainPluginL(const RMessage2& aMessage,
							   CProtectionKey&  aProtKey)
	{
	__ASSERT_ALWAYS(!IsActive(),
					User::Panic(KAuthServerShortName,
								EPanicTrainingMgrBusy));	

	iProtKey = &aProtKey;
 	iIdentity = aMessage.Int0();
	iCurrentPlugin = aMessage.Int1();
	iMessage = new (ELeave) RMessage2(aMessage);
	CAuthPluginInterface* plugin = 0;

	TRAPD(err, plugin = iPluginMgr->PluginL(iCurrentPlugin));
	
	if (err != KErrNone)
		{
		iMessage->Complete(err);
		Cleanup();
		return;
		}
	else
		{
		SetActive();
		iState=ETrainingDone;
		if (plugin->IsActive())
			{
			plugin->Train(iIdentity, iResult, iStatus);
			}
		else
			{
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrAuthServPluginNotActive);
			}
		}
	}

TInt CTrainingMgr::RunError(TInt aError)
    {
    iMessage->Complete(aError);
	return KErrNone;
	
  }

void CTrainingMgr::RunL()
	{
	if (iStatus != KErrNone && 
		(iStatus != KErrAuthServPluginCancelled && 
		iStatus != KErrAuthServPluginNotActive))
		{	
		// error results other than plugin cancelled or inactive handled  here
		Complete();
		return;
		}
	switch (iState)
		{
    	case ERegistrationFirstStep:
			if (AddIdentityL())
				{
				iState = ERegistering;
				}
			DoRegistrationStepL();
			break;
       case ERegistering:
			AddTrainingResultL();
    		DoRegistrationStepL();
    		break;
       case ETrainingDone:
            if (iStatus == KErrAuthServPluginCancelled)
			  {
    		  DEBUG_PRINTF(_L8("Plugin cancelled in training"));
			  }
    	    AddTrainingResultL();
    		Complete();
    		break;
    	case ERegistrationDone:
			WriteResultToMsgL();
    		Complete();
    		break;
    	}
	}

void CTrainingMgr::DoCancel()
	{
	CAuthPluginInterface* plugin = 0;
	TRAPD(err, plugin = iPluginMgr->PluginL(iCurrentPlugin));
	
	if (err == KErrNone)
		{
		iMessage->Complete(KErrCancel);
		plugin->Cancel();
		}
	else
		{
		User::Panic(KAuthServerShortName, EPanicNoSuchAuthPlugin);
		}
	Cleanup();
	}

TBool CTrainingMgr::IsBusy() const
  {
  return iState != EIdle;
  }

CTransientKeyInfo* CTrainingMgr::CreateKeyInfoLC()
	{
    __ASSERT_ALWAYS(iResult != 0 && iResult->Size() > 0,
					User::Panic(KAuthServerShortName,
								EPanicInvalidDefaultData));	

	CTransientKeyInfo* keyInfo = CTransientKeyInfo::NewLC(iCurrentPlugin);

	CTransientKey* key = keyInfo->CreateTransientKeyL(*iResult);
	CleanupStack::PushL(key);
    
	CEncryptedProtectionKey* encKey = key->EncryptL(*iProtKey);
	CleanupStack::PushL(encKey);
		
	keyInfo->SetEncryptedProtectionKeyL(encKey);
	CleanupStack::Pop(encKey);
	CleanupStack::PopAndDestroy(key);
	return keyInfo;
	}

TBool CTrainingMgr::AddTrainingResultL()
	{
	TBool result = EFalse;
	if (iStatus == KErrNone)
		{	
		CTransientKeyInfo* keyInfo = CreateKeyInfoLC();
		iAuthDb->SetTrainedPluginL(iIdentity, keyInfo->PluginId(),
								   *keyInfo);
		CleanupStack::PopAndDestroy(keyInfo);
		result = ETrue;
		}
	return result;
	}

TBool CTrainingMgr::AddIdentityL()
	{
	TBool result = EFalse;
	if (iStatus == KErrNone)
		{
		CTransientKeyInfo* keyInfo = CreateKeyInfoLC();
		iAuthDb->AddIdentityWithTrainedPluginL(iIdentity, *iDescription, *keyInfo);
		CleanupStack::PopAndDestroy(keyInfo);
		result = ETrue;
		}
	return result;
	}

void CTrainingMgr::DoRegistrationStepL()
	{
	

	if (iCurrentPluginIdx == iPluginMgr->ImplementationsL().Count())
		{	
		SetActive();
		iState = ERegistrationDone;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		return;
		}

	iCurrentPlugin  =
		iPluginMgr->ImplementationsL()[iCurrentPluginIdx++]
	      ->ImplementationUid().iUid;
	
	CAuthPluginInterface* plugin = 0;
	TRAPD(err, plugin = iPluginMgr->PluginL(iCurrentPlugin));
	
	SetActive();
	switch (err)
		{
    	case KErrAuthServNoSuchPlugin:
			{
			// skip this plugin
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			return;
			}
    	case KErrNone:
			break;
    	default:
			User::Leave(err);
			break;
		}
	delete iResult;
	iResult = 0;
	if (plugin->IsActive())
		{
		plugin->Train(iIdentity, iResult, iStatus);
		}
	else
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrAuthServPluginNotActive);
		}
	}

void CTrainingMgr::WriteResultToMsgL()
    {

	if (iAuthDb->NumTrainedPluginsL(iIdentity) > 0)
		{
		CIdentity* identity = CIdentity::NewLC(iIdentity, iProtKey,
											   iDescription);
		iProtKey = 0;
		iDescription = 0;
	
		HBufC8* idBuff = HBufC8::NewLC(KDefaultBufferSize);
		TPtr8  idPtr =  idBuff->Des();
    
		RDesWriteStream writeStream(idPtr);
		CleanupClosePushL(writeStream);
    
		writeStream << *identity;
		writeStream.CommitL();

		TInt clientBuffSize = iMessage->GetDesMaxLength(0);
  
		if (clientBuffSize >= idBuff->Size())
			{
				iMessage->Write(0, *idBuff);
			}
		else
			{
				User::Leave(KErrUnderflow);
			}
		CleanupStack::PopAndDestroy(3,identity);
		}
	else
		{

		iStatus = KErrAuthServRegistrationFailed;
		}

    }
    
void CTrainingMgr::Complete()
	{
	iMessage->Complete(iStatus.Int());
	Cleanup();
	}

void CTrainingMgr::Cleanup()
	{
	if(iDescription)
		{
		delete iDescription;
		iDescription = 0;
		}
	
	if(iResult)
		{
		delete iResult;
		iResult = 0;
		}
	
	if(iMessage)
		{
		delete iMessage;
		iMessage = 0;
		}
	
	if(iProtKey)
		{
		delete iProtKey;
		iProtKey = 0;
		}
	
	iState = EIdle;
	}
