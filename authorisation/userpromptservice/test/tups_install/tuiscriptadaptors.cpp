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


#define __INCLUDE_CAPABILITY_NAMES__

#include <e32test.h>
#include <f32file.h>
#include <test/testexecutelogger.h>
#include "tuiscriptadaptors.h"
#include "languagetable.h"

#include <swi/launcher.h>
#include <e32capability.h>
#include <utf.h>

using namespace Swi;
using namespace Swi::Test;
using namespace Xml;

_LIT8(KParserDataType, "text/xml");
_LIT8(KTrueValue,      "true");
_LIT8(KFalseValue,     "false");
_LIT8(KIndexType,      "item");
_LIT8(KReturnType,     "return");
_LIT8(KDialogName,     "dialog");
_LIT8(KUpdateProgressReturn, "updateprogressreturn");
_LIT8(KUpdateProgressFinalReturn, "updateprogressfinalreturn");


enum TDialogs
		{
		KDisplayInstall1      = 1, 
		KDisplayLanguage1,
		KDisplayDrive1,
		KDisplayUpgrade1,	
		KDisplayOptions1,
		KDisplaySecurityWarning1,
		KDisplayOcspResult1,
		KDisplayCannotOverwriteFile1,
		KDisplayUninstall1,
		KDisplayText1,
		KDisplayError1,
		KDisplayDependencyBreak1,
		KDisplayApplicationsInUse1,
		KDisplayQuestion1,
		KDisplayMissingDependency1,
		KInstallEvenDialogSetProgressBar,
		KInstallEvenDialogUpdateProgressBar,
		KInstallEvenDialogCompletedInstall,
		KInstallEvenDialogOcspCheckEnd,
		KInstallEvenDialogCompletedUnInstall,
		KInstallEvenDevCert,
		KCancelInstallRemoving,
		KCancelInstallShuttingDown,
		KCancelInstallCopyingFiles,
		KCancelInstallOcspCheckStart
		};
/* Functions Literals */
_LIT8(KDisplayInstall,               "DisplayInstall");
_LIT8(KDisplayGrantCapabilities,     "DisplayGrantCapabilities");
_LIT8(KDisplayLanguage,              "DisplayLanguage");
_LIT8(KDisplayDrive,                 "DisplayDrive");
_LIT8(KDisplayUpgrade,               "DisplayUpgrade");
_LIT8(KDisplayOptions,               "DisplayOptions");
_LIT8(KHandleInstallEvent,           "HandleInstallEvent");
_LIT8(KHandleCancellableInstallEvent,"HandleCancellableInstallEvent");
_LIT8(KDisplaySecurityWarning,       "DisplaySecurityWarning");
_LIT8(KDisplayOcspResult,            "DisplayOcspResult");
_LIT8(KDisplayCannotOverwriteFile,   "DisplayCannotOverwriteFile");
_LIT8(KDisplayUninstall,             "DisplayUninstall");
_LIT8(KDisplayText,                  "DisplayText");
_LIT8(KDisplayError,                 "DisplayError");
_LIT8(KDisplayDependencyBreak,       "DisplayDependencyBreak");
_LIT8(KDisplayApplicationsInUse,     "DisplayApplicationsInUse");
_LIT8(KDisplayQuestion,              "DisplayQuestion");
_LIT8(KDisplayMissingDependency,     "DisplayMissingDependency");
_LIT8(KDisplaySIDMismatch,           "DisplaySIDMismatch"); 

#define	KGenericBufferSize		      100

// Init defaults which will be overridden by script data (or not)
TXmlFileData::TXmlFileData()
	: InstallResult(ETrue)
	, CapabilitiesResult(ETrue)
	, LanguagesResult(0)
	, DriveResult('c'-'a') // Default to installing to C:
	, UpgradeResult(ETrue)
	, OptionsResult(ETrue)
	, WarningResult(ETrue)
	, OcspResult(ETrue)
	, UninstallResult(ETrue)
	, TextResult(ETrue)
	, DependencyResult(EFalse)
	, ApplicationsInUseResult(ETrue)
	, QuestionResult(EFalse)
	, MissingDependencyResult(EFalse)
	, DevCertsResult(ETrue)
	, UpdateProgressBarResult(ETrue)
	, ProgressBarFinalValueResult(ETrue)
	{
	}
	
CUIScriptAdaptor::CUIScriptAdaptor(const TFileName& aXmlFileName, CTestExecuteLogger& aLogger,
								   RArray<TTableOfInstallSizes>* aExpectedValue)
	:
	iXmlFileName(aXmlFileName), 
	iLogger(aLogger),
	iExpectedValue(aExpectedValue),
	iDelayDialogNumber(-1)
	{
	}

