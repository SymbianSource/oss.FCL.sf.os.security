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
* CAuthDb Authentication server data store implementation
*
*/


/**
 @file 
*/

#include "authserver_impl.h"

using namespace AuthServer;

// The number of decimal digits needed to represent a 32 bit unsigned integer
const TInt KUint32Digits = 10;

// -------- DB helper functions --------


static void RollbackDb(TAny* aPtr)
/**
	Implements the TCleanupOperation prototype.  This
	function is used to rollback a database transaction
	if a leave occurs before it is committed.
	
	@param	aPtr			Pointer to RDbDatabase object
							to roll back.
	@see CleanupRollbackPushL
 */
	{
	RDbDatabase* db = static_cast<RDbDatabase*>(aPtr);
	if (db->InTransaction())
		db->Rollback();
	}


inline void CleanupRollbackPushL(RDbDatabase& aDb)
/**
	Puts the supplied database on the cleanup stack.
	The database is rolled back if a leave occurs.
	
	If a leave does not occur then the supplied database
	should be committed.  Do not call CleanupStack::PopAndDestroy
	to delete this object.
	
	@param	aDb				Database to roll back if
							a leave occurs.
 */
	{
	CleanupStack::PushL(TCleanupItem(RollbackDb, &aDb));
	}


static void StartTransactionLC(RDbDatabase& aDb)
/**
	Helper function starts a database transaction and
	puts a rollback cleanup item on the cleanup stack.
	
	@param	aDb				Database to start transaction on.
	@see CommitTransactionLD
 */
	{
	User::LeaveIfError(aDb.Begin());
	CleanupRollbackPushL(aDb);
	}


static void CommitTransactionLD(RDbDatabase& aDb)
/**
	Complements StartTransactionLC by removing the
	the rollback cleanup item from the cleanup stack
	and committing the database.
	
	@see StartTransactionLC
 */
	{
	CleanupStack::Pop(&aDb);
	User::LeaveIfError(aDb.Commit());
	}


// -------- factory functions --------


CAuthDb2* CAuthDb2::NewL(RFs& aFs)
/**
	Factory function allocates a new instance of CAuthDb.
	The database file is created if it does not already exist.
	
	@return					New instance of CAuthDb.
 */
	{
	CAuthDb2* self = CAuthDb2::NewLC(aFs);
	CleanupStack::Pop(self);
	return self;
	}


CAuthDb2* CAuthDb2::NewLC(RFs& aFs)
/**
	Factory function allocates new instance of CAuthDb2,
	putting it on the cleanup stack.  The database file
	is created if it does not already exist.
	
	@return					New instance of CAuthDb.
 */
	{
	CAuthDb2* self = new(ELeave) CAuthDb2;
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	return self;
	}


void CAuthDb2::ConstructL(RFs& aFs)
/**
	This second-phase constructor opens the
	database file, creating it if it does not
	already exist.
 */
	{
	TInt r;
	TFileName dbName(KDbName);
	dbName[0] = RFs::GetSystemDriveChar();

	r = OpenDbFileL(aFs, dbName);
	
	if (r == KErrNone)
		{
		if (! iAuthDbFile.IsDamaged() || iAuthDbFile.Recover() == KErrNone)
			return;
		}
	
	User::LeaveIfError(iAuthDbFile.Replace(aFs, dbName));
	iIsDbOpen = ETrue;
	StartTransactionLC(iAuthDbFile);
	
	// These commands are stored as narrow instead of wide
	// text to save c. 500 bytes in decompressed exe size.
	// The non-XIP EXE size is almost the same because DEFLATE
	// is used on the ELF file.
	
	static const TText8*const cmds[] =
		{
		_S8("CREATE TABLE Identities (idName COUNTER, IdentityId UNSIGNED INTEGER NOT NULL, Description VARCHAR(255) NOT NULL)"),
		_S8("CREATE UNIQUE INDEX idIndex ON Identities (idName)"),
		_S8("CREATE UNIQUE INDEX identityIdx ON Identities (IdentityId DESC)"),
		_S8("CREATE TABLE Prefs (PluginType UNSIGNED INTEGER NOT NULL, PluginId UNSIGNED INTEGER NOT NULL)"),
		_S8("CREATE UNIQUE INDEX typeIdx ON Prefs (PluginType ASC)"),
		_S8("CREATE TABLE TrainedPlugins (IdentityId UNSIGNED INTEGER NOT NULL, PluginId UNSIGNED INTEGER NOT NULL, TransientKey LONG VARBINARY NOT NULL)"),
		_S8("CREATE UNIQUE INDEX identityPluginIdx ON TrainedPlugins (IdentityId DESC, PluginId ASC)")
		};
	
	const TInt elemCount = sizeof(cmds) / sizeof(cmds[0]);
	TBuf<140> cmd;
	for (TInt i = 0; i < elemCount; ++i)
		{
		TPtrC8 cmd8(cmds[i]);
		cmd.Copy(cmd8);
		User::LeaveIfError(iAuthDbFile.Execute(cmd));
		}

	CommitTransactionLD(iAuthDbFile);
	}


