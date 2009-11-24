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


#include <test/testexecutelog.h>
#include "tauthcliservstep.h"
#include "authserver/aspubsubdefs.h"
#include <e32property.h>
#include <authserver/authpatchdata.h>
#include <u32hal.h> 
using namespace AuthServer;

class CAuthActive : CActive
    {
public:
    CAuthActive(RAuthClient& aClient, CIdentity*& aResult, CTStepClient* aStep,
				TBool aStop = ETrue) : 
         CActive(EPriorityNormal),
         iFirstTime(true),
         iClient(aClient), 
         iResult(aResult), 
         iStep(aStep),
		 iStop(aStop)
	    {
        CActiveScheduler::Add(this);
        }
	
    void doAuthenticate(TPluginId aPlugin, TTimeIntervalSeconds aTime )
        {
		iPlugin = aPlugin;
        SetActive();		
		TRequestStatus* status = &iStatus;		    
		User::RequestComplete(status, KErrNone);
		iFirstTime = ETrue;
		iTime = aTime;
        }
	
        void DoCancel() 
        {
        }
        void RunL() 
        {
		static TBool client = ETrue;
		static TBool withString = ETrue;
		client = !client;
        if (iFirstTime)
            {
			// inactive | plugin id
            iAe = AuthExpr(0x10274106) | AuthExpr(iPlugin);
			User::LeaveIfNull(iAe);
			SetActive();
            iStatus = KRequestPending;
            iClient.AuthenticateL(*iAe, iTime, client, withString,
								  iResult, iStatus);
            iFirstTime = false;
            }
        else
            {
			if (iStop)
				{
				CActiveScheduler::Stop();
				}
            delete iAe;
			}
		iErr =  iStatus.Int();
		}
    TBool iFirstTime;
    RAuthClient& iClient;
    CIdentity*& iResult;
    CTStepClient* iStep;
    CAuthExpression* iAe;
    TPluginId        iPlugin;
	TTimeIntervalSeconds iTime;
	TInt iErr;
	TBool iStop;
    };



const TPluginId KIdentity1Id = 'I1ID';
_LIT(KIdentity1Desc, "identity-1-desc");
const TPluginId KIdentity2Id = 'I2ID';
_LIT(KIdentity2Desc, "identity-2-desc");
const TPluginId KIdentity3Id = 'I3ID';


CTStepClient::CTStepClient()
/**
	Record this test step's name.
 */
	{
 	SetTestStepName(KTStepClient);
	}


void CTStepClient::TestIdentitiesL()
/**
	Test can retrieve correct identity arrays and
	descritptinos from auth db.
 */
	{
	__UHEAP_MARK;
	RemoveExistingDbL();
	
	RArray<TIdentityId> ids;
	

	// one identities (first start adds one)
	GetIdentitiesLC(ids);
	TESTL(ids.Count() == 1);
	CleanupStack::PopAndDestroy(&ids);

	
	// two identity
	AddIdentityL(KIdentity1Id, KIdentity1Desc);
	GetIdentitiesLC(ids);
	TESTL(ids.Count() == 2);
	TESTL(ids[0] == KIdentity1Id || ids[1] == KIdentity1Id);
	CleanupStack::PopAndDestroy(&ids);
	TestIdentityL(KIdentity1Id, KIdentity1Desc);
	
	
	// three identities
	AddIdentityL(KIdentity2Id, KIdentity2Desc);
	GetIdentitiesLC(ids);
	TESTL(ids.Count() == 3);
	TESTL(	ids.Find(KIdentity1Id) >= 0 && 
	        ids.Find(KIdentity2Id) >= 0);
	CleanupStack::PopAndDestroy(&ids);
	TestIdentityL(KIdentity1Id, KIdentity1Desc);
	TestIdentityL(KIdentity2Id, KIdentity2Desc);

	// test fail to get description for non-existent identity
	RAuthClient ac;
	User::LeaveIfError(ac.Connect());
	CleanupClosePushL(ac);
	
	
	TRAPD(r, ac.IdentityStringL(KIdentity3Id));
	TESTL(r == KErrAuthServIdentityNotFound);

	// check id and string retrieval
	RIdAndStringArray idWithString;
	ac.IdentitiesWithStringL(idWithString);
	CleanupClosePushL(idWithString);
	
    for (TInt i = 0 ; i < idWithString.Count() ; ++i)
	    {
	    TestIdentityL(idWithString[i]->Id(), *idWithString[i]->String());
		INFO_PRINTF3(_L("0x%x - %S"), idWithString[i]->Id(), idWithString[i]->String()); 
	    }

	CleanupStack::PopAndDestroy(2,&ac);
	
	__UHEAP_MARKEND;
	}


