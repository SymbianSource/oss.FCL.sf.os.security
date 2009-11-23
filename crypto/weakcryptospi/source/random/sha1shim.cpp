/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* hashshim.cpp
* hash shim implementation
* hash shim implementation
*
*/


/**
 @file
*/

#include "sha1shim.h"
#include "sha1impl.h"
#include <cryptospi/cryptospidef.h>
#include <cryptospi/plugincharacteristics.h>


using namespace CryptoSpi;
using namespace SoftwareCrypto;

//
// Implementation of SHA1 shim
//	
	
CSHA1Shim* CSHA1Shim::NewL()
	{
	CSHA1Shim* self=CSHA1Shim::NewLC();
	CleanupStack::Pop();
	return self;
	}

CSHA1Shim* CSHA1Shim::NewLC()
	{
	CSHA1Shim* self=new(ELeave) CSHA1Shim();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSHA1Shim::CSHA1Shim()
	{
	}
	
CSHA1Shim::~CSHA1Shim()
	{
	iHashImpl->Close();
	}

void CSHA1Shim::ConstructL()
	{
	iHashImpl=CSHA1Impl::NewL();
	}

TInt CSHA1Shim::HashSize()
	{
	const TCharacteristics* ptr(NULL);
	TRAPD(err, iHashImpl->GetCharacteristicsL(ptr);)
	if (err)
		{
		return err;
		}
	const THashCharacteristics* hashPtr=static_cast<const THashCharacteristics*>(ptr);
	return hashPtr->iOutputSize/8;
	}

TPtrC8 CSHA1Shim::Hash(const TDesC8& aMessage)
	{
	return iHashImpl->Hash(aMessage);
	}

// The following methods are kept for linkage compatibility
// but are not used by randsvr.exe. Turn off coverage for these
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

void CSHA1Shim::Reset()
	{
	// Provide a stub for compatibility; not used in random server
	ASSERT(EFalse);
	}

void CSHA1Shim::RestoreState()
	{
	// Provide a stub for compatibility; not used in random server
	ASSERT(EFalse);
	}

void CSHA1Shim::StoreState()
	{
	// Provide a stub for compatibility; not used in random server
	ASSERT(EFalse);
	}

CMessageDigest* CSHA1Shim::CopyL()
	{
	// Provide a stub for compatibility; not used in random server
	User::Leave(KErrNotSupported);
	return NULL;
	}

CMessageDigest* CSHA1Shim::ReplicateL()
	{
	// Provide a stub for compatibility; not used in random server
	User::Leave(KErrNotSupported);
	return NULL;
	}

TInt CSHA1Shim::BlockSize()
	{
	// Provide a stub for compatibility; not used in random server
	ASSERT(EFalse);
	return 0;
	}

TPtrC8 CSHA1Shim::Final(const TDesC8& /*aMessage*/)
	{
	// Provide a stub for compatibility; not used in random server
	ASSERT(EFalse);
	return NULL;
	}

TPtrC8 CSHA1Shim::Final()
	{
	// Provide a stub for compatibility; not used in random server
	ASSERT(EFalse);
	return NULL;
	}

void CSHA1Shim::Update(const TDesC8& /*aMessage*/)
	{
	// Provide a stub for compatibility; not used in random server
	ASSERT(EFalse);
	}

