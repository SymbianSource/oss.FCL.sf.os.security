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
*
*/


#include <s32file.h>
#include "tauthkeysstep.h"

using namespace AuthServer;

CTStepEncryptKey::CTStepEncryptKey()
	{
	SetTestStepName(KTStepEncryptKey);
	}

TVerdict CTStepEncryptKey::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks

    TBool res = ETrue; 
    TBool tmp = ETrue; 
	TInt  err = KErrNone;
 

	_LIT8(KPluginData, "ABCDEFGHIJ");
	_LIT8(KBadPluginData, "KLMNOPQRST");
	_LIT(KPluginName, "PasswordPlugin");

    HBufC* pluginName = HBufC::NewLC(14);
	
	*pluginName = KPluginName;

	CTransientKeyInfo* transInfo =
	  CTransientKeyInfo::NewLC(9999);
	  
	CTransientKey* transient =
		transInfo->CreateTransientKeyL(KPluginData);
	CleanupStack::PushL(transient);

	CProtectionKey* protection =
		CProtectionKey::NewL(8);
	CleanupStack::PushL(protection);
		
	CEncryptedProtectionKey* encrypted =
		transient->EncryptL(*protection);
	
	transInfo->SetEncryptedProtectionKeyL(encrypted);
	
	// try to set another key 
	TRAP(err, transInfo->SetEncryptedProtectionKeyL(encrypted));
	TEST(tmp = (err == KErrAlreadyExists));
	res = tmp && res;
	
	// check the encrypted key is different from the protection key
	TEST(tmp = (encrypted->KeyData() != protection->KeyData()));
	res = tmp && res;
	
	CProtectionKey* decrypted =
		transient->DecryptL(*encrypted);
	CleanupStack::PushL(decrypted);

	// check that the decrypted key is the same as the protection key
	TEST(tmp = (protection->KeyData() == decrypted->KeyData()));
	res = tmp && res;
	
	// check we can't use the wrong plugin data
	CTransientKey* transient2 = 0;
	TRAP(err, transient2 = transInfo->CreateTransientKeyL(KBadPluginData)); 
	TEST(tmp = (err != KErrNone));
	res = tmp && res;

	// avoid set but unused warning...
	if (transient2 == 0) transient2 = 0;
	
	CProtectionKey* client = protection->ClientKeyL(0xabababab);
	CleanupStack::PushL(client);

	CProtectionKey* client2 = protection->ClientKeyL(0xcdcdcdcd);
	CleanupStack::PushL(client2);

	// check protection key is different from the client key 
	TEST(tmp = (protection->KeyData() != client->KeyData()));
	res = tmp && res;
	TEST(tmp = (protection->KeyData().Length() == client->KeyData().Length()));
	res = tmp && res;

	// check two client keys are different 
	TEST(tmp = (client->KeyData() != client2->KeyData()));
	res = tmp && res;
	TEST(tmp = (client->KeyData().Length() == client2->KeyData().Length()));
	res = tmp && res;

	// check key lengths larger than SHA1 output is handled correctly 
	CProtectionKey* protection2 =
		CProtectionKey::NewL(512);
	CleanupStack::PushL(protection2);

	CProtectionKey* client3 = protection2->ClientKeyL(0xcdcdcdcd);
	CleanupStack::PushL(client3);
	
	// check protection key is different from the client key 
	TEST(tmp = (protection2->KeyData() != client3->KeyData()));
	res = tmp && res;
	TEST(tmp = (protection2->KeyData().Length() ==
				client3->KeyData().Length()));
	res = tmp && res;

	// cleanup
	CleanupStack::PopAndDestroy(8, transInfo);
	CleanupStack::PopAndDestroy(pluginName);

	SetTestStepResult(res ? EPass : EFail);
	
__UHEAP_MARKEND;	
	return TestStepResult();
	}

