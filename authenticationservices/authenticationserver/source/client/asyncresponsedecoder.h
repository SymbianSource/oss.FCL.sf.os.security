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
* CAsyncResponseDecoder - Auth Client helper class
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef ASYNCRESPONSEDECODER_H
#define ASYNCRESPONSEDECODER_H
namespace AuthServer
{

/** 
 * This AO handles the return buffers from asynchronous RAuthClient /
 * RAuthMgrClient calls and reconstructs the object from the stream returned,
 * completing the caller when finished.
 **/
class CAsyncResponseDecoder : public CActive
	{
public:
  
	CAsyncResponseDecoder(const RAuthClient& aSession);
	~CAsyncResponseDecoder();
	
	/**
	 * Initiate the authenticate method
	 */
		
	void AuthenticateL(const CAuthExpression& aExpression,
						   TTimeIntervalSeconds   aTimeout,
						   TBool                  aClientSpecificKey,
						   TUid 				  aClientSid,
						   TBool                  aWithString,
						   const TDesC& 		  aClientMessage,
						   CIdentity*&            aIdentityResult, 
						   TRequestStatus&        aClientStatus);
	
	/**
	 * Initiate the register identity method
	 */
	void RegisterIdentityL(CIdentity*& aIdentity,
						  const TDesC& aDescription,  
						  TRequestStatus& aRequest);
	/**
	 * common code to send the request and kickoff the active object
	 **/
	void StartCall(TAuthServerMessages aCmd);
  
	void RunL();

	void DoCancel();
  
private:
	void Cleanup();
	TInt RunError(TInt aError);
	
	const RAuthClient&     		iSession;
	const CAuthExpression* 		iExpression;
	CIdentity**            		iResult;
	HBufC8*                		iBuffer;
	HBufC8*						iParamsBuffer;
	TPtr8                  		iBufDes;
	TPtr8                  		iParamsPtr;
	TRequestStatus*        		iClientStatus;
	TIpcArgs*              		iArgs;
	HBufC8*                		iExprBuf;
	HBufC*                		iDescription;
	TAuthServerMessages         iCmd;
};
}
#endif // ASYNCRESPONSEDECODER_H