void CTStepClient::GetIdentitiesLC(RArray<TIdentityId>& aIds)
/**
	Populate the supplied array with the currently available
	identities, as returned by the server.
	
	@param	aIds			Array to populate.  Any existing entries
							are removed when this function is called.
							On success, the array is placed on the
							cleanup stack.
 */
	{
	aIds.Reset();
	CleanupClosePushL(aIds);
	
	RAuthClient ac;
	User::LeaveIfError(ac.Connect());
	CleanupClosePushL(ac);
	ac.IdentitiesL(aIds);	// clears array on failure?
	CleanupStack::PopAndDestroy(&ac);
	}


void CTStepClient::AddIdentityL(TIdentityId aIdentityId, const TDesC& aDesc)
/**
	Add the described identity directly to the database.
	
	This function does not use the authentication manager APIs.
 */
	{
	WaitForServerToReleaseDb();
	CAuthDb2* db = CAuthDb2::NewL(iFs);
	CleanupStack::PushL(db);
	db->AddIdentityL(aIdentityId, aDesc);
	CleanupStack::PopAndDestroy(db);
	}


void CTStepClient::TestIdentityL(TIdentityId aIdentityId, const TDesC& aExpDesc)
/**
	Test the supplied identity has the supplied description,
	as read via the RAuthClient API.
	
	@param	aIdentityId		Identity to retrieve description for.
	@param	aExpDesc		The expected description.
 */
	{
	RAuthClient ac;
	CleanupClosePushL(ac);
	User::LeaveIfError(ac.Connect());
	HBufC* desc = ac.IdentityStringL(aIdentityId);
	CleanupStack::PushL(desc);
	TESTL(*desc == aExpDesc);
	CleanupStack::PopAndDestroy(2, &ac);
	}


/**
	TestClientPluginListsL poulates this with a description for
	every plugin that can be seen with ECOM.
 */
static RCPointerArray<CPluginDesc> allPlugins;

/**
	Total number of plugins on the device.  Should be equal to
	allPlugins.Count().
 */
static TInt totalCount;

/**
	Maximum number of plugins which can are expected.  This is
	an arbitrary value which is used to size an array.  It can be
	changed if required.
 */
const TInt KMaxSeenCount = 17;

/**
	This array stores how many times each plugin from allPlugins
	has been seen.  It is set by SetSeenCountsL.
 */
static TFixedArray<TInt, KMaxSeenCount> seenCounts;


