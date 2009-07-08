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
* Implements CFileKeyData and CFileKeyDataManager 
*
*/




/**
 @file 
 @internalTechnology
*/
 
#ifndef __CKEYDATAMANAGER_H__
#define __CKEYDATAMANAGER_H__

#include <s32file.h>
#include <tcttokenobjecthandle.h>
#include <ct.h>

class CKeyInfo;
class CPassphraseManager;
class CPassphrase;

/**
 * Server side in-memory representation of a key for file key store management
 * purposes. Objects of this type are stored in an in-memory array for quick
 * keystore lookup.
 */
class CFileKeyData : public CBase
	{
public:
	/// Create a new key data object
	static CFileKeyData* NewLC(TInt aObjectId, const TDesC& aLabel, TStreamId aInfoData,
							   TStreamId aPassphraseId, TStreamId aPublicData, TStreamId aPrivateData);
    /// Read a key data object from a stream
	static CFileKeyData* NewL(RStoreReadStream& aReadStream);
public:
	~CFileKeyData();	
public:
	/// Write out the key data to a stream
	void ExternalizeL(RWriteStream&) const;
public:
	inline const TDesC& Label() const;
	inline TInt32 Handle() const;
	inline TStreamId InfoDataStreamId() const;
	inline TStreamId PassphraseStreamId() const;
	inline TStreamId PublicDataStreamId() const;
	inline TStreamId PrivateDataStreamId() const;
private:
	CFileKeyData(TInt aObjectId, TStreamId aInfoData, TStreamId aPassphraseId,
				 TStreamId aPublicData, TStreamId aPrivateData);
	CFileKeyData();
	void ConstructL(const TDesC& aLabel);
	void InternalizeL(RReadStream&);
private:
	TInt iObjectId;				///< Data to identify the key
	TStreamId iInfoData;		///< ID of stream holding publicly available data for key 
	TStreamId iPassphraseId;    ///< ID of stream holding passphrase data (not currently used)
	TStreamId iPublicKeyData;	///< ID of stream holding public key data
	TStreamId iPrivateKeyData;	///< ID of stream holding private key data
	HBufC* iLabel;				///< Key label data
};

inline const TDesC& CFileKeyData::Label() const
	{
	return *iLabel;
	}

inline TInt32 CFileKeyData::Handle() const
	{
	return iObjectId;
	}

inline TStreamId CFileKeyData::InfoDataStreamId() const
	{
	return iInfoData;
	}

inline TStreamId CFileKeyData::PassphraseStreamId() const
	{
	return iPassphraseId;
	}

inline TStreamId CFileKeyData::PublicDataStreamId() const
	{
	return iPublicKeyData;
	}

inline TStreamId CFileKeyData::PrivateDataStreamId() const
	{
	return iPrivateKeyData;
	}

/**
 * Access the server file store of all keys and key data. The only class to
 * access the store, which maintains store integrity When a new key is created,
 * it is represented by a CFileKeyData object and added to the array.  
 */
class CFileKeyDataManager : public CBase
{
public:
	static CFileKeyDataManager* NewL();
	~CFileKeyDataManager();
public:	
	CPassphraseManager* CreatePassphraseManagerLC();
	void AddL(const CFileKeyData*);
	void RemoveL(TInt aObjectId);
	TBool IsKeyAlreadyInStore(const TDesC& aKeyLabel) const;
public:
	/// Get the id of the default passphrase, or KNullStreamId if it doesn't exist yet.
	TStreamId DefaultPassphraseId() const;
	/// Create a new key data object for a key create/import and leave it one the cleanup stack
	const CFileKeyData* CreateKeyDataLC(const TDesC& aLabel, TStreamId aPassphrase);
	///	Reads the info data for a given key, returning a new CKeyInfo that's on the cleanup stack
	CKeyInfo* ReadKeyInfoLC(const CFileKeyData& aKeyData) const;
	/// Writes key info data for a key
	void WriteKeyInfoL(const CFileKeyData& aKeyData, const CKeyInfo& aKeyInfo);
	/// Writes key info data and reverts changes to the store if if leaves
	void SafeWriteKeyInfoL(const CFileKeyData& aKeyData, const CKeyInfo& aKeyInfo);
	// Methods for opening data streams for a key
	void OpenPublicDataStreamLC(const CFileKeyData& aKeyData, RStoreWriteStream& aStream);
	void OpenPublicDataStreamLC(const CFileKeyData& aKeyData, RStoreReadStream& aStream) const;
	void OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, CPassphrase& aPassphrase, RStoreWriteStream& aStream);
	void OpenPrivateDataStreamLC(const CFileKeyData& aKeyData, CPassphrase& aPassphrase, RStoreReadStream& aStream);

	/*
	 * not currently implemented, but might be someday
	/// Create a new passphrase by prompting the user and pass back its id.
	void CreatePassphrase(CPassphraseManager& aPassMan, TStreamId& aIdOut, TRequestStatus& aStatus);
	/// Remove an existing passphrase.  Leaves if the passphrase is used by any key.
	void RemovePassphraseL(TStreamId aId);
	 */
	
public:
	TInt Count() const;
	const CFileKeyData* operator[](TInt aIndex) const;
	const CFileKeyData* Lookup(TInt aObjectId) const;
public:
	/**
	 * Get the passphrase timeout.  A timeout of zero indicates
	 * that passphrases are never cached.  A timeout of -1 means cache until an
	 * explicit close operation occurs.
	 */
	TInt GetPassphraseTimeout() const;
	void SetPassphraseTimeoutL(TInt aTimeout);
private:
	CFileKeyDataManager();
	void ConstructL();
private:											//	Manages access to store
	void OpenStoreL();
	void OpenStoreInFileL(const TDesC& aFile);
	void CreateStoreInFileL(const TDesC& aFile);
private:
	void OpenInfoDataStreamLC(const CFileKeyData& aKeyData, RStoreWriteStream&);
	static void RevertStore(TAny* aStore);			//	Cleanupitem
	void WriteKeysToStoreL();
	TStreamId CreateWriteStreamL();
	void ReadPassphraseTimeoutL();
	void WritePassphraseTimeoutL();
	void CompactStore();
private:
	RFile iFile;
	RFs iFs;
	CPermanentFileStore* iFileStore;
	TStreamId iRootStreamId;	 		///< Root of the store
	TStreamId iInfoStreamId;	 		///< Stream that contains list of key data
	TStreamId iPassStreamId;	 		///< Stream for the default passphrase
	TStreamId iTimeoutStreamId;  		///< Stream for timeout data
private:
	TInt iKeyIdentifier;
	RPointerArray<const CFileKeyData> iKeys;	///< In memory representation of keys in the store
	TInt iTimeout;						///< The passphrase timeout
};

#endif
