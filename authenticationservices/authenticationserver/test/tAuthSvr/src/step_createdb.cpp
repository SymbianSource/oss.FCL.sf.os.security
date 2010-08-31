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

#include "tAuthSvrStep.h"
#include <testexecutelog.h>
#include "authserver/protectionkey.h"
#include "../source/server/encryptedprotectionkey.h"
#include "../source/server/transientkey.h"  
#include "../source/server/transientkey.h"  
#include "../source/server/transientkeyinfo.h"
#include "../source/server/authdb.h"

#include <s32file.h>
#include <f32file.h>

using namespace AuthServer;

CTStepCreateDb::CTStepCreateDb()
	{
	SetTestStepName(KTStepCreateDb);
	}

TVerdict CTStepCreateDb::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks

	TBool res = ETrue;

	/*
	*/
//	TBuf<50> path;
	_LIT(kAuthDbFile, "authdb.db");
	_LIT(kPath,"\\private\\102740FC\\");

	TDriveUnit sysDrive (RFs::GetSystemDrive());
	iFs.CreatePrivatePath(sysDrive);

	TBuf<128> filePath (sysDrive.Name());
	filePath.Append(kPath);
	TInt err = iFs.MkDirAll(filePath);
    if (err != KErrAlreadyExists)
	  {
	  User::LeaveIfError(err);
	  }
	
	filePath.Insert(filePath.Length(), kAuthDbFile);

	// Set up database
	CAuthDb* authDb = CAuthDb::NewL();
	CleanupStack::PushL(authDb);

	for (TInt i = 0 ; i < KNumPlugins ; ++i)
	  {
		authDb->AddPluginL(iPlugins[i]);
		iPlugins[i] = 0; // transfer ownership
	  }

	_LIT(kId1Name, "Identity1");
	_LIT(kId2Name, "Identity2");
	_LIT(kId3Name, "Identity3");
	
	authDb->AddIdentityL(1, kId1Name, iId1Keys[0]);
	iId1Keys[0] = 0; // transfer ownership
	authDb->AddIdentityL(2, kId2Name, iId2Keys[0]); 
	iId2Keys[0] = 0; // transfer ownership
	authDb->AddIdentityL(3, kId3Name, iId3Keys[0]); 
    iId3Keys[0] = 0; // transfer ownership

	for (TInt i = 1 ; i < KNumPlugins ; ++i)
	  {
		// add a new key info
		authDb->UpdateL(1, iId1Keys[i]);
		iId1Keys[i] = 0; // transfer ownership
		authDb->UpdateL(2, iId2Keys[i]);
		iId2Keys[i] = 0; // transfer ownership
		authDb->UpdateL(3, iId3Keys[i]);
		iId3Keys[i] = 0; // transfer ownership
	  }

	// Externalize
	RFileWriteStream writeStore;
	User::LeaveIfError(writeStore.Replace(iFs, filePath, 
										  EFileShareExclusive | EFileStream));
    CleanupClosePushL(writeStore);

	authDb->ExternalizeL(writeStore);

	CleanupStack::PopAndDestroy(&writeStore);

	CleanupStack::PopAndDestroy(1,authDb);

	SetTestStepResult(res ? EPass : EFail);
	
__UHEAP_MARKEND;	
	return TestStepResult();
	}

