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
* AuthServerUtil a utility class used by Authentication Server
* implemented as static functions
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef __AUTHSERVERUTIL_H__
#define __AUTHSERVERUTIL_H__

#include <f32file.h>
#include <s32mem.h>
#include "arrayutils.h"

namespace AuthServer
{

class AuthServerUtil
	{
public:
	// IPC helper methods
	template<class T> 
		static HBufC8* FlattenDataArrayLC(const RArray<T>& aArray);
	template<class T> 
		static void SendDataArrayL(const RMessage2& aMessage, const RArray<T>& aArray, TInt aIpcIndx);

	template<class T> 
		static HBufC8* FlattenDataPointerArrayLC(const RPointerArray<T>& aPointerArray);
	template<class T> 
		static void SendDataPointerArrayL(const RMessage2& aMessage, const RPointerArray<T>& aPointerArray, TInt aIpcIndx);
	};

// Templated function definitions must appear in the header file

template<class T> 
HBufC8* AuthServerUtil::FlattenDataArrayLC(const RArray<T>& aArray)
	{
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
	
	// externalise the array of objects		
	ExternalizeArrayL(aArray, stream);
	stream.CommitL();
	
	// Now, create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewL(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	CleanupStack::PopAndDestroy(2, tempBuffer); // tempBuffer,  stream

	CleanupStack::PushL(buffer);
	return buffer;
	}

template<class T> 
void AuthServerUtil::SendDataArrayL(const RMessage2& aMessage,
					const RArray<T>& aArray,
					TInt aIpcIndx)
	{
	HBufC8* buffer = FlattenDataArrayLC(aArray);
    TPtr8 pbuffer(buffer->Des());
    
	if (aMessage.GetDesMaxLengthL(aIpcIndx) < buffer->Size())
		{
		TInt bufferSize = buffer->Size();
		TPckgC<TInt> bufferSizePackage(bufferSize);
		aMessage.WriteL(aIpcIndx, bufferSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		aMessage.WriteL(aIpcIndx, *buffer);
		aMessage.Complete(KErrNone);
		}
	
	CleanupStack::PopAndDestroy(buffer);
	}	

template<class T> 
HBufC8* AuthServerUtil::FlattenDataPointerArrayLC(const RPointerArray<T>& aPointerArray)
	{
	// dynamic buffer since we don't know in advance the size required
    CBufFlat* tempBuffer = CBufFlat::NewL(KDefaultBufferSize);
	CleanupStack::PushL(tempBuffer);
	
	RBufWriteStream stream(*tempBuffer);
	CleanupClosePushL(stream);
	
	// externalise the pointer array		
	ExternalizePointerArrayL(aPointerArray, stream);
	stream.CommitL();
	
	// Create an HBufC8 from the stream buf's length, and copy 
	// the stream buffer into this descriptor
	HBufC8* buffer = HBufC8::NewL(tempBuffer->Size());
	TPtr8 ptr(buffer->Des());
	tempBuffer->Read(0, ptr, tempBuffer->Size());
	CleanupStack::PopAndDestroy(2, tempBuffer); // tempBuffer, stream,
	
	CleanupStack::PushL(buffer);
	return buffer;
	}

template<class T> 
void AuthServerUtil::SendDataPointerArrayL(const RMessage2& aMessage,
						   const RPointerArray<T>& aPointerArray,
						   TInt aIpcIndx)
	{
	HBufC8* buffer = FlattenDataPointerArrayLC(aPointerArray);
    TPtr8 pbuffer(buffer->Des());
    
	if (aMessage.GetDesMaxLengthL(aIpcIndx) < buffer->Size())
		{
		TInt bufferSize = buffer->Size();
		TPckgC<TInt> bufferSizePackage(bufferSize);
		aMessage.WriteL(aIpcIndx, bufferSizePackage);
		aMessage.Complete(KErrOverflow);
		}
	else
		{
		aMessage.WriteL(aIpcIndx, *buffer);
		aMessage.Complete(KErrNone);
		}
		
	CleanupStack::PopAndDestroy(buffer);
	}

} // namespace
#endif //__AUTHSERVERUTIL_H__
