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
* CTransientKeyInfo declaration
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef TRANSIENTKEYINFO_H
#define TRANSIENTKEYINFO_H

#include "transientkey.h"

namespace AuthServer
{

/**
 * CTransientKeyInfo holds persistent data related to transient keys which it
 * also generates. The class records the parameters required to (re)generate a
 * transient key from the plugin data, and is used to link encrypted protection
 * keys a generated transient key.
 */
class CTransientKeyInfo : public CBase
	{
public:
	/**
	 * Sets member data from parameters and randomly initialises the transient
	 * key generation parameters. Use this method to create a key
	 * for encrypting a new key.
	 **/
	static CTransientKeyInfo* NewL(TPluginId  aPluginId);

	/**
	 * Sets member data from parameters and randomly initialises the transient
	 * key generation parameters. Use this method to create a key
	 * for encrypting a new key.
	 **/
	static CTransientKeyInfo* NewLC(TPluginId  aPluginId);

	/**
	 * Reads all data from the input stream. Use this method to recreate a key
	 * used to encrypt a key.
	 **/
	static CTransientKeyInfo* NewL(RReadStream& aInputStream);
	
	/**
	 * Reads all data from the input stream. Use this method to recreate a key
	 * used to encrypt a key.
	 **/
	static CTransientKeyInfo* NewLC(RReadStream& aInputStream);

	~CTransientKeyInfo();

	/**
	 * @return the id of the plugin that was used to generate the transient
	 * key.
	 **/
	TPluginId PluginId() const;

	/**
	 * @return the encrypted protection key
	 * key.
	 **/
	const CEncryptedProtectionKey& EncryptedKey() const;

    /**
	 * Assign the encrypted protection key associated with the transient
	 * key. Ownership of the key is taken by this object.
 	 *
	 * @param aEncryptedKey the protection key encrypted with this transient key.
	 **/
    void SetEncryptedProtectionKeyL(CEncryptedProtectionKey* aEncryptedKey);
	  
	/**
	 * Create transient key using the plugin data. If this object was created
	 * from a stream the method will leave with KErrBadPassword if the plugin
	 * data does not match the original. Ownership of the CTransientKey object
	 * is returned to the caller.
	 **/ 
	CTransientKey* CreateTransientKeyL(const TDesC8& aPluginData) const;

	/**
	 *
	 **/
	void ExternalizeL(RWriteStream& aOutStream) const;

private:
	
    void ConstructL(TPluginId aPluginId);
    void InternalizeL(RReadStream& aInStream);

	/// the id of the plugin that encrypted the key
	TPluginId iPluginId;
		
	/// the encrypted protection key
	CEncryptedProtectionKey* iEncryptedKey;
	
	/// the encryption parameters
	mutable CPBEncryptionData* iEncryptionData;
    };
}

#endif // TRANSIENTKEYINFO_H
