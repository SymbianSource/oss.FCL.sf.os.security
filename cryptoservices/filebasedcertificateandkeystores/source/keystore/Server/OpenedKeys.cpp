/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "OpenedKeys.h"
#include "cfskeystoreserver.h"
#include "keystreamutils.h"
#include "fsdatatypes.h"
#include "keystorepassphrase.h"

#include <asymmetric.h>
#include <asymmetrickeys.h>
#include <bigint.h>
#include <ct.h>
#include <securityerr.h>
#include <e32base.h>

// COpenedKey //////////////////////////////////////////////////////////////////

COpenedKey* COpenedKey::NewL(const CFileKeyData& aKeyData, TUid aType, const RMessage2& aMessage,
							 CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan)
	{
	COpenedKey* self = NULL;

	if (aType == KRSARepudiableSignerUID)
		{
		self = new (ELeave) CRSARepudiableSigner(aKeyData, aKeyDataMan, aPassMan);
		}
	else if (aType == KDSARepudiableSignerUID)
		{
		self = new (ELeave) CDSARepudiableSigner(aKeyData, aKeyDataMan, aPassMan);
		}
	else if (aType == KPrivateDecryptorUID)
		{
		self = new (ELeave) CFSRSADecryptor(aKeyData, aKeyDataMan, aPassMan);
		}
	else if (aType == KKeyAgreementUID)
		{
		self = new (ELeave) CDHAgreement(aKeyData, aKeyDataMan, aPassMan);
		}
	else
		{
		User::Invariant();
		}

	CleanupStack::PushL(self);
	self->ConstructL(aMessage);
	CleanupStack::Pop(self);
	return self;
	}

COpenedKey::COpenedKey(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	CActive(EPriorityStandard),
	iKeyData(aKeyData),
	iKeyDataMan(aKeyDataMan),
	iPassMan(aPassMan)
	{
	}

void COpenedKey::ConstructL(const RMessage2& aMessage)
	{
	CKeyInfo* keyInfo = iKeyDataMan.ReadKeyInfoLC(iKeyData);	
	CheckKeyL(*keyInfo, aMessage);
	iLabel = keyInfo->Label().AllocL();
	CleanupStack::PopAndDestroy(keyInfo);
	CActiveScheduler::Add(this);
	}

COpenedKey::~COpenedKey()
	{
	Cancel();
	delete iLabel;
	}

const TDesC& COpenedKey::Label() const
	{
	return *iLabel;
	}

TInt COpenedKey::Handle() const
	{
	return iKeyData.Handle();
	}

void COpenedKey::CheckKeyL(const CKeyInfo& aKeyInfo, const RMessage2& aMessage)
	{
	// Check the client is allowed to use the key
	if (!aKeyInfo.UsePolicy().CheckPolicy(aMessage))
		{
		User::Leave(KErrPermissionDenied);
		}

	// Check that the operation represented by this object is supported for this
	// type of key
	if (aKeyInfo.Algorithm() != Algorithm())
		{
		User::Leave(KErrKeyAlgorithm);
		}

	// Check the key usage allows the operation
	if ((aKeyInfo.Usage() & RequiredUsage()) == 0)
		{
		User::Leave(KErrKeyUsage);
		}

	// Check current time is after start date (if set) and before end date (if
	// set)
	TTime timeNow;
	timeNow.UniversalTime();
	if (aKeyInfo.StartDate().Int64() != 0 && timeNow < aKeyInfo.StartDate())
		{
		User::Leave(KErrKeyValidity);
		}
	if (aKeyInfo.EndDate().Int64() != 0 && timeNow >= aKeyInfo.EndDate())
		{
		User::Leave(KErrKeyValidity);
		}
	}

void COpenedKey::GetPassphrase(TRequestStatus& aStatus)
	{
	ASSERT(iState == EIdle);

	TInt timeout = iKeyDataMan.GetPassphraseTimeout();
	TStreamId passphraseId = iKeyDataMan.DefaultPassphraseId();
	ASSERT(passphraseId != KNullStreamId);
	iClientStatus = &aStatus;
	
	iPassMan.GetPassphrase(passphraseId, timeout, iPassphrase, iStatus);
	iState = EGetPassphrase;
	SetActive();
	}

void COpenedKey::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case EGetPassphrase:
			ASSERT(iPassphrase);
			if (!iKeyRead)
				{
				RStoreReadStream stream;
				iKeyDataMan.OpenPrivateDataStreamLC(iKeyData, *iPassphrase, stream);
				ReadPrivateKeyL(stream);
				CleanupStack::PopAndDestroy(&stream);
				iKeyRead = ETrue;
				}
			PerformOperationL();
			break;

		default:
			ASSERT(EFalse);
		}
	
	Complete(KErrNone);
	}

TInt COpenedKey::RunError(TInt aError)
	{
	Complete(aError);
	return KErrNone;
	}

void COpenedKey::DoCancel()
	{
	Complete(KErrCancel);
	}

void COpenedKey::Complete(TInt aError)
	{
	Cleanup();
	iPassphrase = NULL;
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	iState = EIdle;
	}

