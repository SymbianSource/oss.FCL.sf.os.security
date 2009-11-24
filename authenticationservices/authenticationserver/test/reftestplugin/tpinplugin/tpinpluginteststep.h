/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef TPINPLUGINTESTSTEP_H
#define TPINPLUGINTESTSTEP_H

#include <test/TestExecuteStepBase.h>
#include <authserver/authtypes.h>
#include <pinplugindialogdefs.h>
#include <authserver/authplugininterface.h>
#include "pinplugin.h"

class CPinPluginTestStep : public CTestStep
	{
public:
	/** Constructor */
	CPinPluginTestStep();

	/** Destructor */
	~CPinPluginTestStep();
	
	/**	
	 Override of base class virtual
 	 @return - TVerdict code
  	 */
	virtual TVerdict doTestStepPreambleL();

	/**
 	 Override of base class virtual
 	 @return - TVerdict code
 	 */
	virtual TVerdict doTestStepPostambleL();

	/**
	 Override of base class pure virtual
 	 Demonstrates reading configuration parameters fom an ini file section
 	 @return - TVerdict code
 	 */
	virtual TVerdict doTestStepL();

private:
	TVerdict iVerdict;
	TPtrC iPinProcessType;
	AuthServer::TIdentityId iIdentityId;
	};

class CPinPluginTestActive : public CActive
	{
public:
	static CPinPluginTestActive* NewL(CTestExecuteLogger& aLogger, AuthServer::TIdentityId& aIdentityId, TPtrC& aPinProcessType, CPinPluginTestStep &aTestStep);
	/** Destructor */
	~CPinPluginTestActive();

	TVerdict DoTestPropertyL();
	TVerdict DoTestDefaultDataL();	
	TVerdict DoTestIdentifyL();
	TVerdict DoTestTrainL();
	TVerdict DoTestForgetL();
	TVerdict DoTestReTrainL();
	TVerdict DoTestServerCancelL();
	TVerdict DoTestSequenceL();
	TVerdict DoTestOomL();	
		
	//Active
	void DoCancel();
	void RunL();
	CTestExecuteLogger& Logger(){return iLogger;}
	//Log buffer
	TBuf<150> iLogInfo;
   	CTestExecuteLogger& iLogger;

private:
	/** Constructor */
	CPinPluginTestActive(CTestExecuteLogger& aLogger, AuthServer::TIdentityId& aIdentityId, TPtrC& aPinProcessType, CPinPluginTestStep &aTestStep);
	void ContructL();
	
private:
	void WriteOutputDetailsL();
	void ReadOutputDetailsL(TInt& aDialogNumber, TPinValue& aPinValue, TPinPluginInfoMessage& aDisplayedMsg);
	TBool ReadDetailsFromIni(CPinPluginTestStep& aStep);
	void DeleteFileL();
	TBool VerifyResultL(TInt aOutputVal);
	TBool CheckResultL(TInt aResult);
	TBool TrainIdentifyL();
	TBool ForgetIdentifyL();
	TBool DoTrainL(HBufC8*& aResult);
	TBool DoIdentifyL(HBufC8*& aResult);
	TBool DoForgetL();
	CPinPlugin* LoadPluginL();
	void UnLoadPlugin();
	TVerdict ReturnResultL(TBool aRes);
	void PreferDialogInputL(const TDesC& aExpectedOutput, TPinPluginDialogOperation aOp, 
		TInt aDialogNo);
private:
	
	static TInt KAuthenticationServerStackSize;
	static TInt KAuthenticationServerInitHeapSize;
	static TInt KAuthenticationServerMaxHeapSize;
		
  	AuthServer::CAuthPluginInterface* iPinPlugin;
   	RFs iFs;
	TPinPluginDialogOperation iOperation;
	TInt iIndex;
	TInt iNewPinLength;
	TPinValue iPinValue;
	TPinValue iTempPinValue;
	TPtrC iExpectedOutput;
	TPinPluginDialogResult iCmdResult,iCmdFinalResult;
	TPinPluginInfoMessage iInfoMsg;
	TInt iDialogNumber;
	AuthServer::TIdentityId iIdentityId;
	TPtrC iPinProcessType;	
	TInt iOomTest;
	HBufC* iClientMessage;
    CPinPluginTestStep &iTestStep;
	TUid iDtor_ID_Key;
	};

class CCreateFile : public CTestStep
	{
public:
	/** Constructor */
	CCreateFile();

	/** Destructor */
	~CCreateFile();
	
	/**	
	 Override of base class virtual
 	 @return - TVerdict code
  	 */
	virtual TVerdict doTestStepPreambleL();

	/**
 	 Override of base class virtual
 	 @return - TVerdict code
 	 */
	virtual TVerdict doTestStepPostambleL();

	/**
	 Override of base class pure virtual
 	 Demonstrates reading configuration parameters fom an ini file section
 	 @return - TVerdict code
 	 */
	virtual TVerdict doTestStepL();

private:
	TPinValue GetPinFromOutputFileL();

private:
	TPinPluginDialogOperation iOperation;
	TPinValue iPinValue;
	TInt iIndex;
	TInt iNewPinLength;
	TPinPluginDialogResult iCmdResult;
	TPinPluginDialogResult iCmdFinalResult;
	TInt iDialogNumber;
	
	};

// Strings for the server create test step code
_LIT(KPinPluginTestStep,	"tPinPluginTestStep");
_LIT(KCreateInputFile,	"CreateInputFile");

_LIT(KTrainServerCancel,  	"TrainServerCancel");
_LIT(KReTrainServerCancel,  "ReTrainServerCancel");
_LIT(KIdentifyServerCancel, "IdentifyServerCancel");


#endif /* TPINPLUGINTESTSTEP_H */


