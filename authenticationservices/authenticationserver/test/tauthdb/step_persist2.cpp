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
*
*/


#include <s32mem.h>
#include "tauthdbstep.h"
#include <f32file.h>

using namespace AuthServer;

const TIdentityId KIdentity1Id = 'I1ID';
const TIdentityId KIdentity2Id = 'I2ID';
const TIdentityId KIdentity3Id = 'I3ID';
_LIT(KIdentity1Desc, "identity-1-desc");
_LIT(KIdentity1DescB, "identity-1-descB");
_LIT(KIdentity2Desc, "identity-2-desc");
_LIT(KIdentity3Desc, "identity-3-desc");

const TPluginId KPlugin1Id = 'PID1';
const TPluginId KPlugin2Id = 'PID2';
const TPluginId KPlugin3Id = 'PID3';
const TPluginId KPlugin4Id = 'PID4';
const TPluginId KPluginAId = 'PIDA';
const TPluginId KPluginBId = 'PIDB';


CTStepPersist2::CTStepPersist2()
	{
	SetTestStepName(KTStepCreateTestDb);
	}


TVerdict CTStepPersist2::doTestStepL()
	{
	__UHEAP_MARK;
	
	RemoveExistingDbL();
		
	TestOpenCloseL();
	TestIdentitiesL();
	TestPrefsL();
	TestTrainedPluginsL();
	TestTrainingStatusL();
	TestRepairL();
	TestCompressL();
	
	__UHEAP_MARKEND;
	return EPass;
	}


void CTStepPersist2::TestOpenCloseL()
/**
	Test simply opening and closing a database,
	without using any other operations.
 */
	{
	CAuthDb2* db = CAuthDb2::NewL(iFs);
	delete db;
	}


void CTStepPersist2::TestIdentitiesL()
	{
	TInt r;
	
	__UHEAP_MARK;
	RemoveExistingDbL();
	CAuthDb2* db = CAuthDb2::NewLC(iFs);
	
	RArray<TIdentityId> ids;
	
	// get id list before any ids defined
	ids.AppendL('SYMB');	// garbage; ensure removed
	db->IdentitiesL(ids);
	TESTL(ids.Count() == 0);
	
	// add an identity
	db->AddIdentityL(KIdentity1Id, KIdentity1Desc);
	db->IdentitiesL(ids);
	TESTL(ids.Count() == 1);
	TESTL(ids[0] == KIdentity1Id);
	
	// retrieve the identity's description
	TestDescriptionL(db, KIdentity1Id, KIdentity1Desc);
	
	// change the identity's description
	db->SetDescriptionL(KIdentity1Id, KIdentity1DescB);
	TestDescriptionL(db, KIdentity1Id, KIdentity1DescB);

	// change the identity's description to a string which is
	// too long; ensure fails.
	TBuf<KMaxDescLen + 1> dbuf;
	dbuf.Fill('a', KMaxDescLen - 1);		// limit - 1
	db->SetDescriptionL(KIdentity1Id, dbuf);
	TestDescriptionL(db, KIdentity1Id, dbuf);
	
	dbuf.Fill('a', KMaxDescLen);			// limit
	db->SetDescriptionL(KIdentity1Id, dbuf);
	TestDescriptionL(db, KIdentity1Id, dbuf);
	
	db->SetDescriptionL(KIdentity1Id, KIdentity1DescB);
	TestDescriptionL(db, KIdentity1Id, KIdentity1DescB);
	dbuf.Fill('a', KMaxDescLen + 1);		// limit + 1
	TRAP(r, db->SetDescriptionL(KIdentity1Id, dbuf));
	TESTL(r == KErrAuthServDescTooLong);
	TestDescriptionL(db, KIdentity1Id, KIdentity1DescB);	// keep old desc
	
	// add a second identity
	db->AddIdentityL(KIdentity2Id, KIdentity2Desc);
	db->IdentitiesL(ids);
	TESTL(ids.Count() == 2);
	TESTL(	(ids[0] == KIdentity1Id && ids[1] == KIdentity2Id)
		||	(ids[0] == KIdentity2Id && ids[1] == KIdentity1Id) );
	TestDescriptionL(db, KIdentity1Id, KIdentity1DescB);
	TestDescriptionL(db, KIdentity2Id, KIdentity2Desc);
	
	// add an identity which already exists
	TRAP(r, db->AddIdentityL(KIdentity2Id, KIdentity2Desc));
	TESTL(r == KErrAuthServIdentityAlreadyExists);
	
	// retrieve description for an unregistered identity
	TRAP(r, db->DescriptionL(KIdentity3Id));
	TESTL(r == KErrAuthServIdentityNotFound);
	
	// set description for an unregistered identity
	TRAP(r, db->SetDescriptionL(KIdentity3Id, KIdentity3Desc));
	TESTL(r == KErrAuthServIdentityNotFound);
	
	// remove an unregistered identity
	TRAP(r, db->RemoveIdentityL(KIdentity3Id));
	TESTL(r == KErrAuthServIdentityNotFound);
	
	// remove an identity
	db->RemoveIdentityL(KIdentity1Id);
	db->IdentitiesL(ids);
	TESTL(ids.Count() == 1);
	TESTL(ids[0] == KIdentity2Id);
	TestDescriptionL(db, KIdentity2Id, KIdentity2Desc);
	
	// remove the last identity
	db->RemoveIdentityL(KIdentity2Id);
	db->IdentitiesL(ids);
	TESTL(ids.Count() == 0);
	
	// create an identity with a description to a string which is
	// too long; ensure fails.
	dbuf.Fill('a', KMaxDescLen - 1);		// limit - 1
	db->AddIdentityL(KIdentity3Id, dbuf);
	TestDescriptionL(db, KIdentity3Id, dbuf);
	db->RemoveIdentityL(KIdentity3Id);
	
	dbuf.Fill('a', KMaxDescLen);			// limit
	db->AddIdentityL(KIdentity3Id, dbuf);
	TestDescriptionL(db, KIdentity3Id, dbuf);
	db->RemoveIdentityL(KIdentity3Id);
	
	dbuf.Fill('a', KMaxDescLen + 1);		// limit + 1
	TRAP(r, db->AddIdentityL(KIdentity3Id, dbuf));
	TESTL(r == KErrAuthServDescTooLong);
	db->IdentitiesL(ids);							// no identity created
	TESTL(ids.Count() == 0);
	
	CleanupStack::PopAndDestroy(db);
	__UHEAP_MARKEND;
	}

