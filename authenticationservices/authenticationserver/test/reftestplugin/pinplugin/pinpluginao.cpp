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
* CPinPluginAO implementation
*
*/


/**
 @file 
*/

#include "pinpluginao.h"
#include <hash.h>
#include <authserver/auth_srv_errs.h>
#include <authserver/authtypes.h>
#include <e32math.h>

using namespace AuthServer;

/** 
 KDefaultPinDigit is used to generate the default pinvalue for the DefaultData().
 The default pinvalue is generated by appending KDefaultPinDigit for iPinSize times.
 */
const TUint KDefaultPinDigit = 1;

/** 
 The selected pin index for the Train/Retrain operation, returned by
 Dialog Notifier should be in the range KIndexLow and KIndexHigh. This should
 be validated in the Dialog implementation. If Pinplugin receives any other
 values, then pinplugin will panic.
 */
const TInt KIndexLow = 0;
const TInt KIndexHigh = 3;

CPinPluginAO* CPinPluginAO::NewL(TInt aPinSize, TInt aPinMinSize, TInt aPinMaxSize, TInt aRetryCount)
	{
	CPinPluginAO* self = CPinPluginAO::NewLC(aPinSize, aPinMinSize, aPinMaxSize, aRetryCount);
	CleanupStack::Pop(self);
	return self;
	}

CPinPluginAO* CPinPluginAO::NewLC(TInt aPinSize, TInt aPinMinSize, TInt aPinMaxSize, TInt aRetryCount)
	{
	CPinPluginAO* self = new(ELeave) CPinPluginAO(aPinSize, aPinMinSize, aPinMaxSize, aRetryCount);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CPinPluginAO::CPinPluginAO(TInt aPinSize, TInt aPinMinSize, TInt aPinMaxSize, TInt aRetryCount)
	:CActive(EPriorityStandard), iRetryCount(aRetryCount), iPinSize(aPinSize), iPinMinSize(aPinMinSize),
	iPinMaxSize(aPinMaxSize)
	{
	CActiveScheduler::Add(this);
	}
	
void CPinPluginAO::ConstructL()
	{
	iNewPinSize = iPinSize;
	
	// construct PinpluginDialog
	iPinPluginDialog = CPinPluginDialog::NewL();
	// construct the DB
	iPinPluginDb = CPinPluginDB::NewL();
	iDialogResult = new (ELeave) TPinPluginDialogResult;
	iRetryRefCount = iRetryCount;
	}

CPinPluginAO::~CPinPluginAO()
	{
	Deque();
	delete iDialogResult;
	delete iPinPluginDialog;
	delete iPinPluginDb;
	iPinList.ResetAndDestroy();
	iIdKeyList.ResetAndDestroy();
	iIdKeyHashList.ResetAndDestroy();
	}
	
void CPinPluginAO::Identify(TIdentityId& aId, const TDesC& aClientMessage,
							HBufC8*& aResult, TRequestStatus& aRequest)
	{
	iState = EIdentify;
	iIdentityId = aId;
	iIdentityIdPtr = &aId;
	
	aRequest = KRequestPending;
	iRequestStatus = &aRequest;

	iClientMessage = static_cast<const HBufC*>(&aClientMessage);
	iResult = &aResult;
	aResult = NULL;	
 
	SetActive();
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, KErrNone);
	}

void CPinPluginAO::Train(TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest)
	{
	iState = ETrain;
	iIdentityId = aId;

	aRequest = KRequestPending;
	iRequestStatus = &aRequest;

	iResult = &aResult;
	aResult = NULL;	

	SetActive();
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, KErrNone);
    }
	
TInt CPinPluginAO::DefaultData(TIdentityId aId, HBufC8*& aOutputBuf)
	{
	aOutputBuf = NULL;
	TPinValue defaultPin;
	for (TInt i = 0; i < iPinSize; i++)
		{
		defaultPin.AppendNum(KDefaultPinDigit);	
		}
		
	HBufC8* identityKey = NULL;
	HBufC8* identityKeyHash = NULL;
	TRAPD(err, 
		identityKeyHash = GenerateKeyHashL(defaultPin, identityKey);
		CleanupStack::PushL(identityKeyHash);
		CleanupStack::PushL(identityKey);
		aOutputBuf = (*identityKey).AllocL();
		iPinPluginDb->AddPinL(aId, *identityKeyHash);
		CleanupStack::PopAndDestroy(2, identityKeyHash));
		
	return err;
	}

TInt CPinPluginAO::Forget(TIdentityId aId)
	{
	TInt err = KErrNone;
	TRAP(err, iPinPluginDb->RemovePinL(aId));
	if (err == KErrNotFound)
		{
		err = KErrAuthServNoSuchIdentity;
		}
	return err;
	}

