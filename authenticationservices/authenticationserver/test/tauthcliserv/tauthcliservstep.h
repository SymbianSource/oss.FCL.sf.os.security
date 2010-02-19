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


#ifndef TAUTHCLISERVSTEP_H
#define TAUTHCLISERVSTEP_H

#include <e32cmn.h>
#include <ct/rcpointerarray.h>
#include <ecom/ecom.h>

#include <test/testexecutestepbase.h>
#include <test/testexecutelog.h>

#include "tauthcliserv.h"

#ifndef __INIPARSER_H__
#include <cinidata.h>
#endif // __INIPARSER_H__

// publicly available include file
#include "authserver_client.h"
#include <authserver/authplugininterface.h>

// these tests use some information which is normally
// only visible to the server, or to the client DLL.

#include "authserveripc.h"
#include "authdb.h"
#include "transientkey.h"
#include "transientkeyinfo.h"
#include "evaluator.h"

// load test plugin data
#include "../tauthplugins/consts.h"

using AuthServer::TIdentityId;
using AuthServer::TPluginId;
using AuthServer::TAuthPluginType;
using AuthServer::TAuthTrainingStatus;
using AuthServer::CAuthExpression;
using AuthServer::CAuthExpressionImpl;
using AuthServer::TAuthExpressionWrapper;
using AuthServer::CEvaluator;
using AuthServer::CPluginDesc;
using AuthServer::RAuthClient;

// Authserver name
_LIT(KAuthServerName, "!AuthServer");

_LIT(KTStepCreateDb,"CREATEDB");
_LIT(KTStepClient,	"CLIENT");
_LIT(KTStepMgrClient,	"MGRCLIENT");
_LIT(KTStepFirstStart,	"FIRSTSTART");

_LIT(KTStepAuthExprBuild, "AuthExprBuild");
_LIT(KTStepBadExtAuthExpr, "AuthExprBadAuthExt");
_LIT(KTStepAuthExprHighVer, "AuthExprHighVer");
_LIT(KTStepAuthExprEval, "AuthExprEval");

// debug-mode tests for client-side panicks
_LIT(KTStepAuthExprTypePnc, "AuthExprTypePanic");
_LIT(KTStepAuthExprTypePncBadLeft, "AuthExprTypePanicBadLeft");
_LIT(KTStepAuthExprTypePncBadRight, "AuthExprTypePanicBadRight");

// These are used in the RemoveExistingDbL() function
// applying to DBs used by tPinAuthPlugin
_LIT(KPlugin3100File,		"\\tAuth\\tAuthSvr\\testdata\\Pin11113100Db.ini");
_LIT(KPlugin3101File,		"\\tAuth\\tAuthSvr\\testdata\\Pin11113101Db.ini");
_LIT(KPlugin3102File,		"\\tAuth\\tAuthSvr\\testdata\\Pin11113102Db.ini");
_LIT(KAuthSvrPolicyFile, 	"\\tAuth\\tAuthSvr\\testdata\\AuthSvrPolicy.ini");
_LIT(KPinDbTag,				"Identity&PinValues");
_LIT(KTotalDbTag,			"AllUserID&PinValues");
_LIT(KInitPinDatabaseValue,	",");
_LIT(KDisplayMessage,		"DisplayMessage");

#define KNumPlugins 8

const TInt KTestPluginId22       = 0x10274104;
const TInt KTestPluginBlocking   = 0x10274105;
const TInt KTestPluginIdInactive = 0x10274106;
const TInt KTestPluginIdUnknown  = 0x10274107;