void CTStepClient::GetDescriptionsFromEComL(RPointerArray<CPluginDesc>& aDescs)
/**
	Populate the supplied array with plugin descriptions generated
	from the available ECOM plugins.  The array can then be used to
	check the results from the server.
 */
	{
	WaitForServerToReleaseDb();
	
	CAuthDb2* db = CAuthDb2::NewL(iFs);
	CleanupStack::PushL(db);
	
	// typedef RPointerArray<CImplementationInformation> RImplInfoPtrArray;
	RCPointerArray<CImplementationInformation> implInfo;
	REComSession::ListImplementationsL(KCAuthPluginInterfaceUid, implInfo);
	CleanupClosePushL(implInfo);
	
	TInt implCount = implInfo.Count();
	TInt err = 0;
	for (TInt i = 0; i < implCount; ++i)
		{
		// avoid RVCT warning C2874W: pi may be used before being set
		CAuthPluginInterface* pi = 0;

		TEComResolverParams resolverParams;
		TBufC8<16> pluginIdTxt;
		  
		pluginIdTxt.Des().Format(_L8("%x"), implInfo[i]->ImplementationUid().iUid);
		pluginIdTxt.Des().UpperCase();	  
		resolverParams.SetDataType(pluginIdTxt);
		 
		//To load plugins from sources other than ROM the patch 
		// data KEnablePostMarketAuthenticationPlugins must be set to True.
		TUint32 enablePostMarketPlugin = KEnablePostMarketAuthenticationPlugins;

#ifdef __WINS__

    	// Default SymbianOS behavior is to only load auth plugins from ROM.
		enablePostMarketPlugin = 0;

		// For the emulator allow the constant to be patched via epoc.ini
		UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
		(TAny*)"KEnablePostMarketAuthenticationPlugins", &enablePostMarketPlugin); // read emulator property (if present)

#endif			  
		TAny* plugin = 0; 
		TUid Dtor_ID_Key = TUid::Null();
		if(enablePostMarketPlugin == 0) 
			{
			TRAP(err, plugin = 
			REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
												Dtor_ID_Key,
												resolverParams,
												KRomOnlyResolverUid));
			}
			  
		else
		  	{
		  	TRAP(err, plugin = 
			REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
												Dtor_ID_Key,
												resolverParams));
		  	}
					  
		if (err == KErrAuthServNoSuchPlugin)
			continue;
		User::LeaveIfError(err);
		pi = reinterpret_cast<CAuthPluginInterface*>(plugin);	
	
		// get training status from db
		TAuthTrainingStatus ts = db->PluginStatusL(pi->Id());
		
		CleanupStack::PushL(pi);
		CPluginDesc* pd = CPluginDesc::NewL(
			pi->Id(), pi->Name(), pi->Type(),
			ts, pi->MinEntropy(),
			pi->FalsePositiveRate(), pi->FalseNegativeRate() );
		CleanupStack::PopAndDestroy(pi);
		REComSession::DestroyedImplementation(Dtor_ID_Key);
		
		CleanupStack::PushL(pd);
		aDescs.AppendL(pd);
		CleanupStack::Pop(pd);
		}
	
	CleanupStack::PopAndDestroy(2, db);
	REComSession::FinalClose();
	}


static TBool AreDescsEqual(const CPluginDesc& aLeft, const CPluginDesc& aRight)
/**
	Predicate function determines whether the two supplied
	plugin descriptions contain the same information.

	@param	aLeft			Plugin description to compare to aRight.
	@param	aRight			Plugin description to compare to aLeft.
	@return					Zero if the two descriptions contains different
							information, non-zero otherwise.
 */
	{
	return	aLeft.Id() == aRight.Id()
		&&	*aLeft.Name() == *aRight.Name()
		&&	aLeft.Type() == aRight.Type()
		&&	aLeft.TrainingStatus() == aRight.TrainingStatus()
		&&	aLeft.MinEntropy() == aRight.MinEntropy()
		&&	aLeft.FalsePositiveRate() == aRight.FalsePositiveRate()
		&&	aLeft.FalseNegativeRate() == aRight.FalseNegativeRate();
	}


inline TBool AreDescsEqual(const CPluginDesc* aLeftP, const CPluginDesc* aRightP)
	{
	return AreDescsEqual(*aLeftP, *aRightP);
	}
	

void CTStepClient::SetSeenCountsL(const RCPointerArray<const CPluginDesc>& aPlugins)
/**
	Update seenCounts array so each entry contains the
	number of times that plugin was seen in aPlugins.
 */
	{
	const TInt suppliedCount = aPlugins.Count();
	const TInt totalCount = allPlugins.Count();
	
	// i = index into seenCounts
	for (TInt i = 0; i < totalCount; ++i)
		{
		seenCounts[i] = 0;
		
		// j = index into aPlugins
		TInt j = 0;
		for (; j < suppliedCount; ++j)
			{
			if (AreDescsEqual(*allPlugins[i], *aPlugins[j]))
				{
				++seenCounts[i];
				break;		// assume plugins in allPlugins are unique
				}
			}
		
		// if plugin not found in reference set then abort
		TESTL(j < totalCount);
		}
	}


