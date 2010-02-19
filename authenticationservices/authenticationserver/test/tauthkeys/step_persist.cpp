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
#include <f32file.h>

using namespace AuthServer;

CTStepPersist::CTStepPersist()
	{
	SetTestStepName(KTStepPersist);
	}

TVerdict CTStepPersist::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks
    
    TBool res = ETrue;
	TBool tmp = ETrue;

	iFs.CreatePrivatePath(RFs::GetSystemDrive());
    TBuf<50> path;
    iFs.PrivatePath(path);
    
    _LIT(kProtFile, "prot.xxx");
    _LIT(kTransFile, "trans.xxx");
	_LIT8(KPluginData, "ABCDEFGHIJ");
	_LIT(KPluginName, "PasswordPlugin");

    HBufC* pluginName = HBufC::NewLC(14);
	*pluginName = KPluginName;

	// create elements
	CTransientKeyInfo* transInfo =
	  CTransientKeyInfo::NewLC(9999);
	  
    CTransientKey* transient =
		transInfo->CreateTransientKeyL(KPluginData);
	CleanupStack::PushL(transient);
	
	CProtectionKey* protection =
		CProtectionKey::NewLC(8);
	
    CEncryptedProtectionKey* encrypted =
		transient->EncryptL(*protection);
	CleanupStack::PushL(encrypted);

	transInfo->SetEncryptedProtectionKeyL(encrypted);
	CleanupStack::Pop(encrypted);
	
	// write the encrypted key
	RFileWriteStream writeProt;
	
    path.Insert(path.Length(), kProtFile);
	User::LeaveIfError(writeProt.Replace(iFs, path,
										 EFileShareExclusive | EFileStream));
    CleanupClosePushL(writeProt);
	
	encrypted->ExternalizeL(writeProt);
		
	CleanupStack::PopAndDestroy(&writeProt);
	
	// read the encrypted key	
	RFileReadStream readStream;

	User::LeaveIfError(readStream.Open(iFs, path,
									   EFileShareExclusive | EFileStream));
	CleanupClosePushL(readStream);
	
	CEncryptedProtectionKey* encrypted2 =
		CEncryptedProtectionKey::NewL(readStream);
	CleanupStack::PushL(encrypted2);
 
	// test the restored key matches the saved
	TEST(tmp = (encrypted->KeyData() == encrypted2->KeyData()));
	res = tmp && res;

	// write the transient key info
	RFileWriteStream writeTrans;

	iFs.PrivatePath(path);
    path.Insert(path.Length(), kTransFile);
	User::LeaveIfError(writeTrans.Replace(iFs, path,
										 EFileShareExclusive | EFileStream));
    CleanupClosePushL(writeTrans);
	
	transInfo->ExternalizeL(writeTrans);
		
	CleanupStack::PopAndDestroy(&writeTrans);

	// read the transient key info 
	RFileReadStream readStream2;

	User::LeaveIfError(readStream2.Open(iFs, path,
									   EFileShareExclusive | EFileStream));
	CleanupClosePushL(readStream2);
	
	CTransientKeyInfo* transInfo2 =
		CTransientKeyInfo::NewL(readStream2);
	CleanupStack::PushL(transInfo2);

	// recreate the transient key
	CTransientKey* transient2 = 
		transInfo2->CreateTransientKeyL(KPluginData);
	CleanupStack::PushL(transient2);

	// recreate the protection key.
	CProtectionKey* decrypted =
		transient2->DecryptL(transInfo2->EncryptedKey());
	CleanupStack::PushL(decrypted);
    
    // test that the protection key and decrypted key are the same
	TEST(tmp = (protection->KeyData() == decrypted->KeyData()));
	res = tmp && res;
		
	SetTestStepResult(res ? EPass : EFail);

	CleanupStack::PopAndDestroy(9, transInfo);
    CleanupStack::PopAndDestroy(pluginName);

    iFs.Delete(path);
	
__UHEAP_MARKEND;	
	return TestStepResult();
	}

