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
*
*/


#ifndef __TUISCRIPTADAPATORS_H__
#define __TUISCRIPTADAPATORS_H__

#include <xml/parser.h>
#include <xml/contenthandler.h>
#include <swi/msisuihandlers.h>
#include <e32property.h>
#include <e32uid.h>


class TTableOfInstallSizes
	{
public:	
	TBuf<64>  iPkgFileName;
	TInt      iInstallSize; 
	};

namespace Swi
{
namespace Test
{
const TInt KBufSize = 5;

enum TFunctions{
	EScript = 0,
	EDisplayInstall,
    EDisplayGrantCapabilities,
	EDisplayLanguage,
	EDisplayDrive,
	EDisplayUpgrade,
	EDisplayOptions,
	EHandleInstallEvent,
	EHandleCancellableInstallEvent,
	EDisplaySecurityWarning,
	EDisplayOcspResult,
	EDisplayCannotOverwriteFile,
	EDisplayUninstall,
	EDisplayText,
	EDisplayError,
	EDisplayDependencyBreak,
	EDisplayApplicationsInUse,
	EDisplayQuestion,
	EDisplayMissingDependency,
	EDisplaySIDMismatch,
};

enum TLeveltags{
	EItemIndex =1,
	EReturn,
	EUpdateProgress,
	EUpdateProgressFinal
};

enum TSwiCancelType{
	ENoCancel,
	ECancelTimerFromStart,
	ECancelFromDialog,
	ECancelAfterDialog
};


/**
 * Structure to hold the parsed value of the XML file.
 */
struct TXmlFileData {
	TBool			  InstallResult;
	TBool			  CapabilitiesResult; 
	TInt			  LanguagesResult;
	TInt			  DriveResult;
	TBool			  UpgradeResult;
	TBool			  OptionsResult;
	RArray< TBool >	  OptionSelection;
	TBool			  WarningResult;
	TBool			  OcspResult;
	TBool			  UninstallResult;
	TBool			  TextResult;
	TBool			  DependencyResult;
	TBool			  ApplicationsInUseResult;
	TBool			  QuestionResult;
	TBool             MissingDependencyResult;
	TBool			  DevCertsResult;
	TBool			  UpdateProgressBarResult;
	TBool			  ProgressBarFinalValueResult;
		
	TXmlFileData();
};

/**
 * This class defines the interface required by a client of the xml framework.
 * It allows a client to be placed in a chain with other clients, i.e. a Parser,
 * a Validator, or a User, and therefore allows the flow of information between 
 * these links.
 */
class CRebuildingContentHandler : public CBase, public Xml::MContentHandler
	{
public:
	static CRebuildingContentHandler* NewL(CTestExecuteLogger& aLogger);
	static CRebuildingContentHandler* NewLC(CTestExecuteLogger& aLogger);
	~CRebuildingContentHandler();
	void OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode);
	void OnEndDocumentL(TInt aErrorCode);
	void OnStartElementL(const Xml::RTagInfo& aElement, const Xml::RAttributeArray& aAttributes, TInt aErrorCode);
	void OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode);
	void OnContentL(const TDesC8& aBytes, TInt aErrorCode);
	void OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, TInt aErrorCode);
	void OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode);
	void OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode);
	void OnSkippedEntityL(const RString& aName, TInt aErrorCode) ;
	void OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, TInt aErrorCode);
	void OnExtensionL(const RString& aData, TInt aToken, TInt aErrorCode);
	void OnOutOfData();
	void OnError(TInt aErrorCode);
	TAny* GetExtendedInterface(const TInt32 aUid);
	TXmlFileData& GetOptions();
	
public:
	TInt iError;
	
private:
	CRebuildingContentHandler(CTestExecuteLogger& aLogger);
		
	CTestExecuteLogger& Logger();
	TXmlFileData iResult;
	CTestExecuteLogger& iLogger;
	TInt		 iLevel1;
	TInt		 iLevel2;
	TInt		 iLevel3;
	TBool        iLangLogged;    ///< Has language dialog result been logged?
	TBool        iDriveLogged;   ///< Has drive dialog result been logged?
	TBool        iSecWarnLogged; ///< Has security warning result been logged?
	};