void CTStepPersist2::TestDescriptionL(
	CAuthDb2* aDb, TIdentityId aIdentityId, const TDesC& aExpDesc)
/**
	Helper function for TestIdentitiesL.  Ensures the
	supplied identity has the supplied description.
	
	@param	aDb				Database which contains the identity.
	@param	aIdentity		Identity whose description should be
							retrieved.
	@param	aDesc			Expected description.
 */
	{
	HBufC* desc = aDb->DescriptionL(aIdentityId);
	CleanupStack::PushL(desc);
	TESTL(*desc == aExpDesc);
	CleanupStack::PopAndDestroy(desc);
	}


void CTStepPersist2::TestPrefsL()
/**
	Tests adding, modifying, and removing
	preferences from the database.
 */
	{
	TPluginId prefId;	
	__UHEAP_MARK;
	
	RemoveExistingDbL();
	CAuthDb2* db = CAuthDb2::NewLC(iFs);

	// get a preferred plugin for a type with no preferred plugin
	prefId = db->PreferredPluginL(EAuthBiometric);
	TESTL(prefId == KUnknownPluginId);

	// set a type's preferred plugin
	db->SetPreferredPluginL(EAuthBiometric, KPlugin1Id);
	prefId = db->PreferredPluginL(EAuthBiometric);
	TESTL(prefId == KPlugin1Id);
	
	// change a type's preferred plugin
	db->SetPreferredPluginL(EAuthBiometric, KPlugin2Id);
	prefId = db->PreferredPluginL(EAuthBiometric);
	TESTL(prefId == KPlugin2Id);
	
	// clear a type's preferred plugin
	db->ClearPreferredPluginL(EAuthBiometric);
	prefId = db->PreferredPluginL(EAuthBiometric);
	TESTL(prefId == KUnknownPluginId);
	
	// set preferred plugin for more than one type
	db->SetPreferredPluginL(EAuthBiometric, KPlugin3Id);
	db->SetPreferredPluginL(EAuthKnowledge, KPlugin4Id);
	prefId = db->PreferredPluginL(EAuthBiometric);
	TESTL(prefId == KPlugin3Id);
	prefId = db->PreferredPluginL(EAuthKnowledge);
	TESTL(prefId == KPlugin4Id);
	
	CleanupStack::PopAndDestroy(db);
	
	__UHEAP_MARKEND;
	}


