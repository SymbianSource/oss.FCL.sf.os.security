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
* CEncryptedProtectionKey declaration
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef ENCRYPTEDPROTECTIONKEY_H
#define ENCRYPTEDPROTECTIONKEY_H

#include "authcommon_impl.h"

namespace AuthServer
{

/**
 * CEncryptedProtectionKey containst the encrypted version of an identity's
 * protection key.  Encrypted protection keys are generated via the
 * CTransientKey class and are stored on the device, unlike the raw
 * protection key data.
 */
class CEncryptedProtectionKey : public CBase
	{
public:	

	/**
	 * Creates a new encrypted key, passing in the encrypted key data. Should
	 * really only be called by a CTransientKey::EncryptL. The new object
	 * assumes ownership of the data.
	 *
	 * @param aKeyData the data to use as the encrypted key
	 **/
	static CEncryptedProtectionKey* NewL(HBufC8* aKeyData);
	
	/**
	 * Creates a new encrypted key, passing in the encrypted key data. Should
	 * really only be called by a CTransientKey::EncryptL
	 *
	 * @param aKeyData the data to use as the encrypted key
	 **/
	static CEncryptedProtectionKey* NewLC(HBufC8* aKeyData);
	
	/**
	 * Internalize a persisted key  from the stream.
	 * @param aSrcData the source data
	 **/
	static CEncryptedProtectionKey* NewL(RReadStream& aSrcData);	

	/**
	 * Internalize a persisted key from the stream.
	 * @param aSrcData the source data
	 **/
	static CEncryptedProtectionKey* NewLC(RReadStream& aSrcData);

	/**
	 * Returns a descriptor to the encrypted key data
	 *
	 * @return a descriptor pointing to the encrypted key data. 
	 **/
	TPtrC8 KeyData() const;

	/**
	 * Writes the key data to the specified write stream. The key can be
	 * recreated using the appropriate NewL member.
	 *
	 * @param aDest the destination write stream.
	 *
	 **/
	void ExternalizeL(RWriteStream& aDest) const;
	
	~CEncryptedProtectionKey();

private:
	void ConstructL(HBufC8* aKeyData);
	void InternalizeL(RReadStream& aSrcData);

	HBufC8* iKeyData;
    };

}

#endif // ENCRYPTEDPROTECTIONKEY_H
