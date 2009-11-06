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
* Defines the class which represents the structure of the key on 
* which Crypto Token HAI internally operates. It contains the key 
* information relevant to Crypto Token HAI.
*
*/


#include "tkeydetails.h"
#include <mctkeystore.h>

CKeyDetails::CKeyDetails()
	{}

CKeyDetails::~CKeyDetails()
	{
	delete iPrivateKey;
	delete iPublicKey;
	}

TInt CKeyDetails::Handle() const
	{
	return iHandle;
	}

HBufC8* CKeyDetails::PrivateKey() const
	{
	return iPrivateKey;
	}

HBufC8* CKeyDetails::PublicKey() const
    {
    return iPublicKey;
    }

CKeyDetails* CKeyDetails::NewL(	TInt aHandle, 
								const TDesC& aLabel, 
								const TDesC8& aPrivateKey, 
								const TDesC8& aPublicKey )
	{
	CKeyDetails* keyDetails = new (ELeave) CKeyDetails();
	CleanupStack::PushL(keyDetails);
	keyDetails->ConstructL(aHandle, aLabel, aPrivateKey, aPublicKey);
	CleanupStack::Pop(keyDetails);
	return keyDetails;
	}


CKeyDetails* CKeyDetails::NewL(RStoreReadStream& aReadStream)
    {
    CKeyDetails* self = new (ELeave) CKeyDetails();
    CleanupStack::PushL(self);
    self->InternalizeL(aReadStream);
    CleanupStack::Pop(self);
    return (self);
    }

void CKeyDetails::ConstructL(	TInt aHandle, const TDesC& aLabel, 
								const TDesC8& aPrivateKey, const TDesC8& aPublicKey )
	{
	CKeyInfoBase::ConstructL();
	iHandle = aHandle;
	iLabel = aLabel.AllocL();
	iPrivateKey = aPrivateKey.AllocL();
	iPublicKey = aPublicKey.AllocL();
	    
	// set the access type to never extractable
	iAccessType |= CKeyInfoBase::ENeverExtractable;
	iAccessType |= CKeyInfoBase::ELocal;
	// Policy set for keys in hardware depends on the vendor requirements 
	// this reference implementation assumes that accessibility of keys
	// does not need any restriction. Hence for the testing environment policy is set to always pass.
	TSecurityPolicy  temp(TSecurityPolicy::EAlwaysPass);
	iUsePolicy = temp;
	}

void CKeyDetails::ExternalizeL(RWriteStream& aWriteStream) const
    {
    aWriteStream.WriteInt32L(iHandle);

    TInt stringLen = iLabel->Length();
    aWriteStream.WriteInt32L(stringLen);
    TPtr stringPtr = iLabel->Des();
    stringPtr.SetLength(stringLen);
    aWriteStream.WriteL(stringPtr);
    
    stringLen = iPrivateKey->Length();
    aWriteStream.WriteInt32L(stringLen);
    TPtr8 keyPtr = iPrivateKey->Des();
    keyPtr.SetLength(stringLen);
    aWriteStream.WriteL(keyPtr);

    stringLen = iPublicKey->Length();
    aWriteStream.WriteInt32L(stringLen);
    keyPtr = iPublicKey->Des();
    keyPtr.SetLength(stringLen);
    aWriteStream.WriteL(keyPtr);
    }

void CKeyDetails::InternalizeL(RReadStream& aReadStream)
    {
    iHandle = aReadStream.ReadInt32L();

    TInt stringLen = aReadStream.ReadInt32L();
    iLabel = HBufC::NewMaxL(stringLen);
    TPtr labelPtr((TUint16*)iLabel->Ptr(), stringLen, stringLen);
    labelPtr.FillZ(stringLen);
    aReadStream.ReadL(labelPtr);
    
    stringLen = aReadStream.ReadInt32L();
    iPrivateKey = HBufC8::NewMaxL(stringLen);
    TPtr8 privateKeyPtr((TUint8*)iPrivateKey->Ptr(), stringLen, stringLen);
    privateKeyPtr.FillZ(stringLen);
    aReadStream.ReadL(privateKeyPtr);
        
    stringLen = aReadStream.ReadInt32L();
    iPublicKey = HBufC8::NewMaxL(stringLen);
    TPtr8 publicKeyPtr((TUint8*)iPublicKey->Ptr(), stringLen, stringLen);
    publicKeyPtr.FillZ(stringLen);
    aReadStream.ReadL(publicKeyPtr);
    // Policy set for keys in hardware depends on the vendor requirements 
    // this reference implementation assumes that accessibility of keys
    // does not need any restriction. Hence for the testing environment policy is set to always pass.
    TSecurityPolicy  temp(TSecurityPolicy::EAlwaysPass);
    iUsePolicy = temp;
    }
