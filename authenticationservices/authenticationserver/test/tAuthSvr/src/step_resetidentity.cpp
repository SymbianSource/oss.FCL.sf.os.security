/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CTestStep derived implementation
*
*/

 
#include "tAuthSvrStep.h"
#include <scs/cleanuputils.h>
#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"

using namespace AuthServer;

_LIT(KResetType, "ResetType");
_LIT(KResetTypeResetAll, "ResetAll");
_LIT(KResetTypeResetByType, "ResetByType");
_LIT(KResetTypeResetByList, "ResetByList");
_LIT(KResetIdentityString, "IdentityString");
_LIT(KResetRegDataSingle, "RegistrationData");
_LIT(KResetPluginCount, "PluginCount");
_LIT(KResetPluginId, "PluginId%d");
_LIT(KResetRegData, "RegistrationData%d");
_LIT(KResetPluginType, "PluginType");
_LIT(KResetPluginTypeKnowledge, "EAuthKnowledge");
_LIT(KResetPluginTypeToken, "EAuthToken");
_LIT(KResetPluginTypeBiometric, "EAuthBiometric");


CTResetIdentity::CTResetIdentity(CTAuthSvrServer& aParent): iParent(aParent)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTResetIdentity);
	}

CTResetIdentity::~CTResetIdentity()
	{}

TVerdict CTResetIdentity::doTestStepPreambleL()
	{
	// Call the parent class preamble, setting up the file server, etc
	CTStepActSch::doTestStepPreambleL();
	
	return TestStepResult();
	}

TVerdict CTResetIdentity::doTestStepL()
	{	
	INFO_PRINTF1(_L("In CTResetIdentity doTestStepL"));

	// If any test step leaves or panics, the test step thread won't exit, further calls are made.
	// but the html log is updated, so that subsequent failures could be traced here if necessary. 
	if (TestStepResult() != EPass)
		{
		ERR_PRINTF1(_L("There was an error in a previous test step"));
		//return TestStepResult();
		}

	SetTestStepResult(EPass);

__UHEAP_MARK;		// Check for memory leaks

	//-----------------------------------------------------------------------------------------------------
	InitAuthServerFromFileL();	// Set things like 'iSupportsDefaultData' and 'DefaultPlugin'
	CActiveScheduler::Install(iActSchd);

	TRAPD(res2, doResetIdentityL());
	if(KErrNone != res2)
		{
		ERR_PRINTF2(_L("doResetIdentityL() performed a Leave with code %d"), res2 );
		//SetTestStepResult(EPass);
		SetTestStepError(res2);
		}
	
	//Garbage collect the last previously destroyed implementation 
	// and close the REComSession if no longer in use
	REComSession::FinalClose(); 
__UHEAP_MARKEND;
	return TestStepResult();
	}

TVerdict CTResetIdentity::doTestStepPostambleL()
	{
	// Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();
	return TestStepResult();
	}