class CTAuthcliservStepBase : public CTestStep
	{
protected:
	CTAuthcliservStepBase() {}
	virtual ~CTAuthcliservStepBase();
	virtual TVerdict doTestStepPreambleL()
	   {
	   iFs.Connect();
	
	   CreatePlugins();
	   CreateKeyInfo();
	   
	   return TestStepResult();
	   }
	virtual TVerdict doTestStepPostambleL()
    	{
		iFs.Close();
		return TestStepResult();
		}
	
	inline void CreatePlugins();
	inline void CreateKeyInfo();
	HBufC* GetNameLC(TInt i);

	void PluginData(AuthServer::TIdentityId aId,
					AuthServer::TPluginId aPlugin,
					TPtr8 aDes) 
	{
	aDes.Fill(aId*aPlugin);
	}

	void WaitForServerToReleaseDb();
	void RemoveExistingDbL();
	//Utility function allowing text INI file parsing abilities	
	//Section name unspecified
	TInt writeToNamedConfig(const TDesC &aFileName, const TDesC &aKeyName,const TPtrC &aResult);
	
	
protected:
	RFs		iFs;


	AuthServer::CPluginDesc*       iPlugins[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId1Keys[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId2Keys[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId3Keys[KNumPlugins];
  };

inline CTAuthcliservStepBase::~CTAuthcliservStepBase() 
    {
    for (TInt i = 0 ; i < KNumPlugins ; ++i)
        {
        delete iPlugins[i];
        delete iId1Keys[i];
        delete iId2Keys[i];
        delete iId3Keys[i];
        }
    }

class CTStepActSch : public CTAuthcliservStepBase
/**
	This abstract subclass of CTAuthcliservStepBase
	overrides the doTestStep(Pre|Post)ambleL functions
	from CTestBase to allocate and free an active
	scheduler.
	
	The scheduler is stored in the protected iActSchd
	variable.  This class does not start or stop the
	scheduler.
 */
	{
private:
	// override CTestStep
	inline virtual TVerdict doTestStepPreambleL();
	inline virtual TVerdict doTestStepPostambleL();
	
protected:
	/** The active scheduler which subclasses can use. */
	CActiveScheduler* iActSchd;
	};



inline TVerdict CTStepActSch::doTestStepPreambleL()
/**
	Override CTestStep by allocating an active
	scheduler which is available to the subclass.
	
	@return					This test step's result,
							which isn't actually used
							by ThreadStepExecutionL.
 */
	{
	CTAuthcliservStepBase::doTestStepPreambleL();
	iActSchd = new(ELeave) CActiveScheduler;
	return TestStepResult();
	}


inline TVerdict CTStepActSch::doTestStepPostambleL()
/**
	Override CTestStep by deleting the active
	scheduler which allocated in doTestStepPreambleL.
	
	@return					This test step's result,
							which isn't actually used
							by ThreadStepExecutionL.
 */
	{
	CTAuthcliservStepBase::doTestStepPostambleL();
	delete iActSchd;
	return TestStepResult();
	}


class CTStepClient : public CTStepActSch
	{
public:
	CTStepClient();
	virtual TVerdict doTestStepL();
	TBool TestAuthenticate(AuthServer::RAuthClient& client);
	TBool TestAuthenticate2(AuthServer::RAuthClient& client);
	TBool TestAuthenticate3(AuthServer::RAuthClient& client);
	TBool TestAuthenticateFail(AuthServer::RAuthClient& client);



private:
	void GetDescriptionsFromEComL(RPointerArray<CPluginDesc>& aDescs);
	
	void TestIdentitiesL();
	void GetIdentitiesLC(RArray<TIdentityId>& aIds);
	void AddIdentityL(TIdentityId aIdentityId, const TDesC& aDesc);
	void TestIdentityL(TIdentityId aIdentityId, const TDesC& aExpDesc);
	void TestSetIdentityStrL();

	void TestClientPluginListsL();
	void SetSeenCountsL(const RCPointerArray<const CPluginDesc>& aPlugins);
	void TestTypedPluginsL(RAuthClient& aClient, TAuthPluginType aPluginType);
	void TestTrainedPluginsL();
	void PrepareTrainedPluginsL();
	void TestActivePluginsL(RAuthClient& aClient);
	TBool TestAuthenticateL();
	TBool TestMultiAuthL();
	
private:
	RArray<TUid> iPluginIds;
	};
	
class CTStepMgrClient : public CTStepActSch
	{
public:
	CTStepMgrClient();
	virtual TVerdict doTestStepL();
	TBool TestRegister(AuthServer::RAuthMgrClient& client);
	TBool TestMultiRegisterL(AuthServer::RAuthMgrClient& client);
	
	TBool TestMultiTrainL(AuthServer::RAuthMgrClient& client);
	TBool TestTrain(AuthServer::RAuthMgrClient& client);
	TBool TestCancellation(AuthServer::RAuthMgrClient& client);
	};

class CTStepFirstStart : public CTAuthcliservStepBase
	{
public:
	CTStepFirstStart();
	virtual TVerdict doTestStepL();
	};


inline HBufC* CTAuthcliservStepBase::GetNameLC(TInt id)
    {
    _LIT(nameTmpl, "Plugin_%d");
	HBufC* name = HBufC::NewLC(15);
	name->Des().Format(nameTmpl, id);
	return name;
    }
    
inline void CTAuthcliservStepBase::CreatePlugins()
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
			
			iPlugins[i] = CPluginDesc::NewL(i,
											*name,
											types[i],
											training[i],
											i*10000,
											i,
											i*4);
		    CleanupStack::PopAndDestroy(name);									
		}
	}
inline void CTAuthcliservStepBase::CreateKeyInfo()
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

// -------- Authentication expression tests --------

class CTStepAuthExprBuild : public CTAuthcliservStepBase
/**
	This test step ensures that complex authentication
	expressions can be constructed, or fail cleanly in OOM.
 */
	{
public:
	CTStepAuthExprBuild();
	
	// implement CTestStep
	virtual TVerdict doTestStepL();
	
private:
	void RunTestsL();
	void TestExprLD(
		void (CTStepAuthExprBuild::*aTestFuncL)(AuthServer::CAuthExpression*),
		AuthServer::CAuthExpression* aExpr);

#ifdef AUTH_EXPR_BINARY_OPS
	inline void OpTestExprLD(
		void (CTStepAuthExprBuild::*aTestFuncL)(AuthServer::CAuthExpression*),
		AuthServer::CAuthExpression* aExpr)
		{
		TestExprLD(aTestFuncL, aExpr);
		}
#else
	// this isn't an empty inline because don't want compiler
	// to build expression with binary operators.
	#define OpTestExprLD(___f, ___expr)
#endif

	void TestSimpleExprL();
	void TestPluginIdL(AuthServer::CAuthExpression* aExpr);
	void TestPluginTypeL(AuthServer::CAuthExpression* aExpr);

	void TestSimpleExprCombsConsL();
	void TestTypeAndTypeL(AuthServer::CAuthExpression* aExpr);
	void TestTypeOrTypeL(AuthServer::CAuthExpression* aExpr);
	void TestIdAndTypeL(AuthServer::CAuthExpression* aExpr);
	void TestTypeAndIdL(AuthServer::CAuthExpression* aExpr);
	void TestIdAndIdL(AuthServer::CAuthExpression* aExpr);

	void TestComplexExprCombsConsL();
	void TestAndAndL(AuthServer::CAuthExpression* aExpr);
	void TestAndOrL(AuthServer::CAuthExpression* aExpr);
	void TestOrAndL(AuthServer::CAuthExpression* aExpr);
	void TestOrOrL(AuthServer::CAuthExpression* aExpr);

	void TestFailedCombsL();
	void TestCorruptPersistL();
	};

class CTStepAuthExprTypePnc : public CTAuthcliservStepBase
/**
	This test step ensures the client is panicked when
	they attempt to retrieve the type of a corrupt expression.
 */
	{
public:
	CTStepAuthExprTypePnc();
	
	// implement CTestStep
	virtual TVerdict doTestStepL();
	};

class CTStepAuthExprTypePncBadLeft : public CTAuthcliservStepBase
/**
	This test step ensures the client is panicked when
	they attempt to retrieve the type of a expression,
	where the left subtree is corrupt.
 */
	{
public:
	CTStepAuthExprTypePncBadLeft();
	
	// implement CTestStep
	virtual TVerdict doTestStepL();
	};

class CTStepAuthExprTypePncBadRight : public CTAuthcliservStepBase
/**
	This test step ensures the client is panicked when
	they attempt to retrieve the type of a expression,
	where the right subtree is corrupt.
 */
	{
public:
	CTStepAuthExprTypePncBadRight();
	
	// implement CTestStep
	virtual TVerdict doTestStepL();
	};

class CTStepAuthExprHighVer : public CTAuthcliservStepBase
/**
	This step tests sending an unsupported authentication
	expression to the server.
	
	The server should fail the request.
 */
	{
public:
	CTStepAuthExprHighVer();
	
	// implement CTestStep
	virtual TVerdict doTestStepL();
	};
	
class CTStepBadExtAuthExpr : public CTAuthcliservStepBase
/**
	This step tests trying to authenticate a server with
	an invalid authentication expression.
	
	The client code should be panicked.
 */
	{
public:
	CTStepBadExtAuthExpr();
	
	// implement CTestStep
	virtual TVerdict doTestStepL();
	};
	

class TTestPluginInterface : public AuthServer::MEvaluatorPluginInterface
/**
	This implementation records which calls were made
	to the interface, so the test code can test the
	evaluator made the right calls in the right order.
 */
	{
public:
	virtual void Evaluate(TPluginId aPluginId, TIdentityId& aIdentity,
			   CAuthExpressionImpl::TType aType, TRequestStatus& aStatus);
	virtual void Evaluate(TAuthPluginType aPluginType, TIdentityId& aIdentity,
			   CAuthExpressionImpl::TType aType, TRequestStatus& aStatus);
	virtual void CancelEvaluate() {} ;
public:
	class TCallEntry
		{
	public:
		inline TCallEntry(TPluginId aPluginId)
		:	iCallType(CAuthExpressionImpl::EPluginId),
			iPluginId(aPluginId)
			{
			// empty.
			}
		
		inline TCallEntry(TAuthPluginType aPluginType)
		:	iCallType(CAuthExpressionImpl::EPluginType),
			iPluginType(aPluginType)
			{
			// empty.
			}
		
		bool operator==(const TCallEntry& aRhs) const;
		
	public:
		/** Type of call - ID or plugin type. */
		CAuthExpressionImpl::TType iCallType;
		
		union
			{
			TPluginId iPluginId;
			TAuthPluginType iPluginType;
			};
		};

	/**
		The sequence of Evaluate requests which have been received
		by this object.  This is a non-standard case of a T class
		owning resources.  These resources are freed by CLaunchEval
		because it is not worth making this an R-class or C-class
		for test code.
	 */
	RArray<TCallEntry> iCallLog;
	};

class TTestClientInterface : public AuthServer::MEvaluatorClientInterface
/**
	This implementation records whether the evaluation
	succeeded or failed, so the test code can check the
	result.
 */
	{
public:
	virtual void EvaluationSucceeded(TIdentityId aIdentityId);
	virtual void EvaluationFailed(TInt aReason);
	
public:
	enum TCompletionMode {ENone = 0x10, ESucceeded, EFailed};
	TCompletionMode iMode;
	/** This is valid iff iMode == ESucceeded. */
	TIdentityId iIdentityId;
	/** This is valid iff iMode == EFailed. */
	TInt iReason;
	};


class CLaunchEval : public CActive
/**
	Active object which launches an evaluation.
	
	This object is defined so there is one pending
	object when the active scheduler is started.
 */
	{
public:
	static CLaunchEval* NewL();
	virtual ~CLaunchEval();
	
	void Evaluate(const CAuthExpression* aExpr);
	
private:
	CLaunchEval();
	void ConstructL();
	void ResetInterfaces();

	// implement CActive
	virtual void RunL();
	virtual void DoCancel();

public:	
	TTestClientInterface iClientInterface;
	TTestPluginInterface iPluginInterface;
	
private:
	/**
		Expression to evaluate.  This is set for each
		call to Evaluate.
	 */
	const CAuthExpression* iExpr;
	
	/** Evaluator, which is allocated at construction. */
	CEvaluator* iEval;
	};


class CTStepAuthExprEval : public CTStepActSch
/**
	This step tests trying to authenticate a server with
	an invalid authentication expression.
	
	The client code should be panicked.
 */
	{
public:
	CTStepAuthExprEval();
	
	// implement CTestStep
	virtual TVerdict doTestStepL();
	
private:
	void TestEvalCreateL();
	void TestEvalSimpleL();
	void TestEvalAndL();
	void TestEvalOrL();
	void TestEvalResultL(
		CLaunchEval* aLaunchEval, TIdentityId aIdentityId,
		const TTestPluginInterface::TCallEntry* aExpEntries, TInt aEntryCount);
	
	void TestRPNReallocL();
	void RunOomTestsL(
		TAuthExpressionWrapper (*aAllocator)(TInt),
		TIdentityId aExpectedIdentity, TInt aInitDepth);
	};
	
const TPluginId KTestPluginId0 = 'PID0';
const TPluginId KTestPluginId1 = 'PID1';
const TPluginId KTestPluginId2 = 'PID2';
const TPluginId KTestPluginUnknown = 'UNKW';

// These constants are defined to provide short
// names for readability only.

const CAuthExpressionImpl::TType KAnd = CAuthExpressionImpl::EAnd;
const CAuthExpressionImpl::TType KOr = CAuthExpressionImpl::EOr;
const CAuthExpressionImpl::TType KPluginId = CAuthExpressionImpl::EPluginId;
const CAuthExpressionImpl::TType KPluginType = CAuthExpressionImpl::EPluginType;
#endif	/* TAUTHCLISERVSTEP_H */
