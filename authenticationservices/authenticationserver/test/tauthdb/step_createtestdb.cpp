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


#include <bautils.h>
#include "tauthdbstep.h"

using namespace AuthServer;


CTStepCreateTestDb::CTStepCreateTestDb()
	{
	SetTestStepName(KTStepCreateTestDb);
	}


TVerdict CTStepCreateTestDb::doTestStepL()
/**
	Create a test database and copy the file to
	authdb0.db to system drive.  This database is used by both
	tauthdb and tauthsvr.
 */
	{
	TInt r;
	
	// create the server's private directory.  This happens
	// when the server starts up, but some of the tests use
	// the server's classes directly.
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());

	TBuf<128> dbName(KDbName);
	dbName[0] = 'A' + sysDrive;	
	
	r = iFs.MkDirAll(dbName);			// db name not used
	if (r != KErrAlreadyExists)
		User::LeaveIfError(r);
	
	RemoveExistingDbL();
	
	CAuthDb2* db = CAuthDb2::NewLC(iFs);
	
	_LIT(kId1Name, "Identity1");
	_LIT(kId2Name, "Identity2");
	_LIT(kId3Name, "Identity3");
	_LIT(kId22Name, "Identity22");
	
	db->AddIdentityL(1, kId1Name);
	const CTransientKeyInfo* tki1 = iId1Keys[0];
	db->SetTrainedPluginL(1, tki1->PluginId(), *tki1);
	
	db->AddIdentityL(2, kId2Name);
	const CTransientKeyInfo* tki2 = iId2Keys[0];
	db->SetTrainedPluginL(2, tki2->PluginId(), *tki2);
	
	db->AddIdentityL(3, kId3Name);
	const CTransientKeyInfo* tki3 = iId3Keys[0];
	db->SetTrainedPluginL(3, tki3->PluginId(), *tki3); 
	
	// create additional trained plugins
	for (TInt i = 1 ; i < KNumPlugins ; ++i)
		{
		db->SetTrainedPluginL(1, iId1Keys[i]->PluginId(), *iId1Keys[i]);
		db->SetTrainedPluginL(2, iId1Keys[i]->PluginId(), *iId2Keys[i]);
		db->SetTrainedPluginL(3, iId1Keys[i]->PluginId(), *iId3Keys[i]);
		}
    
    // Add 1 more id for use with test plugin
    CProtectionKey* key = CProtectionKey::NewLC(8);
    
    CTransientKeyInfo* tki = CTransientKeyInfo::NewLC(0x10274104);
    
	_LIT8(KIdentifyData, "ABABABABABABABABABAB");
    CTransientKey* tk = tki->CreateTransientKeyL(KIdentifyData);
    CleanupStack::PushL(tk);
    
    CEncryptedProtectionKey* epKey = tk->EncryptL(*key);
    CleanupStack::PushL(epKey);	// epKey takes ownership
        
    tki->SetEncryptedProtectionKeyL(epKey);
    CleanupStack::Pop(epKey);	// eki takes ownership
    
	db->AddIdentityL(22, kId22Name);
	db->SetTrainedPluginL(22, tki->PluginId(), *tki); 

	CleanupStack::PopAndDestroy(3, key);
	
	CleanupStack::PopAndDestroy(db);
	
	_LIT(KDbTrgFileName ,"\\authdb0.db");
	TBuf<128> dbTrgFileName (sysDriveName);
	dbTrgFileName.Append(KDbTrgFileName);
	
	// copy the file to the system drive 
	r = BaflUtils::CopyFile(iFs, dbName, dbTrgFileName);
	TESTL(r == KErrNone);
	
	return EPass;
	}