CUIScriptAdaptor* CUIScriptAdaptor::NewLC(const TFileName& aXmlFileName, CTestExecuteLogger& aLogger,
										  RArray<TTableOfInstallSizes>* aExpectedValue)
	{
	CUIScriptAdaptor* self = new(ELeave) CUIScriptAdaptor(aXmlFileName, aLogger, aExpectedValue);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUIScriptAdaptor* CUIScriptAdaptor::NewL(const TFileName& aXmlFileName, CTestExecuteLogger& aLogger,
										 RArray<TTableOfInstallSizes>* aExpectedValue)
	{
	CUIScriptAdaptor* self = NewLC(aXmlFileName, aLogger, aExpectedValue);
	CleanupStack::Pop(self);
	return self;
	}
	
CUIScriptAdaptor::CUIScriptAdaptor(const TFileName& aXmlFileName, CTestExecuteLogger& aLogger,
								   TPtrC aExpectedText,
								   TPtrC aTextIsHash,
								   TInt aExpectedDailog,
								   TInt aKerrCode)
	:
	iXmlFileName(aXmlFileName), 
	iLogger(aLogger),
	iExpectedDisplayText(aExpectedText),
	iExpectedHash(aTextIsHash),
	iExpectedDialog(aExpectedDailog),
	iKerrCode(aKerrCode),
	iDelayDialogNumber(-1),
	iDisplayedTextActual(NULL)
	{
	}
	
CUIScriptAdaptor* CUIScriptAdaptor::NewLC(const TFileName& aXmlFileName, CTestExecuteLogger& aLogger,
										  TPtrC aExpectedText,
										  TPtrC aTextIsHash,
										  TInt aExpectedDailog,
										  TInt aKerrCode
										  )
	{
	CUIScriptAdaptor* self = new(ELeave) CUIScriptAdaptor(aXmlFileName, aLogger, aExpectedText, aTextIsHash, aExpectedDailog, aKerrCode);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CUIScriptAdaptor* CUIScriptAdaptor::NewL(const TFileName& aXmlFileName, CTestExecuteLogger& aLogger,
	   								     TPtrC aExpectedText,
	   								     TPtrC aTextIsHash,
	   								     TInt aExpectedDailog,
										 TInt aKerrCode
	   								     )
	{
	CUIScriptAdaptor* self = NewLC(aXmlFileName, aLogger, aExpectedText,aTextIsHash, aExpectedDailog, aKerrCode);
	CleanupStack::Pop(self);
	return self;
	}	
void CUIScriptAdaptor::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	ScriptParseL();
	}

CUIScriptAdaptor::~CUIScriptAdaptor()
	{
	iResult.OptionSelection.Close();
	iFs.Close();
    iObservedOcspOutcomes.ResetAndDestroy();
    delete iFileOwner;
   	delete iDisplayedTextActual;
	}

/**
 * The function is called to parse the XML file.
 */
void CUIScriptAdaptor::ScriptParseL()
	{
	CRebuildingContentHandler* contentHandler = 
		CRebuildingContentHandler::NewLC(iLogger);
	
	// Open parser session
	CParser* parser = CParser::NewLC(KParserDataType, *contentHandler);
	
	// Starts parser session and active scheduler.
	INFO_PRINTF1(_L("Parsing XML..."));
	ParseL(*parser, iFs, iXmlFileName);
			
	iResult = contentHandler->GetOptions();
	CleanupStack::PopAndDestroy(2, contentHandler); // parser, contentHandler
	}

/**
 * Extracts the nPos command line argument.
 */

HBufC* GetArgumentL(TInt nPos)
	{
	HBufC *argv = HBufC::NewLC(User::CommandLineLength());
	TPtr cmd(argv->Des());
	User::CommandLine(cmd);

	TLex arguments(cmd);

	// finds nth parameter
	while(nPos && !arguments.Eos())
		{
		TPtrC token = arguments.NextToken();
		if(token.Length() > 0)
			{
			nPos--;
			}
		}

	HBufC* result = NULL;

	if(!arguments.Eos())
		{
		TPtrC testfile(arguments.NextToken());

		if(testfile.Length() > 0)
			{
			result = testfile.AllocL();
			}
		};

	// no parameter found, but must return something so..
	if(!result)
		{
		result = HBufC::NewL(0);
		}

	CleanupStack::PopAndDestroy(argv);

	return result;
	}

CRebuildingContentHandler* CRebuildingContentHandler::NewLC(
	CTestExecuteLogger& aLogger)
	{
	CRebuildingContentHandler* self = new(ELeave) CRebuildingContentHandler(
		aLogger);
	CleanupStack::PushL(self);
	return self;
	}

CRebuildingContentHandler* CRebuildingContentHandler::NewL(
	CTestExecuteLogger& aLogger)
	{
	CRebuildingContentHandler* self = new(ELeave) CRebuildingContentHandler(
		aLogger);
	return self;
	}

CRebuildingContentHandler::CRebuildingContentHandler(
	CTestExecuteLogger& aLogger)
	: 
	iError(KErrNone), 
	iLogger(aLogger),
	iLevel1(EScript)
	{
	}

CRebuildingContentHandler::~CRebuildingContentHandler()
	{
	}

void CRebuildingContentHandler::OnStartDocumentL(const RDocumentParameters& 
											/*aDocParam*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CRebuildingContentHandler::OnEndDocumentL(TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}
	
void CRebuildingContentHandler::OnStartElementL(const RTagInfo& aElement, 
						const RAttributeArray& aAttributes, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);

	TInt nAttributes = aAttributes.Count();
	for(TInt i=0; i<nAttributes; i++)
		{
		const RAttribute& attribute = aAttributes[i];
		const TDesC8& value8 = attribute.Value().DesC();
		const TDesC8& tagname8 = aElement.LocalName().DesC();
		switch(iLevel1)
			{
			case EScript:
				if(value8.Compare(KDisplaySIDMismatch)==KErrNone)
					{
					 iLevel1 = EDisplaySIDMismatch;
					  break;
					}
				else if (value8.Compare(KDisplayInstall) == KErrNone )
					{
					iLevel1 = EDisplayInstall;
					break;
					}
				else if (value8.Compare(KDisplayGrantCapabilities) == KErrNone)
					{
					iLevel1 = EDisplayGrantCapabilities;
					break;
					}
				else if (value8.Compare(KDisplayLanguage) == KErrNone )
					{
					iLevel1 = EDisplayLanguage;
					break;
					}
				else if (value8.Compare(KDisplayDrive) == KErrNone )
					{
					iLevel1 = EDisplayDrive;
					break;
					}
				else if (value8.Compare(KDisplayUpgrade) == KErrNone )
					{
					iLevel1 = EDisplayUpgrade;
					break;
					}
				else if (value8.Compare(KDisplayOptions) == KErrNone )
					{
					iLevel1 = EDisplayOptions;
					break;
					}
				else if (value8.Compare(KHandleInstallEvent) == KErrNone )
					{
					iLevel1 = EHandleInstallEvent;
					break;
					}
				else if (value8.Compare(KHandleCancellableInstallEvent) == KErrNone )
					{
					iLevel1 = EHandleCancellableInstallEvent;
					break;
					}
				else if (value8.Compare(KDisplaySecurityWarning) == KErrNone )
					{
					iLevel1 = EDisplaySecurityWarning;
					break;
					}
				else if (value8.Compare(KDisplayOcspResult) == KErrNone )
					{
					iLevel1 = EDisplayOcspResult;
					break;
					}
				else if (value8.Compare(KDisplayCannotOverwriteFile) == KErrNone )
					{
					iLevel1 = EDisplayCannotOverwriteFile;
					break;
					}
				else if (value8.Compare(KDisplayUninstall) == KErrNone )
					{
					iLevel1 = EDisplayUninstall;
					break;
					}
				else if (value8.Compare(KDisplayText) == KErrNone )
					{
					iLevel1 = EDisplayText;
					break;
					}
				else if (value8.Compare(KDisplayError) == KErrNone )
					{
					iLevel1 = EDisplayError;
					break;
					}
				else if (value8.Compare(KDisplayDependencyBreak) == KErrNone )
					{
					iLevel1 = EDisplayDependencyBreak;
					break;
					}
				else if (value8.Compare(KDisplayApplicationsInUse) == KErrNone )
					{
					iLevel1 = EDisplayApplicationsInUse;
					break;
					}
				else if (value8.Compare(KDisplayQuestion) == KErrNone )
					{
					iLevel1 = EDisplayQuestion;
					break;
					}
				else if (value8.Compare(KDisplayMissingDependency) == KErrNone )
					{
					iLevel1 = EDisplayMissingDependency;
					break;
					}
			case EDisplayInstall:
			case EDisplayGrantCapabilities:
			case EDisplayLanguage:
			case EDisplayDrive:
			case EDisplayUpgrade:
			case EHandleCancellableInstallEvent:
			case EDisplaySecurityWarning:
			case EDisplayOcspResult:
			case EDisplayCannotOverwriteFile:
			case EDisplayUninstall:
			case EDisplayText:
			case EDisplayError:
			case EDisplayDependencyBreak:
			case EDisplayApplicationsInUse:
			case EDisplayQuestion:
			case EDisplayMissingDependency:
			case EDisplaySIDMismatch:
				if ( tagname8.Compare(KReturnType) == KErrNone )
					{
					iLevel2 = EReturn;
					}
				break;
			case EDisplayOptions:
				if ( tagname8.Compare(KIndexType) == KErrNone )
					{
					iLevel3 = EItemIndex;
					}
				else if ( tagname8.Compare(KReturnType) == KErrNone )
						{
						iLevel2 = EReturn;
						}
				break;
			case EHandleInstallEvent:
				if ( tagname8.Compare(KUpdateProgressReturn) == KErrNone )
					{
					iLevel2 = EUpdateProgress;
					}
				else if (tagname8.Compare(KUpdateProgressFinalReturn) == KErrNone )
					{
					iLevel2 = EUpdateProgressFinal;
					}
				else if ( tagname8.Compare(KReturnType) == KErrNone )
					{
					iLevel2 = EReturn;
					}
				break;
			default:
				break;
			}
		}
	}

void CRebuildingContentHandler::OnEndElementL(const RTagInfo& aElement, 
												TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	const TDesC8& localPart8 = aElement.LocalName().DesC();
	if ( localPart8.Compare(KDialogName) == KErrNone )
		{
		iLevel1 = EScript;
		iLevel2 = KErrNone;
		iLevel3 = KErrNone;
		}
	}

void CRebuildingContentHandler::OnContentL(const TDesC8& aData8, 
												TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);

	switch(iLevel1)
		{
		case EDisplayInstall:
			if ( iLevel2 == EReturn )	
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayInstallL will return ETrue"));
					iResult.InstallResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayInstallL will return EFalse"));
					iResult.InstallResult = EFalse;
					}
				}
			break;
		case EDisplayGrantCapabilities:
			if ( iLevel2 == EReturn )	
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayGrantCapabilitiesL will return ETrue"));
					iResult.CapabilitiesResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayGrantCapabilitiesL will return EFalse"));
					iResult.CapabilitiesResult = EFalse;
					}
				}
			break;
		case EDisplayLanguage:
			if ( iLevel2 == EReturn )	
				{
				TName	tagdata;
				TInt	lang=0;
				tagdata.Copy(aData8);
				tagdata.Trim();
				TLex value(tagdata);
				value.Val(lang);
				if (!iLangLogged)
					{
					INFO_PRINTF2(_L("DisplayLanguageL will return %d"), lang);
					iResult.LanguagesResult = lang;
					iLangLogged = ETrue;
					}
				}
			break;
		case EDisplayDrive:
			if ( iLevel2 == EReturn )	
				{
				TInt	drive=-1;
				TName	tagdata;
				tagdata.Copy(aData8);
				tagdata.Trim();
				
				if (tagdata.Size() && TChar(tagdata[0]).IsAlphaDigit()) 
					{
					drive=tagdata[0]-'a';
					}
				if (!iDriveLogged)
					{
					INFO_PRINTF2(_L("DisplayDriveL will return drive '%c'"), drive+'A');
					iResult.DriveResult = drive;
					iDriveLogged = ETrue;
					}
				}
			break;
		case EDisplayUpgrade:
			if ( iLevel2 == EReturn )
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayUpgradeL will return ETrue"));
					iResult.UpgradeResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayUpgradeL will return EFalse"));
					iResult.UpgradeResult = EFalse;
					}
				}
			break;
		case EDisplayOptions:
			if ( iLevel3 == EItemIndex && iLevel2 != EReturn)
				{			
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Selection Index of the DisplayOptionsL	  : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.OptionSelection.Append(ETrue);
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Selection Index of the DisplayOptionsL	  : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.OptionSelection.Append(EFalse);
					}
				}
			else if ( iLevel2 == EReturn )	
					{
					if ( aData8.Compare(KTrueValue) == KErrNone )
						{
						/*
						TBuf8<KGenericBufferSize> text;
						text.Format(_L8 ("\015\012The Input value of the DisplayOptionsL	 	  : "));
						iOutFile.Write(text);	
						iOutFile.Write(aData8);
						*/
						iResult.OptionsResult = ETrue;
						}
					else if ( aData8.Compare(KFalseValue) == KErrNone )
						{
						/*
						TBuf8<KGenericBufferSize> text;
						text.Format(_L8 ("\015\012The Input value of the DisplayOptionsL	  : "));
						iOutFile.Write(text);
						iOutFile.Write(aData8);
						*/
						iResult.OptionsResult = EFalse;
						}
					}
			break;
			
		case EHandleInstallEvent:
			if (iLevel2 == EUpdateProgress)
				{
				if (aData8.Compare(KTrueValue) == KErrNone)
					{
					INFO_PRINTF1(_L("HandleInstallEvent-UpdateProgress will return ETrue"));
					iResult.UpdateProgressBarResult = ETrue;
					}
				else if (aData8.Compare(KFalseValue) == KErrNone)
					{
					INFO_PRINTF1(_L("HandleInstallEvent-UpdateProgress will return EFalse"));
					iResult.UpdateProgressBarResult = EFalse;
					}
				}
			else if (iLevel2 == EUpdateProgressFinal)
				{
				if (aData8.Compare(KTrueValue) == KErrNone)
					{
					INFO_PRINTF1(_L("HandleInstallEvent-UpdateProgressFinal will return ETrue"));
					iResult.ProgressBarFinalValueResult = ETrue;
					}
				else if (aData8.Compare(KFalseValue) == KErrNone)
					{
					INFO_PRINTF1(_L("HandleInstallEvent-UpdateProgressFinal will return EFalse"));
					iResult.ProgressBarFinalValueResult = EFalse;
					}
				}
			else if (iLevel2 == EReturn)
				{
				if (aData8.Compare(KTrueValue) == KErrNone)
					{
					INFO_PRINTF1(_L("HandleInstallEvent-DevCert will return ETrue"));
					iResult.DevCertsResult = ETrue;
					}
				else if (aData8.Compare(KFalseValue) == KErrNone)
					{
					INFO_PRINTF1(_L("HandleInstallEvent-DevCert will return EFalse"));
					iResult.DevCertsResult = EFalse;
					}					
				}
			break;
			
		case EHandleCancellableInstallEvent:
			// No return values for these events.
			break;
			
		case EDisplaySecurityWarning:
			if (iLevel2 == EReturn)
				{
				TBool found=EFalse;
				if (aData8.Compare(KTrueValue) == KErrNone)
					{
					iResult.WarningResult = ETrue;
					found=ETrue;
					}
				else if (aData8.Compare(KFalseValue) == KErrNone)
					{
					iResult.WarningResult = EFalse;
					found=ETrue;
					}
					
				if (found && !iSecWarnLogged)
					{
					_LIT(KETrue, "ETrue");
					_LIT(KEFalse, "EFalse");
					INFO_PRINTF2(
						_L("DisplaySecurityWarningL will return '%S'"), 
						iResult.WarningResult ? &KETrue : &KEFalse);
					iSecWarnLogged=ETrue;
					}
				}
			break;
			
		case EDisplayOcspResult:
			if ( iLevel2 == EReturn )
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					iResult.OcspResult = ETrue;
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					iResult.OcspResult = EFalse;
				_LIT(KETrue, "ETrue");
				_LIT(KEFalse, "EFalse");
				INFO_PRINTF2(
					_L("DisplayOcspResultL will return '%S'"), 
					iResult.OcspResult ? &KETrue : &KEFalse);
				}
			break;
		case EDisplayCannotOverwriteFile:
			break;
			
		case EDisplayUninstall:
			if ( iLevel2 == EReturn )
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayUninstallL will return ETrue"));
					iResult.UninstallResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayUninstallL will return EFalse"));
					iResult.UninstallResult = EFalse;
					}
				}
			break;
			
		case EDisplayText:
			if ( iLevel2 == EReturn )
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Input value of the DisplayTextL		  : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.TextResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Input value of the DisplayTextL		  : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.TextResult = EFalse;
					}
				}
			break;
		case EDisplayError:
			break;
		case EDisplayDependencyBreak:
			if ( iLevel2 == EReturn )
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Input value of the DisplayDependencyBreakL    : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.DependencyResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Input value of the DisplayDependencyBreakL    : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.DependencyResult = EFalse;
					}
				}
			break;
		case EDisplayApplicationsInUse:
			if ( iLevel2 == EReturn )
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Input value of the DisplayApplicationsInUseL  : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.ApplicationsInUseResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Input value of the DisplayApplicationsInUseL  : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.ApplicationsInUseResult = EFalse;
					}
				}
			break;
		case EDisplayQuestion:
			if ( iLevel2 == EReturn )
				{
				if ( aData8.Compare(KTrueValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Input value of the DisplayQuestionL		  : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.QuestionResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					/*
					TBuf8<KGenericBufferSize> text;
					text.Format(_L8 ("\015\012The Input value of the DisplayQuestionL		  : "));
					iOutFile.Write(text);
					iOutFile.Write(aData8);
					*/
					iResult.QuestionResult = EFalse;
					}
				}
			break;
		case EDisplayMissingDependency:
			if (iLevel2 == EReturn)
				{
				if (aData8.Compare(KTrueValue)==KErrNone)
					{
					INFO_PRINTF1(_L("DisplayMissingDependencyL will return ETrue"));
					iResult.MissingDependencyResult = ETrue;
					}
				else if ( aData8.Compare(KFalseValue) == KErrNone )
					{
					INFO_PRINTF1(_L("DisplayMissingDependencyL will return EFalse"));
					iResult.MissingDependencyResult = EFalse;
					}
				}
			break;
		default:
			break;
		}
	}

