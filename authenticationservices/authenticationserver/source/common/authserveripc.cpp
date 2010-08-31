/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* authserver client server IPC parameters implementation
*
*/


/**
 @file 
*/
#include "authcommon_impl.h"
#include "authserveripc.h"

using namespace AuthServer;



EXPORT_C CAuthParams* CAuthParams::NewL(TTimeIntervalSeconds aTimeout,
							   TBool aClientKey,
							   TUid  aClientSid,
							   TBool aWithString,
							   const TDesC& aClientMessage)
	{
	CAuthParams* self = new (ELeave)CAuthParams(aTimeout,
												aClientKey,
												aClientSid,
												aWithString);
	CleanupStack::PushL(self);
	self->ConstructL(aClientMessage);
	CleanupStack::Pop(self);
	return self;
	}


void CAuthParams::ConstructL(const TDesC& aClientMessage)
	{
	iClientMessage = aClientMessage.AllocL();
	}


CAuthParams::CAuthParams(TTimeIntervalSeconds aTimeout,
						TBool aClientKey,
						TUid  aClientSid,
						TBool aWithString):
	iTimeout(aTimeout),
	iClientKey(aClientKey),
	iClientSid(aClientSid),
	iWithString(aWithString)
	{
	
	}
	

EXPORT_C void CAuthParams::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iClientMessage->Des().Length());
	aStream.WriteInt32L(iTimeout.Int());
	aStream.WriteInt8L(iClientKey);
	aStream.WriteInt32L(iClientSid.iUid);
	aStream.WriteInt8L(iWithString);

	aStream << *iClientMessage;
	}


EXPORT_C void CAuthParams::InternalizeL(RReadStream& aStream)
	{
	TInt maxLength 	= aStream.ReadInt32L();
	iTimeout       	= aStream.ReadInt32L();
	iClientKey     	= aStream.ReadInt8L();
	iClientSid.iUid	= aStream.ReadInt32L();
	iWithString     = aStream.ReadInt8L();
	
	*iClientMessage = KNullDesC;
	iClientMessage = iClientMessage->ReAllocL(maxLength);
	TPtr ptr(iClientMessage->Des());
	aStream >> ptr;
	}


CAuthParams::~CAuthParams()
	{
	delete iClientMessage;
	iClientMessage = 0;
	}