CAuthDb2::~CAuthDb2()
/**
	Close the database file used by this object.
 */
	{
	CloseDbFile();
	}

TInt CAuthDb2::OpenDbFileL(RFs& aFs, const TFileName& aDbName)
	{
	
	TInt r(0);
	if(!iIsDbOpen)
		{
		r = iAuthDbFile.Open(aFs, aDbName);
		if(KErrNone == r)
			{
			iIsDbOpen = ETrue;
			}
		else if (KErrNotFound != r)
			{
			User::Leave(r);
			}
		}
	return r;
	}

void CAuthDb2::CloseDbFile()
	{
	if(iIsDbOpen)
		{
		RollbackDb(&iAuthDbFile);
		if(iAuthDbFile.IsDamaged())
			{
			iAuthDbFile.Recover();
			}
		iAuthDbFile.Close();
		iIsDbOpen = EFalse;
		}
	}

// -------- identities --------


void CAuthDb2::AddIdentityWithTrainedPluginL(
	TIdentityId aIdentityId, const TDesC& aDesc,
	const CTransientKeyInfo& aTransKeyInfo)
/**
	Adds an identity and trains a plugin for it in a single
	atomic operation.
	
	@param	aIdentityId		New identity.
	@param	aDesc			Identity's description.  This must
							have no more than KMaxDescLen characters.
	@param	aTransKeyInfo	A transient key which trains this identity
							for a plugin.
	@leave	KErrAuthServIdentityAlreadyExists  The supplied identity
							is already in the database.
 */
	{
	StartTransactionLC(iAuthDbFile);
	
	AddIdentityL(aIdentityId, aDesc);
	
	SetTrainedPluginL(aIdentityId, aTransKeyInfo.PluginId(), aTransKeyInfo);
	
	CommitTransactionLD(iAuthDbFile);
	}


void CAuthDb2::AddIdentityL(TIdentityId aIdentityId, const TDesC& aDesc)
/**
	Add the supplied identity to the database.
	
	@param	aIdentityId		New identity.
	@param	aDesc			Identity's description.  This must
							have no more than KMaxDescLen characters.
	@leave	KErrAuthServDescTooLong The supplied description is more than
							255 characters.
	@leave	KErrAuthServIdentityAlreadyExists  The supplied identity
							is already in the database.
 */
	{
	if (aDesc.Length() > KMaxDescLen)
		User::Leave(KErrAuthServDescTooLong);

	RDbTable table;
	
	// EUpdatable instead of EInsertOnly so can navigate to any
	// existing row.
	if (FindIdentityLC(table, RDbView::EUpdatable, aIdentityId))
		User::Leave(KErrAuthServIdentityAlreadyExists);
	
	table.InsertL();		// start automatic transaction
	table.SetColL(KIdentitiesIdentityIdCol, aIdentityId);
	table.SetColL(KIdentitiesDescCol, aDesc);
	table.PutL();			// end automatic transaction
	
	CleanupStack::PopAndDestroy(&table);
	}


void CAuthDb2::RemoveIdentityL(TIdentityId aIdentityId)
/**
	Remove the supplied identity from the auth server's database.
	Any trained entries for the supplied identity are also removed.
	All updates to the database happen within a single transaction.

	@param	aIdentityId		Identity to remove from database;
	@leave	KErrAuthServIdentityNotFound The supplied identity could
							not be found in the database.
 */
	{
	StartTransactionLC(iAuthDbFile);
	
	RDbTable table;
	FindExistingIdentityLC(table, RDbView::EUpdatable, aIdentityId);
	table.DeleteL();
	CleanupStack::PopAndDestroy(&table);

	// remove each training record for this identity
	_LIT(KDelTrainFmt, "DELETE FROM TrainedPlugins WHERE IdentityId=%u");
	TBuf<(46 - 2) + 10> bufCmd;
	bufCmd.Format(KDelTrainFmt, aIdentityId);
	iAuthDbFile.Execute(bufCmd);
	
	CommitTransactionLD(iAuthDbFile);
	}