void CRebuildingContentHandler::OnStartPrefixMappingL(const RString& /*aPrefix*/, 
									  const RString& /*aUri*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CRebuildingContentHandler::OnEndPrefixMappingL(const RString& /*aPrefix*/, 
													TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CRebuildingContentHandler::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, 
													  TInt /*aErrorCode*/)
	{
	}

void CRebuildingContentHandler::OnSkippedEntityL(const RString& /*aName*/, 
												 TInt /*aErrorCode*/)
	{
	}

void CRebuildingContentHandler::OnProcessingInstructionL(const TDesC8& /*aTarget8*/, 
									 const TDesC8& /*aData8*/, TInt aErrorCode)
	{
	User::LeaveIfError(aErrorCode);
	}

void CRebuildingContentHandler::OnExtensionL(const RString& /*aData*/, 
										TInt /*aToken*/, TInt /*aErrorCode*/)
	{
	/*
	_LIT(KUnsupportedFeature,"Illegal call to TRebuildingContentHandler:: \
												OnExtensionL - aborting\n");
	test.Panic(KErrNotSupported, KUnsupportedFeature);
	*/
	}

void CRebuildingContentHandler::OnOutOfData()
	{
	//_LIT(KUnexpectedOutOfData,"Unexpected call to OnOutOfData - aborting\n");
	}

