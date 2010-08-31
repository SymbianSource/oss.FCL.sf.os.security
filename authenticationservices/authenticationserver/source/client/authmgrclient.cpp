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
* authmgrclient - exported authentication client session  implementation
*
*/


/**
 @file 
*/

#include "authclient_impl.h"
#include "authserverutil.h"

using namespace AuthServer;

/**
 * Register a new identity. This causes the server to create a new
 * identity and attempt to train the user with all available active
 * plugins. At least one plugin must be trained for this to be succesful.
 *
 * @param aIdentity The new heap allocated identity details will be
 * pointed at by this parameter upon successful completion.
 *
 * @param aDescription The identity's description text.
 *
 * @param aRequest This status object will be completed when this
 * asynchronous call finishes, the return values are described below. 
 *
 * @capability TrustedUI
 *
 * @return KErrServerTerminated, if the server no longer present
 * @return KErrServerBusy, if the request cannot be handled at this time. 
 * @return KErrNoMemory, if there is insufficient memory available.
 * @return KErrPermissionDenied, if the caller has insufficient capabilities.
 * @return KErrAuthServRegistrationFailed, if the all plugins failed to
 * successfully train for the identity.
 **/
EXPORT_C void RAuthMgrClient::RegisterIdentityL(
    CIdentity*& aIdentity,
	const TDesC& aDescription,  
    TRequestStatus& aRequest)
    {
    CheckAsyncDecoderL();
	iAsyncResponseDecoder->RegisterIdentityL(aIdentity, aDescription, aRequest);
    }

/** 
 * Remove an identity.
 *
 * @param aId The identity to remove.
 *
 * @capability WriteDeviceData
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 * @leave KErrPermissionDenied, if the caller has insufficient capabilities.
 * @leave KErrAuthServIdentityNotFound, if the id does not exist.
 **/
EXPORT_C void RAuthMgrClient::RemoveIdentityL(
    TIdentityId aId)
    {
	User::LeaveIfError(CallSessionFunction(ERemoveIdentity, TIpcArgs(aId)));
    }

/**
 * Train an authentication plugin for the specified identity. The identity
 * to be trained must be authenticated prior to training. This is
 * necessary for the AuthServer to decrypt the protection key prior to
 * encrypting it with the new key generated through training.
 *
 * @param aId The identity for whom to train the plugin.
 *
 * @param aPlugin The id of the plugin to train.
 *
 * @param aRequest This status object will be completed when this
 * asynchronous call finishes.
 *
 * @capability WriteUserData
 *
 * @return KErrServerTerminated, if the server no longer present
 * @return KErrServerBusy, if the request cannot be handled at this time. 
 * @return KErrNoMemory, if there is insufficient memory available.
 * @return KErrPermissionDenied, if the caller has insufficient capabilities.
 * @return KErrAuthServIdentityNotFound, if the id does not exist.
 * @return KErrAuthServNoSuchPlugin, if the plugin does not exist.
 * @return KErrAuthServAuthenticationRequired, if the identity to be
 * trained is not currently authenticated.
 **/
EXPORT_C void RAuthMgrClient::TrainPlugin(
    TIdentityId aId,
	TPluginId aPlugin,
	TRequestStatus& aRequest)
    {
    CallSessionFunction(ETrainPlugin, TIpcArgs(aId, aPlugin), aRequest);
    }


/**
 * Remove the specified plugin as an authentication method for the
 * identity.
 *
 * @param aId The identity for whom to forget plugin training.
 *
 * @param aPlugin The id of the plugin to retrain.
 *
 * @capability WriteUserData
 *
 * @return KErrServerTerminated, if the server no longer present
 * @return KErrServerBusy, if the request cannot be handled at this time. 
 * @return KErrNoMemory, if there is insufficient memory available.
 * @return KErrPermissionDenied, if the caller has insufficient capabilities.
 * @return KErrAuthServIdentityNotFound, if the id does not exist.
 * @return KErrAuthServNoSuchPlugin, if the plugin does not exist.
 **/
EXPORT_C void RAuthMgrClient::ForgetPluginL(
    TIdentityId aId,
	TPluginId aPlugin)
    {
	User::LeaveIfError(CallSessionFunction(EForgetPlugin, TIpcArgs(aId, aPlugin)));
    }


/**
 * Specifies the preferred plugin for the named type.
 *
 * @param aType The type of plugin for which to define the preference.
 *
 * @param aPluginId The id of the preferred plugin for the specified type.
 *
 * @capability WriteDeviceData
 *
 * @return KErrServerTerminated, if the server no longer present
 * @return KErrServerBusy, if the request cannot be handled at this time. 
 * @return KErrNoMemory, if there is insufficient memory available.
 * @return KErrPermissionDenied, if the caller has insufficient capabilities.
 * @return KErrAuthServNoSuchPlugin, if the plugin does not exist.
 * @return KErrArgument, if aType does not match the plugin's type.
 **/
EXPORT_C void RAuthMgrClient::SetPreferredTypePluginL(TAuthPluginType aType,
													  TPluginId aPluginId)
	{
	User::LeaveIfError(CallSessionFunction(ESetAuthPreferences, TIpcArgs(aType, aPluginId)));
	}

