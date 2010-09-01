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
#include "swi/swispubsubdefs.h"

CTStepRestore::CTStepRestore()
	{
	SetTestStepName(KTStepRestore);
	}

TVerdict CTStepRestore::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
	
	TInt err = KErrNone;
	TRAP(err, RestoreL());
	INFO_PRINTF2(_L("RestoreL returned %d"), err);
	if (err != KErrNone)
		{
		// Can't set error to KErrServerBusy, or Testexecute will retry.
		SetTestStepResult((err != KErrServerBusy)? static_cast<TVerdict>(err) : EFail);
		}

	CheckExpectedResultCodesL();
	return TestStepResult();
	}
	
void CTStepRestore::RestoreL()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.ShareProtected();
	
	Swi::RRestoreSession session;
	User::LeaveIfError(session.Connect());
	CleanupClosePushL(session);
	
	while (NextPackage())
		{
		HBufC8* metadata = ReadMetaDataL(MetaDataCropLength());
		CleanupStack::PushL(metadata);
		session.StartPackageL(PackageUID(), *metadata);
		while (NextFile())
			{
			RFile sourcefile;
			User::LeaveIfError(sourcefile.Open(fs, BackupFileName(), EFileRead));
			TPtrC destfilename = InstalledFileName();
			session.RestoreFileL(sourcefile, destfilename);
			}
			
		// Create dummy files to simulate restore of data files
		while (NextDataFile())
			{
			TouchL(DataFileName());
			}
			
		session.CommitPackageL();
		CleanupStack::PopAndDestroy(metadata);
		
		TInt property;
		User::LeaveIfError(RProperty::Get(KUidSystemCategory, Swi::KUidSoftwareInstallKey, property));
			
		if (property != Swi::ESwisNone)
			{
				
			// The Pub&Sub property should have been reset to none,
			// Regardless of the actual result of the test.
			SetTestStepResult(EFail);
			User::Leave(KErrGeneral);
				
			}
		
		}	
	CleanupStack::PopAndDestroy(2, &fs);	// session	
	}