void CTStepClient::TestClientPluginListsL()
/**
	Test the expected plugins are returned when the
	client asks the server to list them.
 */
	{	
	__UHEAP_MARK;
	
	CleanupClosePushL(allPlugins);
	GetDescriptionsFromEComL(allPlugins);
	PrepareTrainedPluginsL();
	
	// re-read plugins now training statuses have been set
	allPlugins.ResetAndDestroy();	
	GetDescriptionsFromEComL(allPlugins);
	CleanupClosePushL(allPlugins);
	
	totalCount = allPlugins.Count();
	TESTL(totalCount <= KMaxSeenCount);
	
	RAuthClient ac;
	User::LeaveIfError(ac.Connect());
	CleanupClosePushL(ac);
	
	// ensure all plugins are returned by PluginsL
	RCPointerArray<const CPluginDesc> fullPlugins;
	ac.PluginsL(fullPlugins);
	CleanupClosePushL(fullPlugins);
	TESTL(fullPlugins.Count() == totalCount);
	SetSeenCountsL(fullPlugins);
	for (TInt k = 0; k < totalCount; ++k)
		{
		TESTL(seenCounts[k] == 1);
		}
	CleanupStack::PopAndDestroy(&fullPlugins);
	
	// ensure expected plugins are returned for each type
	TestTypedPluginsL(ac, EAuthKnowledge);
	TestTypedPluginsL(ac, EAuthBiometric);
	TestTypedPluginsL(ac, EAuthToken);

	TestActivePluginsL(ac);
	
	CleanupStack::PopAndDestroy(&ac);
	
	TestTrainedPluginsL();
	
	CleanupStack::PopAndDestroy(2, &allPlugins);
	
	__UHEAP_MARKEND;
	}


void CTStepClient::TestTypedPluginsL(RAuthClient& aClient,
									 TAuthPluginType aPluginType)
/**
	Ensure the plugin descriptions returned by the server
	match those directly seen by ECOM.
	
	@param	aPluginType		Type of plugin to retrieve.
 */
	{
	__UHEAP_MARK;
	RCPointerArray<const CPluginDesc> typedPlugins;
	aClient.PluginsOfTypeL(aPluginType, typedPlugins);
	CleanupClosePushL(typedPlugins);
	SetSeenCountsL(typedPlugins);
	
	for (TInt k = 0; k < totalCount; ++k)
		{
		TInt& recCount = seenCounts[k];
		TESTL(recCount == (allPlugins[k]->Type() == aPluginType) ? 1 : 0);
		}
	
	CleanupStack::PopAndDestroy(&typedPlugins);
	__UHEAP_MARKEND;
	}


void CTStepClient::TestActivePluginsL(RAuthClient& aClient)
/**
	Ensure the plugin descriptions returned by the server
	match those directly seen by ECOM.
 */
	{
	__UHEAP_MARK;
	RCPointerArray<const CPluginDesc> activePlugins;
	aClient.ActivePluginsL(activePlugins);
	CleanupClosePushL(activePlugins);
		
	CleanupStack::PopAndDestroy(&activePlugins);
	__UHEAP_MARKEND;
	}

