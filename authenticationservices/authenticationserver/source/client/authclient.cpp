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
* authclient - exported authentication client session  implementation
*
*/


/**
 @file 
*/

#include <e32debug.h>
#include <s32mem.h>
#include "authclient_impl.h"
#include "authrepository.h"

namespace AuthServer
{

/**
	Helper function for Authenticate.  This allocates
	a buffer to hold the externalized authentication expression.
	
	@param	aExpr			Expression to externalize.
	@return					Descriptor containing externalized expression.
							The memory must be freed by the caller.
 */
HBufC8* ExternalizeExpressionL(const CAuthExpression* aExpr)
	{
	AuthServer::TSizeStream ss;
	RWriteStream ws(&ss);
	aExpr->ExternalizeL(ws);
	
	TInt len = ss.Size();
	HBufC8* buf = HBufC8::NewLC(len);
	TPtr8 bufDes(buf->Des());
	
	RDesWriteStream dws(bufDes);
	aExpr->ExternalizeL(dws);
	dws.CommitL();
		
	CleanupStack::Pop(buf);
	return buf;
	}
}

using namespace AuthServer;


/**
 * Connect to the server, attempt to start it if it is not yet running
 *
 * @return KErrNone if successful or an error code
 */
EXPORT_C TInt RAuthClient::Connect()
	{
	const TUidType serverUid(KNullUid, KNullUid, KAuthServerSecureId);

	TInt err = RScsClientBase::Connect(AuthServer::KAuthServerName, TVersion(1, 0, 0), 
									   AuthServer::KAuthServerImg, serverUid );
	return err;
	}

EXPORT_C RAuthClient::RAuthClient() : 
	RScsClientBase(), iAsyncResponseDecoder(0)
    {
	
    }
    
 
    
/**
  Authenticate the current device holder using a specified combination of
  authentication methods. The ownership of the heap allocated CIdentity
  object is passed to the caller. Requesting a user specific key
  requires ReadUserData capability.
 
  @capability None/ReadUserData
 
  @param aExpression An authentication expression specifying which 
  combination of methods to use to authenticate the device holder.
 
  @param aTimeout If an authentication has previously been performed 
  within this period then a cached identity is returned.
 
  @param aClientSpecificKey If this value is true then the key returned
  by this server will be unique to the calling client. This is achieved
  by combining the identity protection key with the client process UID.
  If this value is set to false then the call requires ReadUserData.
 
  @param aWithString If this value is true then the identity object returned
  by the server will contain the identities string. If this value is set to
  true then the call requires ReadUserData.
 
  @return CIdentity object corresponding to the authenticated device
  holder. It is possible for the identity to be 'unknown'.
  
  @leave KErrServerTerminated, if the server no longer present
  @leave KErrServerBusy, if the request cannot be handled at this time. 
  @leave KErrNoMemory, if there is insufficient memory available.
  @leave KErrPermissionDenied, if the caller has insufficient capabilities.
  @leave ...			One of the AuthServer error codes defined in 
  						auth_srv_errs.h or one of the system-wide error codes.
 **/
EXPORT_C CIdentity* RAuthClient::AuthenticateL(
	const CAuthExpression& aExpression,
	TTimeIntervalSeconds   aTimeout,
	TBool                  aClientSpecificKey,
	TBool                  aWithString)
	{
	TUid clientSid = {0};
		
	CIdentity* identity = AuthenticateL(aExpression, aTimeout,
										aClientSpecificKey, clientSid, 
										aWithString, KNullDesC());
		
	return identity;
	
    }

    

/**
  Authenticate the current device holder using a specified combination of
  authentication methods. The ownership of the heap allocated CIdentity
  object is passed to the caller. Requesting a user specific key
  requires ReadUserData capability.
 
  @capability None/ReadUserData
 
  @param aExpression An authentication expression specifying which 
  combination of methods to use to authenticate the device holder.
 
  @param aTimeout If an authentication has previously been performed 
  within this period then a cached identity is returned.
 
  @param aClientSpecificKey If this value is true then the key returned
  by this server will be unique to the calling client. This is achieved
  by combining the identity protection key with the client process UID.
  If this value is set to false then the call requires ReadUserData.
 
  @param aWithString If this value is true then the identity object returned
  by the server will contain the identities string. If this value is set to
  true then the call requires ReadUserData.
 
  @param aIdentityResult The returned identity will be placed in this
  parameter when the asynchronous request completes.  
 
  @param aStatus The request status for this asynchronous request.
 
  @leave KErrServerTerminated, if the server no longer present
  @leave KErrServerBusy, if the request cannot be handled at this time. 
  @leave KErrNoMemory, if there is insufficient memory available.
  @leave KErrPermissionDenied, if the caller has insufficient capabilities.
  @leave ...			One of the AuthServer error codes defined in 
  						auth_srv_errs.h or one of the system-wide error codes.

 **/
