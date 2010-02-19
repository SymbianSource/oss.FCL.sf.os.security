/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tbackuprestorestep.h"
#include <test/testexecutelog.h>

#include "swi/backuprestore.h"
#include "cleanuputils.h"

CTStepBackup::CTStepBackup()
	{
	SetTestStepName(KTStepBackup);
	}

TVerdict CTStepBackup::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
		
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	Swi::RBackupSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	
	while (NextPackage())
		{
		TInt cellCountBefore = User::CountAllocCells();
		
 		RPointerArray<HBufC> files;
 		CleanupResetAndDestroyPushL(files);
 		HBufC8* metadata = session.GetMetaDataL(PackageUID(), files);
 		CleanupStack::PopAndDestroy(&files); // ignore this parameter
		CleanupStack::PushL(metadata);
		WriteMetaDataL(metadata);
		CleanupStack::PopAndDestroy(metadata);
		
		TInt cellCountAfter = User::CountAllocCells();
		
		if ( cellCountAfter - cellCountBefore )
			{
			ERR_PRINTF2(_L("Memory Leak is Detected in TVerdict CTStepBackup::doTestStepL(): %d cell(s)"),
						cellCountAfter - cellCountBefore);
			SetTestStepResult(EFail);
			CleanupStack::PopAndDestroy(2, &fs); // session;
			return TestStepResult();
			}
			
		
		while (NextFile())
			{
			CopyFileL(InstalledFileName(), BackupFileName());
			if (ModifyFile())
				{
				// write some note to the end of this file
				RFile backup;
				User::LeaveIfError(backup.Open(fs, BackupFileName(), EFileWrite));
				CleanupClosePushL(backup);
				
				_LIT8(KSomeSpuriousCrap, "ALL YOUR RESTORES ARE BELONG TO US");
				TInt pos = 0;
				User::LeaveIfError(backup.Seek(ESeekEnd, pos));
				User::LeaveIfError(backup.Write(KSomeSpuriousCrap));
				
				CleanupStack::PopAndDestroy(&backup);
				}
			}
		}
		
	CleanupStack::PopAndDestroy(2, &fs); // session;

	return TestStepResult();
	}
