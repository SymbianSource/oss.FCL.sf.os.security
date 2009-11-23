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

#include <swi/launcher.h>
#include "tui.h"

CTStepUninstall::CTStepUninstall()
	{
	SetTestStepName(KTStepUninstall);
	}

TVerdict CTStepUninstall::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

	TInt expectedResult;
	if (GetIntFromConfig(ConfigSection(),_L("UninstallResult"), expectedResult) == EFalse)
		{
		expectedResult = KErrNone;
		}
		
	while (NextPackage())
		{
		TUI ui;	
		TInt err = Launcher::Uninstall(ui, PackageUID());
		if (expectedResult == KErrNotFound)
			{
			if (err != KErrNone)
				{
				// Can't set error to KErrServerBusy, or Testexecute will retry.
				SetTestStepResult((err != KErrServerBusy)? static_cast<TVerdict>(err) : EFail);
				}
			CheckExpectedResultCodesL();
			err = expectedResult;
			}
		if (err != expectedResult)
			{
			INFO_PRINTF3(_L("Expected %d, got %d"), expectedResult, err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}
	return TestStepResult();
	}