void CTStepPersist2::TestTrainedPluginsL()
/**
	Test adding and removing trained plugins.
 */
	{
	__UHEAP_MARK;
	
	TInt r;
	RemoveExistingDbL();
	CAuthDb2* db = CAuthDb2::NewLC(iFs);

	const CTransientKeyInfo& tkiA = *iId1Keys[0];
	const CTransientKeyInfo& tkiB = *iId1Keys[1];
	const CTransientKeyInfo& tkiC = *iId1Keys[2];

	// set key info for an unregistered identity
	TRAP(r, db->SetTrainedPluginL(KIdentity1Id, KPluginAId, tkiA));
	TESTL(r == KErrAuthServIdentityNotFound);

	// set key info for a registered identity
	db->AddIdentityL(KIdentity1Id, KIdentity1Desc);
	db->SetTrainedPluginL(KIdentity1Id, KPluginAId, tkiA);
	
	// get key info for a trained identity
	TestKeyPresentL(db, KIdentity1Id, KPluginAId, tkiA);

	// change key info for a trained identity
	db->SetTrainedPluginL(KIdentity1Id, KPluginAId, tkiB);
	TestKeyPresentL(db, KIdentity1Id, KPluginAId, tkiB);

	// ---- bad KeyInfoL args ----
	
	// get key info for registered identity but untrained plugin
	TRAP(r, db->KeyInfoL(KIdentity1Id, KPluginBId))
	TESTL(r == KErrAuthServTrainingNotFound);

	// get key info for unregistered identity but trained plugin	
	TRAP(r, db->KeyInfoL(KIdentity3Id, KPluginAId))
	TESTL(r == KErrAuthServTrainingNotFound);
	
	// get key info for unregistered identity and untrained plugin
	TRAP(r, db->KeyInfoL(KIdentity3Id, KPluginBId));
	TESTL(r == KErrAuthServTrainingNotFound);
	
	// ---- bad RemoveTrainedPluginL args ----
	
	// clear key info for registered identity but untrained plugin
	TRAP(r, db->KeyInfoL(KIdentity1Id, KPluginBId))
	TESTL(r == KErrAuthServTrainingNotFound);

	// clear key info for unregistered identity but trained plugin	
	TRAP(r, db->RemoveTrainedPluginL(KIdentity3Id, KPluginAId))
	TESTL(r == KErrAuthServTrainingNotFound);
	
	// clear key info for unregistered identity and untrained plugin
	TRAP(r, db->RemoveTrainedPluginL(KIdentity3Id, KPluginBId));
	TESTL(r == KErrAuthServTrainingNotFound);
	
	// -----

	// clear a present trained plugin
	db->RemoveTrainedPluginL(KIdentity1Id, KPluginAId);
	TRAP(r, db->KeyInfoL(KIdentity3Id, KPluginAId))
	TESTL(r == KErrAuthServTrainingNotFound);
	
	// register multiple trained plugins for a single identity	
	db->SetTrainedPluginL(KIdentity1Id, KPluginAId, tkiA);
	db->SetTrainedPluginL(KIdentity1Id, KPluginBId, tkiB);
	TestKeyPresentL(db, KIdentity1Id, KPluginAId, tkiA);
	TestKeyPresentL(db, KIdentity1Id, KPluginAId, tkiB);

	// train a single plugin for multiple identities
	db->AddIdentityL(KIdentity2Id, KIdentity2Desc);
	db->SetTrainedPluginL(KIdentity2Id, KPluginAId, tkiC);
	TestKeyPresentL(db, KIdentity1Id, KPluginAId, tkiA);
	TestKeyPresentL(db, KIdentity1Id, KPluginBId, tkiB);
	TestKeyPresentL(db, KIdentity2Id, KPluginAId, tkiC);
	
	// ---- atomic identity and key addition ----
	
	// add identity with trained plugin
	db->AddIdentityWithTrainedPluginL(KIdentity3Id, KIdentity3Desc, tkiA);
	TestKeyPresentL(db, KIdentity3Id, tkiA.PluginId(), tkiA);
	
	// fail to add identity with trained plugin - id already exists
	TRAP(r, db->AddIdentityWithTrainedPluginL(KIdentity3Id, KIdentity3Desc, tkiA));
	TESTL(r == KErrAuthServIdentityAlreadyExists);
	
	CleanupStack::PopAndDestroy(db);
	
	__UHEAP_MARKEND;
	}