void CRebuildingContentHandler::OnError(TInt aError)
	{
	iError = aError;
//	test.Printf(_L("TRebuildingContentHandler::OnError - ERROR: code=%d - \
//									  Aborting parsing process\n"), aError);
	}

TAny* CRebuildingContentHandler::GetExtendedInterface(const TInt32)
	{
	return NULL;
	}
	

TXmlFileData& CRebuildingContentHandler::GetOptions()
	{
	return iResult;
	}

// CUISCriptAdaptor functions
void CUIScriptAdaptor::DialogReached()
	{

	if ((iDialogDelay.Int() != 0) &&
		(iDelayDialogNumber < 0 || iDelayDialogNumber == iCurrentDialogNumber))
			{
			// We need to make sure that (un)installation is not too quick,
			// otherwise the async cancel tests will fail.
			User::After(iDialogDelay);
			}

	if ((iCurrentDialogNumber++ == iCancelDialogNumber) &&
		(iDialogCancelType == ECancelFromDialog ||
		 iDialogCancelType == ECancelAfterDialog))
		{
		iInstallationCancelled=ETrue;
		// Cancel the installation

		if (iDialogCancelType == ECancelFromDialog)
			{
			if(iCancelHandler)
				{
				INFO_PRINTF1(_L("Canceling via CancellableInstallEvent callback"));
				iCancelHandler->HandleCancel();
				}
			else
				{
				INFO_PRINTF1(_L("Cancelling via static Cancel API"));
				Launcher::Cancel();
				}	
			}
		else
			{
			// Cancelling after a dialog is done for the async install
			// and uninstall steps.  Stopping the active scheduler will let
			// CSwisAsyncStep::RunOperation start a cancel delay timer, and
			// the cancel will actually be invoked when the timer expires.
			// The install or uninstall will carry on until this happens.
			INFO_PRINTF1(_L("Calling CActiveScheduler::Stop()"));
			CActiveScheduler::Stop();
			}
		}

	if (iCurrentDialogNumber > iCancelDialogNumber && 
		(iDialogCancelType == ECancelFromDialog ||
		 iDialogCancelType == ECancelAfterDialog))
		{
		ERR_PRINTF3(_L("Reached an unexpected dialog. Dialogs expected: %d, dialog reached: %d"), iCancelDialogNumber, iCurrentDialogNumber);
		}
	}


void CUIScriptAdaptor::DisplayAppInfo(const TDesC& aPreText, const CAppInfo& aAppInfo)
	{
	INFO_PRINTF7(_L("\t%S {\"%S\", \"%S\", V%u.%u.%u}"),
		&aPreText,
		&aAppInfo.AppName(), &aAppInfo.AppVendor(),
		aAppInfo.AppVersion().iMajor,
		aAppInfo.AppVersion().iMinor,
		aAppInfo.AppVersion().iBuild);
	}

void CUIScriptAdaptor::DisplayBoolReturn(TBool aReturn)
	{
	_LIT(KTrue, "True");
	_LIT(KFalse, "False");
	INFO_PRINTF2(_L("\tReturning %S"), aReturn ? &KTrue : &KFalse);
	}

void CUIScriptAdaptor::DisplayIntReturn(TInt aReturn)
	{
	INFO_PRINTF2(_L("\tReturning %d"), aReturn);
	}
	
void CUIScriptAdaptor::CheckFinalProgressBarL()
	{
	if (iProgressBarCurrent != iProgressBarFinal)
		{
		INFO_PRINTF3(_L("\tProgress for installation not finished %d out of %d"), iProgressBarCurrent, iProgressBarFinal);
		User::Leave(KErrAbort);
		}
	}