inline CTestExecuteLogger& CRebuildingContentHandler::Logger()
	{
	return iLogger;
	}

/**
	When the FT option is used to display a dialog, up to this
	many characters are recorded from the displayed string.  They
	can be compared with the text specified in ExpectedDisplayText
	if it is in the ini file.
 */
const TInt KMaxDisplayText = 32;

class CUIScriptAdaptor : public CBase, public MUiHandler
	{
public:	
	~CUIScriptAdaptor();
	static CUIScriptAdaptor* NewL(const TFileName& aXmlFileName,
		CTestExecuteLogger& aLogger, RArray<TTableOfInstallSizes>* aExpectedValue = NULL);
	static CUIScriptAdaptor* NewLC(const TFileName& aXmlFileName,
		CTestExecuteLogger& aLogger, RArray<TTableOfInstallSizes>* aExpectedValue = NULL);
	static CUIScriptAdaptor* NewL(const TFileName& aXmlFileName,
		CTestExecuteLogger& aLogger, TPtrC aExpectedText,TPtrC aTextIsHash, TInt aExpectedDialog, TInt aKerrCode);
	static CUIScriptAdaptor* NewLC(const TFileName& aXmlFileName,
		CTestExecuteLogger& aLogger, TPtrC aExpectedText,TPtrC aTextIsHash, TInt aExpectedDialog, TInt aKerrCode);
	
	/// Displays install dialog, logging all incoming information.
	TBool DisplayInstallL(const CAppInfo& aAppInfo,
			const CApaMaskedBitmap* aLogo,
				const RPointerArray<CCertificateInfo>& aCertificates);

	TBool DisplayGrantCapabilitiesL(const CAppInfo& aAppInfo,
			const TCapabilitySet& aCapabilitySet);

	TInt DisplayLanguageL(const CAppInfo& aAppInfo, 
			const RArray<TLanguage>& aLanguages);

	TInt DisplayDriveL(const CAppInfo& aAppInfo,TInt64 aSize,
			const RArray<TChar>& aDriveLetters,
				const RArray<TInt64>& aDriveSpaces);

	TBool DisplayUpgradeL(const CAppInfo& aAppInfo,
			const CAppInfo& aExistingAppInfo);

	TBool DisplayOptionsL(const CAppInfo& aAppInfo, 
			const RPointerArray<TDesC>& aOptions,
				RArray<TBool>& aSelections); 

	TBool HandleInstallEventL(const CAppInfo& aAppInfo,
			TInstallEvent aEvent, TInt aValue=0, 
				const TDesC& aDes=KNullDesC);

	void HandleCancellableInstallEventL(const CAppInfo& aAppInfo,
			TInstallCancellableEvent aEvent, 
				MCancelHandler& aCancelHandler,TInt aValue=0, 
					const TDesC& aDes=KNullDesC);

	TBool DisplaySecurityWarningL(const CAppInfo& aAppInfo,
			TSignatureValidationResult aSigValidationResult,
			RPointerArray<CPKIXValidationResultBase>& aPkixResults,
			RPointerArray<CCertificateInfo>& aCertificates,
			TBool aInstallAnyway);

	TBool DisplayOcspResultL(const CAppInfo& aAppInfo,
			TRevocationDialogMessage aMessage, 
			RPointerArray<TOCSPOutcome>& aOutcomes,
			RPointerArray<CCertificateInfo>& aCertificates,
			TBool aWarningOnly);

	void DisplayCannotOverwriteFileL(const CAppInfo& aAppInfo,
			const CAppInfo& aInstalledAppInfo,
				const TDesC& aFileName);

	TBool DisplayUninstallL(const CAppInfo& aAppInfo);

	TBool DisplayTextL(const CAppInfo& aAppInfo, 
			TFileTextOption aOption,
				const TDesC& aText);

	void DisplayErrorL(const CAppInfo& aAppInfo,
			TErrorDialog aType, 
				const TDesC& aParam);

	TBool DisplayDependencyBreakL(const CAppInfo& aAppInfo,
	        const RPointerArray<TDesC>& aComponents);

	TBool DisplayApplicationsInUseL(const CAppInfo& aAppInfo, 
		    const RPointerArray<TDesC>& aAppNames);

	TBool DisplayQuestionL(const CAppInfo& aAppInfo, 
		    TQuestionDialog aQuestion, const TDesC& aDes=KNullDesC);
		    
	TBool DisplayMissingDependencyL(const CAppInfo& aAppInfo,
		const TDesC& aDependencyName,TVersion aWantedVersionFrom,
		TVersion aWantedVersionTo,TVersion aInstalledVersion);

	// functions to handle displaying info to file
	void DisplayAppInfo(const TDesC& aPreText, const CAppInfo& aAppInfo);
	void DisplayBoolReturn(TBool aReturn);
	void DisplayIntReturn(TInt aReturn);

	// function to check progress bar
	void CheckFinalProgressBarL();

	// Function that aborts a test case when the expected
	// installation size does not match the calculated size.
	void CheckExpectedAndCalculatedInstallSizesL(const TDesC& aAppInfoName, TInt64 aSize);

	// function to check if the "Display Drive" dialog have been called
 	TBool DisplayDriveDialogCalled() const;

	// function to check the "Grant Capabilities" dialog has been called
	TBool GrantUserCapsDialogCalled() const;
	
	// function to check the "CannotOverwrite" dialog has been called
	TBool CannotOverwriteFileDialogCalled() const;

	// function to check the "file owner" name in the CannotOverwrite dialog.
	const TDesC& FileOwner() const;

	// function to check whether a fatal OCSP response was encountered
	TBool FatalOcspResponseEncountered() const;
		
	//Function to check the expected Ocsp Outcomes
    const RPointerArray<TOCSPOutcome>& ObservedOcspOutcomes() const;
	
	/// Use to enable and disable cancel testing inside dialogs
	void SetCancelDialogTesting(TSwiCancelType aCancelType);
	
	// Set the dialog number to cancel on
	void SetCancelDialogNumber(TInt aDialogNumber);

	/// Use to enable and disable dialog delays
	void SetDialogDelay(TTimeIntervalMicroSeconds32 aUsec);

	// Set specific dialog number to delay at (default is all dialogs if dialog delay > 0)
	void SetDelayDialogNumber(TInt aDialog);

	// Reset the current dialog number, and set iInstallationCancelled to False
	void ResetCurrentDialogNumber(TInt aDialogNumber = 0);
	TBool InstallationCancelled() const;

	inline const TDesC& DisplayedText() const;
	inline const TDesC& DisplayedTextActual() const;

	//Leave with error code
	void LeaveError(TInt aReason);
	
	inline TInt NumberOfSecurityDialogsInvoked() const;
		
private:
	/** 
	 * Constructor
	 * @param aXmlFileName    Name of the XML file containing dialog parameters
	 * @param aLogger         TestExecute logger
	 * @param aExpectedValue  Array of install sizes, if present.
	 */
	CUIScriptAdaptor(const TFileName& aXmlFileName, CTestExecuteLogger& aLogger,
					 RArray<TTableOfInstallSizes>* aExpectedValue = NULL);
	CUIScriptAdaptor(const TFileName& aXmlFileName, CTestExecuteLogger& aLogger,
					 TPtrC aExpectedText,
					 TPtrC aTextIsHash,
					 TInt aExpectedDailog,
					 TInt aKerrcode
					 );				 					 
					 					 
	void ConstructL();
	void ScriptParseL();
	CTestExecuteLogger& Logger();
	
	// Called whenever a dialog is reached
	void DialogReached();

	TXmlFileData         iResult;
	TFileName            iXmlFileName;
	RFs			         iFs;
	CTestExecuteLogger& iLogger;
	
	TInt iProgressBarFinal;	///< Progress bar final value.
	TInt iProgressBarCurrent;	///< Progress bar current value.
	TPtrC iExpectedDisplayText;
	TPtrC iExpectedHash;
	TInt iExpectedDialog;
	TInt  iKerrCode;
	RArray<TTableOfInstallSizes>* iExpectedValue;

	TBool iDialogCancelType;			///< Controls whether we do cancel testing
	TInt iCancelDialogNumber;		///< The dialog to cancel at 
	TInt iCurrentDialogNumber;		///< The currentDialogNumber.
	TBool iInstallationCancelled;	///< Whether we've cancelled the install yet
	MCancelHandler *iCancelHandler; ///< Current CancellableInstallEvent cancel callback

	// TBools for whether certain dialogs have been called
	TBool iDisplayDriveDialogCalled;
	TBool iGrantUserCapsDialogCalled;
	TBool iCannotOverwriteFileDialogCalled;
	HBufC* iFileOwner;
	TTimeIntervalMicroSeconds32 iDialogDelay; // For async cancel testing
	TInt iDelayDialogNumber;
	TBool iFatalOcspResponseEncountered;
	/** Possibly truncated text which was displayed with FT. */
	TBuf<KMaxDisplayText> iDisplayedText;
	HBufC *iDisplayedTextActual;
	TInt iSecurityWarningDialogsInvoked;
	// Array of observed ocsp outcomes for comparison to expected values.
    RPointerArray<TOCSPOutcome> iObservedOcspOutcomes;
    };