void CTStepPersist2::TestKeyPresentL(
	CAuthDb2* aAuthDb, TIdentityId aIdentityId, TPluginId aPluginId,
	const CTransientKeyInfo& aTarget)
/**
	Helper function for TestTrainedPluginsL.
	
	Test the described transient key exists in the database,
	and that is is equal to the supplied key.  Leaves if not
	the case.

	@param	aAuthDb			Authorisation database to retrieve
							transient key info from.
	@param	aIdentityId		Identity which should be trained for
							the supplied plugin.
	@param	aPluginId		Plugin for which the supplied identity
							should be trained.
	@param	aTarget			The retrieved transient key info should
							be equal to this.
 */
	{
	CTransientKeyInfo* tkiActual = aAuthDb->KeyInfoL(aIdentityId, aPluginId);
	CleanupStack::PushL(tkiActual);

	// ensure key infos have same externalized size
	TSizeStream ssTarget;
	RWriteStream wsTarget(&ssTarget);
	aTarget.ExternalizeL(wsTarget);
	TInt targetSize = ssTarget.Size();

	TSizeStream ssActual;
	RWriteStream wsActual(&ssActual);
	tkiActual->ExternalizeL(wsActual);
	TESTL(targetSize == ssActual.Size());

	HBufC8* targetBuf = HBufC8::NewLC(targetSize);
	TPtr8 targetBufDes = targetBuf->Des();
	RDesWriteStream dwsTarget(targetBufDes);
	aTarget.ExternalizeL(dwsTarget);

	HBufC8* actualBuf = HBufC8::NewLC(targetSize);
	TPtr8 actualBufDes = actualBuf->Des();
	RDesWriteStream dwsActual(actualBufDes);
	tkiActual->ExternalizeL(dwsActual);

	TESTL(targetBufDes == actualBufDes);

	CleanupStack::PopAndDestroy(3, tkiActual);
	}


void CTStepPersist2::TestTrainingStatusL()
/**
	Test the plugins' training statuses accurately
	reflect the identities which are registered
	with them.
 */
	{
	__UHEAP_MARK;

	RemoveExistingDbL();
	
	CAuthDb2* db = CAuthDb2::NewLC(iFs);

	// if there are no users then a plugin should be marked
	// as untrained, even though its training count is equal
	// to the number of registered identities.
	TESTL(db->PluginStatusL(KPluginAId) == EAuthUntrained);

	// untrained when no users trained
	db->AddIdentityL(KIdentity1Id, KIdentity1Desc);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthUntrained);
	db->AddIdentityL(KIdentity2Id, KIdentity2Desc);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthUntrained);

	// trained when some, but not all, users trained
	const CTransientKeyInfo& tkiA = *iId1Keys[0];
	db->SetTrainedPluginL(KIdentity1Id, KPluginAId, tkiA);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthTrained);
	
	// fully trained when all users trained
	db->SetTrainedPluginL(KIdentity2Id, KPluginAId, tkiA);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthFullyTrained);
	
	// back to trained when identity untrained
	db->RemoveTrainedPluginL(KIdentity1Id, KPluginAId);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthTrained);
	
	// back to untrained when last training removed
	db->RemoveTrainedPluginL(KIdentity2Id, KPluginAId);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthUntrained);
	
	// restored to trained when identity trained
	db->SetTrainedPluginL(KIdentity1Id, KPluginAId, tkiA);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthTrained);

	// restored to fully trained when last identity trained
	db->SetTrainedPluginL(KIdentity2Id, KPluginAId, tkiA);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthFullyTrained);
	
	// back to trained when new identity added
	db->AddIdentityL(KIdentity3Id, KIdentity3Desc);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthTrained);

	// upgraded to fully trained when untrained identity removed
	db->RemoveIdentityL(KIdentity3Id);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthFullyTrained);
	
	// kept at fully trained when trained identity removed
	db->RemoveIdentityL(KIdentity2Id);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthFullyTrained);
	
	// drop to untrained when last identity removed
	db->RemoveIdentityL(KIdentity1Id);
	TESTL(db->PluginStatusL(KPluginAId) == EAuthUntrained);
	
	CleanupStack::PopAndDestroy(db);
	
	__UHEAP_MARKEND;
	}


