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
* CProtectionKey implementation
*
*/


/**
 @file 
*/

#include "authcommon_impl.h"
#include <hash.h>

using namespace AuthServer;

/**
 * Generate a new key upon construction
 * @param aKeySize the length of the key in bits.  
 **/
EXPORT_C CProtectionKey* CProtectionKey::NewL(TInt aKeySize)
	{
	CProtectionKey* key = NewLC(aKeySize);
	CleanupStack::Pop(key);
	return key;
	}
/**
 * Generate a new key upon construction
 * @param aKeySize the length of the key in bits.  
 **/
EXPORT_C CProtectionKey* CProtectionKey::NewLC(TInt aKeySize)
	{
	CProtectionKey* key = new (ELeave) CProtectionKey();
	CleanupStack::PushL(key);
	key->ConstructL(aKeySize);
	return key;
	}

/**
 * Creates a new key, passing in the key data. Should
 * really only be called by a CTransientKey::EncryptL. The new object
 * assumes ownership of the data.
 *
 * @param aKeyData the data to use as the key
 **/
EXPORT_C CProtectionKey* CProtectionKey::NewL(HBufC8* aKeyData)
	{
	CProtectionKey*  key =
	  CProtectionKey::NewLC(aKeyData);
	CleanupStack::Pop(key);
	return key;
	}

/**
 * Creates a new key, passing in the key data. Should
 * really only be called by a CTransientKey::EncryptL. The new object
 * assumes ownership of the data.
 *
 * @param aKeyData the data to use as the key
 **/
EXPORT_C CProtectionKey* CProtectionKey::NewLC(HBufC8* aKeyData)
	{
	CProtectionKey*  key = new (ELeave) CProtectionKey();
	CleanupStack::PushL(key);
	key->ConstructL(aKeyData);
	return key;
	}
/**
 * Constructor
 */
CProtectionKey::CProtectionKey()
	{
	}

/**
 * Creates a new random key of aKeySize
 * @param aKeySize the size of the key to generate
 **/
void CProtectionKey::ConstructL(TInt aKeySize)
	{
	iKeyData = HBufC8::NewMaxL(aKeySize);
	TPtr8 data = iKeyData->Des();
	TRandom::RandomL(data);
	}

/**
 * Deletes the keydata.
 **/
CProtectionKey::~CProtectionKey()
	{
	delete iKeyData;
	}


/**
 * Creates a new protection key generated using a combination of this key
 * and the client UID. Ownership of the returned object is passed to the 
 * caller.
 *
 * @param aClientUid the client uid to use in the generation process.
 *
 * @return a new heap allocated protection key 
 **/
EXPORT_C CProtectionKey* CProtectionKey::ClientKeyL(TInt aClientUid) const
	{
	CSHA1* sha1 = CSHA1::NewL();
	CleanupStack::PushL(sha1);

	TPckg<TIdentityId> idPckg(aClientUid);
	
	sha1->Update(idPckg);
	sha1->Update(*iKeyData);

	TInt keyLength = iKeyData->Length();
	
	HBufC8* newKey = HBufC8::NewLC(keyLength);

	TInt remaining = keyLength;
	do
	  {
		newKey->Des().Append(sha1->Hash(idPckg).Left(remaining));
		remaining = keyLength - newKey->Length(); 
	  } while ( remaining > 0);
		  
	CProtectionKey* key = NewL(newKey);
	CleanupStack::Pop(newKey);
	CleanupStack::PopAndDestroy(sha1);
	return key;
	}
	
/**
 * Returns a descriptor to the key data for use in encryption and 
 * decryption methods. 
 *
 * @return a descripter pointing to the key data. 
 **/
EXPORT_C TPtrC8 CProtectionKey::KeyData() const
    {
	return *iKeyData;
    }

/**
 * Constructs the key using supplied data.
 **/
void CProtectionKey::ConstructL(HBufC8* aKeyData)
	{
	__ASSERT_DEBUG(((iKeyData == 0) &&
				   (aKeyData != 0)),
					User::Leave(KErrArgument));
	iKeyData = aKeyData;
    }