EXPORT_C void RAuthClient::AuthenticateL(
	const CAuthExpression& 	aExpression,
	TTimeIntervalSeconds   	aTimeout,
	TBool                  	aClientSpecificKey,
	TBool                  	aWithString, 
	CIdentity*&            	aIdentityResult, 
	TRequestStatus&        	aStatus)
	{
	TUid clientSid = {0};
	AuthenticateL(aExpression, aTimeout, aClientSpecificKey,
				  clientSid, aWithString, KNullDesC(),
			      aIdentityResult, aStatus);	
	
	}


/**
   Authenticate the current device holder using a specified combination of
  authentication methods. The ownership of the heap allocated CIdentity 
  object is passed to the caller. Requesting a user specific key requires 
  ReadUserData capability.
 
   @capability None/ReadUserData
 
   @param aAuthStrength Descriptor specifying the authentication strength, 
   which inturn maps to a combination of methods to be used to authenticate 
   the device holder.
 
   @param aTimeout If an authentication has previously been performed 
   within this period then a cached identity is returned.
 
   @param aClientSpecificKey If this value is true then the key returned
   by this server will be unique to the calling client. This is achieved
   by combining the identity protection key with the client process UID.
   If this value is set to false then the call requires ReadUserData.
          
   @param aClientSid Sid of the client application from where the authentication
   request originated and is used to generate protection key.This value is ignored 
   when aClientSpecificKey is set to false. If aClientSid is non-zero and is not 
   the calling application's sid, then the call requires ReadUserData.
  
   @param aWithString If this value is true then the identity object returned
   by the server will contain the identities string. If this value is set to
   true then the call requires ReadUserData.
 
   @param aClientMessage A displayable text string parameter for authentication
   requests.It shall be passed to plug-ins to display to the users.

   @return the CIdentity object corresponding to the authenticated device
   holder. It is possible for the identity to be 'unknown'.
  
   @leave KErrServerTerminated, if the server no longer present
   @leave KErrServerBusy, if the request cannot be handled at this time. 
   @leave KErrNoMemory, if there is insufficient memory available.
   @leave KErrPermissionDenied, if the caller has insufficient capabilities.
   @leave ...			One of the AuthServer error codes defined in 
  						auth_srv_errs.h or one of the system-wide error codes.

 **/	

EXPORT_C CIdentity* RAuthClient::AuthenticateL(
		const CAuthExpression& aAuthExpression,
	    TTimeIntervalSeconds   aTimeout,
		TBool                  aClientSpecificKey,
		TUid 				   aClientSid,
		TBool                  aWithString,
		const TDesC&		   aClientMessage)
	{
	
	//Create a flat buffer
	CBufFlat* flatBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(flatBuffer);
	
	//Initialize the CAuthParams object
	CAuthParams* params = CAuthParams::NewL(aTimeout,
											aClientSpecificKey,
											aClientSid,
											aWithString,
											aClientMessage);
	CleanupStack::PushL(params);
	//Externalize params
	RBufWriteStream stream(*flatBuffer);
	CleanupClosePushL(stream);
	params->ExternalizeL(stream);
	CleanupStack::PopAndDestroy(2, params);
	
	HBufC8* paramsBuffer = HBufC8::NewLC(flatBuffer->Size());
	TPtr8 paramsPtr(paramsBuffer->Des());
	flatBuffer->Read(0, paramsPtr, flatBuffer->Size());
		
	//Externalize the expression
	HBufC8* exprBuf = ExternalizeExpressionL(&aAuthExpression);
	CleanupStack::PushL(exprBuf);
		
	// allocate buffer for identity result
	HBufC8* identityResultbuf = HBufC8::NewLC(KDefaultBufferSize);
	TPtr8 bufDes(identityResultbuf->Des());
		
	TIpcArgs args(exprBuf, &paramsPtr, &bufDes); 
	User::LeaveIfError(CallSessionFunction(EAuthenticate, args));
		

		
	RDesReadStream readStream(*identityResultbuf);
	CIdentity* identity = CIdentity::InternalizeL(readStream);
	    
	CleanupStack::PopAndDestroy(4,flatBuffer);//identityResultbuf, exprBuf,
											  //paramsBuffer, flatBuffer	
	return identity;
	}

