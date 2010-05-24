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
* authserverserversession.cpp
* CAuthserverSession class implementation
*
*/


/**
 @file 
*/

#include "authserver_impl.h"
#include "log.h"

using namespace AuthServer;

void CAuthServerSession::PanicClient(const RMessagePtr2& aMessage,
									 TAuthServerPanic aPanic)
	{
	aMessage.Panic(KAuthServerShortName, aPanic);
	}

//
// 2nd phase construct for sessions - called by the CServer framework
//
void CAuthServerSession::CreateL()
	{
	iAuthServer =
	  static_cast<CAuthServer*>(const_cast<CServer2*>(CSession2::Server()));
	}
CAuthServerSession* CAuthServerSession::NewL(CAuthServer &aServer)
/**
	Factory function allocates new instance of CAuthServerSession.

	@return					New, initialized instance of CAuthServerSession
							which is owned by the caller.
 */
	{
	CAuthServerSession* self = new(ELeave) CAuthServerSession(aServer);
	CleanupStack::PushL(self);
	self->ConstructL();			// CScsSession implementation
	CleanupStack::Pop(self);
	return self;
	}

CAuthServerSession::CAuthServerSession(CAuthServer &aServer)
/**
	This private constructor prevents direct instantiation.
 */
 :	CScsSession(aServer)
	{
	// empty.
	}


CAuthServerSession::~CAuthServerSession()
	{
	}

//
// Handle a client request.
// Leaving is handled by CAuthServer::ServiceError() which reports
// the error code to the client
//
TBool CAuthServerSession::DoServiceL(TInt aFunction, const RMessage2& aMessage)
/**
	Implement CScsSession by handling the supplied message.

	Note the subsession creation command is automatically sent to
	DoCreateSubsessionL, and not this function.

	@param	aFunction		Function identifier without SCS code.
	@param	aMessage		Standard server-side handle to message.	 Not used.
 */
	{
	TAuthServerMessages func = static_cast<TAuthServerMessages>(aFunction);
	
	switch (func)
		{
    case EAuthenticate:
        iAuthServer->AuthenticateL(aMessage);
        break;
    case ECancel:
		iAuthServer->CancelL(aMessage);
		break;
	case EPlugins:
		iAuthServer->PluginsL(aMessage);
		break;
	case EActivePlugins:
		iAuthServer->ActivePluginsL(aMessage);
		break;
	case EPluginsByType:
		iAuthServer->PluginsByTypeL(aMessage);
		break;
	case EPluginsByTraining:
		iAuthServer->PluginsByTrainingL(aMessage);
		break;
	case EIdentities:
		iAuthServer->IdentitiesL(aMessage);
		break;
	case EIdentityString:
		iAuthServer->IdentityStringL(aMessage);
		break;
	case ESetIdentityString:
		iAuthServer->SetIdentityStringL(aMessage);
		break;
	case ERegisterIdentity:
		iAuthServer->RegisterIdentityL(aMessage);
		break;
	case ERemoveIdentity:
		iAuthServer->RemoveIdentityL(aMessage);
		break;
	case ETrainPlugin:
		iAuthServer->TrainPluginL(aMessage);
		break;
	case EForgetPlugin:
		iAuthServer->ForgetPluginL(aMessage); 
		break;
    case EDeauthenticate:
		iAuthServer->DeauthenticateL(aMessage);
		break;
	case EGetAuthPreferences:
		iAuthServer->PreferredTypePluginL(aMessage);
	    break;
	case ESetAuthPreferences:
		iAuthServer->SetPreferredTypePluginL(aMessage);
		break;
	case EIdentitiesWithString:
		iAuthServer->IdentitiesWithStringL(aMessage);
		break;
	case EListAuthAliases:
		iAuthServer->ListAuthAliasesL(aMessage);
		break;
	case EResolveExpression:
		iAuthServer->ResolveExpressionL(aMessage);
		break;
	case EResetIdentity:
	case EResetIdentityByType:
		iAuthServer->ResetIdentityL(aFunction, aMessage);
		break;
	case EResetIdentityByList:
		iAuthServer->ResetIdentityByListL(aMessage);
		break;
	default:
		PanicClient(aMessage,EPanicInvalidFunction);
		break;
		}
	return EFalse;
	}

//
// Handle an error from CAuthServerSession::ServiceL()
// A bad descriptor error implies a badly programmed client, so panic it;
// otherwise use the default handling (report the error to the client)
//
void CAuthServerSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	TBool panicClient = ETrue;
	TAuthServerPanic cliPanicReason = EPanicCorruptDescriptor;	//Init
	
	switch (aError)
		{
	case KErrBadDescriptor:
		cliPanicReason = EPanicCorruptDescriptor;
		break;
	
	case KErrAuthServInvalidInternalizeExpr:
		cliPanicReason = EPanicBadExternalizedAuthExpr;
		break;
	case KErrAuthServNoSuchPlugin:
		cliPanicReason = EPanicNoSuchAuthPlugin;
		break;
	
	default:
		panicClient = EFalse;
		}
	DEBUG_PRINTF3(_L8("Error in CAuthServerSession %d, func %d"), aError, aMessage.Function());

	if (panicClient)
		PanicClient(aMessage, cliPanicReason);
	else
		CScsSession::ServiceError(aMessage, aError);
	}