void CTStepClient::TestTrainedPluginsL()
/**
	Test the expected plugins are returned when filtering
	for training types.
	
	@param	aClient			Client with open connection to auth server.
 */
	{
	RCPointerArray<const CPluginDesc> pids;
	CleanupClosePushL(pids);
	
	RAuthClient ac;
	User::LeaveIfError(ac.Connect());
	CleanupClosePushL(ac);
	
	const CPluginDesc* pdsc0 = allPlugins[0];
	const CPluginDesc* pdsc1 = allPlugins[1];
	const CPluginDesc* pdsc2 = allPlugins[2];
	const CPluginDesc* pdsc3 = allPlugins[3];

	ac.PluginsWithTrainingStatusL(EAuthUntrained, pids);
	TInt filterCount = pids.Count();
	TESTL(filterCount == totalCount - 4);
	for (TInt i = 0; i < filterCount; ++i)
	{
		const CPluginDesc* pidF = pids[i];
		TBool eq0 = AreDescsEqual(pidF, pdsc0);
		TBool eq1 = AreDescsEqual(pidF, pdsc1);
		TBool eq2 = AreDescsEqual(pidF, pdsc2);
		TBool eq3 = AreDescsEqual(pidF, pdsc3);
		TESTL(!eq0 && !eq1 && !eq2 && !eq3);
		}

	pids.ResetAndDestroy();
	ac.PluginsWithTrainingStatusL(EAuthTrained, pids);
	TESTL(pids.Count() == 2);
	TESTL(	(AreDescsEqual(pids[0], pdsc0) && AreDescsEqual(pids[1], pdsc1))
		||	(AreDescsEqual(pids[0], pdsc1) && AreDescsEqual(pids[1], pdsc0)) );
	
	pids.ResetAndDestroy();
	ac.PluginsWithTrainingStatusL(EAuthFullyTrained, pids);
	TESTL(pids.Count() == 2);
	TESTL(	(AreDescsEqual(pids[0], pdsc2) && AreDescsEqual(pids[1], pdsc3))
		||	(AreDescsEqual(pids[0], pdsc3) && AreDescsEqual(pids[1], pdsc2)) );
	
	CleanupStack::PopAndDestroy(2, &pids);
	}


void CTStepClient::PrepareTrainedPluginsL()
/**
	Helper function for TestTrainedPluginsL.
	
	Train some plugins so they can be filtered out later.
	This function replaces the database.
 */
	{
	// There have to be at least four available plugins
	// to run the training status test.
	TESTL(allPlugins.Count() >= 4);
	
	RemoveExistingDbL();
	CAuthDb2* db = CAuthDb2::NewLC(iFs);

	db->AddIdentityL(KIdentity1Id, KIdentity1Desc);
	db->AddIdentityL(KIdentity2Id, KIdentity2Desc);

	const CTransientKeyInfo& tki = *iId1Keys[0];
	
	// partially train plugins zero and one
	TPluginId pid0 = allPlugins[0]->Id();
	TPluginId pid1 = allPlugins[1]->Id();
	db->SetTrainedPluginL(KIdentity1Id, pid0, tki);
	db->SetTrainedPluginL(KIdentity2Id, pid1, tki);

	// fully train plugins two and three
	TPluginId pid2 = allPlugins[2]->Id();
	TPluginId pid3 = allPlugins[3]->Id();
	db->SetTrainedPluginL(KIdentity1Id, pid2, tki);
	db->SetTrainedPluginL(KIdentity2Id, pid2, tki);
	db->SetTrainedPluginL(KIdentity1Id, pid3, tki);
	db->SetTrainedPluginL(KIdentity2Id, pid3, tki);

	CleanupStack::PopAndDestroy(db);
	}

TBool CTStepClient::TestMultiAuthL()
	{
	INFO_PRINTF1(_L("Testing only a single client can make an auth "));
	INFO_PRINTF1(_L("request at any one time"));	
	
	RAuthClient ac1;
	User::LeaveIfError(ac1.Connect());
	CleanupClosePushL(ac1);

	RAuthClient ac2;
	User::LeaveIfError(ac2.Connect());
	CleanupClosePushL(ac2);

    CIdentity* result1 = 0;
    CAuthActive active1(ac1, result1, this);
    active1.doAuthenticate(KTestPluginBlocking,0);

    CIdentity* result2 = 0;
    CAuthActive active2(ac2, result2, this, EFalse);
    active2.doAuthenticate(KTestPluginBlocking,0);
	
	CActiveScheduler::Start();

    TEST(active1.iErr == KErrNone);
	TEST(active2.iErr == KErrServerBusy);
	
    delete result1;
	delete result2;
	
	CleanupStack::PopAndDestroy(2,&ac1);
	return ETrue;
	}


