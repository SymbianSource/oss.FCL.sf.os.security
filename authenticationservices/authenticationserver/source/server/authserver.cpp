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
* CAuthServer implementation
*
*/


/**
 @file 
*/

#include <s32mem.h>
#include <s32file.h>
#include <ecom/ecom.h>
#include <pbedata.h>
#include <scs/cleanuputils.h>
#include "authserver_impl.h"
#include "authtransaction.h"
#include "authserver/aspubsubdefs.h"
#include "authserverutil.h"

using namespace AuthServer;

const TUint CAuthServer::iRangeCount = KPolicyRanges;

const TInt CAuthServer::iRanges[KPolicyRanges] = 
	{
	0,
	CScsServer::EBaseSession,
	CScsServer::EBaseSession |ERequireNoCapabilities, 		   // accessible by all clients.
	CScsServer::EBaseSession |ERequireReadUserData,		   	   // accessible by ReadUserData clients only.
	CScsServer::EBaseSession |ERequireWriteUserData,	   	   // accessible by WriteUserData clients only.
	CScsServer::EBaseSession |ERequireReadDeviceData, 	       // accessible by ReadDeviceData clients only.
	CScsServer::EBaseSession |ERequireWriteDeviceData,         // accessible by WriteDeviceData clients only.
	CScsServer::EBaseSession |ERequireTrustedUi,               // accessible by TrustedUi clients only.
	CScsServer::EBaseSession |ERequireReadDeviceAndUserData,   // accessible by clients with both
																// ReadUserData and ReadDeviceData.
	CScsServer::EBaseSession |ERequireCustomCheck,		       // Require custom check.
	CScsServer::EBaseSession |ELastService,
	CScsServer::EBaseMustAllow     			                   // SCS Internal.
	};

const TUint8 CAuthServer::iElementsIndex[iRangeCount] = 
	{
	CPolicyServer::ENotSupported,
	CPolicyServer::EAlwaysPass,
	0,  // all clients can have access 
	1,  // ReadUserData clients only
	2,  // WriteUserData clients only
	3,	// ReadDeviceData
	4,  // WriteDeviceData
	5,  // trusted ui
	6,  // ReadUserData and ReadDeviceData
	CPolicyServer::ECustomCheck,	// custom check
	CPolicyServer::EAlwaysPass, 
	CPolicyServer::EAlwaysPass, 
	};

const CPolicyServer::TPolicyElement CAuthServer::iPolicyElements[] = 
	{
	{_INIT_SECURITY_POLICY_C1(ECapability_None), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C1(ECapabilityWriteUserData), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient}, 
	{_INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C1(ECapabilityTrustedUI), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C2(ECapabilityReadDeviceData, ECapabilityReadUserData), CPolicyServer::EFailClient},
	};

const CPolicyServer::TPolicy CAuthServer::iPolicy =
	{
		CPolicyServer::EAlwaysPass, // so that any client can connect	
		iRangeCount,                                   
		iRanges,
		iElementsIndex,
		iPolicyElements,
	};

_LIT_SECURITY_POLICY_S0(CAuthServer::iPropertyWritePolicy, KAuthServerSecureId.iUid);
_LIT_SECURITY_POLICY_PASS(CAuthServer::iPropertyReadPolicy);


inline CAuthServer::CAuthServer(CActive::TPriority aPriority)
	:CScsServer(TVersion(1,0,0), iPolicy, aPriority)
	{}
	
