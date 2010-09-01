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
* tswisstep.cpp
* UPS test step implementation
*
*/




/**
 @file
*/


#include <e32std.h>
#include "tupsstep.h"
#include "testutilclient.h"
#include <test/testexecutelog.h>
#include <swi/launcher.h>
#include "swi/sisregistryentry.h"
#include "swi/sisregistrysession.h"
#include "swi/sisregistrypackage.h"
#include "cleanuputils.h"

using namespace Swi;
using namespace Swi::Test;

//
// CUpsInstallStep
//

CUpsInstallStep::~CUpsInstallStep()
	{
	delete iUi;
	}
	

CUpsInstallStep::CUpsInstallStep(TInstallType aInstallType)
	: iInstallType(aInstallType)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUpsInstallStep);
	}

/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CUpsInstallStep::doTestStepPreambleL()
	{
	// get step parameters
	TPtrC str;
	if (!GetStringFromConfig(ConfigSection(), _L("sis"), str))
		{
		ERR_PRINTF1(_L("Missing SIS file name"));
		SetTestStepResult(EFail);
		}
	else
		{
		iSisFileName.Copy(str);
		if (!GetStringFromConfig(ConfigSection(), _L("script"), str))
			{
			ERR_PRINTF1(_L("Missing XML file name"));
			SetTestStepResult(EFail);
			}
		else
			{
			iXmlFileName.Copy(str);
			INFO_PRINTF3(_L("Installing '%S' using script '%S'"), 
				&iSisFileName, &iXmlFileName);
			// create UI handler with default entries
			iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger());
			}
		}

	return TestStepResult();
	}
	
/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 * @return TVerdict code
 */
TVerdict CUpsInstallStep::doTestStepL()
	{
	INFO_PRINTF1(KUpsInstallStep);

	// launch the installation
	CInstallPrefs* prefs = CInstallPrefs::NewLC();

	TInt err = DoInstallL(*prefs);
	INFO_PRINTF2(_L("DoInstallL returned %d"), err);
	if (err != KErrNone)
		{
		// Can't set error to KErrServerBusy, or Testexecute will retry.
		SetTestStepResult((err != KErrServerBusy)? static_cast<TVerdict>(err) : EFail);
		}
	CleanupStack::PopAndDestroy(prefs);
	return TestStepResult();
	}

TInt CUpsInstallStep::DoInstallL(CInstallPrefs& aInstallPrefs)
	{
	switch (iInstallType)
		{
		case EUseFileName:
			return Launcher::Install(*iUi, iSisFileName, aInstallPrefs);
		}
			
	// Shouldn't get here
	return KErrGeneral;
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CUpsInstallStep::doTestStepPostambleL()
	{
/*	if((TestStepResult() == EPass))
		{
		RSisRegistrySession registrySession;
		User::LeaveIfError(registrySession.Connect());
		CleanupClosePushL(registrySession);

		RSisRegistryEntry registryEntry;
		TInt error=registryEntry.Open(registrySession, iUid);
		if(error != KErrNone)
			{
			ERR_PRINTF2(_L("RSisRegistrySession::Open failed error=%d\n"), error);
			SetTestStepResult(EFail);
			}
		else
			{
			CleanupClosePushL(registryEntry);
				
			TInt userDrive =0;
			TRAP(error, userDrive = registryEntry.SelectedDriveL());
			if((error != KErrNone) || (userDrive != iExpectedUserDrive))
				{
				ERR_PRINTF4(_L("SelectedDriveL err=%d, userDrive expected %d got %d\n"),
							error, iExpectedUserDrive, userDrive);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF2(_L("SelectedDriveL userDrive = %d OK\n"), userDrive);
				}
		
			CleanupStack::PopAndDestroy(&registryEntry);
			}
		CleanupStack::PopAndDestroy(&registrySession);
		}*/

	return TestStepResult();
	}

//
// CSwisUninstallStep
//

CUpsUninstallStep::~CUpsUninstallStep()
	{
	delete iUi;
	}

CUpsUninstallStep::CUpsUninstallStep(TUninstallType aType)
	: iType(aType)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUpsUnInstallStep);
	}

	
	
	
/**
 * Override of base class virtual. Prepares for the test run of SWIS
 * @return TVerdict code
 */