/**
  Authenticate the current device holder using a specified combination of
  authentication methods. The ownership of the heap allocated CIdentity 
  object is passed to the caller. Requesting a user specific key requires 
  ReadUserData capability.
 
  @capability None/ReadUserData
 
  @param aAuthStrength	Descriptor specifying the authentication strength, 
  which inturn maps to a combination of methods to be used to authenticate the device holder.
 
  @param aTimeout If an authentication has previously been performed 
  within this period then a cached identity is returned.
 
  @param aClientSpecificKey If this value is true then the key returned
  by this server will be unique to the calling client. This is achieved
  by combining the identity protection key with the client process UID.
  If this value is set to false then the call requires ReadUserData.
  
  @param aClientSid Sid of the client application from where the authentication
  request originated and is used to generate protection key.This value is ignored 
  when aClientSpecificKey is set to false. If aClientSid is non-zero and is not 
  the calling application's sid, then the call requires ReadUserData.
 
  @param aWithString If this value is true then the identity object returned
  by the server will contain the identities string. If this value is set to
  true then the call requires ReadUserData.
 
  @param aClientMessage A displayable text string parameter for authentication
  requests.It shall be passed to plug-ins to display to the users.
   
  @param aIdentityResult The returned identity will be placed in this
  parameter when the asynchronous request completes.  
 
  @param aStatus The request status for this asynchronous request.
 
  @leave KErrServerTerminated, if the server no longer present
  @leave KErrServerBusy, if the request cannot be handled at this time. 
  @leave KErrNoMemory, if there is insufficient memory available.
  @leave KErrPermissionDenied, if the caller has insufficient capabilities.
  @leave ...			One of the AuthServer error codes defined in 
  						auth_srv_errs.h or one of the system-wide error codes.

 **/
 	
EXPORT_C void RAuthClient::AuthenticateL(
	const CAuthExpression& aExpression,
	TTimeIntervalSeconds   aTimeout,
	TBool                  aClientSpecificKey,
	TUid 				   aClientSid,	
	TBool                  aWithString, 
	const TDesC&		   aClientMessage,
	CIdentity*&            aIdentityResult,
	TRequestStatus&        aStatus
	)
	{
	
	CheckAsyncDecoderL();
	iAsyncResponseDecoder->AuthenticateL(aExpression,
										 aTimeout,
										 aClientSpecificKey,
										 aClientSid,
										 aWithString,
										 aClientMessage,
										 aIdentityResult,
										 aStatus);
										 
	
	}

/**
 * creates the async decoder if it's not already been created. 
 */
void RAuthClient::CheckAsyncDecoderL()
    {
    if (iAsyncResponseDecoder == 0)
	    {
	    iAsyncResponseDecoder = new (ELeave) CAsyncResponseDecoder(*this);
	    }
    }
    
	    


/**
 * Retrieves all plugin descriptions. 
 *
 * @param aPluginList will be filled with  the full list of plugins available on the device. 
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 **/
EXPORT_C void RAuthClient::PluginsL(RPluginDescriptions& aPluginList)
    {
	HBufC8* buffer = SendReceiveBufferLC(EPlugins);
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aPluginList, stream);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
    }
	
/**
 * @param aPluginList the list of active plugins available on the device. 
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 **/
EXPORT_C void RAuthClient::ActivePluginsL(RPluginDescriptions& aPluginList)
    {
 	HBufC8* buffer = SendReceiveBufferLC(EActivePlugins);
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aPluginList, stream);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
	}
	
/**
 *
 * Retrieves plugin descriptions for plugins matching the specified
 * type.
 *
 * @param aType the plugin type for which the method should return 
 * the list of plugins. 
 *
 * @param aPluginList the list of plugins with the specified type available
 * on the device. 
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 *
 * @see TAuthPluginType
 **/
EXPORT_C void RAuthClient::PluginsOfTypeL(
    TAuthPluginType aType,
    RPluginDescriptions& aPluginList)
    {
	TIpcArgs args(TIpcArgs::ENothing, aType);
	HBufC8* buffer = SendReceiveBufferLC(EPluginsByType, args);
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aPluginList, stream);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
    }
	
/**
 * Retrieves plugin descriptions for plugins matching the specified
 * training status.
 *
 * @param aStatus the training status for which the method should 
 * return the list of plugins. 
 *
 * @param aPluginList the list of plugins with the specified type available
 * on the device.
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 *
 * @see TAuthTrainingStatus
 **/