static void IdToTestDesc(TIdentityId aId, TDes& aDesc)
/**
	Helper function for TestRepairL generates a description
	from the supplied identity.
	
	@param	aId				Identity Id.
	@param	aDesc			Out paramater is populated with
							description text.
 */
	{
	_LIT(KDescFmt, "desc_%08x");
	aDesc.Format(KDescFmt, aId);
	}


void CTStepPersist2::TestRepairL()
/**
	Test CAuthDb repairs the database if possible.
 */
	{
	RemoveExistingDbL();
	
	CAuthDb2* db = CAuthDb2::NewLC(iFs);
	
	// create a set of identities
	const TInt KTestIdCount = 8;
	for (TInt i = 1; i <= KTestIdCount; ++i)
		{
		TBuf<13> buf;
		IdToTestDesc(i, buf);
		db->AddIdentityL(i, buf);
		}
	CleanupStack::PopAndDestroy(db);
	
	// damage the database by inserting a new entry but then
	// rolling it back.
	RDbNamedDatabase ndb;
	TFileName dbName(KDbName);
	dbName[0] = RFs::GetSystemDriveChar();
	
	User::LeaveIfError(ndb.Open(iFs, dbName));
	CleanupClosePushL(ndb);
	
	User::LeaveIfError(ndb.Begin());
	
	RDbTable table;
	CleanupClosePushL(table);
	User::LeaveIfError(table.Open(ndb, KIdentitiesTableName));
	
	table.InsertL();
	table.SetColL(KIdentitiesIdentityIdCol, KTestIdCount+1);
	table.SetColL(KIdentitiesDescCol, _L("descb"));
	table.PutL();
	table.Close();
	ndb.Rollback();
	TESTL(ndb.IsDamaged());
	
	CleanupStack::PopAndDestroy(2, &ndb);	// table ndb
	
	// ensure db contains the same identities when it
	// is reopened.
	db = CAuthDb2::NewLC(iFs);
	
	RArray<TIdentityId> ids;
	db->IdentitiesL(ids);
	TInt idCount = ids.Count();
	ids.Reset();
	TESTL(idCount == KTestIdCount);
	
	for (TInt index = 1; index <= KTestIdCount; ++index)
		{
		TBuf<13> bufExp;
		IdToTestDesc(index, bufExp);
		TBuf<KMaxDescLen> actDesc;
		HBufC* descAct = db->DescriptionL(index);
		CleanupStack::PushL(descAct);
		TESTL(bufExp == *descAct);
		CleanupStack::PopAndDestroy(descAct);
		}
	CleanupStack::PopAndDestroy(db);
	}


void CTStepPersist2::TestCompressL()
/**
	Creates and deletes identities to create unused
	space in database file, and tests compressed.
 */
	{
	RemoveExistingDbL();
	CAuthDb2* db = CAuthDb2::NewLC(iFs);
	
	const TInt KIdentityCount = 32;
	for (TInt i = 0; i < KIdentityCount; ++i)
		{
		_LIT(KCompDesc, "tc-test");
		db->AddIdentityL(i, KCompDesc);
		db->RemoveIdentityL(i);
		}
	CleanupStack::PopAndDestroy(db);
	
	TInt szPreComp = DbFileSizeL();
	
	db = CAuthDb2::NewLC(iFs);
	db->CompactIfRequired();
	CleanupStack::PopAndDestroy(db);
	
	TInt szPostComp = DbFileSizeL();
	
	TESTL(szPreComp > szPostComp);
	}


TInt CTStepPersist2::DbFileSizeL()
/**
	Helper function for TestCompressL returns
	the size of the database file in bytes.
 */
	{
	RFile f;
	TFileName dbName(KDbName);
	dbName[0] = RFs::GetSystemDriveChar();

	User::LeaveIfError(f.Open(iFs, dbName, EFileRead | EFileStream));
	CleanupClosePushL(f);
	
	TInt sz;
	User::LeaveIfError(f.Size(sz));
	CleanupStack::PopAndDestroy(&f);
	
	return sz;
	}