inline TBool CUIScriptAdaptor::DisplayDriveDialogCalled() const
 	{
 	return iDisplayDriveDialogCalled;
 	}
 
inline TBool CUIScriptAdaptor::GrantUserCapsDialogCalled() const
	{
	return iGrantUserCapsDialogCalled;
	}

inline TBool CUIScriptAdaptor::FatalOcspResponseEncountered() const 
	{
	return iFatalOcspResponseEncountered;
	}

inline const RPointerArray<TOCSPOutcome>& CUIScriptAdaptor::ObservedOcspOutcomes() const
    {
    return iObservedOcspOutcomes;  	
    }
 
inline 	CTestExecuteLogger& CUIScriptAdaptor::Logger()
	{
	return iLogger;
	}

inline void CUIScriptAdaptor::SetCancelDialogTesting(TSwiCancelType aCancelType)
	{
	iDialogCancelType = aCancelType;
	}

inline void CUIScriptAdaptor::SetCancelDialogNumber(TInt aDialogNumber)
	{
	iCancelDialogNumber = aDialogNumber;
	}

inline void CUIScriptAdaptor::ResetCurrentDialogNumber(TInt aDialogNumber)
	{
	iCurrentDialogNumber = aDialogNumber;
	iInstallationCancelled = EFalse;
	if(aDialogNumber == 0) iCancelHandler = 0;
	}