TInt CAuthDb2::NumIdentitiesL() 
 /**
	Retrieve the number of identities in the database.
	@return the number of identities in the database.
  */
	{
	TInt result = 0;
	RDbTable table;
	OpenTableLC(table, KIdentitiesTableName, RDbRowSet::EReadOnly, KIdentitiesIndex);
	result = table.CountL();
	CleanupStack::PopAndDestroy(&table);
	return result;
	}
  
void CAuthDb2::IdentitiesL(RArray<TIdentityId>& aResults)
/**
	Populate the supplied array with the currently
	registered identities.
	
	@param	aResults		On success this array is populated
							with the currently registered identities.
							Any items which were in the array when
							this function is called are removed.  On
							failure this array is emptied so the caller
							does not have to place it on the cleanup stack.
 */
	{
	aResults.Reset();
	CleanupClosePushL(aResults);
	
	RDbTable table;
	OpenTableLC(table, KIdentitiesTableName, RDbRowSet::EReadOnly, KIdIndexNum);
	while (table.NextL())
		{
		table.GetL();
		aResults.AppendL(table.ColUint32(KIdentitiesIdentityIdCol));
		}
	
	CleanupStack::PopAndDestroy(&table);
	CleanupStack::Pop(&aResults);
	}

void CAuthDb2::IdentitiesWithDescriptionL(RIdAndStringArray& aList)
/**
	Populate the supplied array with the currently
	registered identities and descriptions.
	
	@param	aList   		On success this array is populated
							with the currently registered identities.
							Any items which were in the array when
							this function is called are removed.  On
							failure this array is emptied so the caller
							does not have to place it on the cleanup stack.
 */
	{
	aList.Reset();
	CleanupClosePushL(aList);
	
	RDbTable table;
	OpenTableLC(table, KIdentitiesTableName, RDbRowSet::EReadOnly, KIdentitiesIndex);
	while (table.NextL())
		{
		table.GetL();
		CIdAndString* id =
		  CIdAndString::NewLC(table.ColUint32(KIdentitiesIdentityIdCol),
							  table.ColDes(KIdentitiesDescCol));
		aList.AppendL(id);
		CleanupStack::Pop(id);
		}
	
	CleanupStack::PopAndDestroy(&table);
	CleanupStack::Pop(&aList);
	}

HBufC* CAuthDb2::DescriptionL(TIdentityId aIdentityId)
/**
	Allocate a descriptor containing the supplied identity's
	description.
	
	@param	aIdentityId		Identity whose description is returned.
	@return					Newly-allocated descriptor containing the
							identity's description.  The client must
							free this object.
	@leave	KErrAuthServIdentityNotFound The supplied identity could
							not be found in the database.
 */
	{
	RDbTable table;
	FindExistingIdentityLC(table, RDbView::EReadOnly, aIdentityId);
	
	table.GetL();
	HBufC* retDesc = table.ColDes(KIdentitiesDescCol).AllocL();
	
	CleanupStack::PopAndDestroy(&table);
	return retDesc;
	}



void CAuthDb2::SetDescriptionL(TIdentityId aIdentityId, const TDesC& aDesc)
/**
	Update an identity's description.
	
	@param	aIdentityId		Identity to update.
	@param	aDesc			New description text.  This must have
							no more than KMaxDescLen characters.
	@leave	KErrAuthServIdentityNotFound The supplied identity could
							not be found in the database.
 */
	{
	if (aDesc.Length() > KMaxDescLen)
		User::Leave(KErrAuthServDescTooLong);
	
	RDbTable table;
	FindExistingIdentityLC(table, RDbView::EUpdatable, aIdentityId);
	
	table.UpdateL();			// start automatic transaction
	table.SetColL(KIdentitiesDescCol, aDesc);
	table.PutL();				// end automatic transaction
	
	CleanupStack::PopAndDestroy(&table);
	}


