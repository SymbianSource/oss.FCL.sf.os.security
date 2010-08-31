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
* CPinPluginDB pin plugin data store implementation
*
*/


/**
 @file 
*/

#include "pinplugindb.h"

_LIT(KPluginStoreName, "plugin.store");
_LIT(KFileDrive, "c:");

using namespace AuthServer;

CPinPluginDB* CPinPluginDB::NewL()
	{
	CPinPluginDB* self = CPinPluginDB::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CPinPluginDB* CPinPluginDB::NewLC()
	{
	CPinPluginDB* self = new(ELeave) CPinPluginDB();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CPinPluginDB::CPinPluginDB() :
	iRootStreamId(KNullStreamId),
	iIdentityStreamId(KNullStreamId)
	{
	}

CPinPluginDB::~CPinPluginDB()
	{
	if (iFileStore)
		{
		CompactStore();
		delete iFileStore;
		}

	iFile.Close(); 
	iFs.Close();

	iId.Reset();
	iId.Close();
	iIdKeyHash.ResetAndDestroy();
	iIdKeyHash.Close();
	}
	
		
void CPinPluginDB::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	
	TFileName privatePath;
	iFs.PrivatePath(privatePath);
	iPluginDBName.Copy(KFileDrive);
	iPluginDBName.Append(privatePath);
	iPluginDBName.Append(KPluginStoreName);
	OpenStoreL();
	
	ReadIdKeysFromStoreL();
	}


void CPinPluginDB::AddPinL(const TIdentityId& aId, const TDesC8& aIdentityKeyHash)
	{
	if(IdIndex(aId) == KErrNotFound && IsUniquePin(aIdentityKeyHash))
		{
		// Add the IdentityId to the array, rewrite the identitystream and 
		// Only then commit the store
		HBufC8* keyHash = aIdentityKeyHash.AllocLC();
		TInt err = KErrNone;
		iIdKeyHash.AppendL(keyHash);
		err = iId.Append(aId);
		if(err != KErrNone)
			{
			iIdKeyHash.Remove(iIdKeyHash.Count() - 1);
			User::Leave(err);
			}
		TCleanupItem cleanupStore(RevertStore, iFileStore);
		// write the idkeys to the store.
		TRAP(err,
			CleanupStack::PushL(cleanupStore);
			WriteIdKeysToStoreL();
			CleanupStack::Pop(iFileStore));

		if (err != KErrNone)
			{
			iId.Remove(iId.Count() - 1);
			iIdKeyHash.Remove(iIdKeyHash.Count() - 1);
			User::Leave(err);
			}
		CleanupStack::Pop(keyHash); 
		}
	else
		{
		User::Leave(KErrAlreadyExists);
		}
 	}

void CPinPluginDB::UpdatePinL(const TIdentityId& aId, const TDesC8& aIdentityKeyHash)
	{
	if(IdIndex(aId) != KErrNotFound)
		{
		// Get the index of the existing id and key hash
		TInt index = IdIndex(aId);
		TInt err = KErrNone;
		HBufC8* keyHash = aIdentityKeyHash.AllocLC();
		// delete the old keyHash and store the new keyHash to the index
		delete iIdKeyHash[index];
		iIdKeyHash[index] = keyHash;

		TCleanupItem cleanupStore(RevertStore, iFileStore);
		// write the idkeys to the store.
		TRAP(err ,
			CleanupStack::PushL(cleanupStore);
			WriteIdKeysToStoreL();
			CleanupStack::Pop(iFileStore));
		if (err!= KErrNone)
			{
			CleanupStack::Pop(keyHash);
			ReadIdKeysFromStoreL();
			User::Leave(err);
			}
		CleanupStack::Pop(keyHash);
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}
	
void CPinPluginDB::RemovePinL(const TIdentityId& aId)
	{
	if(IdIndex(aId) != KErrNotFound)
		{
		TInt err = KErrNone;
		TInt index = IdIndex(aId);

		// Delete the IdentityId from the array, rewrite the identitystream and 
	   	// Only then commit the store
		delete iIdKeyHash[index];
  		iIdKeyHash.Remove(index);
   		iId.Remove(index);
 
		TCleanupItem cleanupStore(RevertStore, iFileStore);
		// write the idkeys to the store.
		TRAP(err, 
			CleanupStack::PushL(cleanupStore);
			WriteIdKeysToStoreL();
			CleanupStack::Pop(iFileStore));
		if(err != KErrNone)
			{
			ReadIdKeysFromStoreL();
   			User::Leave(err);
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	}
	
TBool CPinPluginDB::IsUniquePin(const TDesC8& aIdentityKeyHash) const
	{
	//	Check each iIdKeyHash in the store to determine if aIdentityId already exists
	TInt count = iIdKeyHash.Count();
	TBool isUnique = ETrue;
	for (TInt index = 0; index < count; ++index)
		{
		if (aIdentityKeyHash.CompareF(*iIdKeyHash[index]) == 0)
			{
			isUnique = EFalse;
			break;
			}
		}
	return isUnique;
	}
	
TInt CPinPluginDB::IdIndex(const TIdentityId& aId) const
	{
	TInt count = iId.Count();
	for (TInt index = 0; index < count; ++index)
		{
		if (aId == iId[index])
			{
			return index;
			}
		}
	return KErrNotFound;
	}
	
TIdentityId CPinPluginDB::IdFromPin(const TDesC8& aIdentityKeyHash) const
	{
	TInt count = iIdKeyHash.Count();
	for (TInt index = 0; index < count; ++index)
		{
		if (aIdentityKeyHash.CompareF(*iIdKeyHash[index]) == 0)
			{
			return iId[index];
			}
		}
	return KUnknownIdentity;
	}
	
void CPinPluginDB::OpenStoreL()
	{
	//	Tries to Open a plugin store file on the private path of the process. 
	//	If it cannot find one, create a file with permanent file store
	//	it should initialise iFileStore unless it cannot
	//	create the file/store/streams
	
	TRAPD(result, OpenStoreInFileL(iPluginDBName));

	if (result == KErrNotFound || result == KErrPathNotFound) 
		{		
		// Not yet opened a valid store, either no file to be found, or no valid
		// store in it.
		CreateStoreInFileL(iPluginDBName);
		}
	else if (result != KErrNone)
		{
		// DB gets corrupted or File may be in use. Abort startup.
		User::Leave(result);
		}
	}


void CPinPluginDB::CreateStoreInFileL(const TDesC& aFile)
	{
	TInt r = iFs.MkDirAll(aFile);
	if ((r!=KErrNone) && (r!=KErrAlreadyExists))
		{
		User::Leave(r);
		}
	
	delete iFileStore;
	iFileStore = NULL;

	iFileStore = CPermanentFileStore::ReplaceL(iFs, aFile, EFileRead | EFileWrite | EFileStream | EFileShareExclusive);
	iFileStore->SetTypeL(KPermanentFileStoreLayoutUid);

	TCleanupItem cleanupStore(RevertStore, iFileStore);
	CleanupStack::PushL(cleanupStore);
	
	// Create Identity stream - Currently no Identity created, and no IdentityKeyHash
	RStoreWriteStream identityStream;
	iIdentityStreamId = identityStream.CreateLC(*iFileStore);
	identityStream.WriteUint32L(KNullStreamId.Value()); //contains the id of dataStream
	identityStream.WriteUint32L(0); // Write IdentityId count of zero
	identityStream.CommitL();
	CleanupStack::PopAndDestroy(&identityStream);

	// Create root stream - just contains id of Identity stream
	RStoreWriteStream rootStream;
	iRootStreamId = rootStream.CreateLC(*iFileStore);
	iFileStore->SetRootL(iRootStreamId);
	rootStream.WriteUint32L(iIdentityStreamId.Value());		
	rootStream.CommitL();
	CleanupStack::PopAndDestroy(&rootStream);
	
	iFileStore->CommitL();
	CleanupStack::Pop(iFileStore); 
	}

void CPinPluginDB::OpenStoreInFileL(const TDesC& aFile)
	{
	// Make sure the file isn't write protected
	User::LeaveIfError(iFs.SetAtt(aFile, 0, KEntryAttReadOnly));
	
	User::LeaveIfError(iFile.Open(iFs, aFile, EFileRead | EFileWrite | EFileStream | EFileShareExclusive));
	
	delete iFileStore;
	iFileStore = NULL;

	iFileStore = CPermanentFileStore::FromL(iFile);		

	// Get the root StreamId
	iRootStreamId = iFileStore->Root();
	if (iRootStreamId == KNullStreamId)
		{
		User::Leave(KErrCorrupt);
		}
	
	RStoreReadStream rootStream;
	rootStream.OpenLC(*iFileStore, iRootStreamId);
	iIdentityStreamId = (TStreamId)(rootStream.ReadUint32L());
	CleanupStack::PopAndDestroy(&rootStream);
	}

// Rewrites the Identity stream (ie the array of IdentityId) to the store
void CPinPluginDB::WriteIdKeysToStoreL()
	{
	TInt count = iId.Count();
	TInt hashCount = iIdKeyHash.Count();
	if (count != hashCount)
		{
		User::Leave(KErrCorrupt);
		}
	
	TStreamId dataStreamId = KNullStreamId;
	if (count > 0)
		{
		RStoreWriteStream dataStream;
		dataStreamId = dataStream.CreateLC(*iFileStore);
		//write the Identity Id key to a data stream
		for (TInt index = 0; index < count; ++index)
			{
			dataStream.WriteUint32L(iId[index]);
			dataStream.WriteUint32L((*iIdKeyHash[index]).Length());
			dataStream.WriteL(*iIdKeyHash[index]);
			}
		dataStream.CommitL();
		CleanupStack::PopAndDestroy(&dataStream); 	
		}
	RStoreReadStream readStream;
	readStream.OpenLC(*iFileStore, iIdentityStreamId);
	TStreamId oldDataStreamId = (TStreamId)(readStream.ReadInt32L());
	CleanupStack::PopAndDestroy(&readStream);
		
	iFileStore->DeleteL(oldDataStreamId);
	
	RStoreWriteStream writeStream;
	writeStream.OpenLC(*iFileStore, iIdentityStreamId);
	writeStream.WriteUint32L(dataStreamId.Value());
	writeStream.WriteInt32L(count);
	writeStream.CommitL();
	CleanupStack::PopAndDestroy(&writeStream); 	
	iFileStore->CommitL();
	CompactStore();
	}
	
void CPinPluginDB::ReadIdKeysFromStoreL()
	{
	iId.Reset();
	iIdKeyHash.ResetAndDestroy();
	
	RStoreReadStream readStream;
	readStream.OpenLC(*iFileStore, iIdentityStreamId);
	TStreamId dataStreamId = (TStreamId)(readStream.ReadInt32L());
	TInt count = readStream.ReadInt32L();
	CleanupStack::PopAndDestroy(&readStream);
	
	if (count > 0)
		{
		RStoreReadStream dataStream;
		dataStream.OpenLC(*iFileStore, dataStreamId);
		for (TInt index = 0; index < count; ++index)
			{
			TIdentityId identityId = (TIdentityId)(dataStream.ReadInt32L());
			iId.AppendL(identityId);
			TInt length = dataStream.ReadInt32L();
			HBufC8* idKeyHash = HBufC8::NewMaxLC(length);
			TPtr8 idKeyHashPtr = idKeyHash->Des();
			dataStream.ReadL(idKeyHashPtr, length);
			iIdKeyHash.AppendL(idKeyHash);
			CleanupStack::Pop(idKeyHash);
			}
		CleanupStack::PopAndDestroy(&dataStream);
		}
	}

void CPinPluginDB::RevertStore(TAny* aStore)
	{
	CPermanentFileStore* store = reinterpret_cast<CPermanentFileStore*>(aStore);
	TRAP_IGNORE(store->RevertL());
	// We're ignoring the leave code from this becuase there's no way we can
	// handle this sensibly.  This shouldn't be a problem in practice - this
	// will leave if for example the file store is on removable which is
	// unexpectedly remove, and this is never the case for us.
	}


void CPinPluginDB::CompactStore()
	{
	ASSERT(iFileStore);
	TRAP_IGNORE(iFileStore->ReclaimL(); iFileStore->CompactL());
	}