void CUIScriptAdaptor::CheckExpectedAndCalculatedInstallSizesL(const TDesC& aAppInfoName, TInt64 aSize)
	{		
	TInt i=0;
	TBool foundExpectedInstallSize = EFalse;
	
	RArray<TTableOfInstallSizes> arrayOfExpectedValues = *iExpectedValue;

	while ((!foundExpectedInstallSize) && (i<arrayOfExpectedValues.Count()))
		{
		if (arrayOfExpectedValues[i].iPkgFileName == aAppInfoName)
			{
			if (arrayOfExpectedValues[i].iInstallSize != aSize)
				{
				INFO_PRINTF3(_L("\tThe calculated install size (%D bytes) does not match the expected install size (%D bytes)."),
								I64LOW(aSize), arrayOfExpectedValues[i].iInstallSize);
				User::Leave(KErrAbort);
				}
			else
				{
				INFO_PRINTF2(_L("\tSize (calculated value matches expected value): %D bytes."), I64LOW(aSize));
				foundExpectedInstallSize = ETrue;
				}
			}
		else
			{
			i = i + 1;
			}
		}
	if (!foundExpectedInstallSize)
		{
		INFO_PRINTF2(_L("\tThe calculated install size (%D bytes) does not match any of the expected install sizes."),
						I64LOW(aSize));
		User::Leave(KErrAbort);
		}
	}
	
	
//
// Implementation for the virtual functions for the MUiHandler class. 
//

/**
 * Displays Install dialog and logs all incoming information.
 *
 * @param aAppInfo      Application information.
 * @param aLogo         Optional bitmap logo.
 * @param aCertificates Array of end-entity certificates (can be empty).
 *
 * @return A value from script: \b ETrue to continue, \b EFalse to abort 
 *         installation.
 */
