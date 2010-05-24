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
*/

#include <e32debug.h>
#include <s32mem.h>
#include "authclient_impl.h"

using namespace AuthServer;

namespace AuthServer
{
HBufC8* ExternalizeExpressionL(const CAuthExpression* aExpr);

enum {  EPanicBadCmd = 1 };

_LIT(KAsyncDecoderBusy, "AuthClient busy");

}

CAsyncResponseDecoder::CAsyncResponseDecoder(const RAuthClient& aSession) :
	CActive(EPriorityStandard), iSession(aSession), 
	iBufDes(0,0), iParamsPtr(0,0)
    {
	CActiveScheduler::Add(this);
	}

CAsyncResponseDecoder::~CAsyncResponseDecoder()
    {
    Cleanup();
    }


void CAsyncResponseDecoder::AuthenticateL(
						   const CAuthExpression& aExpression,
						   TTimeIntervalSeconds   aTimeout,
						   TBool                  aClientSpecificKey,
						   TUid 				  aClientSid,
						   TBool                  aWithString,
						   const TDesC& 		  aClientMessage,
						   CIdentity*&            aIdentityResult, 
						   TRequestStatus&        aClientStatus)
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(AuthServer::KAsyncDecoderBusy, 0));
	
	iExpression       	     = &aExpression;
	iResult            	     = &aIdentityResult;
	iClientStatus            = &aClientStatus;
	
	//Create a flat buffer.
  	CBufFlat* flatBuffer = CBufFlat::NewL(KDefaultBufferSize);
  	CleanupStack::PushL(flatBuffer);
  	
  	//Initialize the CAuthParams object.
  	CAuthParams* params = CAuthParams::NewL(aTimeout,
  											aClientSpecificKey,
											aClientSid,
  											aWithString,
  											aClientMessage);
  	CleanupStack::PushL(params);
  	
  	//Externalize params.
  	RBufWriteStream stream(*flatBuffer);
  	CleanupClosePushL(stream);
  	params->ExternalizeL(stream);
  	CleanupStack::PopAndDestroy(2, params);
  		
  	iParamsBuffer = HBufC8::NewL(flatBuffer->Size());
  	iParamsPtr.Set(iParamsBuffer->Des());
  	flatBuffer->Read(0, iParamsPtr, flatBuffer->Size());
  	CleanupStack::PopAndDestroy(flatBuffer);
  
	//Externalize the expression.
  	iExprBuf = ExternalizeExpressionL(iExpression);
  
	// allocate buffer for identity result.
	iBuffer = HBufC8::NewL(KDefaultBufferSize);
	iBufDes.Set(iBuffer->Des());
  
	iArgs = new (ELeave)TIpcArgs(iExprBuf,
								 &iParamsPtr,
								 &iBufDes); 
	StartCall(EAuthenticate);
	}
void CAsyncResponseDecoder::RegisterIdentityL(CIdentity*& aIdentity,
											 const TDesC& aDescription,  
											 TRequestStatus& aClientStatus)
	{
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KAsyncDecoderBusy, 0));
	
	iBuffer = HBufC8::NewL(KDefaultBufferSize);
	iBufDes.Set(iBuffer->Des());
	iResult = &aIdentity;
	iDescription = HBufC::NewL(aDescription.Size());
	*iDescription = aDescription;
	iClientStatus = &aClientStatus;
	iArgs = new (ELeave)TIpcArgs(&iBufDes, iDescription);
  
	StartCall(ERegisterIdentity);
	}

void CAsyncResponseDecoder::StartCall(TAuthServerMessages aCmd)
	{
	iCmd = aCmd;
	iStatus = KRequestPending;
	*iClientStatus = KRequestPending;
	SetActive();
	iSession.CallSessionFunction(iCmd, *iArgs, iStatus);
	}

void CAsyncResponseDecoder::RunL() 
	{
	if (iStatus.Int() == KErrNone)
		{
		RDesReadStream readStream(*iBuffer);
		switch (iCmd)
			{
			case EAuthenticate:
				*iResult = CIdentity::InternalizeL(readStream);
				break;
			case ERegisterIdentity:
				*iResult = CIdentity::InternalizeL(readStream);
				break;
			default:
				User::Panic(_L("AuthClient-AsyncResponseDecoder"),
							EPanicBadCmd);
			}
		}
	User::RequestComplete(iClientStatus,iStatus.Int());
	Cleanup();
	}

TInt CAsyncResponseDecoder::RunError(TInt aError)
	{	
	User::RequestComplete(iClientStatus, aError);
	Cleanup();    			
	return KErrNone;
	
	}

void CAsyncResponseDecoder::DoCancel()
	{
	switch (iCmd)
		{
		case EAuthenticate:
		case ERegisterIdentity:
			User::RequestComplete(iClientStatus, KErrCancel);
			break;
		default:
		    break;
		}
	Cleanup();
    }

void CAsyncResponseDecoder::Cleanup()
    {
	delete iArgs;
	iArgs = 0;
	delete iExprBuf;
	iExprBuf = 0;
	delete iBuffer;
	iBuffer = 0;
    delete iDescription;
    iDescription = 0;
    delete iParamsBuffer;
    iParamsBuffer = 0;
	iCmd = ELastService;
	iClientStatus = 0;
    }
