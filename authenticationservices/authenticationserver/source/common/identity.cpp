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
* CIdentity implementation
*
*/


/**
 @file 
*/

#include "authcommon_impl.h"

using namespace AuthServer;

EXPORT_C CIdentity* CIdentity::NewL(TIdentityId aId,
									CProtectionKey* aKey,
									HBufC* aString)
	{
	CIdentity* me = CIdentity::NewLC(aId, aKey, aString);
	CleanupStack::Pop();
	return me;
	} 

EXPORT_C CIdentity* CIdentity::NewLC(TIdentityId aId,
									 CProtectionKey* aKey,
									 HBufC* aString)
	{ 
	CIdentity* me = new (ELeave) CIdentity(aId);
	CleanupStack::PushL(me);
	me->ConstructL(aKey, aString);
	return me;
	}

void CIdentity::ConstructL(CProtectionKey* aKey,
						   HBufC* aString)
    {
	iString = aString;
	iKey = aKey;
	}

CIdentity::CIdentity(TIdentityId aId) 
  : iId(aId)
    {
    }

CIdentity::~CIdentity()
	{
	delete iKey;
	delete iString;
	}
	
/**
 * Returns the id number for the identity.
 *
 * @return KUnknown if the identity of the device holder could not be
 * determined.
 **/
EXPORT_C TIdentityId CIdentity::Id() const
	{
	return iId;
	}

/**
 * Returns the protecion key associated with the returned identity. The
 * key will only be valid if the identity is not unknown.
 *
 * @return the protection key of 
 * the method specified to the RAuthClient::Authenticate method.
 **/
EXPORT_C const CProtectionKey& CIdentity::Key() const
	{
	return *iKey;
	}

/**
 * Returns the string associated with the returned identity. The string will
 * only be valid if a) the identity is not unknown and b) the identity was
 * retrieved using the withString parameter of RAuthClient::AuthenticateL set
 * to true. If this is not the case then an empty string is returned.
 *
 * @return the protection key of 
 * the method specified to the RAuthClient::Authenticate method.
 **/
EXPORT_C TDesC& CIdentity::String() const
	{
	return *iString;
	}

EXPORT_C void CIdentity::ExternalizeL(RWriteStream& aWriteStream) const
    {
    aWriteStream.WriteInt32L(iId);
    aWriteStream.WriteInt32L(iKey->KeyData().Length());
    aWriteStream << iKey->KeyData();
    aWriteStream<< *iString;
    }
 
EXPORT_C CIdentity* CIdentity::InternalizeL(RReadStream& aReadStream)
    {
    TIdentityId id;
    id = aReadStream.ReadInt32L();
    TInt keyLength = aReadStream.ReadInt32L();
    HBufC8* keydata = HBufC8::NewLC(aReadStream, keyLength);
    CProtectionKey* prot = CProtectionKey::NewL(keydata);
    CleanupStack::Pop(keydata);
	CleanupStack::PushL(prot);

    HBufC* stringdata = HBufC::NewLC(aReadStream, KMaxDescLen );
    CIdentity* identity = CIdentity::NewL(id, prot, stringdata);
    CleanupStack::Pop(2, prot);
	return identity; 
    }

