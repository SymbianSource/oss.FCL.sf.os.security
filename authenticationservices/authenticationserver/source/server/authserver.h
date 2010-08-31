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
* CAuthServer class definition
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef AUTHSERVER_H
#define AUTHSERVER_H

#include <e32cmn.h>
#include <ct/rcpointerarray.h>
#include <e32property.h>
#include <f32file.h>
#include <scs/scsserver.h>
#include "authserveripc.h"
#include "authrepository.h"

 
namespace AuthServer 
{

/**
 *Panic values associated with the AuthServer
 */
enum TAuthServerPanic
	{
	EPanicCorruptDescriptor = 0,
	/// An invalid service request number has been receieved by the
	/// server. This should not be received by those using the RAuthClient or
	/// derived classes.
	EPanicInvalidFunction = 1,
	
	EPanicAlreadyReceiving = 2,
	/// An internal error indicating the received authentication expression 
	/// is malformed. This should not be received by those using the 
	/// RAuthClient or derived classes.
	EPanicBadExternalizedAuthExpr = 3,
	/// A request has been made that refers to a non-existent plugin.
	EPanicNoSuchAuthPlugin = 4,
	/// In order to create a first identity, the authserver requires that 
	/// at least one plugin supports default data. 
	EPanicNoFirstStartPlugin = 5,
	/// A call has been made to the authserver's training manager while one 
	/// is already in progress.
	EPanicTrainingMgrBusy = 6,
	/// A plugin has reported that it supports default data but has returned
	/// an invalid data buffer.
	EPanicInvalidDefaultData = 7        
	};

const TInt KPolicyRanges= 12;
const TInt KPolicyElements = 7;

const TInt KDefaultShutdownDelay = 3 * 1000 * 1000;	// 3 seconds

class CIdentity;

class CPluginMgr;
class CPluginObserver;
class CTrainingMgr;
class CEvaluator;
class CAuthExpression;
class CAuthTransaction;
struct TLastAuth;

// The description for the default identity.
_LIT(KDefaultUserDescription, "DefaultUser");

/**
 * Helper class to manage details during identity reset operation
 **/
class TPluginResetDetails
	{
public:
	TPluginResetDetails(TPluginId aPluginId, const TDesC& aRegistrationData):
		iPluginId(aPluginId),
		iRegistrationData(aRegistrationData)
		{}

public:
	TPluginId PluginId() const { return iPluginId;};
	const TDesC& RegistrationData() const { return iRegistrationData;};

private:
	TPluginId iPluginId;
	const TDesC& iRegistrationData;
	};


/**
 * Provides authentication and identity management services.
 *
 * A CPolicyServer derived class providing management of identity
 * authentication services. Responsible for managing identities in accordance
 * with the requests made by clients.
 * 
 **/
class CAuthServer : public CScsServer, public MEvaluatorPluginInterface,
    public MEvaluatorClientInterface
	{
public:
	// implement CScsServer.
	virtual CScsSession* DoNewSessionL(const RMessage2& aMessage);
	

	// implement CScsServer.
	virtual void DoPreHeapMarkOrCheckL();
	
	// implement CScsServer.
	virtual void DoPostHeapMarkOrCheckL();
	

	static CAuthServer* NewLC(CActive::TPriority = CActive::EPriorityStandard);

	void AuthenticateL(const RMessage2& aMessage);
	
	void DeauthenticateL(const RMessage2& aMessage);
	void CancelL(const RMessage2& aMessage);
	void PluginsL(const RMessage2& aMessage);
	void ActivePluginsL(const RMessage2& aMessage);
	void PluginsByTypeL(const RMessage2& aMessage);
	void PluginsByTrainingL(const RMessage2& aMessage);
	void IdentitiesL(const RMessage2& aMessage);
	void SetIdentityStringL(const RMessage2& aMessage);
	void IdentityStringL(const RMessage2& aMessage);
	void RegisterIdentityL(const RMessage2& aMessage);
	void RemoveIdentityL(const RMessage2& aMessage);
	void TrainPluginL(const RMessage2& aMessage);
	void ForgetPluginL(const RMessage2& aMessage);
	void PreferredTypePluginL(const RMessage2& aMessage);
	void SetPreferredTypePluginL(const RMessage2& aMessage);
	void IdentitiesWithStringL(const RMessage2& aMessage);
	void ListAuthAliasesL(const RMessage2& aMessage);
	void ResolveExpressionL(const RMessage2& aMessage);
	void ResetIdentityL(TInt aFunction, const RMessage2& aMessage);
	void ResetIdentityByListL(const RMessage2& aMessage);

	virtual void Evaluate(TPluginId aPluginId,
						  TIdentityId& aIdentityId,
						  CAuthExpressionImpl::TType aType,
						  TRequestStatus& aStatus);
	
	virtual void Evaluate(TAuthPluginType aPluginType,
						  TIdentityId& aIdentityId,
						  CAuthExpressionImpl::TType aType,
						  TRequestStatus& aStatus);

	virtual void CancelEvaluate(); 
	virtual void EvaluationSucceeded(TIdentityId aIdentityId);
	virtual void EvaluationFailed(TInt aReason);

private:
	
	CAuthServer(CActive::TPriority aPriority);

	/// 2nd phase construction
	void ConstructL();
	
	~CAuthServer();

    CIdentity* CachedIdentity(const TTimeIntervalSeconds& timeout);						   

	TBool FilterActivePlugins(const CAuthPluginInterface& aInterface);

	void FirstStartL();
	void CreatePropertiesL();
	void ClearPropertiesL();

	TBool ServerBusy();
	
	void EvaluationSucceededL(TIdentityId aIdentityId) ;
	void CompleteAuthenticationL(const RMessagePtr2& aMessage, CIdentity* aId);

	typedef TBool (CAuthServer::* TInterfaceFilter)(const CAuthPluginInterface&);
	
	void FilterPluginsL(
		const RMessage2& aMessage, TInterfaceFilter aFilter);
	
	void GetDescriptionsFromEComLC(
		TInterfaceFilter aFilter, RCPointerArray<const CPluginDesc>& aDescs);
	
	TBool FilterAllPlugins(const CAuthPluginInterface&);
	TBool FilterPluginsByType(const CAuthPluginInterface&);
	TBool FilterPluginsByTraining(const CAuthPluginInterface& aInterface);

	void UpdateAuthL(TLastAuth& aAuth, TPluginId aPlugin);
	
	HBufC* StringOrNullLC(TBool aReturnString, TIdentityId id);

	void EvaluateL(TPluginId aPluginId,
				   TIdentityId& aIdentityId,
				   CAuthExpressionImpl::TType aType,
				   TRequestStatus& aStatus);
	
	void ProcessAliasStringL(RPointerArray<HBufC>& aAuthAliasList, const TDesC& aAliasStringToProcess , RBuf& aResultantAliasString);
	
	TBool CheckForAliasInAliasString(RPointerArray<HBufC>& aAuthAliasList, const TDes& aAliasString);
	CTransientKeyInfo* CreateKeyInfoLC(TPluginId aPluginId, const TDesC8& aPluginData, const CProtectionKey& aProtKey);
	TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, 
		TInt& aAction, TSecurityInfo& aMissing);
	void TokenizeStringL( const TDesC& aStringToBeProcessed, RBuf& aResultantString );
	void ResolveAliasL(	const TDesC& aAliasName, 
						RPointerArray<HBufC>& aAliasList, 
						RBuf& aResultantString);
	