void CAuthDb2::FindExistingIdentityLC(
	RDbTable& aTable, RDbRowSet::TAccess aAccess, TIdentityId aIdentityId)
/**
	Find the supplied identity and navigate to it.
	On success, the table handle is on the cleanup stack.
	
	@param	aTable			Caller's table handle.
	@param	aAccess			Mode in which to open the handle.
	@param	aIdentityId		Identity to navigate to.
	@leave	KErrAuthServIdentityNotFound The supplied identity could
							not be found.
 */
	{
	TBool found = FindIdentityLC(aTable, aAccess, aIdentityId);
	if (! found)
		User::Leave(KErrAuthServIdentityNotFound);
	}


TBool CAuthDb2::FindIdentityLC(RDbTable& aTable, RDbRowSet::TAccess aAccess, TIdentityId aIdentityId)
/**
	Navigate to the supplied identity in the Identities table.
	On success, the table handle is on the cleanup stack.
	
	@param	aTable			Caller's table handle.
	@param	aAccess			Mode in which to open the table.
	@param	aIdentityId		Identity to navigate to.
	@return					Zero if could not find identity in table;
							Non-zero otherwise.
 */
	{
	TBool found = OpenTableAndNavigateLC(
		aTable, KIdentitiesTableName, aAccess, KIdentitiesIndex, aIdentityId);

	return found;
	}


// -------- plugins --------

TInt CAuthDb2::NumTrainedPluginsL(TIdentityId aId)
/**
    Retrieve the number of trained plugins for the specified id.
    
    @param aId the identity for whom to retrieve the number of trained plugins.
    @return the number of trained plugins for the specified id.
 */
	{
	_LIT(KGetTrainedFmt, "SELECT IdentityId FROM TrainedPlugins WHERE IdentityId=%u");
	TBuf<(57 - 2) + 10> bufCmd;
	bufCmd.Format(KGetTrainedFmt, aId);
	
	TDbQuery q(bufCmd);
	RDbView viewTp;
	CleanupClosePushL(viewTp);
	User::LeaveIfError(viewTp.Prepare(iAuthDbFile, q));
	User::LeaveIfError(viewTp.EvaluateAll());
	TInt trainedCount = viewTp.CountL();
	CleanupStack::PopAndDestroy(&viewTp);

	return trainedCount;
	}

/**
	Returns list of plugins that are trained for the given identity

	@param	aIdentityId		The identity for whom to retrieve the list of trained plugins.
	@param 	aTrainedPlugins	The array to fill the returned plugin ids.
*/
void CAuthDb2::TrainedPluginsListL(TIdentityId aIdentityId, RArray<TPluginId>& aTrainedPlugins)
	{
	_LIT(KGetTrainedFmt, "SELECT PluginId FROM TrainedPlugins WHERE IdentityId=%u");
	RBuf queryBuf;
	CleanupClosePushL(queryBuf);
	queryBuf.CreateL(KGetTrainedFmt().Length() + KUint32Digits);
	queryBuf.Format(KGetTrainedFmt, aIdentityId);

	TDbQuery query(queryBuf);
	RDbView viewTp;
	CleanupClosePushL(viewTp);
	User::LeaveIfError(viewTp.Prepare(iAuthDbFile, query));
	User::LeaveIfError(viewTp.EvaluateAll());

	if (viewTp.FirstL())
		{
		do
			{
			viewTp.GetL();
			aTrainedPlugins.AppendL(viewTp.ColUint32(1));
			}
		while (viewTp.NextL());
		}
	CleanupStack::PopAndDestroy(2, &queryBuf); // viewTp
	}