inline TBool CUIScriptAdaptor::InstallationCancelled() const
	{
	return iInstallationCancelled;
	}

inline void CUIScriptAdaptor::SetDialogDelay(TTimeIntervalMicroSeconds32 aDialogDelay)
	{
	iDialogDelay = aDialogDelay;
	}

inline void CUIScriptAdaptor::SetDelayDialogNumber(TInt aDialog)
	{
	iDelayDialogNumber = aDialog;
	}

inline const TDesC& CUIScriptAdaptor::DisplayedText() const
/**
	Returns text displayed with FT.

	@return					Text displayed with FT.
 */
	{
	return iDisplayedText;
	}

inline const TDesC& CUIScriptAdaptor::DisplayedTextActual() const
	{
	return *iDisplayedTextActual;		// Unchanged text
	}

inline TBool CUIScriptAdaptor::CannotOverwriteFileDialogCalled() const
 	{
 	return iCannotOverwriteFileDialogCalled;
 	}

inline const TDesC& CUIScriptAdaptor::FileOwner() const
	{
	return *iFileOwner;
	}

inline TInt CUIScriptAdaptor::NumberOfSecurityDialogsInvoked() const
	{
	return iSecurityWarningDialogsInvoked;
	}

}//swi
}//test
#endif // TUISCRIPTADAPATORS_H