	void FreeMemoryBeforeHeapMark();
	void SetupVariablesAfterHeapMarkEndL();

private:
	RFs iFs;
	/// The persistent authentication database
	CAuthDb2*  iAuthDb2;
	//plugin manager
	CPluginMgr* iPluginMgr;
	// The training manager 
	CTrainingMgr* iTrainingMgr;
	/// the auth expression evaluator
	CEvaluator* iEvaluator;
	/// the last authenticated identity
	CIdentity* iLastIdentity;
	/// the last authentication time
	TTime iLastAuthTime;
	/// holds the information about the authentication taking place
	CAuthTransaction* iAuthTransaction;
	/// used to publish information about the last authentication 
	RProperty iAuthProperty;
	/// used to notify subscribers of changes to the authenticated identity
	RProperty iAuthEventProperty;
	/// The authentication repository
	CAuthRepository* iAuthRepository;
	/// The key size to be used for protection keys
	TInt iKeySize;
	/// The parameters from the IPC Call
	CAuthParams* iParams;
	
	
	TAuthTrainingStatus iFilterTraining;
	TAuthPluginType iFilterType;
	CPluginObserver* iPluginObserver;
	
	// Server Policies
	static const TUint iRangeCount;
	static const TInt iRanges[KPolicyRanges];
	static const TUint8 iElementsIndex[KPolicyRanges];
	static const CPolicyServer::TPolicyElement iPolicyElements[KPolicyElements];
	static const CPolicyServer::TPolicy iPolicy;
	static const TStaticSecurityPolicy iPropertyWritePolicy;
	static const TStaticSecurityPolicy iPropertyReadPolicy;
	};
} //namespace

#endif // AUTHSERVER_H