TAuthTrainingStatus CAuthDb2::PluginStatusL(TPluginId aPluginId)
/**
	Return the supplied plugin's training status.

		EAuthUntrained			No identities have been trained for this plugin.
		EAuthTrained			Some (but not all) identities have been trained for this plugin
		EAuthFullyTrained		All identities have been trained for this plugin.
	
	@param	aPluginId		Plugin whose status should be retrieved.
	@return					The plugin's training status.
 */
	{
	// get number of identities trained for this plugin
	
	// This involves filtering all trainings for this plugin
	// from the TrainedPlugins table.  For small numbers of users
	// it is simpler to do this; for large numbers of users
	// (registered identities) it may be better to maintain another
	// table which matches each plugin against its training count.

	_LIT(KGetTrainedFmt, "SELECT IdentityId FROM TrainedPlugins WHERE PluginId=%u");
	TBuf<(55 - 2) + 10> bufCmd;
	bufCmd.Format(KGetTrainedFmt, aPluginId);
	
	TDbQuery q(bufCmd);
	RDbView viewTp;
	CleanupClosePushL(viewTp);
	User::LeaveIfError(viewTp.Prepare(iAuthDbFile, q));
	User::LeaveIfError(viewTp.EvaluateAll());
	TInt trainCount = viewTp.CountL();
	CleanupStack::PopAndDestroy(&viewTp);
	
	if (trainCount == 0)
		return EAuthUntrained;

	// get total number of identities in the database
	RDbTable tableId;
	OpenTableLC(tableId, KIdentitiesTableName, RDbRowSet::EReadOnly, KIdentitiesIndex);
	TInt idCount = tableId.CountL();
	CleanupStack::PopAndDestroy(&tableId);
	
	return (trainCount < idCount) ? EAuthTrained : EAuthFullyTrained;
	}


// -------- preferences --------


void CAuthDb2::SetPreferredPluginL(TAuthPluginType aPluginType, TPluginId aPluginId)
/**
	Set the preferred plugin for the supplied plugin type.

	If the type already has a preferred plugin type, its entry is
	updated.  Otherwise, a new entry is created.

	@param	aPluginType		Type of plugin to associate with a specific
							plugin ID.
	@param	aPluginId		Specific plugin to use for the plugin type.
 */
	{
	RDbTable table;

	// start automatic transaction
	if (FindPrefLC(table, RDbRowSet::EUpdatable, aPluginType))
		table.UpdateL();
	else
		table.InsertL();
	
	table.SetColL(KPrefsTypeCol, (TUint) aPluginType);
	table.SetColL(KPrefsIdCol, (TUint) aPluginId);
	table.PutL();			// end automatic transaction

	CleanupStack::PopAndDestroy(&table);
	}


void CAuthDb2::ClearPreferredPluginL(TAuthPluginType aPluginType)
/**
	Remove and preferred plugin ID for the supplied plugin type.
	
	If the supplied plugin type does not have a preferred plugin
	then the database is not modified.

	@param	aPluginType		Type of plugin to remove from the
							Prefs table.
 */
	{
	RDbTable table;
	if (FindPrefLC(table, RDbRowSet::EUpdatable, aPluginType))
		table.DeleteL();

	CleanupStack::PopAndDestroy(&table);
	}


TPluginId CAuthDb2::PreferredPluginL(TAuthPluginType aPluginType)
/**
	Retrieve the preferred plugin for the supplied plugin type.
	If the plugin type does not have a preferred plugin this function
	returns KUnknownPluginId.

	@param	aPluginType		Plugin type to find preferred plugin for.
	@return					Preferred plugin for aPluginType.  This is
							KUnknownPluginId if the type does not
							have a preferred plugin.
 */
	{
	TPluginId pluginId;

	RDbTable table;
	if (! FindPrefLC(table, RDbRowSet::EReadOnly, aPluginType))
		pluginId = KUnknownPluginId;
	else
		{
		table.GetL();
		pluginId = static_cast<TPluginId>(table.ColUint(KPrefsIdCol));
		}

	CleanupStack::PopAndDestroy(&table);
	return pluginId;
	}


TBool CAuthDb2::FindPrefLC(RDbTable& aTable, RDbRowSet::TAccess aAccess, TAuthPluginType aPluginType)
/**
	Open the Prefs table and navigate to the supplied plugin type.
	On success the table handle is placed on the cleanup stack.

	@param	aTable			Table handle to open.  On success this
							is placed on the cleanup stack.
	@param	aAccess			Mode in which the table should be opened.
	@param	aPluginType		Plugin type to navigate to.
	@return					Zero if could not find plugin type;
							Non-zero otherwise.
 */
	{
	return OpenTableAndNavigateLC(aTable, KPrefsTableName, aAccess, KPrefsTypeIndex, aPluginType);
	}


// -------- trained plugins --------


void CAuthDb2::SetTrainedPluginL(
	TIdentityId aIdentityId, TPluginId aPluginId, const CTransientKeyInfo& aTransKeyInfo)
