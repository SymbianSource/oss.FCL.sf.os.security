/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <f32file.h>
#include <ecom/ecom.h>
#include <e32test.h>

#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"

using namespace AuthServer;

const TInt KWaitDuration = 30000000; 
#define WAIT_FOR3s User::After(KWaitDuration)
 

_LIT(KSysBinDir, "c:\\sys\\bin\\");
_LIT(KRscDir, "c:\\resource\\plugins\\");

CTPostMarketPlugin::CTPostMarketPlugin()
	{
	SetTestStepName(KTPostMarketPlugin);
	}

TVerdict CTPostMarketPlugin::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EFail);
	
	CTAuthSvrStepBase::doTestStepPreambleL();
	
	if (!GetStringFromConfig(ConfigSection(), _L("srcPlugin"), iSrcPlugin))
		{
		INFO_PRINTF1(_L("Source plugin not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("destPlugin"), iTargetPlugin))
		{
		INFO_PRINTF1(_L("Target plugin not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("srcRscFile"), iSrcRscFile))
		{
		INFO_PRINTF1(_L("Source resource file not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("destRscFile"), iTargetRscFile))
		{
		INFO_PRINTF1(_L("Target resource file not specified in the ini file"));
		return TestStepResult();
		}
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}
	

TBool CTPostMarketPlugin::VerifyObtainedResultsWithExpectedL(AuthServer::RAuthMgrClient& aAuthMgrClient, TInt aExpectedCount, const TDesC& aExpectedPluginName)
	{
	RPluginDescriptions pluginList;
	TCleanupItem cleanup(CleanupEComArray, &pluginList);
	CleanupStack::PushL(cleanup);
	
	//Get a list of all the available plugins.					
	aAuthMgrClient.PluginsL(pluginList);	
	TInt count = pluginList.Count();
	TBool result = ETrue;
	if(count != aExpectedCount)
		{
		INFO_PRINTF3(_L("Expected plugin count %d differs from actual %d..."), &count, &aExpectedCount);
		result = EFalse;
		}
		
	if(aExpectedPluginName.Length())
		{
		TBool found = EFalse;
		const TDesC* pluginName = 0;
		for(TInt i = 0; i < count; i++)
			{
			pluginName = pluginList[i]->Name();
			if(pluginName->Compare(aExpectedPluginName) == 0)
				{
				found = ETrue;
				break;
				}
			}
		
		if(!found)
			{
			result = EFalse;
			}
		}
	
	CleanupStack::PopAndDestroy(&pluginList);	
	return result;	
	}
	

TVerdict CTPostMarketPlugin::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	//Connect to the AuthServer	
	AuthServer::RAuthMgrClient authMgrClient;	
	TInt connectVal = authMgrClient.Connect();
	if (KErrNotFound == connectVal)
		{
		//Retry after a delay
		TTimeIntervalMicroSeconds32 timeInterval = 2000;	//2 Milliseconds
		User::After(timeInterval);
		connectVal = authMgrClient.Connect();
		}
	if (KErrNone != connectVal)
		{
		ERR_PRINTF2(_L("Unable to start a session or other connection error. Err = %d"), connectVal);
		User::LeaveIfError(connectVal);		
		}	
	
	CleanupClosePushL(authMgrClient);
	
	RTestUtilSession testutil;
	User::LeaveIfError(testutil.Connect());
	CleanupClosePushL(testutil);
	
	// test plugin install operation.
	INFO_PRINTF1(_L("Test Plugin Install"));
	TBool success = TestPluginInstallL(authMgrClient, testutil);
	if(!success)
		{
		INFO_PRINTF1(_L("Test Install failed!"));
		CleanupStack::PopAndDestroy(2, &authMgrClient);
		return TestStepResult();
		}
	
	// test plugin uninstall operation.
	INFO_PRINTF1(_L("Test Plugin Uninstall"));
	success = TestPluginUninstallL(authMgrClient, testutil);
	if(!success)
		{
		INFO_PRINTF1(_L("Test Uninstall failed!"));
		CleanupStack::PopAndDestroy(2, &authMgrClient);
		return TestStepResult();
		}
	
	CleanupStack::PopAndDestroy(2, &authMgrClient);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TBool CTPostMarketPlugin::TestPluginInstallL(AuthServer::RAuthMgrClient& aAuthMgrClient, RTestUtilSession& aUtilSession)
	{
	TBool success = EFalse;
	
	// get the list of available plugin implementations, before an plugin installation/upgrade.
	TInt pluginCountPreInstall = 0;
	if(GetIntFromConfig(ConfigSection(),_L("initialPluginCount"), pluginCountPreInstall) == EFalse)
		{
		INFO_PRINTF1(_L("Pre Install Plugin Count not specified in the ini file"));
		return success;
		}
	
	TBool result = VerifyObtainedResultsWithExpectedL(aAuthMgrClient, pluginCountPreInstall);
	if(!result)
		{
		return success;
		}
		
	// Install a plugin.
	// Incase c:\sys\bin or c:\resource\plugins directory ,doesn't exist, 
	// move operation fails with KErrPathNotFound.So create one.
		
	TInt err = aUtilSession.MkDirAll(KSysBinDir);
	if(err != KErrAlreadyExists && err != KErrNone)
		{
		User::Leave(err);
		}
		
	err = aUtilSession.MkDirAll(KRscDir);
	if(err != KErrAlreadyExists && err != KErrNone)
		{
		User::Leave(err);
		}
	
	INFO_PRINTF3(_L("Moving plugin from %S to %S..."), &iSrcPlugin, &iTargetPlugin);
	User::LeaveIfError(aUtilSession.Move(iSrcPlugin, iTargetPlugin));
	
	INFO_PRINTF3(_L("Moving plugin from %S to %S..."), &iSrcRscFile, &iTargetRscFile);
	User::LeaveIfError(aUtilSession.Move(iSrcRscFile, iTargetRscFile));
		
	WAIT_FOR3s;
	
	// get the list of available plugin implementations, post installation/upgrade.
	TInt pluginCountPostInstall = 0;
	if(GetIntFromConfig(ConfigSection(),_L("expectedPluginCount"), pluginCountPostInstall) == EFalse)
		{
		INFO_PRINTF1(_L("Post Install Plugin Count not specified in the ini file"));
		return success;
		}
		
	// get the plugin implementation list post installation.This should load the plugin version based on the ROM_only field.	
	TPtrC pluginNamePostInstall;
	if ((!GetStringFromConfig(ConfigSection(), _L("pluginNamePostInstall"), pluginNamePostInstall)))
		{
		INFO_PRINTF1(_L("Post Install Plugin Name not specified in the ini file"));
		}
	
	result = VerifyObtainedResultsWithExpectedL(aAuthMgrClient, pluginCountPostInstall, pluginNamePostInstall);	
	if(!result)
		{
		return success;
		}
	
	success = ETrue;	
	return success;
	}


TBool CTPostMarketPlugin::TestPluginUninstallL(AuthServer::RAuthMgrClient& aAuthMgrClient, RTestUtilSession& aUtilSession)
	{
	TBool success = EFalse;
	
	// get the list of available plugin implementations, before an plugin installation/upgrade.
	TInt pluginCountPreUninstall = 0;
	if(GetIntFromConfig(ConfigSection(),_L("expectedPluginCount"), pluginCountPreUninstall) == EFalse)
		{
		INFO_PRINTF1(_L("Pre Uninstall Plugin Count not specified in the ini file"));
		return success;
		}
	
	TBool result = VerifyObtainedResultsWithExpectedL(aAuthMgrClient, pluginCountPreUninstall);
	if(!result)
		{
		return success;
		}
		
	// Install a plugin.
	INFO_PRINTF3(_L("Moving plugin from %S to %S..."), &iTargetPlugin, &iSrcPlugin);
	User::LeaveIfError(aUtilSession.Move(iTargetPlugin, iSrcPlugin));
	
	INFO_PRINTF3(_L("Moving plugin from %S to %S..."), &iTargetRscFile, &iSrcRscFile);
	User::LeaveIfError(aUtilSession.Move(iTargetRscFile, iSrcRscFile));
		
	WAIT_FOR3s;
	
	// get the list of available plugin implementations, post uninstallation.
	TInt pluginCountPostUninstall = 0;
	if(GetIntFromConfig(ConfigSection(),_L("initialPluginCount"), pluginCountPostUninstall) == EFalse)
		{
		INFO_PRINTF1(_L("Post Uninstall Plugin Count not specified in the ini file"));
		return success;
		}
		
	// get the plugin name post uninstall.	
	TPtrC pluginNamePostUninstall;
	if ((!GetStringFromConfig(ConfigSection(), _L("pluginNamePostUninstall"), pluginNamePostUninstall)))
		{
		INFO_PRINTF1(_L("Post Uninstall Plugin Name not specified in the ini file"));
		}
	
	result = VerifyObtainedResultsWithExpectedL(aAuthMgrClient, pluginCountPostUninstall, pluginNamePostUninstall);	
	if(!result)
		{
		return success;
		}
	
	success = ETrue;	
	return success;
	}
	
	
CTPostMarketPlugin::~CTPostMarketPlugin()
/**
 Destructor
*/
	{};
	
CTInstallPluginStep::CTInstallPluginStep()
	{
	SetTestStepName(KTInstallPluginStep);
	}

TVerdict CTInstallPluginStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EFail);
	
	CTAuthSvrStepBase::doTestStepPreambleL();
	
	if (!GetStringFromConfig(ConfigSection(), _L("srcPlugin"), iSrcPlugin))
		{
		INFO_PRINTF1(_L("Source plugin not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("destPlugin"), iTargetPlugin))
		{
		INFO_PRINTF1(_L("Target plugin not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("srcRscFile"), iSrcRscFile))
		{
		INFO_PRINTF1(_L("Source resource file not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("destRscFile"), iTargetRscFile))
		{
		INFO_PRINTF1(_L("Target resource file not specified in the ini file"));
		return TestStepResult();
		}
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}
	

TVerdict CTInstallPluginStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	//Connect to the AuthServer	
	RTestUtilSession testUtil;
	User::LeaveIfError(testUtil.Connect());
	CleanupClosePushL(testUtil);
	
	// simulate plugin install operation via move operation.
	INFO_PRINTF1(_L("Install RAM Plugin "));
	
	// Install a plugin.
	// Incase c:\sys\bin or c:\resource\plugins directory ,doesn't exist, 
	// move operation fails with KErrPathNotFound.So create one.
	
	TInt err = testUtil.MkDirAll(KSysBinDir);
	if(err != KErrAlreadyExists && err != KErrNone)
		{
		User::Leave(err);
		}
		
	err = testUtil.MkDirAll(KRscDir);
	if(err != KErrAlreadyExists && err != KErrNone)
		{
		User::Leave(err);
		}
	
	INFO_PRINTF3(_L("Moving plugin from %S to %S..."), &iSrcPlugin, &iTargetPlugin);
	User::LeaveIfError(testUtil.Move(iSrcPlugin, iTargetPlugin));
	
	INFO_PRINTF3(_L("Moving plugin from %S to %S..."), &iSrcRscFile, &iTargetRscFile);
	User::LeaveIfError(testUtil.Move(iSrcRscFile, iTargetRscFile));
			
	CleanupStack::PopAndDestroy(&testUtil);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


CTInstallPluginStep::~CTInstallPluginStep()
/**
 Destructor
*/
	{};
	

CTUninstallPluginStep::CTUninstallPluginStep()
	{
	SetTestStepName(KTUninstallPluginStep);
	}

TVerdict CTUninstallPluginStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EFail);
	
	CTAuthSvrStepBase::doTestStepPreambleL();
	
	if (!GetStringFromConfig(ConfigSection(), _L("srcPlugin"), iSrcPlugin))
		{
		INFO_PRINTF1(_L("Source plugin not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("destPlugin"), iTargetPlugin))
		{
		INFO_PRINTF1(_L("Target plugin not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("srcRscFile"), iSrcRscFile))
		{
		INFO_PRINTF1(_L("Source resource file not specified in the ini file"));
		return TestStepResult();
		}
		
	if (!GetStringFromConfig(ConfigSection(), _L("destRscFile"), iTargetRscFile))
		{
		INFO_PRINTF1(_L("Target resource file not specified in the ini file"));
		return TestStepResult();
		}
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}
	

TVerdict CTUninstallPluginStep::doTestStepL()
	{
	SetTestStepResult(EFail);
	
	//Connect to the AuthServer	
	RTestUtilSession testUtil;
	User::LeaveIfError(testUtil.Connect());
	CleanupClosePushL(testUtil);
	
	// simulate plugin uninstall operation via move operation.
	INFO_PRINTF1(_L("Unnstall RAM Plugin "));
	
	INFO_PRINTF3(_L("Moving plugin from %S to %S..."), &iSrcPlugin, &iTargetPlugin);
	User::LeaveIfError(testUtil.Move(iSrcPlugin, iTargetPlugin));
	
	INFO_PRINTF3(_L("Moving plugin from %S to %S..."), &iSrcRscFile, &iTargetRscFile);
	User::LeaveIfError(testUtil.Move(iSrcRscFile, iTargetRscFile));
	
	CleanupStack::PopAndDestroy(&testUtil);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


CTUninstallPluginStep::~CTUninstallPluginStep()
/**
 Destructor
*/
	{};