EXPORT_C void RAuthClient::PluginsWithTrainingStatusL(
    TAuthTrainingStatus aStatus,
    RPluginDescriptions& aPluginList)
    {
	TIpcArgs args(TIpcArgs::ENothing, aStatus);
	HBufC8* buffer = SendReceiveBufferLC(EPluginsByTraining, args);
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aPluginList, stream);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
    }
	
/**
 * @param aIdList populated with the list of identities known by the 
 * phone.
 *
 * @capability ReadDeviceData
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 * @leave KErrPermissionDenied, if the caller has insufficient capabilities.
 **/
EXPORT_C void RAuthClient::IdentitiesL(RIdentityIdArray& aIdList)
    {
	HBufC8* buffer = SendReceiveBufferLC(EIdentities);
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizeArrayL(aIdList, stream);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
    }

/**
 * @param aIdList populated with the list of identities and their strings
 * known by the phone.
 *
 * @capability ReadDeviceData
 * @capability ReadUserData
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 * @leave KErrPermissionDenied, if the caller has insufficient capabilities.
 **/
EXPORT_C void RAuthClient::IdentitiesWithStringL(RIdAndStringArray& aIdList)
    {
	HBufC8* buffer = SendReceiveBufferLC(EIdentitiesWithString);
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	InternalizePointerArrayL(aIdList, stream);
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
    }

/**
 * @param aId the Id number of the identity for which to set the
 * string.
 *
 * @param aId the identity who's string to set.
 * @param aString the string to use
 *
 * @capability WriteUserData
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 * @leave KErrPermissionDenied, if the caller has insufficient capabilities.
 * @leave KErrAuthServIdentityNotFound, if the id does not exist.
 **/
EXPORT_C void RAuthClient::SetIdentityStringL(TIdentityId aId,
											  const TDesC& aString)
  {
  TIpcArgs args(aId, &aString);

  User::LeaveIfError(CallSessionFunction(ESetIdentityString, args));
  }

/**
 * Deauthenticates the current user. This means that clients requesting an
 * authentication will always cause a plug-in to be called regardless of
 * any timeout value specified.
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 *
 **/
EXPORT_C void RAuthClient::DeauthenticateL()
  {
  User::LeaveIfError(CallSessionFunction(EDeauthenticate));
  }

/**
 * @param aId the Id number of the identity for which to return the
 * string.
 * 
 * @return the string associated with the specified identity.
 *
 * @capability ReadUserData
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 * @leave KErrPermissionDenied, if the caller has insufficient capabilities.
 * @leave KErrAuthServIdentityNotFound, if the id does not exist.
 **/
EXPORT_C HBufC* RAuthClient::IdentityStringL(TIdentityId aId)
    {
    HBufC* buffer = HBufC::NewLC(KDefaultBufferSize);
	TPtr ptr = buffer->Des();
	User::LeaveIfError(CallSessionFunction(EIdentityString, TIpcArgs(&ptr, aId)));
	CleanupStack::Pop(buffer);
	return buffer;
    }


/**
 */
HBufC8* RAuthClient::SendReceiveBufferLC(TInt aMessage) 
	{
	HBufC8* output = HBufC8::NewLC(KDefaultBufferSize);
	
	TPtr8 pOutput(output->Des());
	
	TInt result = CallSessionFunction(aMessage, TIpcArgs(&pOutput));
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded = 0;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);
		
		// Re-allocate buffer after reclaiming memory
		CleanupStack::PopAndDestroy(output);
		output = HBufC8::NewLC(sizeNeeded);

		TPtr8 pResizedOutput(output->Des());
		
		result=CallSessionFunction(aMessage, TIpcArgs(&pResizedOutput));
		}
	User::LeaveIfError(result);
	return output;
	}

/**
 * aArgs[0] is set to the buffer to be sent/received
 */
HBufC8* RAuthClient::SendReceiveBufferLC(
	TInt aMessage,
	TIpcArgs& aArgs) 
	{
	HBufC8* output = HBufC8::NewLC(KDefaultBufferSize);
	
	TPtr8 pOutput(output->Des());

	aArgs.Set(0, &pOutput);
		  
	TInt result = CallSessionFunction(aMessage, aArgs);
	
	if (result == KErrOverflow)
		{
		TInt sizeNeeded;
		TPckg<TInt> sizeNeededPackage(sizeNeeded);
		sizeNeededPackage.Copy(*output);
		
		// Re-allocate buffer
		CleanupStack::PopAndDestroy(output);
		output = HBufC8::NewLC(sizeNeeded);

		TPtr8 pResizedOutput(output->Des());
		aArgs.Set(0, &pResizedOutput);
		result=CallSessionFunction(aMessage, aArgs);
		}
	User::LeaveIfError(result);
	return output;
	}

