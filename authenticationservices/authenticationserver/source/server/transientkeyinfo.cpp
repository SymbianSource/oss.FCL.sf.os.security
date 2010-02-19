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
* CTransientKeyInfo implementation
*
*/


/**
 @file 
*/
#include <pbedata.h>
#include "authserver_impl.h"

using namespace AuthServer;

CTransientKeyInfo* CTransientKeyInfo::NewL(
	TPluginId aPluginId)
	{
	CTransientKeyInfo*  key =
		CTransientKeyInfo::NewLC(aPluginId);
	CleanupStack::Pop(key);
	return key;
	}

CTransientKeyInfo* CTransientKeyInfo::NewLC(
	TPluginId aPluginId)
	{
	CTransientKeyInfo*  key = new (ELeave) CTransientKeyInfo();
	CleanupStack::PushL(key);
	key->ConstructL(aPluginId);
	return key;
	}

void CTransientKeyInfo::ConstructL(TPluginId aPluginId)
	{
	iPluginId = aPluginId;
	}

CTransientKeyInfo* CTransientKeyInfo::NewL(RReadStream& aInputStream)
	{
	CTransientKeyInfo*  key =
		CTransientKeyInfo::NewLC(aInputStream);
	CleanupStack::Pop(key);
	return key;
	}

CTransientKeyInfo* CTransientKeyInfo::NewLC(RReadStream& aInputStream)
	{
	CTransientKeyInfo*  key = new (ELeave) CTransientKeyInfo();
	CleanupStack::PushL(key);
	key->InternalizeL(aInputStream);
	return key;
	}


CTransientKeyInfo::~CTransientKeyInfo()
	{
	delete iEncryptedKey;
	delete iEncryptionData;
	}

CTransientKey*
CTransientKeyInfo::CreateTransientKeyL(const TDesC8& aPluginData) const
	{
	CPBEncryptElement* encrypt = 0;
	if (!iEncryptionData)
		{
		// Plugin data used as password
		encrypt = CPBEncryptElement::NewL(aPluginData);
		CleanupStack::PushL(encrypt);
		iEncryptionData = CPBEncryptionData::NewL(encrypt->EncryptionData());
		}
	else
		{
		// Plugin data used as password
		encrypt = CPBEncryptElement::NewL(*iEncryptionData, aPluginData);			
		CleanupStack::PushL(encrypt);
		}
	
	CTransientKey* key = CTransientKey::NewL(encrypt);	// Ownership of encrypt transferred
	CleanupStack::Pop(encrypt);
	return key;
	}

const CEncryptedProtectionKey& CTransientKeyInfo::EncryptedKey() const
	{
	return *iEncryptedKey;
	}

void CTransientKeyInfo::SetEncryptedProtectionKeyL(
    CEncryptedProtectionKey* aEncryptedKey)
	{
	if (iEncryptedKey != 0)
		{
		User::Leave(KErrAlreadyExists);
		}
	
	iEncryptedKey = aEncryptedKey;
	}
  
void CTransientKeyInfo::ExternalizeL(RWriteStream& aOutStream) const
	{
	ASSERT(iEncryptedKey != 0);
	
	aOutStream << iPluginId;
	aOutStream << *iEncryptedKey;
	aOutStream << *iEncryptionData;
	}

void CTransientKeyInfo::InternalizeL(RReadStream& aInStream)
	{
	iPluginId = aInStream.ReadInt32L();
	iEncryptedKey = CEncryptedProtectionKey::NewL(aInStream);
	iEncryptionData  = CPBEncryptionData::NewL(aInStream);
	}

TPluginId CTransientKeyInfo::PluginId() const
  {
  return iPluginId;
  }