// CRSARepudiableSigner ////////////////////////////////////////////////////////

CRSARepudiableSigner::CRSARepudiableSigner(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	COpenedKey(aKeyData, aKeyDataMan, aPassMan)
	{
	}

CRSARepudiableSigner::~CRSARepudiableSigner()
	{
	delete iPrivateKey;
	}

TUid CRSARepudiableSigner::Type() const
	{
	return KRSARepudiableSignerUID;
	}

CKeyInfo::EKeyAlgorithm CRSARepudiableSigner::Algorithm() const
	{
	return CKeyInfo::ERSA;
	}
 
TKeyUsagePKCS15 CRSARepudiableSigner::RequiredUsage() const
	{
	return EPKCS15UsageSignSignRecover;
	}

void CRSARepudiableSigner::Sign(const TDesC8& aPlaintext,
								CRSASignature*& aSignature,
								TRequestStatus& aStatus)
	{
	ASSERT(iPlaintext.Ptr() == NULL);
	ASSERT(iSignaturePtr == NULL);
	iPlaintext.Set(aPlaintext);
	iSignaturePtr = &aSignature;
	GetPassphrase(aStatus);
	}

void CRSARepudiableSigner::ReadPrivateKeyL(RReadStream& aStream)
	{
	ASSERT(iPrivateKey == NULL);
	CreateL(aStream, iPrivateKey);
	}

void CRSARepudiableSigner::PerformOperationL()
	{
	ASSERT(iPrivateKey);
	
	CRSAPKCS1v15Signer* signer = CRSAPKCS1v15Signer::NewLC(*iPrivateKey);
	const CRSASignature* signature = signer->SignL(iPlaintext);
	CleanupStack::PopAndDestroy(signer);
	*iSignaturePtr = const_cast<CRSASignature*>(signature);
	}

void CRSARepudiableSigner::Cleanup()
	{
	iPlaintext.Set(NULL, 0);
	iSignaturePtr = NULL;
	}

// CDSARepudiableSigner ////////////////////////////////////////////////////////

CDSARepudiableSigner::CDSARepudiableSigner(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	COpenedKey(aKeyData, aKeyDataMan, aPassMan)
	{
	}

CDSARepudiableSigner::~CDSARepudiableSigner()
	{
	delete iPrivateKey;
	}

TUid CDSARepudiableSigner::Type() const
	{
	return KDSARepudiableSignerUID;
	}

CKeyInfo::EKeyAlgorithm CDSARepudiableSigner::Algorithm() const
	{
	return CKeyInfo::EDSA;
	}
 
TKeyUsagePKCS15 CDSARepudiableSigner::RequiredUsage() const
	{
	return EPKCS15UsageSignSignRecover;
	}

void CDSARepudiableSigner::Sign(const TDesC8& aPlaintext,
								CDSASignature*& aSignature,
								TRequestStatus& aStatus)
	{
	ASSERT(iPlaintext.Ptr() == NULL);
	ASSERT(iSignaturePtr == NULL);
	iPlaintext.Set(aPlaintext);
	iSignaturePtr = &aSignature;
	GetPassphrase(aStatus);
	}

void CDSARepudiableSigner::ReadPrivateKeyL(RReadStream& aStream)
	{
	ASSERT(iPrivateKey == NULL);
	CreateL(aStream, iPrivateKey);
	}

void CDSARepudiableSigner::PerformOperationL()
	{
	ASSERT(iPrivateKey);
	
	CDSASigner* signer = CDSASigner::NewLC(*iPrivateKey);
	const CDSASignature* signature = signer->SignL(iPlaintext);
	CleanupStack::PopAndDestroy(signer);
	*iSignaturePtr = const_cast<CDSASignature*>(signature);
	}

void CDSARepudiableSigner::Cleanup()
	{
	iPlaintext.Set(NULL, 0);
	iSignaturePtr = NULL;
	}

// CFSRSADecryptor /////////////////////////////////////////////////////////////

CFSRSADecryptor::CFSRSADecryptor(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	COpenedKey(aKeyData, aKeyDataMan, aPassMan)
	{
	}

CFSRSADecryptor::~CFSRSADecryptor()
	{
	delete iPrivateKey;
	}

TUid CFSRSADecryptor::Type() const
	{
	return KPrivateDecryptorUID;
	}

CKeyInfo::EKeyAlgorithm CFSRSADecryptor::Algorithm() const
	{
	return CKeyInfo::ERSA;
	}
 
TKeyUsagePKCS15 CFSRSADecryptor::RequiredUsage() const
	{
	return EPKCS15UsageDecryptUnwrap;
	}

void CFSRSADecryptor::Decrypt(const TDesC8& aCiphertext,
							  HBufC8*& aPlaintext,
							  TRequestStatus& aStatus)
	{
	ASSERT(iCiphertext.Ptr() == NULL);
	ASSERT(iPlaintextPtr == NULL);
	iCiphertext.Set(aCiphertext);
	iPlaintextPtr = &aPlaintext;
	GetPassphrase(aStatus);
	}

void CFSRSADecryptor::ReadPrivateKeyL(RReadStream& aStream)
	{
	ASSERT(iPrivateKey == NULL);
	CreateL(aStream, iPrivateKey);
	}

void CFSRSADecryptor::PerformOperationL()
	{
	ASSERT(iPrivateKey);
	
	CRSAPKCS1v15Decryptor* decryptor = CRSAPKCS1v15Decryptor::NewLC(*iPrivateKey);
	HBufC8* plaintext = HBufC8::NewMaxLC(decryptor->MaxOutputLength());
	TPtr8 ptr = plaintext->Des();
	decryptor->DecryptL(iCiphertext, ptr);

	*iPlaintextPtr = plaintext;
	CleanupStack::Pop(plaintext); // now owned by client
	CleanupStack::PopAndDestroy(decryptor);
	}

void CFSRSADecryptor::Cleanup()
	{
	iCiphertext.Set(NULL, 0);
	iPlaintextPtr = NULL;
	}

// CDHAgreement ////////////////////////////////////////////////////////////////

CDHAgreement::CDHAgreement(const CFileKeyData& aKeyData, CFileKeyDataManager& aKeyDataMan, CPassphraseManager& aPassMan) :
	COpenedKey(aKeyData, aKeyDataMan, aPassMan)
	{
	}

CDHAgreement::~CDHAgreement()
	{
	iKey.Close();
	}

TUid CDHAgreement::Type() const
	{
	return KKeyAgreementUID;
	}

CKeyInfo::EKeyAlgorithm CDHAgreement::Algorithm() const
	{
	return CKeyInfo::EDH;
	}
 
TKeyUsagePKCS15 CDHAgreement::RequiredUsage() const
	{
	return EPKCS15UsageDerive;
	}

void CDHAgreement::PublicKey(CDHParams& aParameters, RInteger& aPublicKey, TRequestStatus& aStatus)
	{
	ASSERT(iPKParams == NULL);
	ASSERT(iPKPublicKeyPtr == NULL);
	iPKParams = &aParameters;
	iPKPublicKeyPtr = &aPublicKey;
	iDHState = EPublicKey;
	GetPassphrase(aStatus);
	}

void CDHAgreement::Agree(CDHPublicKey& aY, HBufC8*& aAgreedKey, TRequestStatus& aStatus)
	{
	ASSERT(iAKPublicKey == NULL);
	ASSERT(iAKAgreedKeyPtr == NULL);
	iAKPublicKey = &aY;
	iAKAgreedKeyPtr = &aAgreedKey;
	iDHState = EAgree;
	GetPassphrase(aStatus);
	}

void CDHAgreement::ReadPrivateKeyL(RReadStream& aStream)
	{
	CreateLC(aStream, iKey);
	CleanupStack::Pop(&iKey);
	}

void CDHAgreement::PerformOperationL()
	{
	switch (iDHState)
		{
		case EPublicKey:
			DoPublicKeyL();
			break;
		case EAgree:
			DoAgreeL();
			break;
		default:
			ASSERT(FALSE);
		}
	}

void CDHAgreement::DoPublicKeyL()
	{
	ASSERT(iPKParams);
	ASSERT(iPKPublicKeyPtr);
	
	RInteger n = iPKParams->TakeN();
	CleanupStack::PushL(n);
	RInteger g = iPKParams->TakeG();
	CleanupStack::PushL(g);
	RInteger x = RInteger::NewL(iKey);
	CleanupStack::PushL(x);
	CDHKeyPair* keyPair = CDHKeyPair::NewL(n, g, x);
	CleanupStack::Pop(3); // x, g, n
	CleanupStack::PushL(keyPair);
	
	const CDHPublicKey& pubKey = keyPair->PublicKey();
	*iPKPublicKeyPtr = RInteger::NewL(pubKey.X());
	CleanupStack::PopAndDestroy(keyPair);
	}

void CDHAgreement::DoAgreeL()
	{
	ASSERT(iAKPublicKey);
	ASSERT(iAKAgreedKeyPtr);
	
	RInteger n = RInteger::NewL(iAKPublicKey->N());
	CleanupStack::PushL(n);
	RInteger g = RInteger::NewL(iAKPublicKey->G());
	CleanupStack::PushL(g);
	RInteger x = RInteger::NewL(iKey);
	CleanupStack::PushL(x);
	CDHPrivateKey* privKey = CDHPrivateKey::NewL(n, g, x);
	CleanupStack::Pop(3); // x, g, n
	CleanupStack::PushL(privKey);
	CDH* dh = CDH::NewLC(*privKey);
	*iAKAgreedKeyPtr = const_cast<HBufC8*>(dh->AgreeL(*iAKPublicKey));
	CleanupStack::PopAndDestroy(2, privKey);
	}

void CDHAgreement::Cleanup()
	{
	iPKParams = NULL;
	iPKPublicKeyPtr = NULL;
	iAKPublicKey = NULL;
	iAKAgreedKeyPtr = NULL;
	iDHState = EIdle;
	}