TBool CTStepClient::TestAuthenticateL()
/**
	Send simple authentication request.
 */
	{
	// create a transient key
	CProtectionKey* key = CProtectionKey::NewLC(8);
	CTransientKeyInfo* tki = CTransientKeyInfo::NewLC(KTestPluginId22);
	
	CTransientKey* tk = tki->CreateTransientKeyL(KIdentifyData);
	CleanupStack::PushL(tk);
	
	CEncryptedProtectionKey* epKey = tk->EncryptL(*key);
	CleanupStack::PushL(epKey);
	
	tki->SetEncryptedProtectionKeyL(epKey);
	CleanupStack::Pop(epKey);	// now owned by tki
	
	// create identity 22, which identified by test plugin
	WaitForServerToReleaseDb();
	CAuthDb2* db = CAuthDb2::NewL(iFs);
	CleanupStack::PushL(db);	
	db->AddIdentityL(22, _L("22-desc"));
	db->SetTrainedPluginL(22, KTestPluginId22, *tki);

	CleanupStack::PopAndDestroy(4, key);	// tki, tk, db
	
	RAuthClient ac;
	User::LeaveIfError(ac.Connect());
	CleanupClosePushL(ac);

	RAuthMgrClient amc;
	User::LeaveIfError(amc.Connect());
	CleanupClosePushL(amc);

	TInt eventId = KUnknownIdentity;
	TLastAuth lastAuth;
	TPckg<TLastAuth> lastAuthPckg(lastAuth);

	TInt err = RProperty::Get(KAuthServerSecureId, KUidAuthServerLastAuth,
							  lastAuthPckg);
	RProperty::Get(KAuthServerSecureId, KUidAuthServerAuthChangeEvent, eventId);
	TESTL(eventId == KUnknownIdentity);
	TESTL(lastAuth.iId == KUnknownIdentity);
	TESTL(lastAuth.iAuthTime == 0);
	TESTL(lastAuth.iMaxCombinations == 0);
	TESTL(lastAuth.iFalsePositiveRate == 0);
	TESTL(lastAuth.iFalseNegativeRate == 0);
	TESTL(lastAuth.iNumFactors == 0);
	
	// authenticate the client with the test plugin but using type
	
	amc.SetPreferredTypePluginL(EAuthKnowledge,
								KTestPluginId22);
    CleanupStack::PopAndDestroy(&amc);
	TESTL(KTestPluginId22 == ac.PreferredTypePluginL(EAuthKnowledge));

	CAuthExpression* ae = AuthExpr(EAuthKnowledge);		// plugin type
	User::LeaveIfNull(ae);
	CleanupStack::PushL(ae);

    CIdentity* result = 0;
    result = ac.AuthenticateL(
    	*ae,		// aExpression
    	0,			// aTimeout
    	ETrue,     // aClientSpecificKey       
		EFalse);	// aWithString
	INFO_PRINTF3(_L("1. Id = 0x%x , KeyLength = %d\n"), result->Id(),
				 result->Key().KeyData().Size());
	TESTL(result->Id() == 22);
	delete result;

	RProperty::Get(KAuthServerSecureId, KUidAuthServerLastAuth, lastAuthPckg);
	RProperty::Get(KAuthServerSecureId, KUidAuthServerAuthChangeEvent, eventId);
	TESTL(eventId == eventId);
	TESTL(lastAuth.iId == 22);
	TTime now;
	TTimeIntervalSeconds since;
	now.UniversalTime();
	now.SecondsFrom(lastAuth.iAuthTime, since);
	TESTL(since < TTimeIntervalSeconds(5));
	TESTL(lastAuth.iMaxCombinations == KEntropy);
	TESTL(lastAuth.iFalsePositiveRate == KFalsePos);
	TESTL(lastAuth.iFalseNegativeRate == KFalseNeg);
	TESTL(lastAuth.iNumFactors == 1);
	
	
	// authenticate again, to get cached value
    result = ac.AuthenticateL(
    	*ae,		// aExpression
    	55,			// aTimeout
    	ETrue,      // aClientSpecificKey
		ETrue);	    // aWithString
	TDesC p = result->String();
	INFO_PRINTF4(_L("2. Id = 0x%x , KeyLength = %d, String = %S\n"), result->Id(),
				 result->Key().KeyData().Size(), &p);
	TESTL(result->Id() == 22);
	delete result;

	
	// deauthenticate
	TRAP(err, ac.DeauthenticateL());
	TESTL(err == KErrNone);

	RProperty::Get(KAuthServerSecureId, KUidAuthServerLastAuth, lastAuthPckg);
	RProperty::Get(KAuthServerSecureId, KUidAuthServerAuthChangeEvent, eventId);
	TESTL(eventId == KUnknownIdentity);
	TESTL(lastAuth.iId == KUnknownIdentity);	
	TESTL(lastAuth.iAuthTime == 0);
	TESTL(lastAuth.iMaxCombinations == 0);
	TESTL(lastAuth.iFalsePositiveRate == 0);
	TESTL(lastAuth.iFalseNegativeRate == 0);
	TESTL(lastAuth.iNumFactors == 0);
		
    result = 0;
    CAuthActive active(ac, result, this);
    active.doAuthenticate(KTestPluginId22,0);		
	CActiveScheduler::Start();
	INFO_PRINTF3(_L("3. Id = 0x%x , KeyLength = %d"), result->Id(),
				 result->Key().KeyData().Size());
    TESTL(result->Id() == 22);
	delete result;

	
	// unknown
    result = 0;
    active.doAuthenticate(KTestPluginIdUnknown,0);		
	CActiveScheduler::Start();
	INFO_PRINTF2(_L("4. Id = 0x%x"),result->Id());
	TESTL(result->Id() == KUnknownIdentity);
	delete result;

	
	// failure (bad plugin)
    result = 0;
    active.doAuthenticate(0xD0DAD0DA,0);		
	CActiveScheduler::Start();
	INFO_PRINTF3(_L("5. CAuthActive:RunL err %d : res = 0x%x"),
				 active.iErr, result);
	TESTL(active.iErr == KErrAuthServNoSuchPlugin && result == 0);
	
	CleanupStack::PopAndDestroy(2, &ac);	// ae, ac
	return ETrue;
	}