void CPinPluginAO::ResetL(TIdentityId aIdentityId, const TDesC& aRegistrationData, HBufC8*& aResult)
	{
	// Remove the current trained information and register using the newly supplied registration
	// data (Since pin plugin is a knowledge based plugin, the registration data supplied is assumed to be the pin)
	// For other plugin types this information is ignored and the identity is simply set as untrained

	// If no registration data is supplied then just perform a forget
	if (aRegistrationData == KNullDesC)
		{
		aResult = NULL;
		TInt err = Forget(aIdentityId);
		User::LeaveIfError(err);
		return;
		}

	// Ensure registration data length is less than or equal to max allowed pin length
	TInt pinLen = aRegistrationData.Length();
	if (pinLen > KMaxPinLength)
		{
		User::Leave(KErrArgument);
		}

	// Convert registration data to 8 bit 
	// Note that no unicode conversion is being done here since a pin cannot be in unicode
	RBuf8 pinBuf;
	pinBuf.CreateL(pinLen);
	CleanupClosePushL(pinBuf);
	pinBuf.Copy(aRegistrationData.Left(pinLen));
	TPinValue pin(pinBuf);
	CleanupStack::PopAndDestroy(&pinBuf);

	// Generate the identity key and identity key hash
	HBufC8* identityKey = NULL;
	HBufC8* identityKeyHash = GenerateKeyHashL(pin, identityKey);
	CleanupStack::PushL(identityKeyHash);
	CleanupStack::PushL(identityKey);

	// Ensure another identity doesn't have the same pin
	TIdentityId tempId = iPinPluginDb->IdFromPin(*identityKeyHash);
	if ((tempId != aIdentityId) && (tempId != KUnknownIdentity))
		{
		CleanupStack::PopAndDestroy(2, identityKeyHash);
		// This appears to be the most appropriate error code - The important point is to convery Reset has failed.
		User::Leave(KErrInUse);
		}
	else if (tempId == aIdentityId)
		{
		// Nothing to do
		aResult = identityKey;		// Ownership transferred to caller
		CleanupStack::Pop(identityKey);
		CleanupStack::PopAndDestroy(identityKeyHash);
		return;
		}

	// Replace the training data
	iPinPluginDb->UpdatePinL(aIdentityId, *identityKeyHash);
	aResult = identityKey;		// Ownership transferred to caller
	CleanupStack::Pop(identityKey);
	CleanupStack::PopAndDestroy(identityKeyHash);
	}

void CPinPluginAO::DoCancel()
	{
	iPinPluginDialog->Cancel();
	if(iRequestStatus)
		{
		User::RequestComplete(iRequestStatus, KErrCancel);
		}
	}
	
void CPinPluginAO::RunL()
	{
	// Leave if there has been an error
	User::LeaveIfError(iStatus.Int());
	
	switch(iState)
		{
		case EIdentify:
			{
			IdentifyId();	
			}
			break;
		case EIdentifyResult:
			{
			if (*iDialogResult == EOk)
				{
				IdentifyResultL();
				}
			else if (*iDialogResult == ECancel)
				{
				User::RequestComplete(iRequestStatus, KErrAuthServPluginCancelled);
				}
			else if (*iDialogResult == EQuit)
				{
				User::RequestComplete(iRequestStatus, KErrAuthServPluginQuit);
				}
			*iDialogResult = static_cast<TPinPluginDialogResult>(0);
			}
			break;
		case ETrain:
			{
			TrainIdL();	
			}
			break;
		case ETrainResult:
			{
			if (*iDialogResult == EOk)
				{
				AddTrainResultToDBL();
				}
			else if (*iDialogResult == ECancel)
				{
				User::RequestComplete(iRequestStatus, KErrAuthServPluginCancelled);
				}
			else if (*iDialogResult == EQuit)
				{
				User::RequestComplete(iRequestStatus, KErrAuthServPluginQuit);
				}
			else if (*iDialogResult == ENext)
				{
				iState = ETrain;
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				SetActive();
				}
			*iDialogResult = static_cast<TPinPluginDialogResult>(0);
			}
			break;
		case EInfo:
			{
			iPinPluginDialog->PinInfo(iMessage, iStatus);
			iState = EFinished;
			SetActive();
			}
			break;
		case EFinished:
			{
			User::RequestComplete(iRequestStatus, iStatus.Int());
			}
			break;
		default:
			{
			User::Leave(KErrNotSupported);
			}
			break;
		}
	}
	
TInt CPinPluginAO::RunError(TInt aError)
	{
	if(iRequestStatus)
		{
		User::RequestComplete(iRequestStatus, aError);
		}
	return KErrNone;
	}

void CPinPluginAO::IdentifyId()
	{
	if (iRetryRefCount--)
		{
		iPinPluginDialog->PinIdentify(iPinMinSize, iPinMaxSize, ETrue, iPinValue, *iDialogResult, iStatus);
		iState = EIdentifyResult;
		}
	else
		{
		iRetryRefCount = iRetryCount;
		*iIdentityIdPtr = KUnknownIdentity;
		iMessage = EPinPluginIdentificationFailure;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		iState = EInfo;
		}
	SetActive();
	}