CAuthServer* CAuthServer::NewLC(CActive::TPriority aPriority)
	{
	CAuthServer* self = new(ELeave) CAuthServer(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CAuthServer::ConstructL()
    {
    // Delay starting of shutdown timer till we finish construction since
    // loading of ECOM plugins takes a while
    CScsServer::ConstructL(0);
    TInt r;
    StartL(KAuthServerName);
	r = iFs.Connect();
    User::LeaveIfError(r);
    r = iFs.CreatePrivatePath(RFs::GetSystemDrive());
    User::LeaveIfError(r);

    // Retrieve the key size to be used for protection keys
	CPBEncryptElement* pbee = CPBEncryptElement::NewLC(_L(""));
    iKeySize = pbee->EncryptionData().AuthData().Key().Size();
    CleanupStack::PopAndDestroy(pbee);
    iAuthRepository = CAuthRepository::NewL();
	iAuthDb2 = CAuthDb2::NewL(iFs);
	iPluginMgr  = CPluginMgr::NewL();
    iPluginObserver = CPluginObserver::NewL(*iPluginMgr);
    iEvaluator = CEvaluator::NewL(this,this);
    iTrainingMgr = new (ELeave) CTrainingMgr(*iPluginMgr, *iAuthDb2, *iAuthRepository);
	TUid clientSid = {0};
	iParams = CAuthParams::NewL(0,
								EFalse,
								clientSid,
								EFalse,
								KNullDesC());
    CreatePropertiesL();
	FirstStartL();

	// Construction complete, now start the shutdown timer
	CScsServer::EnableShutdownTimerL(AuthServer::KDefaultShutdownDelay);
    }
    
    
CPolicyServer::TCustomResult CAuthServer::CustomSecurityCheckL(const RMessage2& aMessage, 
	TInt& /*aAction*/, TSecurityInfo& aMissing)
	{
	TInt KAuthParams = 1; 
	TInt paramsLen = aMessage.GetDesLength(KAuthParams);
	
	if(paramsLen != KErrBadDescriptor)
		{
		//Create a buffer.
		HBufC8* buffer = HBufC8::NewLC(paramsLen);
		TPtr8 bufDes(buffer->Des());
		aMessage.ReadL(EIpcArgument1, bufDes);
			
		//Internalize the params object.
		RDesReadStream readStream(*buffer);
		CleanupClosePushL(readStream);
		iParams->InternalizeL(readStream);
		CleanupStack::PopAndDestroy(2, buffer);
		}

	TBool clientKey = iParams->iClientKey;
	TBool withString = iParams->iWithString;
	TInt clientSid = iParams->iClientSid.iUid;
		
	if((!clientKey || withString || ((clientSid != aMessage.SecureId()) && clientSid != 0 )) && !(aMessage.HasCapability(ECapabilityReadUserData)))
		{
		// Missing ReadUserData capability.
		aMissing.iCaps.Set(ECapabilityReadUserData);
		return CPolicyServer::EFail;
		}	
		
	else
		{
		return CPolicyServer::EPass;	
		}
	}
	
/**
 * Creates and initialises the two AuthServer properties -
 * KUidAuthServerLastAuth and KUidAuthServerAuthChangeEvent.
 *
 **/
void CAuthServer::CreatePropertiesL()
	{
	// KUidAuthServerLastAuth
	TInt err1 = iAuthProperty.Define(KUidAuthServerLastAuth, RProperty::EText,
									iPropertyReadPolicy, iPropertyWritePolicy,
									sizeof(TLastAuth));

	User::LeaveIfError(iAuthProperty.Attach(KAuthServerSecureId,
											KUidAuthServerLastAuth));

	
	// KUidAuthServerAuthChangeEvent
	TInt err2 = iAuthEventProperty.Define(KUidAuthServerAuthChangeEvent,
									RProperty::EInt,
									iPropertyReadPolicy,
									iPropertyWritePolicy);
	
	User::LeaveIfError(iAuthEventProperty.Attach(KAuthServerSecureId,
												 KUidAuthServerAuthChangeEvent));

	if (err1 == KErrNone || err2 == KErrNone )
		{
		ClearPropertiesL();
		}
	}

void CAuthServer::ClearPropertiesL()
	{
	TLastAuth lastAuth;
	TPckg<TLastAuth> authPckg(lastAuth);
	User::LeaveIfError(iAuthProperty.Set(authPckg));
	User::LeaveIfError(iAuthEventProperty.Set(KUnknownIdentity));
	}


/**
 * Checks the number of identities in the database and iff zero asks the
 * training mgr to register the first identity.
 *
 * @leave KErrAuthServerCanNotCreateFirstId if the first identity cannot
 * be created
 **/
void CAuthServer::FirstStartL()
	{
	TInt numIds = iAuthDb2->NumIdentitiesL();
	if (numIds == 0)
		{
		TIdentityId id = 0;
		TPckg<TIdentityId> idPkg(id);
		TRandom::RandomL(idPkg);
		
		CProtectionKey* key = CProtectionKey::NewL(iKeySize);
		if (!iTrainingMgr->RegisterFirstIdentityL(id, *key))
			{
			User::Panic(KAuthServerShortName, EPanicNoFirstStartPlugin);
			}
		}
	}

CAuthServer::~CAuthServer()
	{
	delete iAuthRepository;
	delete iAuthTransaction;
	
	delete iTrainingMgr;
	delete iLastIdentity;
	delete iEvaluator;
	delete iPluginMgr;
	delete iPluginObserver;	
	delete iParams;
	if (iAuthDb2 != 0)
		{
		iAuthDb2->CompactIfRequired();
		delete iAuthDb2;
		}
	
	iFs.Close();
	REComSession::FinalClose();
	}

		
/**
 *  Creates a new session
 * @param aClientVersion the version of the connecting client
 * @param aMessage the connect message
 */
CScsSession* CAuthServer::DoNewSessionL(const RMessage2& /*aMessage*/)
	{
	return CAuthServerSession::NewL(*this);
	}


/**
 * Returns the last identity to be authenticated within the period 
 * currTime to currTime-timeout.  
 * 
 * @param currTime the current universal time
 * 
 * @param timeout the number of seconds within which the last
 * authentication should have been made
 *
 * @return 0 if there has been no previous authentications either in the
 * server lifetime or within the timeout period. 
 *
 * @return a pointer to the cached identity object
 **/
CIdentity* CAuthServer::CachedIdentity( const TTimeIntervalSeconds& aTimeOut)
    {
    // process request
    TTime currentTime;
	if(currentTime.UniversalTimeSecure() == KErrNoSecureTime)
		{
		// Fall back to nonsecure time. 
		currentTime.UniversalTime();
		}
    
    CIdentity* id = 0;
  
    TTimeIntervalSeconds timeSinceLast;
    currentTime.SecondsFrom(iLastAuthTime, timeSinceLast);    
   
    if (timeSinceLast.Int() >= 0 && timeSinceLast < aTimeOut)
		{
		id = iLastIdentity;
		}
    
    return id;
    }
    


/**
 *
 * @param aMessage the message to process
 **/
void CAuthServer::DeauthenticateL(const RMessage2& aMessage)
	{
	if (ServerBusy())
		{
		aMessage.Complete(KErrServerBusy);
		return;
		}

	delete iLastIdentity;
	iLastIdentity = 0;
	iLastAuthTime = 0L;

	ClearPropertiesL();	
	aMessage.Complete(KErrNone);
	}

HBufC* CAuthServer::StringOrNullLC(TBool aReturnString, TIdentityId aId)
    {
	HBufC* str = 0;
	if (aReturnString)
		{
		TRAPD(err, str = iAuthDb2->DescriptionL(aId));  
		if (!err)
			{
			CleanupStack::PushL(str);
			}
		
		}
	if (str == 0)
		{
		str = HBufC::NewLC(0);  
		}
	return str;
    }

void CAuthServer::AuthenticateL(const RMessage2& aMessage)
    {
	if (ServerBusy())
		{
		aMessage.Complete(KErrServerBusy);
		return;
		}
		
	TInt KAuthParams = 1; 
	TInt paramsLen = aMessage.GetDesLength(KAuthParams);
	
	if(paramsLen != KErrBadDescriptor)
		{
		//Create a buffer.
		HBufC8* buffer = HBufC8::NewLC(paramsLen);
		TPtr8 bufDes(buffer->Des());
			
		aMessage.ReadL(EIpcArgument1, bufDes);
		
		//Internalize the params object.
		RDesReadStream readStream(*buffer);
		CleanupClosePushL(readStream);
		iParams->InternalizeL(readStream);
		CleanupStack::PopAndDestroy(2, buffer);	
		}
    
	CIdentity* id = CachedIdentity(iParams->iTimeout);
  
	if (id == 0) // no valid cache available
		{
		const TInt KAuthExprParam = 0;
	
		// reconstruct the authentication expression
		TInt extLen = aMessage.GetDesLengthL(KAuthExprParam);
		HBufC8* extSrv = HBufC8::NewLC(extLen);
		TPtr8 desSrv8 = extSrv->Des();
		aMessage.ReadL(KAuthExprParam, desSrv8);
		
		RDesReadStream drs(desSrv8);
		CAuthExpression* authExpr = CAuthExpressionImpl::NewL(drs);
		// don't need the stream any more, so free it
		CleanupStack::PopAndDestroy(extSrv);
		CleanupStack::PushL(authExpr);

		const CAuthExpressionImpl* expr = static_cast<const CAuthExpressionImpl*>(authExpr);
		
		iAuthTransaction = CAuthTransaction::NewL(aMessage,
												  iParams->iClientKey,
												  iParams->iClientSid.iUid,
												  iParams->iWithString,
												  *iParams->iClientMessage,
												  expr);
		CleanupStack::Pop(authExpr);
		
		iEvaluator->Evaluate(expr);
		
		return;
        }
        
	else if (iParams->iClientKey && (id->Id() != KUnknownIdentity))
		{
		
		HBufC* str = StringOrNullLC(iParams->iWithString, id->Id());
		CIdentity* id2 = 0;
		
		TInt clientId = aMessage.SecureId();
		
		if(iParams->iClientSid.iUid != 0)
			{
			clientId = iParams->iClientSid.iUid;
			}
		
		id2 = CIdentity::NewL(id->Id(), id->Key().ClientKeyL(clientId), str);
			
		CleanupStack::Pop(str);
		CleanupStack::PushL(id2);
		// id pointer already stored as iLastIdentity
		id = id2; 
		}
  
	CompleteAuthenticationL(aMessage, id);
  
	if (id != iLastIdentity) 
		{
		// only clean up the allocated id if it has not been transferred to us.
		CleanupStack::PopAndDestroy(id);  
		}
   
	}

// -------- plugin lists --------


/**
	Build an array containing descriptions for the
	plugins available on the system and send it to
	the client.
	
	@param	aMessage		Message describing client
							request.
	@param	aFilter			Function which decides whether
							or not to include a plugin in the
							list.
 */
void CAuthServer::FilterPluginsL(
	const RMessage2& aMessage, CAuthServer::TInterfaceFilter aFilter)

	{
	RCPointerArray<const CPluginDesc> descs;
	GetDescriptionsFromEComLC(aFilter, descs);
	AuthServerUtil::SendDataPointerArrayL(aMessage, descs, 0);
	CleanupStack::PopAndDestroy(&descs);
	}


/**
	Populate the supplied array with plugin descriptions generated
	from the available ECOM plugins.
	
	@param	aFilter			Predicate function decides whether or not
							to include each plugin in the list.
	@param	aDescs			Array to populate.  When this function is
							called, any existing entries are removed.
							If this function leaves then any entries are
							removed from the array.  On success, the
							array is on the cleanup stack.
 */
void CAuthServer::GetDescriptionsFromEComLC(
	CAuthServer::TInterfaceFilter aFilter,
	RCPointerArray<const CPluginDesc>& aDescs)
	{
	aDescs.Reset();
	CleanupClosePushL(aDescs);
	
	// ownership of the data pointed by this array is with 
	// the plugin manager. 
	const RPointerArray<CImplementationInformation>& implInfo = iPluginMgr->ImplementationsL();
	TInt implCount = implInfo.Count();
	
	for (TInt i = 0; i < implCount; ++i)
		{
		// avoid RVCT warning C2874W: pi may be used before being set
		CAuthPluginInterface* pi = 0;
		
		TRAPD(r, pi = iPluginMgr->ImplementationL(i));
		
		// skip plugins which are not available
		if (KErrNone != r)
			continue;
		
		if (! (this->*aFilter)(*pi))
			{
			continue;
			}
		
		// get training status from db
		TAuthTrainingStatus ts = iAuthDb2->PluginStatusL(pi->Id());
		
		CPluginDesc* pd = CPluginDesc::NewL(
			pi->Id(), pi->Name(), pi->Type(),
			ts, pi->MinEntropy(),
			pi->FalsePositiveRate(), pi->FalseNegativeRate() );
				
		CleanupStack::PushL(pd);
		aDescs.AppendL(pd);
		CleanupStack::Pop(pd);
		}
	
	}



/**
	Build an array containing plugin descriptions for
	each plugin which is available on the system, and
	copy this into the client's space.
	
	@param	aMessage		Client message.  This contains
							the client-side buffer.
 */
void CAuthServer::PluginsL(const RMessage2& aMessage)
	{
	FilterPluginsL(aMessage, &CAuthServer::FilterAllPlugins);
	}


/**
	Predicate function used by PluginsL.. This accepts all plugins.
	
	@return					Always ETrue.
	@see PluginsL
 */
TBool CAuthServer::FilterAllPlugins(const CAuthPluginInterface&)
	{
	return ETrue;
	}


/**
 *
 * @param aMessage the message to process
 **/
void CAuthServer::ActivePluginsL(const RMessage2& aMessage)
	{
	FilterPluginsL(aMessage, &CAuthServer::FilterActivePlugins);
	}


/**
 * Indicates a plugin is active. 
 *
 * @param aInterface the auth plugin interface to check
 * @return true if aInterface.IsActive()
 **/
TBool CAuthServer::FilterActivePlugins(const CAuthPluginInterface& aInterface)
	{
	return aInterface.IsActive();
	}


/**
	Build an array which containing plugin descriptions
	for each plugin which is available and has the type
	supplied by the client.
	
	@param	aMessage		Client message.  This contains
							the plugin type and points to the
							client-side buffer.
 */
void CAuthServer::PluginsByTypeL(const RMessage2& aMessage)
	{
	iFilterType = static_cast<TAuthPluginType>(aMessage.Int1());
	FilterPluginsL(aMessage, &CAuthServer::FilterPluginsByType);
	}


/**
	Predicate which checks whether the supplied description should
	be included in the result list.
	
	The type to filter on is stored as a member variable.
	
	@param	aInterface		Interface to check.
	@return					Zero if interface's type does not match
							filter type, non-zero otherwise.
	@see PluginsByTypeL
 */
TBool CAuthServer::FilterPluginsByType(const CAuthPluginInterface& aInterface)
	{
	return (aInterface.Type() == iFilterType);
	}


/**
	Build an array which contains plugin descriptions
	for each plugin which has the supplied training status,
	and write that array into the caller's space.
	
	@param	aMessage		Client message contains the status
							to filter on.
 */
void CAuthServer::PluginsByTrainingL(const RMessage2& aMessage)
	{
	iFilterTraining = static_cast<TAuthTrainingStatus>(aMessage.Int1());
	FilterPluginsL(aMessage, &CAuthServer::FilterPluginsByTraining);
	}


/**
	Predicate checks if the supplied interface describes
	a plugin with the required training status.
	
	@param	aInterface		Interface to check.
	@return					Zero if interface's training status
							does not match the filter status;
							non-zero otherwise.
	@see PluginsByTrainingL
 */
TBool
CAuthServer::FilterPluginsByTraining(const CAuthPluginInterface& aInterface)
	{
	// training status is stored in the db, not in the ECOM interface.
	// If this function returns non-zero, the same request will be made
	// on the DB to get the status again to construct the description.
	// This is suboptimal, and could be improved by special-casing the
	// training filter if required.

	TAuthTrainingStatus ts = EAuthUntrained;
	TRAPD(err, ( ts = iAuthDb2->PluginStatusL(aInterface.Id())));
	return err == KErrNone ? ts == iFilterTraining : EFalse;
	}


/**
	Populate a client-side array with the set of identities.
	
	@param	aMessage		Client message which points to the
							user-side array.
 */
void CAuthServer::IdentitiesL(const RMessage2& aMessage)
	{
    RArray<TIdentityId> ids;
    iAuthDb2->IdentitiesL(ids);
    CleanupClosePushL(ids);
    AuthServerUtil::SendDataArrayL(aMessage, ids, 0);
    CleanupStack::PopAndDestroy(&ids);
	}
/**
	Retrieve the preferred plugin for the specified type
	
	@param	aMessage		Client message
 */
void CAuthServer::PreferredTypePluginL(const RMessage2& aMessage)
	{
    TPluginId id =
	  iAuthDb2->PreferredPluginL(static_cast<TAuthPluginType>(aMessage.Int0()));
	
	TPckg<TPluginId> idPckg(id);
	aMessage.WriteL(1, idPckg);
	aMessage.Complete(KErrNone);
	}

/**
	Set the preferred plugin for the specified type
	
	@param	aMessage		Client message
 */
void CAuthServer::SetPreferredTypePluginL(const RMessage2& aMessage)
	{
	TInt            err        = KErrArgument;
	TAuthPluginType pluginType = static_cast<TAuthPluginType>(aMessage.Int0());
	TPluginId       pluginId   = aMessage.Int1();
	
	if (iPluginMgr->PluginL(pluginId)->Type() == pluginType)
		{
		iAuthDb2->SetPreferredPluginL(pluginType,pluginId);
		err = KErrNone;
		}
	aMessage.Complete(err);
	}

/**
	Retrieve description for a supplied identity.
	
	@param	aMessage		Client message which contains the
							identity and points to a client-side
							buffer, to which the description will
							be copied.
 */
void CAuthServer::IdentityStringL(const RMessage2& aMessage)
	{
	HBufC* desc = iAuthDb2->DescriptionL(aMessage.Int1());
	CleanupStack::PushL(desc);
	aMessage.WriteL(EIpcArgument0, *desc);	
	CleanupStack::PopAndDestroy(desc);
	aMessage.Complete(KErrNone);	
	}

/**
	Retrieve description & id for all identities.
	
	@param	aMessage		Client message which contains the
							identity and points to a client-side
							buffer, to which the description will
							be copied.
 */
void CAuthServer::IdentitiesWithStringL(const RMessage2& aMessage)
	{
	RIdAndStringArray result;

	iAuthDb2->IdentitiesWithDescriptionL(result);
	CleanupClosePushL(result);

	AuthServerUtil::SendDataPointerArrayL(aMessage,result,0);
	CleanupStack::PopAndDestroy(&result);
	}

/**
 *
 * @param aMessage the message to process
 **/
void CAuthServer::SetIdentityStringL(const RMessage2& aMessage)
	{
	HBufC* str = HBufC::NewLC(aMessage.GetDesLengthL(1));
	TPtr strDes = str->Des();
	TInt err = aMessage.Read(1, strDes);

	if (err == KErrNone)
	  {
	  iAuthDb2->SetDescriptionL(aMessage.Int0(), *str);
	  }	
	CleanupStack::PopAndDestroy(str);
	aMessage.Complete(err);
	}


 
/**
	Copies the authentication alias list obtained using 
	ListAliasL() method to a client-side buffer .
	
	@param	aMessage		Client message which points to a client-side
							buffer, to which the authentication strength 
							list will be copied.
 */
 
void CAuthServer::ListAuthAliasesL(const RMessage2& aMessage)
	{
	RPointerArray<HBufC> aliasList;
	CleanupResetAndDestroyPushL(aliasList);
	
	//get the list of available authentication strengths from the cenrep file.
	iAuthRepository->ListAliasL(aliasList);

	AuthServerUtil::SendDataPointerArrayL(aMessage, aliasList, EIpcArgument0);
	CleanupStack::PopAndDestroy(&aliasList);
	}
	
/**
	Resolves any alias occurrence in the expression to its corresponding
	value. 
	
	@param	aMessage	Client message which contains the free form
						authentication expression and points to a 
						client-side buffer, to which the string will 
						be copied.
	
 */
void CAuthServer::ResolveExpressionL(const RMessage2& aMessage)
	{
	
	// the auth strength passed by the client. 
	TInt length = aMessage.GetDesLength(EIpcArgument1);
	HBufC* clientExpression = HBufC::NewLC(length);
	TPtr clientExprPtr(clientExpression->Des());

	aMessage.ReadL(EIpcArgument1, clientExprPtr);
	
	RBuf resultantString;
	CleanupClosePushL(resultantString);
	
	resultantString.CreateL(KDefaultBufferSize/2);
		
	// get the alias list
	RPointerArray<HBufC> aliasList;
	CleanupResetAndDestroyPushL(aliasList);
	
	//get the list of available authentication strength from the cenrep file.
	iAuthRepository->ListAliasL(aliasList);
		
		
	// parse the client side expression to see whether there
	// are any alias, if present process them accordingly.
	TLex input(clientExprPtr);
	
	// append the open bracket first so that the entire expression is within brackets.
	resultantString.Append(KOpenBracket);
	
	for(TPtrC token = input.NextToken(); token.Size() > 0; token.Set(input.NextToken()))
		{
		
		if(	token.CompareF(KOpenBracket) == 0 ||
			token.CompareF(KCloseBracket) == 0 ||
			token.CompareF(KAuthOpAnd) == 0 ||
			token.CompareF(KAuthOpOr) == 0
			)
			{
			int reqdBufferLength = resultantString.Length() + token.Length();			
			
			if(resultantString.MaxLength() < reqdBufferLength)
				{
				if(resultantString.MaxLength() == 0)
					{
					resultantString.Close();
					}
				resultantString.ReAllocL(reqdBufferLength);
				}
						
			resultantString.Append(token);
			}
		else if(token.CompareF(KAuthBiometric) == 0 ||
				token.CompareF(KAuthDefault) == 0 ||
				token.CompareF(KAuthKnowledge) == 0 ||
				token.CompareF(KAuthToken) == 0 ||
				token.CompareF(KAuthPerformance) == 0	)
			{
			RBuf tokenType;
			CleanupClosePushL(tokenType);
			TokenizeStringL(token, tokenType);
			int reqdBufferLength = resultantString.Length() + tokenType.Length();
			if(resultantString.MaxLength() < reqdBufferLength)
				{
				if(resultantString.MaxLength() == 0)
					{
					resultantString.Close();
					}
				resultantString.ReAllocL(reqdBufferLength);
				}
			
			resultantString.Append(tokenType);
			CleanupStack::PopAndDestroy(&tokenType);
			}
		else
			{
			// should not be a number
			TInt32 val = 0;
			TLex value(token);
			if(value.Val(val) == KErrNone)
				{
				RBuf tokenPluginId;
				CleanupClosePushL(tokenPluginId);
				TokenizeStringL(token, tokenPluginId);
				int reqdBufferLength = resultantString.Length() + tokenPluginId.Length();
				if(resultantString.MaxLength() < reqdBufferLength)
					{
					if(resultantString.MaxLength() == 0)
						{
						resultantString.Close();
						}
					resultantString.ReAllocL(reqdBufferLength);
					}
				resultantString.Append(tokenPluginId);
				CleanupStack::PopAndDestroy(&tokenPluginId);
				continue;
				}
			
			RBuf aliasString;
			CleanupClosePushL(aliasString);
				
			// this is a alias value which should be processed
			ResolveAliasL(token, aliasList, aliasString);
			int reqdBufferLength = resultantString.Length() + aliasString.Length();
			if(resultantString.MaxLength() < reqdBufferLength)
				{
				if(resultantString.MaxLength() == 0)
					{
					resultantString.Close();
					}
				resultantString.ReAllocL(reqdBufferLength);
				}
			resultantString.Append(aliasString);
			CleanupStack::PopAndDestroy(&aliasString);
			}
		}
	
	int reqdBufferLength = resultantString.Length() + KCloseBracket().Length();
	
	if(resultantString.MaxLength() < reqdBufferLength)
		{
		if(resultantString.MaxLength() == 0)
			{
			resultantString.Close();
			}
		resultantString.ReAllocL(reqdBufferLength);
		}
										
	// append the close bracket in the end so that the entire expression is within brackets.
	resultantString.Append(KCloseBracket);
					
	//see if we have enough space on the client.
	length = resultantString.Length();
	if(aMessage.GetDesMaxLengthL(EIpcArgument0) < length)
		{
		TPckg<TInt> lenPckg(length);
		aMessage.WriteL(EIpcArgument0, lenPckg);
		aMessage.Complete(KErrOverflow);
		}
	
	aMessage.WriteL(EIpcArgument0, resultantString);	
	CleanupStack::PopAndDestroy(3, clientExpression);	//aliasList, resultantString.
	aMessage.Complete(KErrNone);
	}

/**
	Inserts space in between operators and inserts brackets for as 
	expression
	
	@param	aStringToBeProcessed	string to be tokenized.
	@param  aResultantString		would contain the final tokenized
									string
 */

void CAuthServer::TokenizeStringL( const TDesC& aStringToBeProcessed, RBuf& aResultantString )
	{
	TInt newLength = aStringToBeProcessed.Length() + 2;
	HBufC* resultantBuffer = HBufC::NewLC(newLength);
	TPtr resultantBufPtr(resultantBuffer->Des());
	resultantBufPtr.Append(KOpenBracket);
	resultantBufPtr.Append(aStringToBeProcessed);
	TInt index = resultantBufPtr.Length();
	resultantBufPtr.Append(KCloseBracket);
	
	CAuthExpressionImpl::InsertSpaceBetweenOperatorsL(*resultantBuffer, aResultantString);
	CleanupStack::PopAndDestroy(resultantBuffer);
	
	}

/**
	Retrieves the alias string corresponding to the 
	supplied authentication strength.
	
	@param	aMessage	Client message which contains the
						authentication strength and points to a 
						client-side buffer, to which the alais
						string will be copied.
	
 */
 	
void CAuthServer::ResolveAliasL(const TDesC& aAliasName, 
								RPointerArray<HBufC>& aAliasList, 
								RBuf& aResultantString)
	{
	HBufC* aliasString = HBufC::NewLC(KDefaultBufferSize/2);
	TPtr aliasStringPtr(aliasString->Des());
	
	TBool aliasFound = EFalse;
		
	// find a match for the client supplied alias from the aliasList.
	for(TInt i = 0; i < aAliasList.Count(); ++i)
		{
		if(aAliasName.CompareF(*aAliasList[i]) != 0)
			{
			continue;
			}
		
		aliasFound = ETrue;
		// retrieve the alias string corresponding to a given authentication strength.
		iAuthRepository->GetAliasDefinitionL(i, aliasStringPtr);
		if(aliasStringPtr.Length() == 0)
			{
			User::Leave(KErrAuthStrengthAliasNotDefined);
			}
			
		// enclose the alias string within brackets.This would facilitate easy evaluation 
		// of the alias string expression.
		TInt newLength = aliasStringPtr.Length() + 2;
		if(newLength > aliasStringPtr.MaxLength())
			{
			aliasString->ReAllocL(newLength);
			}
			
		aliasStringPtr.Insert(0, KOpenBracket);
		TInt index = aliasStringPtr.Length();
		aliasStringPtr.Insert(index, KCloseBracket);
		
		// tokenize aliasString, to facilitate parsing using TLex.
		CAuthExpressionImpl::InsertSpaceBetweenOperatorsL(*aliasString, aResultantString);
				
		if(aResultantString.Length() > aliasStringPtr.MaxLength())
			{
			aliasString = aliasString->ReAllocL(aResultantString.Length());
			}
		
		aliasStringPtr.Copy(aResultantString);
		aResultantString.Close();
		
		// aliasString contains an alias, so process it until we end up 
		// with an expression containing plugin Id or plugin Type or a combination of both.		
		ProcessAliasStringL(aAliasList, *aliasString, aResultantString);
				
		// ensure that the aliasString processing doesn't end in an infinite loop.
		// In case it does , leave with KErrAuthServInvalidAliasStringExpression.
		// For instance : fast = (medium & weak), medium = fast i.e 
		// medium = (medium & weak).
				
		if(aResultantString.FindC(aAliasName) != KErrNotFound)
			{
			User::Leave(KErrAuthServInvalidAliasStringExpression);
			}
							
		TBool found = CheckForAliasInAliasString(aAliasList, aResultantString);
		// 'resultantAliasString' may inturn contain an alias.So loop through the ProcessAliasStringL,
		// until  we end up with an alias string containing only plugin Id,plugin Type or a combination of both.
		while(found)
			{
			for(TInt j = 0; j < aAliasList.Count(); ++j)
				{
				if(aResultantString.FindC(*aAliasList[j]) != KErrNotFound)
					{
					if(aResultantString.Length() > aliasStringPtr.MaxLength())
						{
						aliasString->ReAllocL(aResultantString.Length());
						}
							
					aliasStringPtr.Copy(aResultantString);
					aResultantString.Close();
										
					CAuthExpressionImpl::InsertSpaceBetweenOperatorsL(*aliasString, aResultantString);
					if(aResultantString.Length() > aliasStringPtr.MaxLength())
						{
						aliasString->ReAllocL(aResultantString.Length());
						}
						
					aliasStringPtr.Copy(aResultantString);
					aResultantString.Close();
								
				 	ProcessAliasStringL(aAliasList, *aliasString, aResultantString);
					// ensure that the aliasString processing doesn't end in an infinite loop.
					// In case it does , leave with KErrArgument.
					// For instance : fast = (medium & weak), medium = fast i.e 
					// medium = (medium & weak).
					if(aResultantString.FindC(aAliasName) != KErrNotFound)
						{
						User::Leave(KErrAuthServInvalidAliasStringExpression);
						}
					break;
					}
				}
					
				// check if 'resultantExpr' still contains an alias.
			found = CheckForAliasInAliasString(aAliasList, aResultantString);				
			
			}
				
		break;
		}
	
	CleanupStack::PopAndDestroy(aliasString);
	
	//if the client supplied alias is not in the alias list, leave.
	if(!aliasFound)
		{
		User::Leave(KErrUnknownAuthStrengthAlias);
		}

	}

// Resets the training data for the supplied identity.
//	
// @param aMessage Client message which contains the details of 
//					identity and plugins for doing the reset
void CAuthServer::ResetIdentityL(TInt aFunction, const RMessage2& aMessage)
	{
	// Check if either an authentication or a training is going on
	if (ServerBusy())
		{
		aMessage.Complete(KErrServerBusy);
		return;
		}

	// For reset by type only EAuthKnowledge type is supported
	if (aFunction == EResetIdentityByType)
		{
		TAuthPluginType pluginType = static_cast<TAuthPluginType>(aMessage.Int1());
		if (pluginType != EAuthKnowledge)
			{
			aMessage.Complete(KErrAuthServResetMayLoseIdentity);
			return;
			}
		}

	// Get the list of trained plugins for this identity
	TIdentityId identityId = static_cast<TIdentityId>(aMessage.Int0());
	RArray<TPluginId> trainedPluginIdList;
	CleanupClosePushL(trainedPluginIdList);
	iAuthDb2->TrainedPluginsListL(identityId, trainedPluginIdList);

	// Ensure at least one plugin knows identity
	TInt numTrained = trainedPluginIdList.Count();
	if (numTrained < 1)
		{
		CleanupStack::PopAndDestroy(&trainedPluginIdList);
		aMessage.Complete(KErrAuthServIdentityNotFound);
		return;
		}

	// Try and ensure that after reset at least one plugin has an identity trained
	// Since all the trained plugins for the identity are being reset make sure
	// at least one of them is of the knowledge type to be reasonably sure it registers 
	// the new identity data
	TBool knowledgePluginFound = EFalse;
	for (TInt index = 0; index < numTrained; ++index)
		{
		TPluginId pluginId = trainedPluginIdList[index];
		CAuthPluginInterface* plugin = iPluginMgr->PluginL(pluginId);
		if (plugin->Type() == EAuthKnowledge)
			{
			knowledgePluginFound = ETrue;
			break;
			}
		}
	if (!knowledgePluginFound)
		{
		CleanupStack::PopAndDestroy(&trainedPluginIdList);
		aMessage.Complete(KErrAuthServResetMayLoseIdentity);
		return;
		}

	// Get the registration data
	HBufC* regData = NULL;
	TInt ipcArg = (aFunction == EResetIdentity) ?  EIpcArgument1:EIpcArgument2;
	regData = HBufC::NewLC(aMessage.GetDesLengthL(ipcArg));
	TPtr regPtr = regData->Des();
	aMessage.ReadL(ipcArg, regPtr);

	// Finally start the reset process
	// Generate a new protection key
	CProtectionKey* protKey = CProtectionKey::NewLC(iKeySize);
	TInt lastErr = KErrNone;
	TBool oneSuccess = EFalse; // To keep track if at least one reset succeeded
	for (TInt index = 0; index < numTrained; ++index)
		{
		TPluginId pluginId = trainedPluginIdList[index];
		CAuthPluginInterface* plugin = iPluginMgr->PluginL(pluginId);
		HBufC8* result = NULL;
		TInt err = KErrNone;
		// For reset by type the registration data needs to be specified only for the plugins of specified type
		// and this restricted to knowledge type only
		if ((aFunction == EResetIdentity) ||
			((aFunction == EResetIdentityByType) && (plugin->Type() == EAuthKnowledge)))
			{
			err = plugin->Reset(identityId, *regData, result);
			}
		else
			{
			err = plugin->Reset(identityId, KNullDesC, result);
			}
		if (err == KErrNone && result)
			{
			oneSuccess = ETrue;
			// Use the plugin data to generate transient key and then encrypt the protection key
			// using the transient key. A plugin may not return data if it does not use the supplied
			// registration information 
			CleanupStack::PushL(result);
			CTransientKeyInfo* keyInfo = CreateKeyInfoLC(pluginId, *result, *protKey);
			// Replace the trained information in the db
			iAuthDb2->SetTrainedPluginL(identityId, pluginId, *keyInfo);
			CleanupStack::PopAndDestroy(2, result); // keyInfo
			}
		else if (err == KErrNone)
			{
			oneSuccess = ETrue;
			// Remove the entry in the auth db for the plugin
			// Ignore errors
			TRAP_IGNORE(iAuthDb2->RemoveTrainedPluginL(identityId, pluginId));
			}
		else
			{
			// Remember the last error
			lastErr = err;
			}
		}

	CleanupStack::PopAndDestroy(3, &trainedPluginIdList); // regData, protKey
	
	// If none of the plugins reset correctly then return the last error
	if (oneSuccess)
		{
		lastErr = KErrNone;
		}
	
	aMessage.Complete(lastErr);
	}

// Resets the training data for the supplied identity.
//	
// @param aMessage Client message which contains the details of 
//					identity and plugins for doing the reset
void CAuthServer::ResetIdentityByListL(const RMessage2& aMessage)
	{
	// Check if either an authentication or a training is going on
	if (ServerBusy())
		{
		aMessage.Complete(KErrServerBusy);
		return;
		}

	// Get the list of trained plugins for this identity
	TIdentityId identityId = static_cast<TIdentityId>(aMessage.Int0());
	RArray<TPluginId> trainedPluginIdList;
	CleanupClosePushL(trainedPluginIdList);
	iAuthDb2->TrainedPluginsListL(identityId, trainedPluginIdList);

	// Ensure at least one plugin knows identity
	TInt numTrained = trainedPluginIdList.Count();
	if (numTrained < 1)
		{
		CleanupStack::PopAndDestroy(&trainedPluginIdList);
		aMessage.Complete(KErrAuthServIdentityNotFound);
		return;
		}

	// Extract the array of plugin ids and their registration information
	RArray<TPluginId> pluginIdList;
	CleanupClosePushL(pluginIdList);
	HBufC8* buf = HBufC8::NewLC(aMessage.GetDesLengthL(EIpcArgument1));
	TPtr8 ptr = buf->Des();
	aMessage.ReadL(EIpcArgument1, ptr);
	RDesReadStream stream(*buf);
	CleanupClosePushL(stream);
	InternalizeArrayL(pluginIdList, stream);
	CleanupStack::PopAndDestroy(2, buf); // stream

	RPointerArray<HBufC> regInfoList;
	CleanupResetAndDestroyPushL(regInfoList);
	buf = HBufC8::NewLC(aMessage.GetDesLengthL(EIpcArgument2));
	ptr.Set(buf->Des());
	aMessage.ReadL(EIpcArgument2, ptr);
	stream.Open(*buf);
	CleanupClosePushL(stream);
	InternalizePointerArrayL(regInfoList, stream);
	CleanupStack::PopAndDestroy(2, buf); // stream

	// Sanity check
	if (pluginIdList.Count() != regInfoList.Count())
		{
		CleanupStack::PopAndDestroy(3, &trainedPluginIdList); // pluginIdList, regInfoList
		aMessage.Complete(KErrArgument);
		return;
		}

	// Prepare an array of TPluginResetDetails to aid during resetting
	RPointerArray<TPluginResetDetails> resetDetails;
	CleanupResetAndDestroyPushL(resetDetails);
	
	for (TInt index = 0; index < numTrained; ++index)
		{
		TPluginId pluginId = trainedPluginIdList[index];
		// Check if the trained plugin needs to be sent registration data
		TInt indexA = pluginIdList.Find(pluginId);
		
		TPluginResetDetails *resetDetailsEntry;
		if (indexA != KErrNotFound)
			{
			// Note: Ownership of the descriptor pointers remains with regInfoList
			resetDetailsEntry = new (ELeave) TPluginResetDetails(pluginId, *regInfoList[indexA]);
			}
		else
			{
			resetDetailsEntry = new (ELeave) TPluginResetDetails(pluginId, KNullDesC());
			}
		CleanupStack::PushL(resetDetailsEntry);
		resetDetails.AppendL(resetDetailsEntry);
		CleanupStack::Pop(resetDetailsEntry);
		}

	// Try and ensure that after reset at least one plugin has an identity trained
	// Since all the trained plugins for the identity are being reset make sure
	// at least one of them is of the knowledge type and is being passed the registration data 
	// to be reasonably sure it registers the new identity data
	TBool knowledgePluginFound = EFalse;
	for (TInt index = 0; index < numTrained; ++index)
		{
		TPluginId pluginId = resetDetails[index]->PluginId();
		CAuthPluginInterface* plugin = iPluginMgr->PluginL(pluginId);
		if ((plugin->Type() == EAuthKnowledge) && (resetDetails[index]->RegistrationData() != KNullDesC))
			{
			knowledgePluginFound = ETrue;
			break;
			}
		}
	if (!knowledgePluginFound)
		{
		CleanupStack::PopAndDestroy(4, &trainedPluginIdList); // pluginIdList, regInfoList, resetDetails
		aMessage.Complete(KErrAuthServResetMayLoseIdentity);
		return;
		}

	// Finally start the reset process
	// Generate a new protection key
	CProtectionKey* protKey = CProtectionKey::NewLC(iKeySize);
	TInt lastErr = KErrNone;
	TBool oneSuccess = EFalse; // To keep track if at least one reset succeeded
	for (TInt index = 0; index < numTrained; ++index)
		{
		TPluginResetDetails* reset = resetDetails[index];
		TPluginId pluginId = reset->PluginId();
		CAuthPluginInterface* plugin = iPluginMgr->PluginL(pluginId);
		HBufC8* result = NULL;
		TInt err = plugin->Reset(identityId, reset->RegistrationData(), result);
		if (err == KErrNone && result)
			{
			oneSuccess = ETrue;
			// Use the plugin data to generate transient key and then encrypt the protection key
			// using the transient key. A plugin may not return data if it does not use the supplied
			// registration information 
			CleanupStack::PushL(result);
			CTransientKeyInfo* keyInfo = CreateKeyInfoLC(pluginId, *result, *protKey);
			// Replace the trained information in the db
			iAuthDb2->SetTrainedPluginL(identityId, pluginId, *keyInfo);
			CleanupStack::PopAndDestroy(2, result); // keyInfo
			}
		else if (err == KErrNone)
			{
			oneSuccess = ETrue;
			// Remove the entry in the auth db for the plugin
			// Ignore errors
			TRAP_IGNORE(iAuthDb2->RemoveTrainedPluginL(identityId, pluginId));
			}
		else
			{
			// Remember the last error
			lastErr = err;
			}
		}
	
	CleanupStack::PopAndDestroy(5, &trainedPluginIdList); // pluginIdList, regInfoList, resetDetails, protKey

	// If none of the plugins reset correctly then return the last error
	if (oneSuccess)
		{
		lastErr = KErrNone;
		}
	
	aMessage.Complete(lastErr);
	}

// Generates and returns transient key info using the supplied plugin data and the protection key
CTransientKeyInfo* CAuthServer::CreateKeyInfoLC(TPluginId aPluginId, const TDesC8& aPluginData, const CProtectionKey& aProtKey)
	{
	CTransientKeyInfo* keyInfo = CTransientKeyInfo::NewLC(aPluginId);

	CTransientKey* transKey = keyInfo->CreateTransientKeyL(aPluginData);
	CleanupStack::PushL(transKey);
    
	CEncryptedProtectionKey* encProtKey = transKey->EncryptL(aProtKey);
	CleanupStack::PushL(encProtKey);
	
	keyInfo->SetEncryptedProtectionKeyL(encProtKey);
	CleanupStack::Pop(encProtKey);
	CleanupStack::PopAndDestroy(transKey);
	return keyInfo;
	}

/**
	Checks if the value of strength alias inturn contains an alias.
	
	@param	aAuthAliasList		an array of authentication strengths as obtained from
								authserver cenrep file.
	@param	aAliasString		an alias string containing the alias to be searched for.
	
 */	
	
TBool CAuthServer::CheckForAliasInAliasString(RPointerArray<HBufC>& aAuthAliasList, const TDes& aAliasString)
	{
	// find a match for the client supplied alias from the aliasList.
	for(TInt i = 0; i < aAuthAliasList.Count(); ++i)
		{
		TInt found = aAliasString.FindC(*aAuthAliasList[i]);
		if(found > 0)
			{
			return ETrue;
			}
		}
		
	return EFalse;
	}
	
/**
	Processes an alias string, This method is called recursively until we end
	up with an alias string containing only pluginIds and pluginTypes or a 
	combination of both.
	
	
	@param	aAliasList					an array of authentication strengths as obtained from
										authserver cenrep file.
	@param	aAliasStringToBeProcessed	an alias string to be processed.	
	@param	aResultantAliasString		Buffer to be populated with an alias string resulting from processing aAliasStringToBeProcessed.				
	 
 */
 	
void CAuthServer::ProcessAliasStringL(RPointerArray<HBufC>& aAliasList, const TDesC& aAliasStringToBeProcessed, RBuf& aResultantAliasString)
	{
	TBuf<KDefaultBufferSize> exprString;
	HBufC* aliasString = HBufC::NewLC(KMaxBufferSize);
	TPtr aliasStringPtr(aliasString->Des());
		
	TLex input(aAliasStringToBeProcessed);
	_LIT(KDelimiter, " ");
	TBool aliasFoundInString = EFalse;
	
	//iterate through the obtained expression to verify if it contains any strength subsets.
	for(TPtrC token = input.NextToken(); token.Size() > 0; token.Set(input.NextToken()))
		{
		TInt resultingLen = 0;
		aliasFoundInString = EFalse;
		for(TInt i = 0; i < aAliasList.Count(); ++i)
			{
			if(token.FindC(*aAliasList[i]) != KErrNotFound)
				{
				aliasFoundInString = ETrue;
				iAuthRepository->GetAliasDefinitionL(i, exprString);
				if(exprString.Length() == 0)
					{
					User::Leave(KErrAuthStrengthAliasNotDefined);
					}
					
				// resulting length obtained by appending exprString ,KCloseBracket and KOpenBracket to aliasString.
				resultingLen = (exprString.Length() + 2);
				if(resultingLen > KMaxBufferSize)
					{
					aliasString->ReAllocL(resultingLen);
					}
				
				aliasStringPtr.Append(KOpenBracket);	
				aliasStringPtr.Append(exprString);
				aliasStringPtr.Append(KCloseBracket);
				break;
				}
			}
		
		//if the token is an operator or a plugin type or pluginId, append it to aResultantExpr.
		if(!aliasFoundInString)
			{
			// resulting length obtained by appending token and delimiter to be aliasString.
			resultingLen = (exprString.Length() + token.Length() + 1);
			if(resultingLen > KMaxBufferSize)
					{
					aliasString->ReAllocL(resultingLen);
					}
					
			aliasStringPtr.Append(token);
			aliasStringPtr.Append(KDelimiter);	
			}
		}
	
	CleanupStack::Pop(aliasString);
	aResultantAliasString.Assign(aliasString);	
	
	}
	

/**
 *
 * @param aMessage the message to process
 **/
void CAuthServer::RegisterIdentityL(const RMessage2& aMessage)
	{
	if (ServerBusy())
		{
		aMessage.Complete(KErrServerBusy);
		return;
		}

	TIdentityId id = 0;
	TPckg<TIdentityId> idPkg(id);
	TRandom::RandomL(idPkg);

	CProtectionKey* key = CProtectionKey::NewL(iKeySize);
	
	iTrainingMgr->RegisterIdentityL(aMessage, id, *key);
	}

/**
 *
 * @param aMessage the message to process
 **/
void CAuthServer::CancelL(const RMessage2& aMessage)
    {
	TInt err = KErrNone;

	if (iTrainingMgr->IsBusy())
		{
		iTrainingMgr->Cancel();
		}
	else if (iAuthTransaction != 0)
		{
		if (aMessage.SecureId() == iAuthTransaction->Message().SecureId())
			{
			iEvaluator->Cancel();
			}
		else
			{
			// Shouldn't come here since we don't support share-able sessions
			err = KErrInUse;
			}
		}
	aMessage.Complete(err);
	}


/**
	Remove the supplied identity from the database.
	
	@param	aMessage		Client which contains identity
							to remove.
 */
void CAuthServer::RemoveIdentityL(const RMessage2& aMessage)
	{
	TInt result = KErrNone;
	
	//The identity to be removed
	TIdentityId id = static_cast<TIdentityId>(aMessage.Int0());
	
	//Check if the identity to be removed is not the default identity.
	TIdentityId defaultIdentity = iAuthDb2->DefaultIdentityL();

	if(defaultIdentity != id)
		{
		iAuthDb2->RemoveIdentityL(id);

		if (iLastIdentity && iLastIdentity->Id() == id)
			{
			delete iLastIdentity;
			iLastIdentity = 0;
			iLastAuthTime = 0L;
			}
		iPluginMgr->ForgetIdentityL(id);
		}
	else
		{
		result = KErrAuthServCanNotRemoveDefaultIdentity;
		}
	aMessage.Complete(result);
	}

/**
 * @return true if either the training mgr or authentication transaction
 * is busy
 **/
TBool CAuthServer::ServerBusy()
	{
	return iTrainingMgr->IsBusy() || iAuthTransaction != 0;
	}


/**
 *
 * @param aMessage the message to process
 **/
void CAuthServer::TrainPluginL(const RMessage2& aMessage)
	{
	if (ServerBusy())
		{
		aMessage.Complete(KErrServerBusy);
		return;
		}

	if (iLastIdentity == 0 || iLastIdentity->Id() != aMessage.Int0())
		{
		// we need a cached identity to get the protection key
		aMessage.Complete(KErrAuthServAuthenticationRequired);
		return;
		}
	
	TIdentityId retrainId = aMessage.Int0();
	
	//The default identity cannot be retrained.
	TIdentityId defaultIdentity = iAuthDb2->DefaultIdentityL();
		
	if(defaultIdentity == retrainId)
		{
		aMessage.Complete(KErrNotSupported);
		return;
		}
	
	HBufC8* data = HBufC8::NewLC(iLastIdentity->Key().KeyData().Size());
	*data = iLastIdentity->Key().KeyData(); 

	CProtectionKey* key = CProtectionKey::NewL(data);
	CleanupStack::Pop(data);
	
	iTrainingMgr->TrainPluginL(aMessage, *key);
	
    }


  
/**
 * Remove the supplied identity, plugin pair from the
 * authentication database.
 *
 * @param aMessage  Client message which contains the
 * 					identity and the plugin.
 **/
void CAuthServer::ForgetPluginL(const RMessage2& aMessage)
	{
	if (ServerBusy())
		{
		aMessage.Complete(KErrServerBusy);
		return;
		}
	
	TIdentityId id   = static_cast<TIdentityId>(aMessage.Int0());
	TPluginId plugin = static_cast<TPluginId>(aMessage.Int1());
	TInt err         = KErrNone;

	TInt numTrained = iAuthDb2->NumTrainedPluginsL(id);

	switch (numTrained)
	  {
	case 0:
	  err = KErrAuthServNoSuchIdentity;
	  break;
	case 1:	
	  err = KErrAuthServCanNotRemoveLastPlugin;	  
	  break;
	default:
	  iAuthDb2->RemoveTrainedPluginL(id, plugin);
	  iPluginMgr->PluginL(plugin)->Forget(id);
	  break;
	  }

	aMessage.Complete(err);
	}


void CAuthServer::EvaluateL(TPluginId aPluginId,
						   TIdentityId& aIdentityId,
						   CAuthExpressionImpl::TType aType,
						   TRequestStatus& aStatus)
	{
	
	if(aPluginId == 0 && aType == CAuthExpressionImpl::ENull)
		{
		aPluginId = iAuthRepository->DefaultPluginL();
		}
		
  	CAuthPluginInterface* plugin = iPluginMgr->PluginL(aPluginId);

	if (plugin != 0)
	  {
	  const HBufC* clientMessage = iAuthTransaction->ClientMessage();
	  
	  HBufC8*& data = iAuthTransaction->AddPluginL(aPluginId, aIdentityId);
	  	
	  if (plugin->IsActive())
		  {
		  plugin->Identify(aIdentityId, *clientMessage, data, aStatus);
		  }
		  
	  else
		  {
		  User::Leave(KErrAuthServPluginNotActive);
		  }
	  }
	}
/**
 * @see MEvaluatorPluginInterface::Evaluate
 **/
void CAuthServer::Evaluate(TPluginId aPluginId,
						   TIdentityId& aIdentityId,
						   CAuthExpressionImpl::TType aType,
						   TRequestStatus& aStatus)
	{
	TRAPD(err, EvaluateL(aPluginId, aIdentityId, aType, aStatus));

	if (err != KErrNone) 
	  {
	  TRequestStatus* status = &aStatus;
      User::RequestComplete(status, err);
	  }
	}

/**
 * Retrieve the preferred plugin for the supplied type and
 * get an identity from it.
 * @see MEvaluatorPluginInterface::Evaluate
 *
 **/
void CAuthServer::Evaluate(TAuthPluginType aPluginType,
						   TIdentityId& aIdentityId,
						   CAuthExpressionImpl::TType aType,	
						   TRequestStatus& aStatus)
	{
	TPluginId id = 0;
	TRAPD(r, id = iAuthDb2->PreferredPluginL(aPluginType));
	if (r == KErrNone)
	    {
	    Evaluate(id, aIdentityId, aType, aStatus);
	    }
	else
		{
		// Pass back error (can happen if a user preference hasn't been defined)
		aStatus = KRequestPending;
		TRequestStatus* rs = &aStatus;
		User::RequestComplete(rs, r);
		}
	}

/**
 * Completes the message and sends the id on it's way back to the client
 **/
void CAuthServer::CompleteAuthenticationL(const RMessagePtr2& aMessage,
										 CIdentity* aId)
	{
	// write to client
	HBufC8* idBuff = HBufC8::NewLC(KDefaultBufferSize);
	TPtr8  idPtr =  idBuff->Des();
	RDesWriteStream writeStream(idPtr);
	writeStream << *aId;
	writeStream.CommitL();

	TInt clientBuffSize = aMessage.GetDesMaxLength(2);
  
	if (clientBuffSize >= idBuff->Size())
		{
		aMessage.Write(2, *idBuff);
		}
	else
		{
		User::Leave(KErrUnderflow);
		}
	
	CleanupStack::PopAndDestroy(idBuff);  
  
	aMessage.Complete(KErrNone);
	}

/**
 * @see MEvaluatorClientInterface::EvaluationSucceeded
 **/
void CAuthServer::EvaluationSucceeded(TIdentityId aIdentityId)
	{

	TRAPD(err, EvaluationSucceededL(aIdentityId));
	
	switch(err)
	    {
	    case KErrNone:
	        break;
	    default:
	        EvaluationFailed(err);
	    }
	}


/**
 * The full, leaving, implementation of EvaluationSucceeded (which is a trap
 * harness).
 *
 * @param aIdentityId the identity discovered
 **/
void CAuthServer::EvaluationSucceededL(TIdentityId aIdentityId)
	{

	CAuthTransaction::RResultArray& results = iAuthTransaction->Results();

	HBufC8* data = 0;
	TPluginId plugin = KUnknownPluginId;

	TLastAuth lastAuth;
	lastAuth.iId = aIdentityId;
	
	for (TInt i = 0 ; i < results.Count() ; ++i)
		{
		if (aIdentityId == *results[i]->iId)
			{
			plugin = results[i]->iPlugin;
			data = results[i]->iData;
			UpdateAuthL(lastAuth, plugin); 
			}
		}
  
    CProtectionKey* key = 0;

    // Ensure that the ID and DATA are valid.
    if (( aIdentityId != KUnknownIdentity ) && ( data != 0 ))
        {
        // get the protection key 
        CTransientKeyInfo* keyInfo = iAuthDb2->KeyInfoL(aIdentityId, plugin);
		CleanupStack::PushL(keyInfo);
        		  
		CTransientKey* transKey = keyInfo->CreateTransientKeyL(*data);
        CleanupStack::PushL(transKey);
  
        key = transKey->DecryptL(keyInfo->EncryptedKey());
        CleanupStack::PopAndDestroy(2,keyInfo);
        CleanupStack::PushL(key);
        
        // convert to a client key if we need to
        if (iAuthTransaction->ClientKey())
           {
           CProtectionKey* key2 = key->ClientKeyL(iAuthTransaction->ClientSid());
            
           CleanupStack::PopAndDestroy(key);
           key = key2;
           CleanupStack::PushL(key);
           }   
        }
	else
	    {
	    // create a blank key
	    key = CProtectionKey::NewLC(0);
	    
	    // clear the cached identity
	    delete iLastIdentity;
        iLastIdentity = 0;
        iLastAuthTime = 0L;
        }

	HBufC* str =
		StringOrNullLC(iAuthTransaction->WithString(), aIdentityId);
	
	// create the client identity object
	CIdentity* identity = CIdentity::NewL(aIdentityId, key, str);
    CleanupStack::Pop(2, key);
	CleanupStack::PushL(identity);
    
    CompleteAuthenticationL(iAuthTransaction->Message(),
    					    identity);
	
    if (aIdentityId != KUnknownIdentity)
        {
		TIdentityId oldId = iLastIdentity ? iLastIdentity->Id() : 0;

        // cache the latest id
        delete iLastIdentity;
        iLastIdentity = identity;
        if(iLastAuthTime.UniversalTimeSecure() == KErrNoSecureTime)
        	{
            // Fall back to nonsecure time. 
            iLastAuthTime.UniversalTime();
            }
		// and publish it
		lastAuth.iAuthTime = iLastAuthTime;
		TPckg<TLastAuth> authPckg(lastAuth);
		User::LeaveIfError(iAuthProperty.Set(authPckg));

		// if the identity has changed publish that fact	
		if (oldId != aIdentityId)
			{
			User::LeaveIfError(iAuthEventProperty.Set(aIdentityId));
			}

		CleanupStack::Pop(identity);
        }
	else
	   {
		CleanupStack::PopAndDestroy(identity);
	   }
	delete iAuthTransaction;
	iAuthTransaction = 0;
    }

/**
 * Tells the authserver to cancel the current evaluation (i.e. call to
 * the plugin)
 *
 * @see MEvaluatorPluginInterface::Evaluate
 *
 **/
void CAuthServer::CancelEvaluate()
	{
	if (iAuthTransaction)
		{
		TPluginId pluginId = iAuthTransaction->LastPluginId();
		CAuthPluginInterface* plugin = 0;
		TRAPD(err, (plugin = iPluginMgr->PluginL(pluginId)));
		if (err == KErrNone)
		  {
			plugin->Cancel();
		  }
		}
	}


/**
 * @see MEvaluatorClientInterface::EvaluationFailed
 **/
void CAuthServer::EvaluationFailed(TInt aReason)
	{
	iAuthTransaction->Message().Complete(aReason);
	delete iAuthTransaction;
	iAuthTransaction = 0;
	// there's nothing we can do here. Panic? 
	TRAP_IGNORE(ClearPropertiesL());
	}
	  

void CAuthServer::UpdateAuthL(TLastAuth& aAuth, TPluginId aPlugin)
	{
	CAuthPluginInterface* plugin = iPluginMgr->PluginL(aPlugin);

	aAuth.iMaxCombinations =
		Max(aAuth.iMaxCombinations, plugin->MinEntropy());
	aAuth.iFalsePositiveRate =
		Max(aAuth.iFalsePositiveRate, plugin->FalsePositiveRate());
	aAuth.iFalseNegativeRate =
		Max(aAuth.iFalseNegativeRate, plugin->FalseNegativeRate());
	++aAuth.iNumFactors;
	}


/**
 * Free all the uncompressable memory before the 
 * heap mark is set/reset to get the OOM tests to pass.
 **/
void CAuthServer::FreeMemoryBeforeHeapMark()
	{
#ifdef _DEBUG
	
	if(iTrainingMgr)
		{
		delete iTrainingMgr;
		iTrainingMgr = 0;
		}

	if(iPluginMgr)
		{
		delete iPluginMgr;
		iPluginMgr = 0;
		}
	
	if(iLastIdentity)
		{
		delete iLastIdentity;
		iLastIdentity = 0;
		}
	
	iAuthDb2->CloseDbFile();
	
	REComSession::FinalClose();
	
#endif
	}


/**
 * Recreate all the variables deleted after the heap mark has 
 * been set/reset.
 **/
void CAuthServer::SetupVariablesAfterHeapMarkEndL()
	{
#ifdef _DEBUG
	
	//Opening Db file.
	TFileName dbName(KDbName);
	dbName[0] = RFs::GetSystemDriveChar();
	iAuthDb2->OpenDbFileL(iFs, dbName);
	
	//Creating plugin manager.
	if(!iPluginMgr)
		{
		iPluginMgr  = CPluginMgr::NewL();
		}
	
	//Creating training manager.
	if(!iTrainingMgr)
		{
		 iTrainingMgr = new (ELeave) CTrainingMgr(*iPluginMgr, *iAuthDb2, *iAuthRepository);
		}
#endif	
	}

/**
 * Things to be done before the heap mark is set/reset 
 * during OOM testing
 **/
void CAuthServer::DoPreHeapMarkOrCheckL()
	{
#ifdef _DEBUG
	FreeMemoryBeforeHeapMark();
#endif
	}

/**
 * Things to be done after the heap mark is set/reset 
 * during OOM testing
 **/
void CAuthServer::DoPostHeapMarkOrCheckL()
	{
#ifdef _DEBUG
	SetupVariablesAfterHeapMarkEndL();
#endif
	}

