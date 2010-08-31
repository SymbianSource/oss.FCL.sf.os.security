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



#ifndef T_AUTHSVR_STEP_H
#define T_AUTHSVR_STEP_H

#include <test/testexecutestepbase.h>
#include <ecom/ecom.h>

#include "authserver_client.h"
#include <authserver/authplugininterface.h>
#include <pinplugindialogdefs.h>

#include "authserver_impl.h"
#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"

#include "tAuthSvrServer.h"
#include "tauthdbstep.h"

// these tests use some information which is normally
// only visible to the server, or to the client DLL.

#include "authdb.h"
#include "authserveripc.h"
#include "transientkey.h"
#include "transientkeyinfo.h"
#include "testutilclient.h"
using namespace AuthServer;


_LIT(KInputFile, 	"C:\\t_pinplugindlg_in.dat");
_LIT(KOutputFile, "C:\\t_pinplugindlg_out.dat");
_LIT(KEmptyString, "");

// Used when creating the initial db
//#define KNumPlugins 8	//Defined in tauthdbstep.h

class CTAuthSvrStepBase : public CTestStep
	{
protected:
	CTAuthSvrStepBase();
	~CTAuthSvrStepBase();
	virtual TVerdict doTestStepPreambleL()	
		{

		iFs.Connect();	
	   	return TestStepResult();
		}
	virtual TVerdict doTestStepPostambleL()
	    {
		iFs.Close();
		return TestStepResult();
		
		}
	void SetDefaultPluginIdL(TInt aDefault);

	HBufC* GetNameLC(TInt i);

	void PluginData(AuthServer::TIdentityId aId, AuthServer::TPluginId aPlugin,	TPtr8 aDes) 
		{
		aDes.Fill(aId*aPlugin);
		}

//Needed because of the use of Step_CreateDb
	inline void CreatePlugins();
	inline void CreateKeyInfo();
		
	//Utility functions allowing text INI file parsing abilities
	TInt writeToNamedConfig(CIniData* aFile, const TDesC &aSectName,const TDesC &aKeyName, const TPtrC &aResult);
	//Section name unspecified
	TInt writeToNamedConfig(CIniData* aFile, const TDesC &aKeyName,const TPtrC &aResult);
	TInt readFromNamedConfigL(const TDesC &aFileName, const TDesC &aSectName,const TDesC &aKeyName, HBufC*& aResult);
	//Section name unspecified
	TInt readFromNamedConfigL(const TDesC &aFileName, const TDesC &aKeyName, HBufC*& aResult);
	//Read data from testexecute ini file that will be used to initialise the Authentication Server
	TInt InitAuthServerFromFileL();
	
	//Read plugin input train or plugin input id data from testexecute ini file 
	//that will be used to initialise the Authentication Server, based on  
	//aInitTrainInput.If set, this method reads plugin input train, else
	//reads plugin input id.
	TInt InitPluginDataFromFileL(TBool aInitTrainInput);
	
	// Wait for the server to shut down and so release the authentication database.
	void WaitForServerToReleaseDb();
	// 	Remove any existing database file.
	void RemoveExistingDbL();
	// 	Utility function coverting HexString to Integer
	TInt HexString2Num(const TDesC8& aStr);
	// Utility function for removing a specified Identity from the global Identity array
	TInt RemoveIdFromGlobalDbL(TUint32 aId);
	// Utility clean up function used when working with locally declared arrays
	//void CleanupEComArray(TAny* aArrayOfPlugins);	
	
	//Utility function to read and set the username of a given identity
	TVerdict checkAndSetUserNameL(AuthServer::RAuthClient& ac, TInt idPosition);	
	// Utility function for checking the status of a pluginId that is specified
	// in the ini file.
	void CheckSpecifiedPluginStatusL();
	// Version of above function that avoids a call to the 'Connect()' function
	void CheckSpecifiedPluginStatusConnectedL(AuthServer::RAuthClient& ac);
	
	// Checks that the status of each plugin is as expected.
	// based on the number of identities trained for that plugin, and the number of identities 
	// present in the Authentication server.
	TBool CheckPluginStatusAllL (AuthServer::RAuthClient& ac);
	
	// Reads the number of identities that have been trained with the plugin that 
	// has its database in the named file
	TInt GetNumIdsFromPluginDbL(const TDesC &aFileName);
	
	// Utility function for retrieving the last authenticated id from a file
	AuthServer::TIdentityId getLastAuthId();
	
    AuthServer::CAuthExpression* CreateAuthExprLC(const TDesC& aString);
    
    void CreatePinPluginInputFileL(TPinPluginDialogOperation aOperation, TPinValue aPinValue);
    
    TPinValue GetPinFromOutputFileL();
    
    void ClientMessage(TBool aIsMessage, const TDesC& aDisplayString);
    
protected:
	RFs iFs;	
	
	// Used when creating the initial db
	AuthServer::CPluginDesc*       iPlugins[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId1Keys[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId2Keys[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId3Keys[KNumPlugins];

	};
	
// Utility clean up function used when working with locally declared arrays
	void CleanupEComArray(TAny* aArrayOfPlugins);	

inline CTAuthSvrStepBase::~CTAuthSvrStepBase() 
    {
    for (TInt i = 0 ; i < KNumPlugins ; ++i)
        {
        delete iPlugins[i];
        delete iId1Keys[i];
        delete iId2Keys[i];
        delete iId3Keys[i];
        }
    }

inline HBufC* CTAuthSvrStepBase::GetNameLC(TInt id)
    {
    _LIT(nameTmpl, "Plugin_%d");
	HBufC* name = HBufC::NewLC(15);
	name->Des().Format(nameTmpl, id);
	return name;
    }
    
inline void CTAuthSvrStepBase::CreatePlugins()
	{
	using namespace AuthServer;
	
	const TAuthPluginType   types[KNumPlugins]
		= { EAuthBiometric, EAuthToken, EAuthKnowledge,
			EAuthBiometric, EAuthToken, EAuthKnowledge,
			EAuthToken, EAuthKnowledge
		};
	const TAuthTrainingStatus   training[KNumPlugins]
		= { EAuthUntrained, EAuthTrained, EAuthFullyTrained,
			EAuthUntrained, EAuthTrained, EAuthFullyTrained,
			EAuthTrained, EAuthFullyTrained
		  };


	for (TInt i = 0 ; i < KNumPlugins ; ++i)
		{
			HBufC* name = GetNameLC(i);
			
			iPlugins[i] = CPluginDesc::NewL(i, *name, types[i],
											training[i], i*10000, i, i*4);
		    CleanupStack::PopAndDestroy(name);									
		}
	}

inline void CTAuthSvrStepBase::CreateKeyInfo()
	{
	using namespace AuthServer;

	CProtectionKey* protKey1 = CProtectionKey::NewLC(8);
	CProtectionKey* protKey2 = CProtectionKey::NewLC(8);
	CProtectionKey* protKey3 = CProtectionKey::NewLC(8);

	HBufC8* data = HBufC8::NewLC(4);
	
	for (TInt plugin = 0 ; plugin < KNumPlugins ; ++plugin)
		{
			CTransientKey* transient = 0;
			// id 1
			iId1Keys[plugin] = CTransientKeyInfo::NewL(plugin);
			PluginData(1, plugin, data->Des());
			transient = iId1Keys[plugin]->CreateTransientKeyL(data->Des());
			CEncryptedProtectionKey* epKey1 =
				transient->EncryptL(*protKey1);
			delete transient;
			iId1Keys[plugin]->SetEncryptedProtectionKeyL(epKey1);
			// id 2
			PluginData(2, plugin, data->Des());
			iId2Keys[plugin] = CTransientKeyInfo::NewL(plugin);
			transient = iId2Keys[plugin]->CreateTransientKeyL(data->Des());
			CEncryptedProtectionKey* epKey2 =
				transient->EncryptL(*protKey2);
			delete transient;
			iId2Keys[plugin]->SetEncryptedProtectionKeyL(epKey2);
			// id 3
			PluginData(3, plugin, data->Des());
			iId3Keys[plugin] = CTransientKeyInfo::NewL(plugin);
			transient = iId3Keys[plugin]->CreateTransientKeyL(data->Des());
			CEncryptedProtectionKey* epKey3 =
				transient->EncryptL(*protKey3);
			delete transient;
			iId3Keys[plugin]->SetEncryptedProtectionKeyL(epKey3);
		}
	CleanupStack::Pop(4, protKey1);
	}


/**
	This abstract subclass of CTAuthcliservStepBase
	overrides the doTestStep(Pre|Post)ambleL functions
	from CTestBase to allocate and free an active
	scheduler.
	
	The scheduler is stored in the protected iActSchd
	variable.  This class does not start or stop the
	scheduler.
 */

class CTStepActSch : public CTAuthSvrStepBase
	{
protected:
	// override CTestStep
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
		
protected:
	/** The active scheduler which subclasses can use. */
	CActiveScheduler* iActSchd;
	};
    

class CTFirstStart : public CTAuthSvrStepBase
	{
public:
	CTFirstStart();	
	//~CTFirstStart();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	//virtual TVerdict doTestStepPostambleL();
	};
	
class CTAuthSvrCheck: public CTAuthSvrStepBase
	{
public:
	CTAuthSvrCheck();
	//~CTAuthSvrCheck();
	//virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	//virtual TVerdict doTestStepPostambleL();
	};
	
class CTRegIdentity: public CTStepActSch
	{
public:
	CTRegIdentity(CTAuthSvrServer& aParent);
	~CTRegIdentity();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	void ListPluginsL(AuthServer::RAuthMgrClient& aAuthMgrClient );
	void SetPinPluginStateL();
	void RemovePinPluginFileL();
	
private:
	CTAuthSvrServer& iParent;
	};
	
class CTEnumIdentities: public CTAuthSvrStepBase
	{
public:
	CTEnumIdentities(CTAuthSvrServer& aParent);
	virtual TVerdict doTestStepL();	
private:
	CTAuthSvrServer& iParent;
	};
	
class CTIdentifyMultiple: public CTAuthSvrStepBase
	{
public:
	//CTIdentifyMultiple(CTAuthSvrServer& aParent);
	CTIdentifyMultiple();
	~CTIdentifyMultiple();
	virtual TVerdict doTestStepPreambleL();	
	virtual TVerdict doTestStepL();	
private:
	//CTAuthSvrServer& iParent;
	RArray<TUid> iPluginIds;
	};
	
class CTRetrainPlugin: public CTStepActSch
	{
public:	
	//CTRetrainPlugin();
	CTRetrainPlugin(CTAuthSvrServer& aParent);
	~CTRetrainPlugin();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();	
	virtual TVerdict doTestStepL();	
	// Utility function that calls 'Train' for a specified plugin	
	void RetrainPlugin (AuthServer::RAuthMgrClient& mgc);
private:
	CTAuthSvrServer& iParent;
	};

class CTRemoveAuthMethod: public CTStepActSch
	{
public:	
	//CTRetrainPlugin();
	CTRemoveAuthMethod(CTAuthSvrServer& aParent);
	~CTRemoveAuthMethod();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();	
	virtual TVerdict doTestStepL();	
	void RemoveAuthMethodL (AuthServer::RAuthMgrClient& mgc);
private:
	CTAuthSvrServer& iParent;
	};

class CTRemoveDbs: public CTStepActSch
	{
public:	
	CTRemoveDbs();
	~CTRemoveDbs();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();	
	virtual TVerdict doTestStepL();	
private:
	
	};
	
class CTRemoveIdentity: public CTStepActSch
	{
public:	
	CTRemoveIdentity(CTAuthSvrServer& aParent);
	~CTRemoveIdentity();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();	
	virtual TVerdict doTestStepL();		
	// Utility function that calls 'authenticate, then removeIdentity', with a specified plugin	
	void doRemoveIdentityL (AuthServer::RAuthMgrClient& mgc);	
private:
	CTAuthSvrServer& iParent;
	};

class CTAuthenticate: public CTStepActSch
	{
public:
	CTAuthenticate(CTAuthSvrServer& aParent);
	~CTAuthenticate();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();	
	virtual TVerdict doTestStepL();
	//Utility function that stores the most recently authenticated identity in the GlobalDb file
	//This should only be called by the Authenticate test step
	void setLastAuthIdL(AuthServer::TIdentityId idToStore);
	
	/**
  	This method calls the synchronous version of AuthenticateL method.
  
   	@param aAuthMgrClient		authMgrClient object.
   	@param aInputString			descriptor containing either the alias string or auth expression
   								string as read from the ini file.
   	@param aFreshness			time period which serves as a check to determine if an authentication 
   								has previously been performed within this period.This value is read from 
   								the ini file.If not present,the default value is used.
 
  	@param aClientSpecificKey 	this value when true represents the key returned by 
  								auth server will be unique to the calling client.This value is read from the 
  								ini file.By default this is set to false.
 
  	@param isAlias 				this value when true specifies that the authentication is performed via alias.
  								This value is read from the ini file.By default it is set to false.
 
 	
  	*/
 
  	void CallSyncAuthenticateL(AuthServer::RAuthMgrClient& aAuthMgrClient, const TDesC& aInputString, TInt aFreshness, TBool aClientSpecificKey, TBool isAlias, TBool isDefault, const TDesC& aClientMessage);
	
	/**
  	This method calls the asynchronous version of AuthenticateL method.
  
   	@param aAuthMgrClient		authMgrClient object.
   	@param aInputString			descriptor containing either the alias string or auth expression
   								string as read from the ini file.
   	@param aFreshness			time period which serves as a check to determine if an authentication 
   								has previously been performed within this period.This value is read from 
   								the ini file.If not present,the default value is used.
 
  	@param aClientSpecificKey 	this value when true represents the key returned by 
  								auth server will be unique to the calling client.This value is read from the 
  								ini file.By default this is set to false.
 
  	@param isAlias 				this value when true specifies that the authentication is performed via alias.
  								This value is read from the ini file.By default it is set to false.
 
 	
  	*/
	void CallAsyncAuthenticateL(AuthServer::RAuthMgrClient& aAuthMgrClient, TDesC& aInputString, TInt aFreshness, TBool aClientSpecificKey, TBool isAlias, TBool isDefault, const TDesC& aClientMessage);
	
private:
	CTAuthSvrServer& iParent;
	};
		
class CTSetAuthPrefs: public CTStepActSch
	{
public:
	CTSetAuthPrefs(CTAuthSvrServer& aParent);
	~CTSetAuthPrefs();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();	
	virtual TVerdict doTestStepL();
	void SetAuthPrefsL(AuthServer::RAuthMgrClient& aAuthMgrClient, AuthServer::RAuthClient& aAuthClient);

private:
	CTAuthSvrServer& iParent;
	};

class CTResetIdentity: public CTStepActSch
	{
public:	
	CTResetIdentity(CTAuthSvrServer& aParent);
	~CTResetIdentity();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();	
	virtual TVerdict doTestStepL();		
	// Utility function that calls 'resetIdentity', with a specified plugin
	void doResetIdentityL();
private:
	CTAuthSvrServer& iParent;
	};

/**
	This class initializes the aliases in the authserver's central repository file 
	with different values as specified in the ini file.
  */

class CTInitCenRep: public CTStepActSch
	{
public:
	CTInitCenRep(CTAuthSvrServer& aParent);
	~CTInitCenRep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();	
	virtual TVerdict doTestStepL();
private:
	CTAuthSvrServer& iParent;
	};

class CResultAvailability: public CTStepActSch
	{
public:
	CResultAvailability(CTAuthSvrServer& aParent);
	~CResultAvailability();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	CTAuthSvrServer& iParent;
	};

class CAuthSvrOom: public CTStepActSch
	{
public:
	CAuthSvrOom(CTAuthSvrServer& aParent);
	~CAuthSvrOom();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	TVerdict doClientOOMTestL();
	TVerdict doServerOOMTestL();
	TVerdict doPerformanceTestL();
	void ReadTestConfigurationL();
	TVerdict doTestL(AuthServer::RAuthMgrClient& ac);
	void doStartupTestL();
	void doRegOOMTestL(AuthServer::RAuthMgrClient &ac);
	void doSyncAuthOOMTestL(AuthServer::RAuthMgrClient& ac);
	void doAsyncAuthOOMTestL(AuthServer::RAuthMgrClient& ac);
	void doSetPreferredTypeL(AuthServer::RAuthMgrClient& ac);
	void doRemoveIdentityL(AuthServer::RAuthMgrClient& ac);
	void doForgetPluginL(AuthServer::RAuthMgrClient& ac);
	void doRetrainPluginL(AuthServer::RAuthMgrClient& ac);
	void doListPluginsL(AuthServer::RAuthMgrClient& ac);
	void doListActivePluginsL(AuthServer::RAuthMgrClient& ac);
	void doListPluginsOfTypeL(AuthServer::RAuthMgrClient& ac);
	void doListPluginsWithTrainingStatusL(AuthServer::RAuthMgrClient& ac);
	void doListAuthStrengthAliasesL(AuthServer::RAuthMgrClient& ac);
	void doResetAll(AuthServer::RAuthMgrClient& ac);
	void doResetType(AuthServer::RAuthMgrClient& ac);
	void doResetList(AuthServer::RAuthMgrClient& ac);
// performance tests.
	void StartTimer();
	void StopTimerAndPrintResultL();
	void PrintPerformanceLog(TTime aTime);
	

private:
	CTAuthSvrServer& iParent;
	TInt iFreshnessVal;
	TPtrC iExprString;
	TBool iClientSpecificKeyVal;
	TBool iDefaultpluginusage;
	TBuf<100> iMessage;
	TTime iStartTime;
	TInt iIterationCount;

	};

class CResultPropertyWatch:CActive
	{
	enum {EPriority=0};
	public:
	static CResultPropertyWatch* NewLC();
	~CResultPropertyWatch();
	void DoCancel();
	private:
	CResultPropertyWatch();
	void ConstructL();
	
	void RunL();
	
	private:
	RProperty iProperty;
	public:	
	TBool iNotified;
	};

class CResultChangeNotify: public CTStepActSch
	{
public:
	CResultChangeNotify(CTAuthSvrServer& aParent);
	~CResultChangeNotify();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
	
private:
	void SetPinPluginStateL();
	void RemovePinPluginFileL();
private:
	CTAuthSvrServer& iParent;
	CResultPropertyWatch* watch;

	};


class CTPostMarketPlugin: public CTAuthSvrStepBase
	{
public:

	CTPostMarketPlugin();
	~CTPostMarketPlugin();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
private:

	/**
  	This method simulates plugin installation.In case of upgrades to ROM, it verifies 
  	that the right plugin is loaded (depending on the ROM_Only field in the plugin registration file),
  	and that the plugin count increments when the macro to enable post market plugins is set to one,
  	and remains the same when the macro is disabled.
  
   	*/
	
	TBool TestPluginInstallL(AuthServer::RAuthMgrClient& aAuthMgrClient, RTestUtilSession& aUtilSession);

 	/**
  	This method simulates plugin uninstallation.In case of upgrades to ROM, it verifies 
  	that the ROM plugin is loaded, and that the plugin count decrements when the macro to enable 
	post market plugins is set to one, 	and remains the same when the macro is disabled.
  
   	*/
	
	TBool TestPluginUninstallL(AuthServer::RAuthMgrClient& aAuthMgrClient, RTestUtilSession& aUtilSession);
	
	/**
  	This method lists the available plugin implementations and tests whether
  	it matches with the expected plugin count.This method also takes plugin name 
  	as a default argument,which assists in testing whether the right plugin has 
  	been loaded during ROM upgrade operat
  
  	@param aAuthMgrClient	authMgrClient object.
   	@param aExpectedCount	expected plugin count.
   	@param aPluginName		expected plugin name.Default argument.
   	@return 				true, when the actual plugin count equals the expected one.
   							When the plugin name is specified, returns true, if
   							the plugin name matches the expected one.
   
   	*/
	TBool VerifyObtainedResultsWithExpectedL(AuthServer::RAuthMgrClient& aAuthMgrClient, TInt aExpectedCount, const TDesC& aPluginName = KEmptyString);
	
private:
	
	TPtrC iSrcPlugin;
	TPtrC iTargetPlugin;
	TPtrC iSrcRscFile;
	TPtrC iTargetRscFile;
  	};
  	
 class CTStepClientSidSupport : public CTStepActSch
	{
public:
	CTStepClientSidSupport(CTAuthSvrServer& aParent);
	
	~CTStepClientSidSupport();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
private:
	TBool VerifyObtainedResultsWithExpectedL();
	
private:
	CTAuthSvrServer& iParent;
	TUid iClientSid;
	TBool iClientSpecificKey;
	TBool iWithString;
	TInt iPluginId;
	AuthServer::CIdentity* iIdentity;
	AuthServer::CIdentity* iIdentityAfterRetrain;
	};
	
class CTInstallPluginStep: public CTAuthSvrStepBase
	{
public:

	CTInstallPluginStep();
	~CTInstallPluginStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
private:
	
	TPtrC iSrcPlugin;
	TPtrC iTargetPlugin;
	TPtrC iSrcRscFile;
	TPtrC iTargetRscFile;
  	};
  	
class CTUninstallPluginStep: public CTAuthSvrStepBase
	{
public:

	CTUninstallPluginStep();
	~CTUninstallPluginStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	
private:
	
	TPtrC iSrcPlugin;
	TPtrC iTargetPlugin;
	TPtrC iSrcRscFile;
	TPtrC iTargetRscFile;
  	};  	
  	

class CTSysTimeUpdate: public CTStepActSch
	{
public:
 	CTSysTimeUpdate(CTAuthSvrServer& aParent);
 	~CTSysTimeUpdate();
 	virtual TVerdict doTestStepPreambleL();
 	virtual TVerdict doTestStepPostambleL();	
 	virtual TVerdict doTestStepL();
private:
 	CTAuthSvrServer& iParent;
 	};
   		
//Parameters class.For now, used only to get the result.
class TThreadParams
    {
public:
    TInt result;
    };


class CTMultiThreaded: public CTStepActSch
    {
public:
    CTMultiThreaded(CTAuthSvrServer& aParent);
    ~CTMultiThreaded();
    virtual TVerdict doTestStepPreambleL();
    virtual TVerdict doTestStepPostambleL();    
    virtual TVerdict doTestStepL();
    static void runMultiThreadedTest(TThreadParams* aParams);
    static void doRegisterL(TThreadParams& aParams);
private:
    CTAuthSvrServer& iParent;
    };

// Strings for the server create test step code
_LIT(KTFirstStart,					"FIRST_START");
_LIT(KTAuthSvrCheck,				"AUTHSVR_CHECK");
_LIT(KTRegIdentity,					"REG_IDENTITY");
_LIT(KTEnumIdentities, 				"ENUM_IDENTITIES");
_LIT(KTIdentifyMultiple, 			"IDENTIFY_MULTIPLE");
_LIT(KTRetrainPlugin,				"RETRAIN_PLUGIN");
_LIT(KTForgetPlugin,				"FORGET_PLUGIN");
_LIT(KTRemoveDbs,					"REMOVE_DBS");
_LIT(KTRemoveIdentity,				"REMOVE_IDENTITY");
_LIT(KTAuthenticate,				"AUTHENTICATE");
_LIT(KTSetPrefs, 			    	"SET_PREFS");
_LIT(KTInitCenRep,					"INIT_CENREP");
_LIT(KTResultAvailability, 			"RESULT_AVAILABILITY");
_LIT(KTResultChangeNotify, 			"RESULT_NOTIFICATION");
_LIT(KTPostMarketPlugin,			"POST_MARKET_PLUGIN");
_LIT(KTStepClientSidSupportCheck, 	"CLIENT_SID_CHECK");
_LIT(KTInstallPluginStep, 			"INSTALL_PLUGIN");
_LIT(KTUninstallPluginStep, 		"UNINSTALL_PLUGIN");
_LIT(KTAuthServerOom,	 			"OOM_TEST");
_LIT(KTResetIdentity,				"RESET_IDENTITY");
_LIT(KTUpdateSysTime,				"UPDATE_SYSTIME");
_LIT(KTMultiThreadedTest,           "MULTI_THREADED");

// Performance related names
_LIT(KMaxDurationName, 				"MaxDuration");
_LIT(KMaxTestCaseDuration, 			"TEST_CASE_MAXIMUM_ALLOWED_DURATION");
_LIT(KActualTestCaseDuration, 		"TEST_CASE_ACTUAL_DURATION");
_LIT(KPerformanceTestInfo, 			"PERFORMANCE_LOG_INFORMATION");
// String used to store the filename that contains the initialisation data used by the PinAuthPlugin, etc

_LIT(KPluginIniFile, 			"\\tAuth\\tAuthSvr\\testdata\\initialisation_Info.ini");
_LIT(KPluginIniSection,			"SectionOne");
_LIT(KAuthSvrPolicyFile, 		"\\tAuth\\tAuthSvr\\testdata\\AuthSvrPolicy.ini");
_LIT(KDefaultPluginTag, 		"DefaultPlugin");
_LIT(KIdEnteredPinTag, 			"IdEnteredPinValue");
_LIT(KTrainEnteredPinTag, 		"TrainEnteredPinValue");
_LIT(KSupportsDefaultTag,		"iSupportsDefaultData");	//used to specify whether the individual plugins support default data 
_LIT(KActiveStateTag,			"activeState");	//used to specify whether the individual plugins are active
_LIT(KPinDbTag,					"Identity&PinValues");
_LIT(KTotalDbTag,				"AllUserID&PinValues");
_LIT(KDisplayMessage,			"DisplayMessage");
_LIT(KLastAuthIdTag,			"LastAuthId");
_LIT(KDefauthPrefsTag,			"DefaultPref");
_LIT(KKnowledgePrefsTag,		"KnowledgePref");
_LIT(KBiometricPrefsTag,		"BiometricPref");
_LIT(KTokenPrefsTag,			"TokenPref");

_LIT(KInitPinDatabaseValue,		",");
_LIT(KDefaultInput,				"0000");

// Strings used to store the filenames for the pluginDb files
_LIT(KPlugin3100File,		"\\tAuth\\tAuthSvr\\testdata\\Pin11113100Db.ini");
_LIT(KPlugin3101File,		"\\tAuth\\tAuthSvr\\testdata\\Pin11113101Db.ini");
_LIT(KPlugin3102File,		"\\tAuth\\tAuthSvr\\testdata\\Pin11113102Db.ini");
_LIT(KPlugin3103File,		"\\tAuth\\tAuthSvr\\testdata\\Pin11113103Db.ini");
_LIT(KPlugin3104File,		"\\tAuth\\tAuthSvr\\testdata\\Pin11113104Db.ini");
_LIT(KPlugin4100File,		"\\tAuth\\tAuthSvr\\testdata\\Pin10234100Db.ini");

#endif	/* T_AUTHSVR_STEP_H */