void CPinPluginAO::IdentifyResultL()
	{
	TIdentityId identityId;
	HBufC8* identityKey = NULL;
	HBufC8* identityKeyHash = GenerateKeyHashL(iPinValue, identityKey);
	CleanupStack::PushL(identityKeyHash);
	CleanupStack::PushL(identityKey);	
	identityId = iPinPluginDb->IdFromPin(*identityKeyHash);
	if (identityId != KUnknownIdentity)
		{
		*iIdentityIdPtr = identityId;
		iRetryRefCount = iRetryCount;
		*iResult = identityKey;
		iMessage = EPinPluginIdentificationSuccess;
		iState = EInfo;
		CleanupStack::Pop(identityKey);
		}
	else
		{	
		CleanupStack::PopAndDestroy(identityKey);
		iState = EIdentify;
		}
	CleanupStack::PopAndDestroy(identityKeyHash);
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}
		
void CPinPluginAO::TrainIdL()
	{
        // Check for invalid pinsize.
	if( !(iNewPinSize >= iPinMinSize && iNewPinSize <= iPinMaxSize)  )
                {
                User::Leave(KErrAuthServRegistrationFailed);
                }


	iPinList.ResetAndDestroy();
	iIdKeyList.ResetAndDestroy();
	iIdKeyHashList.ResetAndDestroy();
	GenerateUniquePinsL(iPinList, iIdKeyList, iIdKeyHashList);
	TInt indexValue = iPinPluginDb->IdIndex(iIdentityId);
	TPinPluginTrainingMessage trainMessage;
	if (indexValue >= 0)
		{
		trainMessage = EReTraining;
		}
	else
		{
		trainMessage = ETraining;
		}
	iPinPluginDialog->PinTraining(trainMessage, iPinList, iPinMinSize, iPinMaxSize, iIndex, iNewPinSize, *iDialogResult, iStatus);
	iState = ETrainResult;
	SetActive();	
	}

void CPinPluginAO::AddTrainResultToDBL()
	{
	__ASSERT_ALWAYS(iIndex >= KIndexLow && iIndex <= KIndexHigh,
		User::Panic(KPinPluginPanicString(), EPinPanicIncorrectIndex));

	TInt indexValue = iPinPluginDb->IdIndex(iIdentityId);
	if (indexValue >= 0)
		{
		iPinPluginDb->UpdatePinL(iIdentityId, *iIdKeyHashList[iIndex]);
		iMessage = EPinPluginReTrainingSuccess;
		}
	else
		{
		iPinPluginDb->AddPinL(iIdentityId, *iIdKeyHashList[iIndex]);
		iMessage = EPinPluginTrainingSuccess;
		}
	TPtrC8 idKeyPtr = *iIdKeyList[iIndex];
	HBufC8* identityKey = idKeyPtr.AllocL();
	*iResult = identityKey;
	iNewPinSize = iPinSize;
	iState = EInfo;
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

HBufC8* CPinPluginAO::GenerateKeyHashL(TPinValue& aPinValue, HBufC8*& aIdentityKey)
	{
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);
	HBufC8* pinValue = aPinValue.AllocLC();
	TPtrC8 hash = sha1->Hash(*pinValue);
	CleanupStack::PopAndDestroy(pinValue);
	aIdentityKey = hash.AllocLC();
	TPtrC8 hash1 = sha1->Hash(hash);
	HBufC8* idkeyHash = hash1.AllocL();
	CleanupStack::Pop(aIdentityKey);
	CleanupStack::PopAndDestroy(sha1);
	return idkeyHash;
	}
	
	
void CPinPluginAO::GenerateUniquePinsL(RPointerArray<TPinValue>& aPinList,
		RPointerArray<HBufC8>& aIdKeyList, RPointerArray<HBufC8>& aIdKeyHashList)
	{
	TInt count = 4; 
	while (count)
		{
		TPinValue* randomPin = new (ELeave) TPinValue;
		CleanupStack::PushL(randomPin);
		for (TInt k = 0; k < iNewPinSize ; k++)
			{
			TUint8 num = Math::Random();
			num = num % 10;
			randomPin->AppendNum(num);
			}
		HBufC8* identityKey = NULL;
		HBufC8* identityKeyHash = GenerateKeyHashL(*randomPin, identityKey);
		CleanupStack::PushL(identityKeyHash);
		CleanupStack::PushL(identityKey);
		if (iPinPluginDb->IsUniquePin(*identityKeyHash))
			{
			aIdKeyList.AppendL(identityKey);
			CleanupStack::Pop(identityKey);
			aIdKeyHashList.AppendL(identityKeyHash);
			CleanupStack::Pop(identityKeyHash);
			aPinList.AppendL(randomPin);
			CleanupStack::Pop(randomPin);
			count--;
			}
		}
	}










	
	
	
	
