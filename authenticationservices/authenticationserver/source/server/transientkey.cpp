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
* CTransientKey implementation
*
*/


/**
 @file 
*/

#include "authserver_impl.h"


using namespace AuthServer;

CTransientKey* CTransientKey::NewL(CPBEncryptElement* aEncryptor)
	{
	CTransientKey*  key =
		CTransientKey::NewLC(aEncryptor);
	CleanupStack::Pop(key);
	return key;
	}

CTransientKey* CTransientKey::NewLC(CPBEncryptElement* aEncryptor)
	{
	CTransientKey*  key = new (ELeave) CTransientKey();
	CleanupStack::PushL(key);
	key->ConstructL(aEncryptor);
	return key;
	}

void CTransientKey::ConstructL(CPBEncryptElement* aEncryptor)
	{
	iPbeElement = aEncryptor;
    }

CTransientKey::~CTransientKey()
	{
	delete iPbeElement;
	}

CEncryptedProtectionKey*
CTransientKey::EncryptL(const CProtectionKey& aKey) const
	{
	TPtrC8 protData = aKey.KeyData();
	
	CPBEncryptor* encryptor = iPbeElement->NewEncryptLC();
	
	HBufC8* ciphertext =
		HBufC8::NewLC(encryptor->MaxFinalOutputLength(protData.Length()));

	TPtr8 ciphertextTemp = ciphertext->Des();
	
	encryptor->ProcessFinalL(protData, ciphertextTemp);

	CEncryptedProtectionKey* key = CEncryptedProtectionKey::NewL(ciphertext);
	
	CleanupStack::Pop(ciphertext); // don't delete cipher text
	CleanupStack::PopAndDestroy(encryptor);
	
	return key;
	}

CProtectionKey*
CTransientKey::DecryptL(const CEncryptedProtectionKey& aKey) const
	{
	TPtrC8 encData = aKey.KeyData();
	
	CPBDecryptor* decryptor = iPbeElement->NewDecryptLC();

	HBufC8* plaintext =
		HBufC8::NewLC(decryptor->MaxFinalOutputLength(encData.Length()));
	TPtr8 plaintextTemp = plaintext->Des();

	// Decrypt the data
	decryptor->ProcessFinalL(encData, plaintextTemp);
		 
	CProtectionKey* key = CProtectionKey::NewL(plaintext);
	CleanupStack::Pop(plaintext);
	CleanupStack::PopAndDestroy(decryptor);
	return key;
	}