/**
 * @param aPluginType the type of plugin for which to return the preferred
 * plugin id
 * 
 * @return the id of the preferred plugin for the specified type
 *
 * @leave KErrServerTerminated, if the server no longer present
 * @leave KErrServerBusy, if the request cannot be handled at this time. 
 * @leave KErrNoMemory, if there is insufficient memory available.
 * @leave KErrPermissionDenied, if the caller has insufficient capabilities.
 **/
EXPORT_C TPluginId RAuthClient::PreferredTypePluginL(TAuthPluginType aPluginType)
	{
	TPluginId id = KUnknownPluginId;
	TPckg<TPluginId> idPckg(id);
	
	User::LeaveIfError(CallSessionFunction(EGetAuthPreferences, TIpcArgs(aPluginType, &idPckg)));

	return id;
	}
	
/**
 * Cancel any operation in progress.
 * 
 * @return KErrNone, if the send operation is successful or no operation
 * is in effect.
 * @return KErrServerTerminated, if the server no longer present
 * @return KErrServerBusy, if the request cannot be handled at this time. 
 * @return KErrNoMemory, if there is insufficient memory available.
 **/
EXPORT_C TUint RAuthClient::Cancel()
    {
    CallSessionFunction(ECancel);
    if(iAsyncResponseDecoder)
    	{
    	iAsyncResponseDecoder->Cancel();
    	}
    
    return KErrNone;
	}
	

EXPORT_C void RAuthClient::Close()
    {
    delete iAsyncResponseDecoder;
    iAsyncResponseDecoder = 0;
     
    RScsClientBase::Close();
    }

/**
  Lists the authentication aliases.
  
  @return An array of authentication strength aliases.
 
  @leave KErrServerTerminated, if the server no longer present
  @leave KErrServerBusy, if the request cannot be handled at this time. 
  @leave KErrNoMemory, if there is insufficient memory available.
  @leave KErrPermissionDenied, if the caller has insufficient capabilities.
 **/
 
EXPORT_C void RAuthClient::ListAuthAliasesL(RPointerArray<HBufC>& aAuthAliasesList)
    {
	HBufC8* buffer = SendReceiveBufferLC(EListAuthAliases);
	
	// create a stream based on the buffer
	RDesReadStream stream(*buffer);
	CleanupClosePushL(stream);
	
	// reassemble the array from the stream
	TInt strengthAliasCount = stream.ReadInt32L();
	for(TInt i = 0; i < strengthAliasCount; ++i)
		{
		HBufC* strengthAlias = HBufC::NewLC(stream, KMaxTInt);
		aAuthAliasesList.AppendL(strengthAlias);
		CleanupStack::Pop(strengthAlias);
		}
	
	CleanupStack::PopAndDestroy(2, buffer);// buffer, stream
    }

/**
   Returns a CAuthExpression object from a free form expression
   which can be a combination of plugin Ids, plugin types and alias names.
   This can be used for calling the authentication APIs.
   
 
   @capability None
 
   @param aAuthString Descriptor specifying a free form expression
   which can be a combination of plugin Ids, plugin types and alias names.
   
 
   @return the CAuthExpression object which can be used to call the 
   authentication APIs.
  
   @leave KErrServerTerminated, if the server no longer present
   @leave KErrServerBusy, if the request cannot be handled at this time. 
   @leave KErrNoMemory, if there is insufficient memory available.
   @leave KErrPermissionDenied, if the caller has insufficient capabilities.
   @leave ...			One of the AuthServer error codes defined in 
  						auth_srv_errs.h or one of the system-wide error codes.

 **/	

EXPORT_C CAuthExpression* RAuthClient::CreateAuthExpressionL(const TDesC& aAuthString) const
	{
	CAuthExpression* authExpr(0);
	
	if(aAuthString == KNullDesC)
		{
		authExpr = AuthExpr();
		return authExpr;
		}
	HBufC* buffer = HBufC::NewLC(KDefaultBufferSize);
	TPtr bufDes(buffer->Des());
		
	// get the string in combination of plugin ID and plugin type.
	User::LeaveIfError(CallSessionFunction(EResolveExpression, TIpcArgs(&bufDes, &aAuthString)));
		
	// create an auth expression from alias string.
	authExpr = CAuthExpressionImpl::CreateAuthExprObjectL(*buffer);
	CleanupStack::PopAndDestroy(buffer);
	
	return authExpr;
	}

 

   