/**
	Sets the transient key for the supplied identity, plugin pair.

	If the identity has already been trained for this plugin
	then the existing record is replaced.

	@param	aIdentityId		Identity to update.
	@param	aPluginId		Plugin to associate with identity.
	@param	aTransKeyInfo	The salt and encrypted protection key
							for this identity / plugin pair.
	@leave	KErrAuthServIdentityNotFound The supplied identity has
							not been registered.
 */
	{
	RDbTable table;

	TBool update = FindTrainedPluginLC(table, RDbRowSet::EUpdatable, aIdentityId, aPluginId);
	
	// start automatic transaction
	if (update)
		{
		table.UpdateL();
		}
	else
		{
		CheckIdentityRegisteredL(aIdentityId);
		table.InsertL();
		}

	table.SetColL(KTpIdentityIdCol, (TUint) aIdentityId);
	table.SetColL(KTpPluginIdCol, (TUint) aPluginId);
	
	RDbColWriteStream dbcws;
	dbcws.OpenLC(table, KTpTransientKeyCol);
	aTransKeyInfo.ExternalizeL(dbcws);
	dbcws.CommitL();
	CleanupStack::PopAndDestroy(&dbcws);
	table.PutL();
	
	CleanupStack::PopAndDestroy(&table);
	}


void CAuthDb2::RemoveTrainedPluginL(TIdentityId aIdentityId, TPluginId aPluginId)
/**
	Remove the transient key info (i.e. the encrypted protection key
	and its salt) for the supplied identity, plugin pair.

	@param	aIdentityId		Identity which should be trained for
							the supplied plugin.
	@param	aPluginId		Plugin which should be trained for the
							supplied identity.
	@leave	KErrAuthServTrainingNotFound Attempted to remove an
							identity, plugin pair which was not in the
							trained plugins table.
 */
	{
	RDbTable table;
	TBool found = FindTrainedPluginLC(table, RDbRowSet::EUpdatable, aIdentityId, aPluginId);

	if (! found)
		User::Leave(KErrAuthServTrainingNotFound);

	table.DeleteL();
	CleanupStack::PopAndDestroy(&table);
	}


CTransientKeyInfo* CAuthDb2::KeyInfoL(TIdentityId aIdentity, TPluginId aPluginId)
/**
	Retrieve the transient key associated with the supplied identity,
	plugin pair.

	@param	aIdentity		Identity to search for.
	@param	aPluginId		Plugin which should be registered
							with the supplied identity.
	@return					Transient key info, including encrypted
							protection key, associated with identity,
							plugin pair.
	@leave KErrAuthServTrainingNotFound There was no transient key
							information for the supplied identity,
							plugin pair.
 */
	{
	RDbTable table;
	TBool found = FindTrainedPluginLC(table, RDbRowSet::EReadOnly, aIdentity, aPluginId);
	if (! found)
		User::Leave(KErrAuthServTrainingNotFound);

	table.GetL();
	RDbColReadStream dbcrs;
	dbcrs.OpenLC(table, KTpTransientKeyCol);
	CTransientKeyInfo* keyInfo = CTransientKeyInfo::NewL(dbcrs);

	CleanupStack::PopAndDestroy(2, &table);		// dbcrs, table
	return keyInfo;
	}


TBool CAuthDb2::FindTrainedPluginLC(
	RDbTable& aTable, RDbRowSet::TAccess aAccess,
	TIdentityId aIdentityId, TPluginId aPluginId)
/**
	Helper function for SetTrainedPluginL and ClearTrainedPluginL.
	This function opens the TrainedPlugins table and navigates to
	the supplied identity, plugin pair if they are present.

	@param	aTable			Table handle to open.
	@param	aAccess			Mode in which to open the table.
	@param	aIdentityId		Identity to navigate to.
	@param	aPluginId		Plugin to navigate to.
	@return					Zero if could not navigate to the
							identity, plugin ID pair; non-zero
							otherwise.
 */
	{
	OpenTableLC(aTable, KTpTableName, aAccess, KTpIndex);

	TDbSeekMultiKey<2> dbsmk;
	dbsmk.Add((TUint) aIdentityId);
	dbsmk.Add((TUint) aPluginId);
	return aTable.SeekL(dbsmk);
	}


// -------- helper functions --------