TVerdict CUpsUninstallStep::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("CUpsUninstallStep::doTestStepPreambleL"));
	// get step parameters
	
	TPtrC str;
	if (!GetStringFromConfig(ConfigSection(), _L("script"), str))
		{
		ERR_PRINTF1(_L("Missing XML file name"));
		SetTestStepResult(EFail);
		return EFail;
		}
	iXmlFileName.Copy(str);	
	TInt uid=0;

	if (!GetHexFromConfig(ConfigSection(), _L("uid"), uid))
		{
		ERR_PRINTF1(_L("Missing uid"));
		SetTestStepResult(EFail);
		return EFail;
		}

	iUid.iUid=uid;
		
	if (iType == EByPackage)
		{
		TPtrC vendorName;
		if (!GetStringFromConfig(ConfigSection(), _L("vendorName"), vendorName))
			{
			ERR_PRINTF1(_L("Missing Vendor Name"));
			SetTestStepResult(EFail);
			return EFail;
			}
		iVendorName.Set(vendorName);
		
		TPtrC packageName;
		if (!GetStringFromConfig(ConfigSection(), _L("packageName"), packageName))
			{
			ERR_PRINTF1(_L("Missing Package Name"));
			SetTestStepResult(EFail);
			return EFail;
			}
		iPackageName.Set(packageName);
			
		
		INFO_PRINTF5(_L("Uninstalling %D, %S, %S  using script '%S'"), 
			iUid.iUid, &iPackageName, &iVendorName, &iXmlFileName);
		// create UI handler with default entries
		iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger());	
		}
	else if (iType== EByUid)
		{
		INFO_PRINTF3(_L("Uninstalling '%D' using script '%S'"), 
			iUid.iUid, &iXmlFileName);
		// create UI handler with default entries
		iUi = CUIScriptAdaptor::NewL(iXmlFileName, Logger());	
		}
	
	return TestStepResult();
	}

/**
 * Override of base class pure virtual
 * Demonstrates reading configuration parameters fom an ini file section
 * @return TVerdict code
 */

TInt CUpsUninstallStep::DoUninstallL()
	{
	TInt error = KErrNotFound;

	if (iType == EByUid)
		{
		// launch the installation
		return Launcher::Uninstall(*iUi, iUid); 
		}
	else if (iType == EByPackage)
		{
		
		TInt err = KErrNotFound;
		// Go through list of packages from base package to get augmentations.
		CSisRegistryPackage* uninstallPackage=CSisRegistryPackage::NewLC(iUid, iPackageName, iVendorName);
		
		RSisRegistrySession registrySession;
		User::LeaveIfError(registrySession.Connect());
		CleanupClosePushL(registrySession);

		RSisRegistryEntry registryEntry;
	
		User::LeaveIfError(registryEntry.Open(registrySession, iUid));
		CleanupClosePushL(registryEntry);

		CSisRegistryPackage* package=registryEntry.PackageL();
		CleanupStack::PushL(package);
		
		if (*package == *uninstallPackage)
			{
				err = -1;
				err=Launcher::Uninstall(*iUi, *package); 
			}
		else
			{
			// check augmenations
			RPointerArray<CSisRegistryPackage> augmentationPackages;
			CleanupResetAndDestroy<RPointerArray<CSisRegistryPackage> >::PushL(augmentationPackages);

			registryEntry.AugmentationsL(augmentationPackages);
			for (TInt i=0; i < augmentationPackages.Count(); ++i)
				{
				if (*augmentationPackages[i] == *uninstallPackage)
					{
					err=User::LeaveIfError(Launcher::Uninstall(*iUi, *augmentationPackages[i])); 
					break;
					}
				}
			CleanupStack::PopAndDestroy(&augmentationPackages);
			}	
		
		CleanupStack::PopAndDestroy(3, &registrySession);
		CleanupStack::PopAndDestroy(uninstallPackage);
		error = err;
		}
	return error;
	}

TVerdict CUpsUninstallStep::doTestStepL()
	{
	INFO_PRINTF1(KUpsUnInstallStep);

	TInt err = DoUninstallL();
	INFO_PRINTF2(_L("DoUninstallL returned %d"), err);
	if (err != KErrNone)
		{
		// Can't set error to KErrServerBusy, or Testexecute will retry.
		SetTestStepResult((err != KErrServerBusy)? static_cast<TVerdict>(err) : EFail);
		}
	return TestStepResult();
	}

/**
 * Override of base class virtual
 * @return TVerdict code
 */
TVerdict CUpsUninstallStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

// End of file
