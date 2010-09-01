/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tswisstep.h
* SWIS test step declaration
*
*/




/**
 @file
*/

#ifndef __TSWISSTEP_H__
#define __TSWISSTEP_H__

#include <test/testexecutestepbase.h>
#include "../../testutil/client/testutilclient.h"
#include "tupsserver.h"
#include "tuiscriptadaptors.h"

#include "e32base.h"

namespace Swi
	{
	class CInstallPrefs;
	}

/**
 * Base test step class containing handy functions
 * @test
 * @internalComponent
 */
class CUpsTestStep : public CTestStep
	{
protected:

	enum TReadType
		{
		ESimpleType,		//to read "numexist" and "numnonexist" value from INI.
		ECancelType,		//to read "numexistcancel" and "numnonexistcancel" value from INI.
		ERegisteredType		//to read "numregistered" value from INI.
		};
		
	/**
	 * Get the file names for checking if present or missing.
	 * The test step's INI file must contain entry "numexistcancel=<n>" and one or more entries
	 * "existcancel<i>=<fname>". 
	 * The test step's INI file must contain entry "numnonexistcancel=<n>" and one or more entries 
	 * "nonexistcancel<i>=<fname>".
	 * The test step's INI file must contain entry "numexist=<n>" and one or more entries 
	 * "exist<i>=<fname>".
	 * The test step's INI file must contain entry "numnonexist=<n>" and one or more entries 
	 * "nonexist<i>=<fname>".
	 * 
	 * @param aFileNumExist     this array contains the name of the files which must exist after
	 *						    installation/un-installation/cancellation.
	 * @param aFileNumNonExist	this array contains the name of the files which must not exist after
	 * 							installation/un-installation/cancellation.
	 * @param aDoCancelTest     Used to make decision for reading from the INI file.
	 *                          ETrue  Cancel Testing is enabled, so read entries "numexistcancel" and "numnonexistcancel".
	 *                          EFalse Read entries "numexist" and "numnonexist".
	 */
	void GetFileNamesForCheck(RArray<TPtrC>& aFileNumExist,RArray<TPtrC>& aFileNumNonExist, TReadType aReadType);
	 
	/**
	 * Extract the file names from the INI file. 
	 * @param aEntries     Number of file name entries. 
	 * @param aEntryBase   Base name of the keys specifying names of files to 
	 *                     check; an index (0..n-1) is appended to the base 
	 *                     name to get the actual key name.
	 * @param aFileArray   Contains all the file names to check for missing or present. 
	 */ 
	void ExtractFileName(TInt aEntries, const TDesC& aEntryBase, RArray<TPtrC>& aFileArray);
	 
	/** 
	 * Checks for files that should exist if the installation was successful,
	 * and which shouldn't exist if the installation was cancelled. 
	 * @param aCheckExist  Controls the kind of testing done
	 *                     ETrue  Files must exist
	 *                     EFalse Files must not exist
	 * @param aFileArray   Contains all the file names to check for missing or present depending
	 *					   on the aCheckExist. If ETrue, this contains the names of files that must exist
	 *					   else if EFalse contains the names of files that must not be present.
	 * @param aTestUtil    Contains reference to the testutilserver session.
	 * @param aMsecTimeout Timeout to wait for file to be created, if not already existing
     */
	void CheckIfFilesExist(TBool aCheckExist, const RArray<TPtrC>& aFileArray, RTestUtilSession& aTestUtil, TInt aMsecTimeout=0);
	 
	/**
	 * Checks whether certain files do or do not exist by checking the registry entry and
	 * sets test step result accordingly.
	 * @param aFileArray   Contains all the file names to check for missing/present.
	 */ 
	void CheckRegistryEntry(RArray<TPtrC>& aFileArray);

	/**
	 * Checks that the correct dialogs have been called during the installation.
	 */
	void CheckDialogsCalled(const Swi::Test::CUIScriptAdaptor& ui);
	
	/**
	 * Checks whether a fatal OCSP response was encountered during the installation.
	 */
	void CheckFatalOcspResponse(const Swi::Test::CUIScriptAdaptor& ui);
	
	/**
	 * Checks whether the values of ocsp outcomes in the ini file matches with that of 
	 * the DisplayOcspResultL function.
	 */
    void  CheckExpectedOcspOutcomes(const Swi::Test::CUIScriptAdaptor& ui);	

   /**
	 * Copy files from one place to another.
	 *
	 * @param aNumEntries Name of the key that specifies number of file name 
	 *                    entries
	 * @param aFrom  Base name of the keys specifying names of files to 
	 *                    copy from; an index (0..n-1) is appended to the base 
	 *                    name to get the actual key name
	 * @param aTo    Base name of the keys specifying names of files to 
	 *                    copy to; an index (0..n-1) is appended to the base 
	 *                    name to get the actual key name
	 */
	void CopyFilesL(const TDesC& aNumEntries, const TDesC& aFrom, const TDesC& aTo);
	
	/**
 	 * Delete files.
 	 *
 	 * @param aNumEntries     Name of the key that specifies number of file name 
 	 *                        entries
 	 * @param aDeleteKeyBase  Base name of the keys specifying names of files
 	 *                        to delete; an index (0..n-1) is appended to the
 	 *                        base name to get the actual key name
 	 */
 	void DeleteFilesL(const TDesC& aNumEntries, const TDesC& aDeleteKeyBase);
	
	/**
	 * Stores all the values of expected install sizes as indicated in the test
	 * step's INI file.
	 * @param aNumExpectedSizeEntries Name of key that specifies the number of
	 *                                expected install sizes.
	 * @param aExpectedInstallSize    A reference to the array of install sizes.
	 */
	void StoreExpectedInstallSizesL(TInt aNumExpectedSizeEntries, RArray<TTableOfInstallSizes>& aExpectedValue);
	
	/** Verify the Pub and Sub value of the current installation/uninstallation */
	void CheckPubSubStatus(TDesC& aPubSubStatus);

	/**
	 * Check that the pub and sub status returns to KErrNone after a test.
	 * This check should typically be last, whereas the previous method should
	 * be done ASAP after an install/uninstall since there's a race condition.
	 */
	void CheckPubSubStatusIdle();
	
   /**
	 * Set or clear read only bit on files.
	 *
	 * @param aNumEntries     Name of the key that specifies number of file name 
	 *                        entries
	 * @param aFileKeyBase    Base name of the keys specifying names of files
	 *                        to set; an index (0..n-1) is appended to the
	 *                        base name to get the actual key name
	 * @param aFileKeyBase    Base name of the keys specifying operation on files
	 *                        ; an index (0..n-1) is appended to the
	 *                        base name to get the actual key name.
	 */
	void SetClearReadOnlyFilesL(const TDesC& aNumEntries,
								const TDesC& aFileKeyBase,
								const TDesC& aSetClearOpBase);
	
   /**
	 * Check current step result against list of expected result codes.
	 */
	void CheckExpectedResultCodesL();

	/**
	 * Compares hashes for pairs of files read from config section, sets the
	 * test step result to fail if any pair does not match or if there is an
	 * error attempting to get a hash.
	 */
	void CompareFilesL();

	/**
	 * Compares file contents with buffer. Sets test step result to fail if
	 * the contents do not match or there is an error in performing the
	 * operation.
	 */
	void CompareFileWithBufferL(const Swi::Test::CUIScriptAdaptor& ui);
	};

