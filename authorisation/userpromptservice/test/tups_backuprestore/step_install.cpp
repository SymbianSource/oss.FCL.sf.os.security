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

#include <f32file.h>

#include <swi/launcher.h>
#include "tui.h"

CTStepInstall::CTStepInstall()
	{
	SetTestStepName(KTStepInstall);
	}

TVerdict CTStepInstall::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.ShareProtected();
		
	CInstallPrefs* prefs = CInstallPrefs::NewLC();
	prefs->SetPerformRevocationCheck(EFalse);
	
	TInt expectedResult;
	if (GetIntFromConfig(ConfigSection(),_L("InstallResult"), expectedResult) == EFalse)
		{
		expectedResult = KErrNone;
		}
			
	while (NextPackage())
		{
		TPtrC sisfilename(SISFileNameL());
		INFO_PRINTF2(_L("Installing %S"), &sisfilename);
		
		TUI ui;
		RFile file;
		User::LeaveIfError(file.Open(fs, sisfilename, EFileRead));

		TInt err = Launcher::Install (ui, file, *prefs);

		if (err != expectedResult)
			{
			INFO_PRINTF3(_L("Expected %d, got %d"), expectedResult, err);
			SetTestStepResult(EFail);
			}
		}
	CleanupStack::PopAndDestroy(2, &fs); // prefs
	
	return TestStepResult();
	}