TBool CUIScriptAdaptor::DisplayInstallL(const CAppInfo& aAppInfo,
	const CApaMaskedBitmap* aLogo,
	const RPointerArray<CCertificateInfo>& aCertificates)
	{
	INFO_PRINTF1(_L("DisplayInstallL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	TInt bootMode = 0;
	User::LeaveIfError(RProperty::Get(KUidSystemCategory, KSystemStartupModeKey, bootMode));
	(void) aLogo;
	if (aCertificates.Count())
		{
		INFO_PRINTF2(_L("\tCertificate(s): %u"),aCertificates.Count());
		for (TInt i=0; i<aCertificates.Count(); i++)
			{
			INFO_PRINTF4(_L("\t[%02u] Subj: \"%S\", Issuer: \"%S\""),i,
				&aCertificates[i]->SubjectName(),
				&aCertificates[i]->IssuerName());

			TTime validFrom(aCertificates[i]->ValidFrom());
			TTime validTo(aCertificates[i]->ValidTo());
			TBuf<KMaxLongDateFormatSpec> validFromBuf, validToBuf;
			validFrom.FormatL(validFromBuf, TLongDateFormatSpec());
			validTo.FormatL(validToBuf, TLongDateFormatSpec());
			INFO_PRINTF3(_L("\t     ValidFrom: \"%S\", ValidTo: \"%S\""),
				&validFromBuf, &validToBuf);
			}
		}
	
	LeaveError(KDisplayInstall1);	
	DialogReached();
	DisplayBoolReturn(iResult.InstallResult);
	return iResult.InstallResult;
	}

TBool CUIScriptAdaptor::DisplayGrantCapabilitiesL(const CAppInfo& aAppInfo, 
	const TCapabilitySet& aCapabilitySet)
   {
   iGrantUserCapsDialogCalled=ETrue;
   
   INFO_PRINTF1(_L("DisplayGrantCapabilitiesL():"));
  	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	// Build requested capabilities string.
	const TInt KMaxBufSize=512; // should be enough for all capability names
	const TInt KMaxCapName=64; // should be enough for one capability name
	_LIT(KSpace," ");
	HBufC* buf=HBufC::NewLC(KMaxBufSize);
	TPtr p=buf->Des();
	TInt capsAppended=0;
	TBuf8<KMaxCapName> capName8;
	TBuf<KMaxCapName> capName;

	for (TInt i=0; i<ECapability_HardLimit; i++)
		{
		if (aCapabilitySet.HasCapability(static_cast<TCapability>(i)))
			{
			if (capsAppended)
				p.Append(KSpace);
			capName8=reinterpret_cast<const TUint8*>(CapabilityNames[i]);
			capName.Copy(capName8);
			p.Append(capName);
			capsAppended++;
			}
		}

	INFO_PRINTF2(_L("\tRequesting capabilities: %S"),&p);
	CleanupStack::PopAndDestroy(buf);

	DialogReached();
	DisplayBoolReturn(iResult.CapabilitiesResult);
	return iResult.CapabilitiesResult;
	}


TInt CUIScriptAdaptor::DisplayLanguageL(const CAppInfo& aAppInfo, 
						 const RArray<TLanguage>& aLanguages)
	{
	INFO_PRINTF1(_L("DisplayLanguageL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* languages=HBufC::NewLC(1024);
	TPtr pLanguages(languages->Des());

	for (TInt i=0; i < aLanguages.Count();)
		{
		pLanguages.AppendFormat(_L("%S"), &KLanguageTable[aLanguages[i]]);
		if (++i  >= aLanguages.Count())
			break;
		pLanguages.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tLanguages: %S"), &pLanguages);
	CleanupStack::PopAndDestroy(languages);

	LeaveError(KDisplayLanguage1);
	DialogReached();
	DisplayIntReturn(iResult.LanguagesResult);
	return iResult.LanguagesResult;
	}


TInt CUIScriptAdaptor::DisplayDriveL(const CAppInfo& aAppInfo,
						 TInt64 aSize,const RArray<TChar>& aDriveLetters,
						 const RArray<TInt64>& aDriveSpaces)
	{
	iDisplayDriveDialogCalled = ETrue;
	INFO_PRINTF1(_L("DisplayDriveL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* drives=HBufC::NewLC(1024);
	TPtr pDrives(drives->Des());
    TInt letter;
	for (TInt i=0; i < aDriveLetters.Count();)
		{
        letter = aDriveLetters[i];
		pDrives.AppendFormat(_L("%c"),letter);
		pDrives.AppendFormat(_L(" (%U bytes)"), I64LOW(aDriveSpaces[i]));
		if (++i  >= aDriveLetters.Count())
			break;
		pDrives.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tDrives: %S"), &pDrives);
	CleanupStack::PopAndDestroy(drives);
	
	// If the test case contains information about expected install sizes,
	// then compare them against the calculated install size. Otherwise,
	// just print out the calculated install size.
	if (iExpectedValue != NULL)
		{
		CheckExpectedAndCalculatedInstallSizesL(aAppInfo.AppName(), aSize);
		}
	else
		{
		INFO_PRINTF2(_L("\tSize: %d"), I64LOW(aSize));
		}
	
	LeaveError(KDisplayDrive1);
	DialogReached();

	// Drive result is the drive we want to return so we need to go through the list
	// looking for it
	for (TInt i=0; i < aDriveLetters.Count() ; ++i)
		{
		if (TChar(iResult.DriveResult+'a') == TChar(aDriveLetters[i]).GetLowerCase())
			{
			DisplayIntReturn(i);
			return i;
			}
		}
	
	DisplayIntReturn(-1);
	return -1; // Could not find drive we want, cancelling installation
	}


TBool CUIScriptAdaptor::DisplayUpgradeL(const CAppInfo& aAppInfo,
	const CAppInfo& aExistingAppInfo)
	{
	INFO_PRINTF1(_L("DisplayUpgradeL():"));
	DisplayAppInfo(_L("From App:"), aExistingAppInfo);
	DisplayAppInfo(_L("To App:"), aAppInfo);
	LeaveError(KDisplayUpgrade1);	
	DialogReached();
	DisplayBoolReturn(iResult.UpgradeResult);
	return iResult.UpgradeResult;
	}

TBool CUIScriptAdaptor::DisplayOptionsL(const CAppInfo& aAppInfo,
						  const RPointerArray<TDesC>& aOptions,
						  RArray<TBool>& aSelections)
	{
	INFO_PRINTF1(_L("DisplayOptionsL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* options=HBufC::NewLC(4096);
	TPtr pOptions(options->Des());

	for (TInt i=0; i < aOptions.Count();)
		{
		pOptions.AppendFormat(_L("%S"), aOptions[i]);
		
		if (i >= iResult.OptionSelection.Count())
			{
			pOptions.Append(_L(" (?)"));		
			}
		else if (iResult.OptionSelection[i])
			{
			pOptions.Append(_L(" (*)"));		
			}
		else
			{
			pOptions.Append(_L(" ( )"));		
			}		

		if (++i  >= aOptions.Count())
			break;
		pOptions.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tOptions: %S"), &pOptions);
	CleanupStack::PopAndDestroy(options);

	TInt count = iResult.OptionSelection.Count();
	for (TInt index=0; index < count; index++)
		{
		aSelections[index]=iResult.OptionSelection[index];	
		}
	LeaveError(KDisplayOptions1);
	DialogReached();
	DisplayBoolReturn(iResult.OptionsResult);
	return 	iResult.OptionsResult;
	}

TBool CUIScriptAdaptor::HandleInstallEventL(
						const CAppInfo& aAppInfo,
						TInstallEvent   aEvent, 
						TInt            aValue, 
						const TDesC&    aDes)
	{
	INFO_PRINTF1(_L("HandleInstallEventL"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	TBool ret(ETrue);
	// update progress
	// We shouldn't cancel on the last dialog, since installation/uninstallation has 
	// finished, and the tests will fail due to cancellation being flagged, but the
	// installation/uninstallation completing with no error, as expected.
	switch (aEvent)
		{
	case EEventSetProgressBarFinalValue:
		INFO_PRINTF2(_L("\tEvent:\tEEventSetProgressBarFinalValue %d"), aValue);
		
		// Keep track of progress bar values
		iProgressBarFinal=aValue;
		iProgressBarCurrent=0;
		LeaveError(KInstallEvenDialogSetProgressBar);
		DialogReached();
		ret = iResult.ProgressBarFinalValueResult;
		break;

	case EEventUpdateProgressBar:
		// Keep track of progress bar values
		iProgressBarCurrent+=aValue;
		INFO_PRINTF4(_L("\tEvent:\tEEventUpdateProgressBar %d - %d/%d"), aValue, iProgressBarCurrent, iProgressBarFinal);
		LeaveError(KInstallEvenDialogUpdateProgressBar);
		DialogReached();
		ret = iResult.UpdateProgressBarResult;
 		break;
      
	case EEventAbortedInstall:
		INFO_PRINTF1(_L("\tEvent:\tEEventAbortedInstall"));
		break;

	case EEventCompletedInstall:
		INFO_PRINTF1(_L("\tEvent:\tEEventCompletedInstall"));
		LeaveError(KInstallEvenDialogCompletedInstall);
     	CheckFinalProgressBarL();
		break;

	case EEventOcspCheckEnd:
		INFO_PRINTF1(_L("\tEvent:\tEEventOcspCheckEnd"));
		LeaveError(KInstallEvenDialogOcspCheckEnd);
		DialogReached();
		break;

	case EEventCompletedNoMessage:
		INFO_PRINTF1(_L("\tEvent:\tEEventCompletedNoMessage"));
		break;

	case EEventAbortedUnInstall:
		INFO_PRINTF1(_L("\tEvent:\tEEventAbortedUnInstall"));
		break;

	case EEventCompletedUnInstall:
		INFO_PRINTF1(_L("\tEvent:\tEEventCompletedUnInstall"));
		LeaveError(KInstallEvenDialogCompletedUnInstall);
     	CheckFinalProgressBarL();
		break;

	case EEventLeave:
		INFO_PRINTF2(_L("\tEvent:\tEEventLeave %d"), aValue);
		INFO_PRINTF1(_L("\tEvent:\tEEventCompletedUnInstall"));
		DialogReached();
		break;
	
	case EEventDevCert:
		INFO_PRINTF2(_L("\tEvent:\tEEventDevCert %S"), &aDes);
		LeaveError(KInstallEvenDevCert);
		DialogReached();
		ret=iResult.DevCertsResult;
		break;

	default:
		INFO_PRINTF4(_L("\tResult:\tUnknown (%d) Value: %d Des %S"), aEvent, aValue, &aDes);
		ASSERT(false);
		}

	return ret;
	}

void CUIScriptAdaptor::HandleCancellableInstallEventL(
						const CAppInfo&          aAppInfo,
						TInstallCancellableEvent aEvent, 
						MCancelHandler&          aCancelHandler,
						TInt                     aValue, 
						const TDesC&             aDes)
	{
	iCancelHandler = &aCancelHandler;

	INFO_PRINTF1(_L("HandleCancellableInstallEventL"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	switch (aEvent)
		{
	case EEventRemovingFiles:
		INFO_PRINTF1(_L("\tEvent:\tEEventRemovingFiles"));
		LeaveError(KCancelInstallRemoving);
      break;
	case EEventShuttingDownApps:
		INFO_PRINTF1(_L("\tEvent:\tEEventShuttingDownApps"));
		LeaveError(KCancelInstallShuttingDown);
      break;
	case EEventCopyingFiles:
		INFO_PRINTF1(_L("\tEvent:\tEEventCopyingFiles"));
		LeaveError(KCancelInstallCopyingFiles);
      break;
	case EEventOcspCheckStart:
		INFO_PRINTF1(_L("\tEvent:\tEEventOcspCheckStart"));
		LeaveError(KCancelInstallOcspCheckStart);
      break;
	
	default:
		INFO_PRINTF4(_L("\tEvent:\tUnknown (%d) Value: %d Des %S"), aEvent, aValue, &aDes);
		}
	DialogReached();
	}

TBool CUIScriptAdaptor::DisplaySecurityWarningL(
		const CAppInfo& aAppInfo,
		TSignatureValidationResult aSigValidationResult,
		RPointerArray<CPKIXValidationResultBase>& /*aPkixResults*/,
		RPointerArray<CCertificateInfo>& /*aCertificates*/,
		TBool aInstallAnyway)
	{
	_LIT(KTrue, "True");
	_LIT(KFalse, "False");
	
	INFO_PRINTF1(_L("DisplaySecurityWarningL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	switch (aSigValidationResult)
	{
	case EValidationSucceeded:
		INFO_PRINTF1(_L("\tResult:\tEValidationSucceeded"));
      break;
    case ESignatureSelfSigned:
    	INFO_PRINTF1(_L("\tResult:\tESignatureSelfSigned"));
    	break;
 	case ENoCertificate:
		INFO_PRINTF1(_L("\tResult:\tENoCertificate"));
        break;
	case ECertificateValidationError:
		INFO_PRINTF1(_L("\tResult:\tECertificateValidationError"));
      break;
	case ESignatureNotPresent:
		INFO_PRINTF1(_L("\tResult:\tESignatureNotPresent"));
      break;
	case ESignatureCouldNotBeValidated:
		INFO_PRINTF1(_L("\tResult:\tESignatureCouldNotBeValidated"));
      break;
	case ENoCodeSigningExtension:
		INFO_PRINTF1(_L("\tResult:\tENoCodeSigningExtension"));
      break;
 	case ENoSupportedPolicyExtension:
		INFO_PRINTF1(_L("\tResult:\tENoSupportedPolicyExtension"));
      break;
	case EMandatorySignatureMissing:
		INFO_PRINTF1(_L("\tResult:\tEMandatorySignatureMissing"));
      break;
 	default:
 		INFO_PRINTF2(_L("\tResult:\tUnknown (%d)"), aSigValidationResult);
      break;
 	};
	
	LeaveError(KDisplaySecurityWarning1);	
	DialogReached();
	
	++iSecurityWarningDialogsInvoked;

	INFO_PRINTF3(
		_L("\tReturning %S (override possible: %S)"), 
		iResult.WarningResult ? &KTrue : &KFalse,
		aInstallAnyway ? &KTrue : &KFalse);
	return iResult.WarningResult;
	}

TBool CUIScriptAdaptor::DisplayOcspResultL(const CAppInfo& aAppInfo,
					 TRevocationDialogMessage aMessage, 
					 RPointerArray<TOCSPOutcome>& aOutcomes, 
					 RPointerArray<CCertificateInfo>& aCertificates,
					 TBool aWarningOnly)
	{	
	INFO_PRINTF1(_L("DisplayOcspResultL():"));
	INFO_PRINTF6(_L("\t  App: { \"%S\" \"%S\" V%u.%u.%u }"),&aAppInfo.AppName(),
		&aAppInfo.AppVendor(),aAppInfo.AppVersion().iMajor,
		aAppInfo.AppVersion().iMinor,aAppInfo.AppVersion().iBuild);
	INFO_PRINTF2(_L("\t  Msg: %u"),aMessage);
	TInt numCerts=aCertificates.Count();
	TInt numOutcomes=aOutcomes.Count();
	INFO_PRINTF3(_L("\tCerts: %u, outcomes: %u"),numCerts,numOutcomes);

	if (numCerts != numOutcomes)
 		{
 		ERR_PRINTF1(_L("\tMismatched certs and outcomes arrays!"));
 		User::Leave(KErrArgument);
 		}

	for (TInt i=0; i<numCerts; i++)
      {
        INFO_PRINTF3(_L("\t Cert: [%u] DN = \"%S\""), i,
                     &aCertificates[i]->SubjectName());
      }
    
	iObservedOcspOutcomes.ResetAndDestroy();
 	for (TInt i=0; i<numOutcomes; i++)
 		{
 		INFO_PRINTF4(_L("\t outcome[%u] = {%d, %d}"),
                     i,
                     aOutcomes[i]->iStatus,
                     aOutcomes[i]->iResult);
        TOCSPOutcome* outcome = new(ELeave) TOCSPOutcome(*aOutcomes[i]);
 		CleanupStack::PushL(outcome);
 		iObservedOcspOutcomes.AppendL(outcome);
 		CleanupStack::Pop(outcome);
 		}

	_LIT(KYes,"Yes");
	_LIT(KNo,"No");
	INFO_PRINTF2(_L("\tFatal: %S"), aWarningOnly ? &KNo : &KYes);
	
	if (! aWarningOnly)	
		{
		iFatalOcspResponseEncountered = ETrue;
		}

	LeaveError(KDisplayOcspResult1);	
	DialogReached();
	DisplayBoolReturn(iResult.OcspResult);
	return iResult.OcspResult;
	}

void CUIScriptAdaptor::DisplayCannotOverwriteFileL(const CAppInfo& aAppInfo,
						 const CAppInfo& aInstalledAppInfo,
						 const TDesC& aFileName)
	{
	iCannotOverwriteFileDialogCalled = ETrue;
	iFileOwner = aInstalledAppInfo.AppName().AllocL();
	INFO_PRINTF1(_L("DisplayCannotOverwriteFileL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	INFO_PRINTF2(_L("\tFile : %S"), &aFileName);
	LeaveError(KDisplayCannotOverwriteFile1);	
	DisplayAppInfo(_L("From App:"), aInstalledAppInfo);
	DialogReached();
	}

TBool CUIScriptAdaptor::DisplayUninstallL(const CAppInfo& aAppInfo)
	{
	INFO_PRINTF1(_L("DisplayUninstallL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	LeaveError(KDisplayUninstall1);	
	DialogReached();
	DisplayBoolReturn(iResult.UninstallResult);
	return iResult.UninstallResult;
	}

TBool CUIScriptAdaptor::DisplayTextL(const CAppInfo& aAppInfo, 
						  TFileTextOption aOption, const TDesC& aText)
	{
	INFO_PRINTF1(_L("DisplayTextL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	switch (aOption)
		{
	case EInstFileTextOptionContinue:
		INFO_PRINTF1(_L("\tOption:\tEInstFileTextOptionContinue"));
      break;
	case EInstFileTextOptionSkipOneIfNo:
		INFO_PRINTF1(_L("\tOption:\tEInstFileTextOptionSkipOneIfNo"));
      break;
	case EInstFileTextOptionAbortIfNo:
		INFO_PRINTF1(_L("\tOption:\tEInstFileTextOptionAbortIfNo"));
      break;
	case EInstFileTextOptionExitIfNo:
		INFO_PRINTF1(_L("\tOption:\tEInstFileTextOptionExitIfNo"));
      break;

	default:
		INFO_PRINTF2(_L("Option: Unknown (%d)"), aOption);
		}
		
	INFO_PRINTF2(_L("Text: \"%S\""), &aText);

	// Record the displayed text so it can be compared with the
	// ExpectedDisplayText configuration value later
	iDisplayedText = aText.Left(Min(KMaxDisplayText, aText.Length()));
	iDisplayedText.TrimRight();

	// Record the displayed text (unchanged) so it can be compared with the
	// contents of a text file
	iDisplayedTextActual = HBufC16::NewL(aText.Length());
	*iDisplayedTextActual = aText;

	LeaveError(KDisplayText1);
	DialogReached();
	DisplayBoolReturn(iResult.TextResult);
	return iResult.TextResult;
	}

void CUIScriptAdaptor::DisplayErrorL(const CAppInfo& aAppInfo,
						 TErrorDialog aType, const TDesC& aDes)
	{
	INFO_PRINTF1(_L("DisplayErrorL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	switch (aType)
		{
	case EUiAlreadyInRom:
		INFO_PRINTF2(_L("\tError:\tEUiAlreadyInRom file=%S"), &aDes);
      break;
	case EUiBlockingEclipsingFile:
		INFO_PRINTF2(_L("\tError:\tEUiBlockingEclipsingFile %S"), &aDes);     
      break;
	case EUiMissingDependency:
		INFO_PRINTF1(_L("\tError:\tEUiMissingDependency"));
      break;
	case EUiRequireVer:
		INFO_PRINTF1(_L("\tError:\tEUiRequireVer"));
      break;
	case EUiRequireVerOrGreater:
		INFO_PRINTF1(_L("\tError:\tEUiRequireVerOrGreater"));
      break;
	case EUiFileCorrupt:
		INFO_PRINTF1(_L("\tError:\tEUiFileCorrupt"));
      break;
	case EUiDiskNotPresent:
		INFO_PRINTF1(_L("\tError:\tEUiDiskNotPresent"));
      break;
	case EUiCannotRead:
		INFO_PRINTF1(_L("\tError:\tEUiCannotRead"));
      break;
	case EUiCannotDelete:
		INFO_PRINTF1(_L("\tError:\tEUiCannotDelete"));
      break;
	case EUiInvalidFileName:
		INFO_PRINTF1(_L("\tError:\tEUiInvalidFileName"));
      break;
	case EUiFileNotFound:
		INFO_PRINTF1(_L("\tError:\tEUiFileNotFound"));
      break;
	case EUiInsufficientSpaceOnDrive:
		INFO_PRINTF1(_L("\tError:\tEUiInsufficientSpaceOnDrive"));
      break;
	case EUiCapabilitiesCannotBeGranted:
		INFO_PRINTF2(_L("\tError:\tEUiCapabilitiesCannotBeGranted %S"), &aDes);   	
      break;
	case EUiUnknownFile:
		INFO_PRINTF1(_L("\tError:\tEUiUnknownFile"));
      break;
	case EUiMissingBasePackage:
		INFO_PRINTF1(_L("\tError:\tEUiMissingBasePackage"));
      break;
	case EUiConstraintsExceeded:
		INFO_PRINTF1(_L("\tError:\tEUiConstraintsExceeded"));
      break;
	case EUiSIDViolation:
		INFO_PRINTF1(_L("\tError:\tEUiSIDViolation"));
      break;
	case EUiVIDViolation:
		INFO_PRINTF1(_L("\tError:\tEUiVIDViolation"));
      break;
	case EUiUIDPackageViolation:
		INFO_PRINTF1(_L("\tError:\tEUiUIDPackageViolation"));
      break;
	case EUiSIDMismatch:
		INFO_PRINTF1(_L("\tError:\tEUiSIDMismatch"));
	  break;
	default:
		INFO_PRINTF3(_L("\tError:\tUnknown (%d) Des %S"), aType, &aDes);
	 	};

	LeaveError(KDisplayError1);	
	DialogReached();
	}

TBool CUIScriptAdaptor::DisplayDependencyBreakL(const CAppInfo& aAppInfo,
					      const RPointerArray<TDesC>& aComponents)
	{
	INFO_PRINTF1(_L("DisplayDependencyBreakL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* components=HBufC::NewLC(1024);
	TPtr pComponents(components->Des());

	for (TInt i=0; i < aComponents.Count();)
		{
		pComponents.AppendFormat(_L("%S"), aComponents[i]);
		if (++i  >= aComponents.Count())
			break;
		pComponents.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tComponents: %S"), &pComponents);
	CleanupStack::PopAndDestroy(components);

	LeaveError(KDisplayDependencyBreak1);
	DialogReached();
	DisplayBoolReturn(iResult.DependencyResult);
	return iResult.DependencyResult;
	}

TBool CUIScriptAdaptor::DisplayApplicationsInUseL(const CAppInfo& aAppInfo, 
							const RPointerArray<TDesC>& aAppNames)
	{
	INFO_PRINTF1(_L("DisplayApplicationsInUseL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	HBufC* applications=HBufC::NewLC(1024);
	TPtr pApplications(applications->Des());

	for (TInt i=0; i < aAppNames.Count();)
		{
		pApplications.AppendFormat(_L("%S"), aAppNames[i]);
		if (++i  >= aAppNames.Count())
			break;
		pApplications.Append(_L(", "));
		}
	INFO_PRINTF2(_L("\tComponents: %S"), &pApplications);
	CleanupStack::PopAndDestroy(applications);

	LeaveError(KDisplayApplicationsInUse1);	
	DialogReached();
	DisplayBoolReturn(iResult.ApplicationsInUseResult);
	return iResult.ApplicationsInUseResult;
	}

TBool CUIScriptAdaptor::DisplayQuestionL(const CAppInfo& aAppInfo, 
							TQuestionDialog aQuestion, const TDesC& aDes)
	{
	INFO_PRINTF1(_L("DisplayQuestionL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);
	
	switch (aQuestion)
		{
	case EQuestionIncompatible:
		INFO_PRINTF1(_L("\tQuestion:\tEQuestionIncompatible"));
      break;
	
	case EQuestionOverwriteFile:
		INFO_PRINTF1(_L("\tQuestion:\tEQuestionOverwriteFile"));
		INFO_PRINTF2(_L("\tFile Name:\t%S"), &aDes);
      break;

	default:
		INFO_PRINTF3(_L("\tQuestion:\tUnknown (%d) Des %S"), aQuestion, &aDes);
		}
	
	LeaveError(KDisplayQuestion1);	
	DialogReached();
	DisplayBoolReturn(iResult.QuestionResult);
	return iResult.QuestionResult;
	}


TBool CUIScriptAdaptor::DisplayMissingDependencyL(const CAppInfo& aAppInfo,
	const TDesC& aDependencyName,TVersion aWantedVersionFrom,
	TVersion aWantedVersionTo,TVersion aInstalledVersion)
	{
	INFO_PRINTF1(_L("DisplayMissingDependencyL():"));
	DisplayAppInfo(_L("AppInfo:"), aAppInfo);

	INFO_PRINTF2(_L("\tMissing dependency name: %S"),&aDependencyName);
	INFO_PRINTF7(_L("\t         Wanted version: %u.%u.%u - %u.%u.%u"),
		aWantedVersionFrom.iMajor,aWantedVersionFrom.iMinor,
		aWantedVersionFrom.iBuild,
		aWantedVersionTo.iMajor,aWantedVersionTo.iMinor,
		aWantedVersionTo.iBuild);
	INFO_PRINTF4(_L("\t       Existing version: %u.%u.%u"),
		aInstalledVersion.iMajor,aInstalledVersion.iMinor,
		aInstalledVersion.iBuild);

	LeaveError(KDisplayMissingDependency1);
	DialogReached();
	DisplayBoolReturn(iResult.MissingDependencyResult);
	return iResult.MissingDependencyResult;
	}
void CUIScriptAdaptor::LeaveError(TInt aReason)
	{
		if (iExpectedDialog == aReason)
		{
		User::Leave(iKerrCode);	
		}
	}