/**
 * Installation test step class
 * @test
 * @internalComponent
 */
class CUpsInstallStep : public CUpsTestStep
	{
public:
	enum TInstallType { EUseFileName };

	CUpsInstallStep(TInstallType aInstallType);
	~CUpsInstallStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	TInt DoInstallL(Swi::CInstallPrefs& aInstallPrefs);
	void GetFilesToHoldOpenL();

private:
	TFileName iSisFileName; // name of the file to install
	TFileName iXmlFileName; // name of the file containing dialog data
	Swi::Test::CUIScriptAdaptor* iUi; // XML script-based UI implementation
	TInstallType iInstallType; // use file handle to install
	RArray<TTableOfInstallSizes> iExpectedValue; // array of expected install sizes
	TBool iDoCancelTest;
	TPtrC iExpectedHash;
	TInt iExpectedDialog;
	TInt  iKErrCode;
	TPtrC Outcomes;// outcomes as received from ini file
	TUid iUid;
	TInt iExpectedUserDrive; // -1 if not checked
	TPtrC iPubSubStatus;
	TBool iExpectFileChange;
	};

/**
 * Uninstallation test step class
 * @test
 * @internalComponent
 */
class CUpsUninstallStep : public CUpsTestStep
	{
public:
	enum TUninstallType { EByUid, EByPackage };
	
	CUpsUninstallStep(TUninstallType aType);
	~CUpsUninstallStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();
private:
	TInt DoUninstallL();

private:
	TUid iUid;
	TFileName iXmlFileName; // name of the file containing dialog data
	TPtrC iVendorName;
	TInt iKErrCode;
	Swi::Test::CUIScriptAdaptor* iUi; // XML script-based UI implementation
	TPtrC iPackageName;
	TUninstallType iType;
	};


_LIT(KUpsInstallStep, "InstallStep");

_LIT(KUpsUnInstallAugStep, "UninstallAugStep");

_LIT(KUpsUnInstallStep, "UninstallStep");

#endif // __TSWISSTEP_H__
