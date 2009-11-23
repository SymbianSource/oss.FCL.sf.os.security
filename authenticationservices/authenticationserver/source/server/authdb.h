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
* CAuthDb Authentication server data store declaration
*
*/


/**
 @file 
 @released
 @internalComponent
*/

#ifndef AUTHDB_H
#define AUTHDB_H

#include <d32dbms.h>
#include "authserver/authtypes.h"
#include "authserver/idandstring.h"

namespace AuthServer
{

class CTransientKeyInfo;

NONSHARABLE_CLASS(CAuthDb2) : public CBase
	{
public:
	static CAuthDb2* NewL(RFs& aFs);
	static CAuthDb2* NewLC(RFs& aFs);
	virtual ~CAuthDb2();
	
	// identities
	TInt NumIdentitiesL();
	void AddIdentityWithTrainedPluginL(
		TIdentityId aIdentityId, const TDesC& aDesc,
		const CTransientKeyInfo& aTransKeyInfo);
	void AddIdentityL(TIdentityId aIdentityId, const TDesC& aDesc);
	void RemoveIdentityL(TIdentityId aIdentityId);
	void IdentitiesL(RArray<TIdentityId>& aResults);
	HBufC* DescriptionL(TIdentityId aIdentityId);
	void SetDescriptionL(TIdentityId aIdentityId, const TDesC& aDesc);
	void IdentitiesWithDescriptionL(RIdAndStringArray& aList);
	TIdentityId DefaultIdentityL();
	// plugins
	TInt NumPluginsL();
	TInt NumTrainedPluginsL(TIdentityId aId);
	// Returns list of plugins that are trained for the given identity
	void TrainedPluginsListL(TIdentityId aIdentityId, RArray<TPluginId>& aTrainedPlugins);
	TAuthTrainingStatus PluginStatusL(TPluginId aPluginId);

	// preferences
	void SetPreferredPluginL(TAuthPluginType aPluginType, TPluginId aPluginId);
	void ClearPreferredPluginL(TAuthPluginType aPluginType);
	TPluginId PreferredPluginL(TAuthPluginType aPluginType);

	// trained plugins
	void SetTrainedPluginL(
		TIdentityId aIdentityId, TPluginId aPluginId, const CTransientKeyInfo& aTransKeyInfo);
	void RemoveTrainedPluginL(TIdentityId aIdentityId, TPluginId aPluginId);
	CTransientKeyInfo* KeyInfoL(TIdentityId aIdentity, TPluginId aPluginId);

	void CompactIfRequired();
	void CloseDbFile();
	TInt OpenDbFileL(RFs& aFs, const TFileName& aDbName);
	
private:
	void ConstructL(RFs& aFs);

	void FindExistingIdentityLC(RDbTable& aTable, RDbRowSet::TAccess aAccess, TIdentityId aIdentityId);
	TBool FindIdentityLC(RDbTable& aTable, RDbRowSet::TAccess aAccess, TIdentityId aIdentityId);

	TBool FindPrefLC(RDbTable& aTable, RDbRowSet::TAccess aAccess, TAuthPluginType aPluginType);

	TBool FindTrainedPluginLC(
		RDbTable& aTable, RDbRowSet::TAccess aAccess,
		TIdentityId aIdentityId, TPluginId aPluginId);

	TBool OpenTableAndNavigateLC(
		RDbTable& aTable, const TDesC& aTableName, RDbRowSet::TAccess aAccess,
		const TDesC& aIndexName, TUint aKey);
	void OpenTableLC(
		RDbTable& aTable, const TDesC& aTableName,
		RDbRowSet::TAccess aAccess, const TDesC& aIndexName);
	void CheckIdentityRegisteredL(TUint aIdentityId);

	void CheckItemRegisteredL(
		const TDesC& aTableName, const TDesC& aIndexName,
		TUint aValue, TInt aNotFoundError);
	


private:	
	/**
	 * The database file itself, used to persist identities
	 * 	when the server is not in use.
	 */
	RDbNamedDatabase iAuthDbFile;
	
	/**
	 * Keep track of whether the database is open or not.
	 */
	TBool iIsDbOpen;
	
	};


// databases structure

_LIT(KDbName, "!:\\private\\102740FC\\auth.db");

/** Identities table name. */
_LIT(KIdentitiesTableName, "Identities");
/** Identities index number */
_LIT(KIdentitiesIndex, "identityIdx");
/** Identities index number.- Based on order of insertion */
_LIT(KIdIndexNum, "idIndex");
/** Identity column number in Identities table. */
const TInt KIdentitiesIdentityIdCol = 2;
/** Description column number in Identities table. */
const TInt KIdentitiesDescCol = 3;

/** Prefs table name. */
_LIT(KPrefsTableName, "Prefs");
/** Prefs index name. */
_LIT(KPrefsTypeIndex, "typeIdx");

/** Type column number in Prefs table. */
const TInt KPrefsTypeCol = 1;
/** PluginId column number is Prefs table. */
const TInt KPrefsIdCol = 2;

/** TrainedPlugins table name. */
_LIT(KTpTableName, "TrainedPlugins");
/** TrainedPlugins index name. */
_LIT(KTpIndex, "identityPluginIdx");

/** IdentityId column number in TrainedPlugins table. */
const TInt KTpIdentityIdCol = 1;
/** PluginId column number in TrainedPlugins table. */
const TInt KTpPluginIdCol = 2;
/** TransientKey column number in TrainedPlugins table. */
const TInt KTpTransientKeyCol = 3;

}	// namespace AuthServer

#endif // AUTHDB_H