/**
 * Reset the training data of a registered identity.
 *
 * @param aId The identity to reset.
 *
 * @param aRegistrationInformation The regisration information to be used for 
 * identifying the user. This data is meaningful for knowledge based  authentication 
 * server plugins (here the registration data could be the passphrase). 
 * Note that a plugin may choose to ignore the supplied registration data and simply 
 * remove the identity from its records.
 *
 * @capability WriteDeviceData
 *
 * @return KErrServerTerminated, if the server no longer present
 * @return KErrServerBusy, if the request cannot be handled at this time. 
 * @return KErrNoMemory, if there is insufficient memory available.
 * @return KErrPermissionDenied, if the caller has insufficient capabilities.
 * @return KErrAuthServIdentityNotFound, if the id cannot be found.
 * @return KErrAuthServResetMayLoseIdentity, if a reset can result in the loss of an identity. 
 * @see KErrAuthServResetMayLoseIdentity.
 * @return KErrArgument, if the supplied arguments are incorrect.
 * @return ... any of the system-wide error codes.
 **/
EXPORT_C void RAuthMgrClient::ResetIdentityL(TIdentityId aId,
							 const TDesC& aRegistrationInformation)
	{
	// Sanity check arguments
	if (aId == 0)
		{
		User::Leave(KErrArgument);
		}
	User::LeaveIfError(CallSessionFunction(EResetIdentity, TIpcArgs(aId, &aRegistrationInformation)));
	}

/**
 * Reset the training data of a registered identity.
 *
 * @param aId The identity to reset.
 *
 * @param aPluginType The type of plugins for which to supply the registration data during the reset.
 * Note that currently only EAuthKnowledge type plugins is supported for this parameter.
 * 
 * @param aRegistrationInformation The regisration information to be used for 
 * identifying the user. This data is meaningful for knowledge based  authentication 
 * server plugins (here the registration data could be the passphrase). 
 * Note that a plugin may choose to ignore the supplied registration data and simply 
 * remove the identity from its records.
 *
 * @capability WriteDeviceData
 *
 * @return KErrServerTerminated, if the server no longer present
 * @return KErrServerBusy, if the request cannot be handled at this time. 
 * @return KErrNoMemory, if there is insufficient memory available.
 * @return KErrPermissionDenied, if the caller has insufficient capabilities.
 * @return KErrAuthServIdentityNotFound, if the id cannot be found.
 * @return KErrAuthServResetMayLooseIdentity, if a reset can result in the loss of an identity. 
 * @see KErrAuthServResetMayLooseIdentity.
 * @return KErrArgument, if the supplied arguments are incorrect.
 * @return ... any of the system-wide error codes.
 **/
EXPORT_C void RAuthMgrClient::ResetIdentityL(TIdentityId aId,
							 TAuthPluginType aPluginType,
							 const TDesC& aRegistrationInformation)
	{
	// Sanity check arguments
	if (aId == 0)
		{
		User::Leave(KErrArgument);
		}
	User::LeaveIfError(CallSessionFunction(EResetIdentityByType, TIpcArgs(aId, aPluginType, &aRegistrationInformation)));
	}

/**
 * Reset the training data of a registered identity.
 *
 * @param aId The identity to reset.
 *
 * @param aPluginIdList The list of plugin ids for which to supply the registration data during the reset.
 *
 * @param aRegistrationInformation An array of regisration information to be used for 
 * identifying the user. The order of elements in this array correspond to the order of plugin ids in 
 * aPluginIdList. This data is meaningful for knowledge based  authentication server 
 * plugins (here the registration data could be the passphrase). 
 * Note that a plugin may choose to ignore the supplied registration data and simply 
 * remove the identity from its records.
 *
 * @capability WriteDeviceData
 *
 * @return KErrServerTerminated, if the server no longer present
 * @return KErrServerBusy, if the request cannot be handled at this time. 
 * @return KErrNoMemory, if there is insufficient memory available.
 * @return KErrPermissionDenied, if the caller has insufficient capabilities.
 * @return KErrAuthServIdentityNotFound, if the id cannot be found.
 * @return KErrAuthServResetMayLooseIdentity, if a reset can result in the loss of an identity. 
 * @see KErrAuthServResetMayLooseIdentity.
 * @return KErrArgument, if the supplied arguments are incorrect.
 * @return ... any of the system-wide error codes.
 **/
EXPORT_C void RAuthMgrClient::ResetIdentityL(TIdentityId aId,
							 RArray<TPluginId>& aPluginIdList,
							 RPointerArray<const HBufC>& aRegistrationInformation)
	{
	// Sanity check arguments
	TInt count = aPluginIdList.Count();
	if ((aId == 0) || (count < 1) || (count != aRegistrationInformation.Count()))
		{
		User::Leave(KErrArgument);
		}

	// Flatten aPluginIdList
	HBufC8* bufPluginIds = AuthServerUtil::FlattenDataArrayLC(aPluginIdList);

	// Flatten aRegistrationInformation
	HBufC8* bufRegInfo = AuthServerUtil::FlattenDataPointerArrayLC(aRegistrationInformation);

	User::LeaveIfError(CallSessionFunction(EResetIdentityByList, TIpcArgs(aId, bufPluginIds, bufRegInfo)));
	CleanupStack::PopAndDestroy(2, bufPluginIds); // bufRegInfo
	}