void CTResetIdentity::doResetIdentityL ()
	{
	INFO_PRINTF1(_L("In CTResetIdentity doResetIdentityL"));
	// Read the identity string whose id which needs to be reset
	TPtrC identityString;
	if (!GetStringFromConfig(ConfigSection(), KResetIdentityString, identityString))
		{
		INFO_PRINTF1(_L("Identity string not found in the ini file"));
		User::Leave(KErrNotFound);
		}

	TIdentityId identityId = KUnknownIdentity;
	if (identityString != KNullDesC)
		{
		INFO_PRINTF1(_L("In doResetIdentityL Here1"));
		// From the AuthDb get the id corresponding to the string
		// Wait for authserver to release the db
		WaitForServerToReleaseDb();
		WaitForServerToReleaseDb();	// In debug mode AuthServer takes some time to terminate
		INFO_PRINTF1(_L("In doResetIdentityL Here2"));
		CAuthDb2* db = CAuthDb2::NewLC(iFs);
		INFO_PRINTF1(_L("In doResetIdentityL Here3"));
		RIdAndStringArray idList;
		CleanupResetAndDestroyPushL(idList);
		TRAPD(err, db->IdentitiesWithDescriptionL(idList));
		INFO_PRINTF1(_L("In doResetIdentityL Here4"));
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("CAuthDb::IdentitiesWithDescriptionL() failed with %d"), err);
			User::Leave(err);
			}
		INFO_PRINTF2(_L("CAuthDb::IdentitiesWithDescriptionL() found %d identities"), idList.Count());
		for (TInt index = 0; index < idList.Count(); ++index)
			{
			INFO_PRINTF2(_L("CAuthDb::IdentitiesWithDescriptionL() found identities %S"), idList[index]->String());
			if (identityString.Compare(*idList[index]->String()) == 0)
				{
				identityId = idList[index]->Id();
				break;
				}
			}
		CleanupStack::PopAndDestroy(2, db); // idList
		}
	
	// Get the reset type form the ini file and relevant details to do the reset
	TPtrC resetType;
	if (!GetStringFromConfig(ConfigSection(), KResetType, resetType))
		{
		INFO_PRINTF2(_L("%s was not specified in the ini file"), KResetType);
		return;
		}

	AuthServer::RAuthMgrClient authMgrClient;
	CleanupClosePushL(authMgrClient);
	TInt err = authMgrClient.Connect();
	if (err == KErrNotFound)
		{
		// Retry after a delay
		TTimeIntervalMicroSeconds32 timeInterval = 2000;	// 2 Milliseconds
		User::After(timeInterval);
		err = authMgrClient.Connect();
		}
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Unable to start a session or other connection error. Err = %d"), err);
		User::Leave(err);
		}

	if (resetType == KResetTypeResetAll)
		{
		INFO_PRINTF1(_L("In CTResetIdentity doResetIdentityL KResetTypeResetAll"));
		// Read the registration data
		TPtrC regData;
		if (!GetStringFromConfig(ConfigSection(), KResetRegDataSingle, regData))
			{
			INFO_PRINTF1(_L("Registration data was not specified in the ini file"));
			User::Leave(KErrNotFound);
			}
		authMgrClient.ResetIdentityL(identityId, regData);
		}
	else if (resetType == KResetTypeResetByType)
		{
		INFO_PRINTF1(_L("In CTResetIdentity doResetIdentityL KResetTypeResetByType"));
		// Get the plugin type
		TPtrC pluginType;
		if (!GetStringFromConfig(ConfigSection(), KResetPluginType, pluginType))
			{
			INFO_PRINTF1(_L("Plugin type was not specified in the ini file"));
			User::Leave(KErrNotFound);
			}
		TAuthPluginType type;
		if (pluginType == KResetPluginTypeKnowledge)
			{
			type = EAuthKnowledge;
			}
		else if (pluginType == KResetPluginTypeToken)
			{
			type = EAuthToken;
			}
		else if (pluginType == KResetPluginTypeBiometric)
			{
			type = EAuthBiometric;
			}
		else
			{
			INFO_PRINTF1(_L("Invalid plugin type was specified in the ini file"));
			CleanupStack::PopAndDestroy(&authMgrClient);
			return;
			}
		// Read the registration data
		TPtrC regData;
		if (!GetStringFromConfig(ConfigSection(), KResetRegDataSingle, regData))
			{
			INFO_PRINTF1(_L("Registration data was not specified in the ini file"));
			User::Leave(KErrNotFound);
			}
		authMgrClient.ResetIdentityL(identityId, type, regData);
		}
	else if (resetType == KResetTypeResetByList)
		{
		// Read the reset information
		INFO_PRINTF1(_L("In CTResetIdentity doResetIdentityL KResetTypeResetByList"));
		// Get the plugin details
		TInt pluginCount;
		if (!GetIntFromConfig(ConfigSection(), KResetPluginCount, pluginCount))
			{
			INFO_PRINTF1(_L("PluginCount missing/invalid in the ini file"));
			User::Leave(KErrNotFound);
			}
		RArray<TPluginId> pluginList;
		CleanupClosePushL(pluginList);
		RPointerArray<const HBufC> regDataList;
		CleanupResetAndDestroyPushL(regDataList);
		if (pluginCount > 0)
			{
			for (TInt index = 0; index < pluginCount; ++index)
				{
				HBufC* fmt = HBufC::NewLC(KResetPluginId().Length() + 10);
				TPtr fmtPtr(fmt->Des());
				fmtPtr.Format(KResetPluginId, index);
				TInt pluginId;
				if (!GetHexFromConfig(ConfigSection(), *fmt, pluginId))
					{
					INFO_PRINTF2(_L("%S missing in the ini file"), fmt);
					User::Leave(KErrNotFound);
					}
				pluginList.AppendL(static_cast<TPluginId>(pluginId));
				CleanupStack::PopAndDestroy(fmt);
				fmt = HBufC::NewLC(KResetRegData().Length() + 10);
				fmtPtr.Set(fmt->Des());
				fmtPtr.Format(KResetRegData, index);
				TPtrC regData;
				if (!GetStringFromConfig(ConfigSection(), *fmt, regData))
					{
					INFO_PRINTF2(_L("%S missing in the ini file"), fmt);
					User::Leave(KErrNotFound);
					}
				HBufC* reg = regData.AllocLC();
				regDataList.AppendL(reg);	// Takes ownership
				CleanupStack::Pop(reg);
				CleanupStack::PopAndDestroy(fmt);
				}
			}
		else
			{
			// If plugin count is negative, simulate a condition where the pluginList count != regDataList count
			pluginList.AppendL(0);
			}
		authMgrClient.ResetIdentityL(identityId, pluginList, regDataList);
		CleanupStack::PopAndDestroy(2, &pluginList); // regDataList
		}
	else
		{
		INFO_PRINTF1(_L("Invalid reset type was specified in the ini file."));
		User::Leave(KErrNotFound);
		}
	
	CleanupStack::PopAndDestroy(&authMgrClient);
	}