void CTStepClient::TestSetIdentityStrL()
    {
	// assumes TestAuthenticateL has been called and id 22 is created
	RAuthClient ac;
	User::LeaveIfError(ac.Connect());
	CleanupClosePushL(ac);

	_LIT(KId22DescNew, "ID22NewDesc");

	HBufC* desc = ac.IdentityStringL(22);
	CleanupStack::PushL(desc);
	TESTL(*desc == _L("22-desc")); 
	CleanupStack::PopAndDestroy(desc); 
	
	ac.SetIdentityStringL(22, KId22DescNew);
	
	desc = ac.IdentityStringL(22);
	CleanupStack::PushL(desc); 
	TESTL(*desc == KId22DescNew); 
	CleanupStack::PopAndDestroy(desc); 
	
	CleanupStack::PopAndDestroy(&ac);
	}    

						 

TVerdict CTStepClient::doTestStepL()
	{
	if (TestStepResult() != EPass)
		return TestStepResult();
	SetTestStepResult(EPass);
	__UHEAP_MARK;
	
	CActiveScheduler::Install(iActSchd);
	
	INFO_PRINTF1(_L("Test identities"));
	TestIdentitiesL();
	
	INFO_PRINTF1(_L("Test ClientPluginLists"));
 	TestClientPluginListsL();
 	
 	INFO_PRINTF1(_L("Test Authenticate"));
	TestAuthenticateL();
	
	INFO_PRINTF1(_L("Test MultiAuth"));
	TestMultiAuthL();
	
	INFO_PRINTF1(_L("Test SetIdentityStr"));
	TestSetIdentityStrL();
	
	__UHEAP_MARKEND;	
	
	return EPass;
	}


