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
* CPinPluginDB pin plugin data store declaration
*
*/


/**
 @file 
*/

#ifndef PIN_PLUGINDB_H
#define PIN_PLUGINDB_H

#include <s32file.h>
#include <authserver/authtypes.h>

// PinPlugin DB
class CPinPluginDB : public CBase
	{
public:
	/** static methods to constuct the object of the CPinPluginDB */
	static CPinPluginDB* NewL();
	static CPinPluginDB* NewLC();
	
	/** Destructor */
	virtual ~CPinPluginDB();

	/** 
	 Add Id and KeyHash to the store if the id is not exist in the
	 store and pin is not used by any other id. 
	 
	 @param aId				 The Id to be Trained 
	 @param aIdentityKeyHash The pin used to Train the Id
	 @leave					 It leaves with KErrAlreadyExists if the id is
	 						 already exist or pin is used by other identity.
	 */
	void AddPinL(const AuthServer::TIdentityId& aId, const TDesC8& aIdentityKeyHash);
	
	/** 
	 Update the Id and Pin in the store With the new Pin, if the id exists.
	 
 	 @param aId				 The Id to be ReTrained 
	 @param aIdentityKeyHash The pin used to ReTrain the Id
	 @leave 				 It leaves with KErrNotFound if the id is
	 						 not exist.
	 */
	void UpdatePinL(const AuthServer::TIdentityId& aId, const TDesC8& aIdentityKeyHash);
	
	/**
	 Remove the Id from the store, if the Id exists
	 
	 @param aId				 The Id to be ReTrained 
	 @leave					 It leaves with KErrNotFound if the id is
	 						 not exist.
	 */
	void RemovePinL(const AuthServer::TIdentityId& aId);
	
	/**
	 Searches the pinvalue in the store
	 
	 @param aIdentityKeyHash The pinvalue to be search for
	 @return				 It return the IdentityId for the pin, if search
	 						 is success else return KUnknownIdentity.
	 */
	AuthServer::TIdentityId IdFromPin(const TDesC8& aIdentityKeyHash) const;
	
	/**
	 Searches the Id in the store.
	 
	 @param aId				 The Id to be search for. 
	 @return				 It return the index of the Id if the search
	 						 is success else return KErrNotFound.
	 */
	TInt IdIndex(const AuthServer::TIdentityId& aId) const ;
	
	/**
 	 Verifies the pin is unique.
	 
	 @param aIdentityKeyHash The pinvalue to be search for
	 @return				 return ETrue if the pin is not used by any of 
	 						 the identity else EFalse.
	 */
	TBool IsUniquePin(const TDesC8& aIdentityKeyHash) const;
	
private:
	CPinPluginDB();
	void ConstructL();

private:					
	/**
	 Create and open a New Store If it is first time
	 else Open the existing store. 
	 */
	void OpenStoreL();
	
	/** 
	 Open the existing store 
	 @param aFile 			The name of the file to open
	 */
	void OpenStoreInFileL(const TDesC& aFile);
	
	/** 
	 Create the new store
	 @param aFile 			The name of the file to create
	 */
	void CreateStoreInFileL(const TDesC& aFile);
	
	/** 
	Write the In memory represtation to the store.
	*/
	void WriteIdKeysToStoreL();
	
	/**
	Read the Id and keys from the store
	*/
	void ReadIdKeysFromStoreL();
	
private:
	/**
	Methods dealing with atomic updates to key data file 
	cleanup item that reverts the store.
	*/
	static void RevertStore(TAny* aStore);
	/**
 	 Attempt to compact the store - it doesn't matter if these calls leave, it
 	 will only mean that the store takes up more space than necessary.
 	*/	
	void CompactStore();
private:
	RFile iFile;
	RFs iFs;
	/** FileStore Object */
	CPermanentFileStore* iFileStore;
	/** Root Stream that constains identityStreamId */
	TStreamId iRootStreamId;
	/** Identity Stream that contains dataStreamId and count of idHashes stored in the DB */
	TStreamId iIdentityStreamId;
	/** DB file Name */
	TFileName iPluginDBName;
private:
	/** In memory representation of id in the store */
	RArray<AuthServer::TIdentityId> iId;
	/** In memory representation of hash in the store */
	RPointerArray<HBufC8> iIdKeyHash;
	};

#endif /* PIN_PLUGINDB_H */
