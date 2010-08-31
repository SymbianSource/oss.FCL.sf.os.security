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
* Authserver - shared client/server definitions
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef AUTHSERVERCLIENTSERVER_H
#define AUTHSERVERCLIENTSERVER_H

#include <e32std.h>

namespace AuthServer
{
_LIT(KAuthServerName,"!AuthServer");    // name to connect to
_LIT(KAuthServerImg, "AuthServer");		// DLL/EXE name
_LIT(KAuthServerShortName, "AuthServer");	    // name used for identication when panicking 
                                                // clients  - has to be less than 16 bytes

const TInt KMaxAuthServerMessage	=100;
const TInt KDefaultBufferSize		=2048;

enum TAuthServerMessages
    {
	/////////////////////////////////////////////////////////////////
	ERequireNoCapabilities = 0x100,

	ECancel,                     		///< cancel current async operation
	EPlugins,                     		///< return all plugins
	EActivePlugins,               		///< return active plugins only
	EPluginsByType,               		///< return plugins matching a type
	EPluginsByTraining,           		///< return plugins matching a training
								  		///< status
	EDeauthenticate,              		///< deauthenticate the current user
	EGetAuthPreferences,          		///< get a plugin type to plugin mapping
	EListAuthAliases,	  				///< get the list of available authentication aliases.				
	EResolveExpression,					///< resolve any alias definitions in the auth expression string.
	
	/////////////////////////////////////////////////////////////////
	ERequireReadUserData = 0x200,
	
	EIdentityString,              ///< return the given identity's description 
 
	/////////////////////////////////////////////////////////////////
	ERequireWriteUserData = 0x300,

	ESetIdentityString,           ///< set the description for an identity
	ETrainPlugin,				  ///< train a plugin for an identity
	EForgetPlugin,				  ///< forget identity training data

	/////////////////////////////////////////////////////////////////
	ERequireReadDeviceData = 0x400,

	EIdentities,                  ///< return a list of identity numbers

	/////////////////////////////////////////////////////////////////
	ERequireWriteDeviceData = 0x500,
	ERemoveIdentity,			  ///< remove an identity
	ESetAuthPreferences,          ///< set a plugin type to plugin mapping
	ESetPreferredPlugin,          ///< set the default plugin to use
	EResetIdentity,				  ///< reset identity for all registered plugins
	EResetIdentityByType,		  ///< reset identity for given plugin type
	EResetIdentityByList,		  ///< reset identity for given list of plugins
	
	/////////////////////////////////////////////////////////////////
	ERequireTrustedUi = 0x600,

	ERegisterIdentity,            ///< register a new identity

	/////////////////////////////////////////////////////////////////
	ERequireReadDeviceAndUserData = 0x700,

	EIdentitiesWithString,
	
	////////////////////////////////////////////////////////////////
	ERequireCustomCheck = 0x800,
	
	EAuthenticate,
	
	ELastService = 0x900,
	};

/**
 * Aggregates the parameters to the RAuthClient::AuthenticateL ipc call.
 *
 **/
class CAuthParams : public CBase
    {
    
public:
	IMPORT_C static CAuthParams* NewL(TTimeIntervalSeconds aTimeout,
									  TBool aClientKey,
									  TUid  aClientSid,
									  TBool aWithString,
									  const TDesC& aClientMessage);
	
	
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
	IMPORT_C void InternalizeL(RReadStream& aStream);
	
	~CAuthParams();
	
private:
	CAuthParams(TTimeIntervalSeconds aTimeout,
				TBool aClientKey,
				TUid  aClientSid,
	    		TBool aWithString);
		
    void ConstructL(const TDesC& aClientMessage);
    
public:    
	TTimeIntervalSeconds iTimeout;
	TBool                iClientKey;
	TUid				 iClientSid;
	TBool                iWithString;
	HBufC*				 iClientMessage;
};

} //namespace
#endif