TBool CAuthDb2::OpenTableAndNavigateLC(
	RDbTable& aTable, const TDesC& aTableName, RDbRowSet::TAccess aAccess,
	const TDesC& aIndexName, TUint aKey)
/**
	Open the described table and navigate to the identified row.

	@param	aTable			Caller's table handle.  On success
							this is placed on the cleanup stack.
	@param	aTableName		Table to open in auth db.
	@param	aAccess			Mode to open table in.
	@param	aIndexName		Index to use for navigating.
	@param	aKey			Key to find in database.
	@return					Zero if could not find row in table;
							Non-zero otherwise.
 */
	{
	OpenTableLC(aTable, aTableName, aAccess, aIndexName);

	TDbSeekKey seekKey((TUint)aKey);	// from TUint32
	return aTable.SeekL(seekKey);
	}


void CAuthDb2::OpenTableLC(
	RDbTable& aTable, const TDesC& aTableName,
	RDbRowSet::TAccess aAccess, const TDesC& aIndexName)
/**
	Open the named table in the supplied access mode,
	and use the index on it.

	@param	aTable			Caller's table handle.  On success
							this is placed on the cleanup stack.
	@param	aTableName		Table to open in the authentication db.
	@param	aAccess			Mode to open table in.
	@param	aIndexName		Index to activate on table.
 */
	{
	CleanupClosePushL(aTable);
	User::LeaveIfError(aTable.Open(iAuthDbFile, aTableName, aAccess));

	if (aAccess != RDbRowSet::EInsertOnly)
		{
		User::LeaveIfError(aTable.SetIndex(aIndexName));
		}
	}


void CAuthDb2::CheckIdentityRegisteredL(TUint aIdentityId)
/**
	Checks if the supplied identity has been registered with
	the database.  If not, it leaves with KErrAuthServIdentityNotFound.
	
	This function is used to validate arguments to other functions.
	
	@param	aIdentityId		Identity to search for.
	@leave	KErrAuthServIdentityNotFound The supplied identity has not
							been registered.
 */
	{
	CheckItemRegisteredL(
		KIdentitiesTableName, KIdentitiesIndex,
		aIdentityId, KErrAuthServIdentityNotFound);
	}


void CAuthDb2::CheckItemRegisteredL(
	const TDesC& aTableName, const TDesC& aIndexName,
	TUint aValue, TInt aNotFoundError)
/**
	Helper function for CheckIdentityRegisteredL.
	
	@param	aTableName		Name of table to search.
	@param	aIndexName		Name of index to use on table.
	@param	aValue			Key value to search for.
	@param	aNotFoundError	If the key value cannot be found
		`					then leave with this value.
 */
 	{
	RDbTable table;
	if (! OpenTableAndNavigateLC(table, aTableName, RDbRowSet::EReadOnly, aIndexName, aValue))
		User::Leave(aNotFoundError);
	CleanupStack::PopAndDestroy(&table);
 	}


// -------- compaction --------


const TInt KBlockSize = 512;
const TInt KBlockMask = KBlockSize - 1;


void CAuthDb2::CompactIfRequired()
/**
	Compacts the database file if compacting it would
	recover at least one 512 byte block.
	
	This function does not report any error status
	but, if the database is damaged, ConstructL will
	attempt to repair it.
 */
	{
	TInt r = iAuthDbFile.UpdateStats();
	
	if (r == KErrNone)
		{
		RDbDatabase::TSize sz = iAuthDbFile.Size();
		
		// approx used bytes - sz.iUsage is a percentage
		TInt usedBytes = sz.iUsage * 100;
		if ((usedBytes & ~KBlockMask) < (sz.iSize & ~KBlockMask))
			/* ignore */ iAuthDbFile.Compact();
		}
	}

TIdentityId CAuthDb2::DefaultIdentityL()
/**
 	Return the default identity which is the first identity
 	in the table when indexed based on the order of insertion.
*/
	{
	TIdentityId identity(0);
	RDbTable table;	
	OpenTableLC(table, KIdentitiesTableName, RDbRowSet::EReadOnly, KIdIndexNum);
	
	//Position the cursor on the first row and retrieve the Identity.
	if (table.FirstL())
		{
		table.GetL();
		identity = table.ColUint32(KIdentitiesIdentityIdCol);
		}	
		
	CleanupStack::PopAndDestroy(&table);
	return identity;
	}
