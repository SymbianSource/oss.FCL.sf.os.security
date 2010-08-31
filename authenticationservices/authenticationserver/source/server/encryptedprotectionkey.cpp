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
* CEncryptedProtectionKey implementation
*
*/


/**
 @file 
*/

#include "authserver_impl.h"


using namespace AuthServer;

CEncryptedProtectionKey* CEncryptedProtectionKey::NewL(HBufC8* aKeyData)
	{
	CEncryptedProtectionKey*  key =
	  CEncryptedProtectionKey::NewLC(aKeyData);
	CleanupStack::Pop(key);
	return key;
	}

CEncryptedProtectionKey* CEncryptedProtectionKey::NewLC(HBufC8* aKeyData)
	{
	CEncryptedProtectionKey*  key = new (ELeave) CEncryptedProtectionKey();
	CleanupStack::PushL(key);
	key->ConstructL(aKeyData);
	return key;
	}

void CEncryptedProtectionKey::ConstructL(HBufC8* aKeyData)
	{
	iKeyData = aKeyData;
    }

CEncryptedProtectionKey::~CEncryptedProtectionKey()
	{
	delete iKeyData;
	}

void CEncryptedProtectionKey::InternalizeL(RReadStream& aSrcData)
	{
	iKeyData = HBufC8::NewL(aSrcData, aSrcData.Source()->SizeL()); 
	}


void CEncryptedProtectionKey::ExternalizeL(RWriteStream& aDest) const
	{
	aDest << *iKeyData;
	}

	
TPtrC8 CEncryptedProtectionKey::KeyData() const
    {
        return *iKeyData;
    }

CEncryptedProtectionKey* CEncryptedProtectionKey::NewL(RReadStream& aSrcData)
	{
	CEncryptedProtectionKey* key = NewLC(aSrcData);
	CleanupStack::Pop(key);
	return key;
	}

CEncryptedProtectionKey* CEncryptedProtectionKey::NewLC(RReadStream& aSrcData)
	{
	CEncryptedProtectionKey* key = new (ELeave) CEncryptedProtectionKey();
	CleanupStack::PushL(key);
	key->InternalizeL(aSrcData);
	return key;
	}

