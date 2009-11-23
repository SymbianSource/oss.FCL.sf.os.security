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
* hashshim.h
* hash shim classes definition
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __HASHSHIM_H__
#define __HASHSHIM_H__

#include <e32def.h>
#include <hash.h>
#include <cryptospi/cryptohashapi.h>
#include "sha1impl.h"

//
// SHA1 shim class
//

NONSHARABLE_CLASS(CSHA1Shim) : public CMessageDigest
	{
public:
	static CSHA1Shim* NewL();
	static CSHA1Shim* NewLC();
	~CSHA1Shim();
	CMessageDigest* CopyL();
	TInt BlockSize();
	TInt HashSize()	;
	void Update(const TDesC8& aMessage);
	TPtrC8 Final(const TDesC8& aMessage);
	TPtrC8 Final();
	void Reset();
	TPtrC8 Hash(const TDesC8& aMessage);
	CMessageDigest* ReplicateL();
	void RestoreState();
	void StoreState();
	
private:	
	CSHA1Shim();
	void ConstructL();
	
private:
	SoftwareCrypto::CSHA1Impl* iHashImpl;
	};

#endif
