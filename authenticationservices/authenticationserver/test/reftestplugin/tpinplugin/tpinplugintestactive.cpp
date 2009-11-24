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


#include "tpinpluginteststep.h"
#include <authserver/authplugininterface.h>
#include <test/testexecutelog.h>
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <s32file.h>
#include <authserver/auth_srv_errs.h>
#include <authserver/authpatchdata.h>
#include <u32hal.h> 
using namespace AuthServer;

_LIT(KTrainSuccess,   	"TrainingSuccess");
_LIT(KReTrainSuccess, 	"ReTrainingSuccess");
_LIT(KIdentifySuccess,	"IdentifySuccess");
_LIT(KIdentifyFailure,  "IdentifyFailure");

_LIT(KSuccess,   	"Success");
_LIT(KFailure,   	"Failure");
_LIT(KDialogCancel, "Cancel");
_LIT(KDialogQuit, 	"Quit");
_LIT(KServerCancel, "ServerCancel");

_LIT(KInputFile, 	"C:\\t_pinplugindlg_in.dat");
_LIT(KOutputFile, 	"C:\\t_pinplugindlg_out.dat");

const TPluginId KPinPluginImplementationUid = 0x200032E5;
const TUint32 KEntropy = 10000;
const TUint32 KFalsePositiveRate = 0;
const TUint32 KFalseNegativeRate = 0;
const TBool KActive = ETrue;
const TAuthPluginType KPluginType = EAuthKnowledge;

_LIT(KPluginName, "PinPlugin");
_LIT(KPluginDesc, "Pinplugin authenticates device holder using PIN numbers generated from 0-9 digits");

TInt CPinPluginTestActive::KAuthenticationServerStackSize=0x2000;
TInt CPinPluginTestActive::KAuthenticationServerInitHeapSize=0x1000;
TInt CPinPluginTestActive::KAuthenticationServerMaxHeapSize=0x1000000;

CPinPluginTestActive* CPinPluginTestActive::NewL(CTestExecuteLogger& aLogger, AuthServer::TIdentityId& aIdentityId, TPtrC& aPinProcessType, CPinPluginTestStep &aTestStep)
	{
	CPinPluginTestActive* pinPlugin = new (ELeave) CPinPluginTestActive(aLogger, aIdentityId, aPinProcessType, aTestStep);
	pinPlugin->ContructL();
	return pinPlugin;
	}

void CPinPluginTestActive::ContructL()
	{
	User::LeaveIfError(iFs.Connect());
	}

CPinPluginTestActive::CPinPluginTestActive(CTestExecuteLogger& aLogger, TIdentityId& aIdentityId,TPtrC& aPinProcessType, CPinPluginTestStep &aTestStep) 
	:CActive(EPriorityStandard), 
	iLogger(aLogger), 
	iOperation(EPinPluginInfo),
	iIdentityId(aIdentityId),
	iPinProcessType(aPinProcessType),
    iTestStep(aTestStep)
	{
	CActiveScheduler::Add( this );
	}

CPinPluginTestActive::~CPinPluginTestActive()
	{
	Cancel();
	iFs.Close();
	if (iPinPlugin)
		{
		REComSession::DestroyedImplementation(iDtor_ID_Key);
		delete iPinPlugin;
		}
	REComSession::FinalClose();
	}

	
void CPinPluginTestActive::RunL()
	{
        iTestStep.SetTestStepError(iStatus.Int()); 
    	CActiveScheduler::Stop();
	return;
	}
	
TVerdict CPinPluginTestActive::DoTestPropertyL()
	{
	TBool res = ETrue; 

__UHEAP_MARK;		// Check for memory leaks
		
	iPinPlugin = LoadPluginL();
	if (iPinPlugin != NULL)
		{
		if(iPinPlugin->Id() != KPinPluginImplementationUid)
			res = EFalse;
		if(iPinPlugin->Name().CompareF(KPluginName) != 0)
			res = EFalse;
		if(iPinPlugin->Description().CompareF(KPluginDesc) != 0)
			res = EFalse;
		if(iPinPlugin->Type() != KPluginType)
			res = EFalse;
		if(iPinPlugin->MinEntropy() != KEntropy)
			res = EFalse;
		if(iPinPlugin->FalsePositiveRate() != KFalsePositiveRate)
			res = EFalse;
		if(iPinPlugin->FalseNegativeRate() != KFalseNegativeRate)
			res = EFalse;
		if(iPinPlugin->IsActive() != KActive)
			res = EFalse;

		INFO_PRINTF2(_L("ID			 : 0x%x"), iPinPlugin->Id());
	    INFO_PRINTF2(_L("Name		 : %S"), &iPinPlugin->Name());
	    INFO_PRINTF2(_L("Description	 : %S"), &iPinPlugin->Description());
	    INFO_PRINTF2(_L("MinEntropy		 : %d"), iPinPlugin->MinEntropy());
	    INFO_PRINTF2(_L("False Positive Rate : %d"), iPinPlugin->FalsePositiveRate());
	    INFO_PRINTF2(_L("False Negative Rate : %d"), iPinPlugin->FalseNegativeRate());
	    _LIT(KETrue, "ETrue");
		_LIT(KEFalse, "EFalse");
	    INFO_PRINTF2(_L("IsActive   	 : %S"), iPinPlugin->IsActive() ? &KETrue : &KEFalse);
	    switch(iPinPlugin->Type())
	    	{
	    	case EAuthDefault:
	    		INFO_PRINTF1(_L("Type       	 : EAuthDefault"));
	    		break;
	    	case EAuthBiometric:
	    		INFO_PRINTF1(_L("Type       	 : EAuthBiometric"));
	    		break;
	    	case EAuthKnowledge:
	    		INFO_PRINTF1(_L("Type       	 : EAuthKnowledge"));
	    		break;
	    	case EAuthToken:
	    		INFO_PRINTF1(_L("Type       	 : EAuthToken"));
	    		break;
	    	case EAuthPerformance:
	    		INFO_PRINTF1(_L("Type       	 : EAuthPerformance"));
	    		break;
	    	default:
	    		INFO_PRINTF1(_L("Invalid Type"));
	    	}
		}
	else
		{
		iLogInfo.Copy( _L("Cannot Load PinPlugin") );
		return EFail;
		}
	UnLoadPlugin();
	
__UHEAP_MARKEND;

	if (!res)
		{
		INFO_PRINTF1(_L("PinPlugin returns Unexpected Values"));
		return EFail;
		}
	return EPass;
	}

TVerdict CPinPluginTestActive::DoTestDefaultDataL()
	{
	TBool res = ETrue;
	
__UHEAP_MARK;		// Check for memory leaks
		
	iPinPlugin = LoadPluginL();
	if (iPinPlugin != NULL)
		{
		HBufC8* result = NULL;
		TInt err = iPinPlugin->DefaultData(iIdentityId, result);
		if(result != NULL)
			{
			delete result;
			}
		if (err != KErrNone)
			{
			if (err == KErrNoMemory)
				{
				User::Leave(err);
				}
			res = EFalse;
			INFO_PRINTF3(_L("Err :%d while receiving the default data for the identity : %d"), err, iIdentityId);
			}
		else
			{
			_LIT(KPinValue, "11111");
			iPinValue.Copy(KPinValue);
			INFO_PRINTF2(_L("Requesting Identify the Identity : %d with the default pin"), iIdentityId);
			TIdentityId identityId = iIdentityId;
			PreferDialogInputL(KIdentifySuccess, EPinPluginIdentify, 1);
			res = DoIdentifyL(result);
			if(result != NULL)
				{
				INFO_PRINTF2(_L("The Identity : %d is Successfully identified"), iIdentityId);
				delete result;
				}
			else
				{
				INFO_PRINTF2(_L("IdentityId : %d is not identified"), identityId);
				}
			}
		}
	else
		{
		iLogInfo.Copy( _L("Cannot Load PinPlugin") );
		return EFail;
		}
	UnLoadPlugin();
	
__UHEAP_MARKEND;

	return ReturnResultL(res);
	}

TVerdict CPinPluginTestActive::DoTestIdentifyL()
	{ 
	if(!ReadDetailsFromIni(iTestStep))
		{
		return EFail;
		}
	PreferDialogInputL(iExpectedOutput, EPinPluginIdentify, iDialogNumber);
	TBool res = ETrue;

__UHEAP_MARK;		// Check for memory leaks
		
	TIdentityId identityId = iIdentityId;
	iPinPlugin = LoadPluginL();
	if (iPinPlugin != NULL)
		{
		HBufC8* result = NULL;
		INFO_PRINTF2(_L("Requesting Identify the Identity : %d"), iIdentityId);
		res = DoIdentifyL(result);
		if(result != NULL)
			{
			INFO_PRINTF2(_L("The Identity : %d is Successfully identified"), iIdentityId);
			delete result;
			}
		else
			{
			INFO_PRINTF2(_L("IdentityId : %d is not identified"), identityId);
			}
		}
	else
		{
		iLogInfo.Copy( _L("Cannot Load PinPlugin") );
		return EFail;
		}
	UnLoadPlugin();
__UHEAP_MARKEND;
	
	return ReturnResultL(res);
	}
	
TVerdict CPinPluginTestActive::DoTestTrainL()
	{
	if(!ReadDetailsFromIni(iTestStep))
		{
		return EFail;
		}
	PreferDialogInputL(iExpectedOutput, EPinPluginTraining, iDialogNumber);
	TBool res = ETrue;
	
__UHEAP_MARK;		// Check for memory leaks
		
	iPinPlugin = LoadPluginL();
	if (iPinPlugin != NULL)
		{
		INFO_PRINTF2(_L("Requesting Train the Identity : %d"), iIdentityId);
		res = TrainIdentifyL();
		}
	else
		{
		iLogInfo.Copy( _L("Cannot Load PinPlugin") );
		return EFail;
		}
	UnLoadPlugin();
	
__UHEAP_MARKEND;
	
	return ReturnResultL(res);
	}

TVerdict CPinPluginTestActive::DoTestForgetL()
	{
	if(!ReadDetailsFromIni(iTestStep))
		{
		return EFail;
		}

	TBool res = ETrue;

__UHEAP_MARK;		// Check for memory leaks
		
	iPinPlugin = LoadPluginL();
	if (iPinPlugin != NULL)
		{
		ForgetIdentifyL();
		}
	else
		{
		iLogInfo.Copy( _L("Cannot Load PinPlugin") );
		return EFail;
		}
	UnLoadPlugin();
__UHEAP_MARKEND;

	return ReturnResultL(res);
	}

void CPinPluginTestActive::DoCancel() 
	{ 
	iPinPlugin->Cancel();
	}

TVerdict CPinPluginTestActive::DoTestReTrainL()
	{
	if(!ReadDetailsFromIni(iTestStep))
		{
		return EFail;
		}

	TInt dialogno = iDialogNumber;
	TBool DialogCancel = EFalse;
	if (iExpectedOutput.CompareF(KDialogCancel) == 0
		|| iExpectedOutput.CompareF(KDialogQuit) == 0)
		{
		DialogCancel = ETrue;
		}
	PreferDialogInputL(KTrainSuccess, EPinPluginTraining, 1);
	TBool res = ETrue;

__UHEAP_MARK;		// Check for memory leaks
	TIdentityId identityId = iIdentityId;
	iPinPlugin = LoadPluginL();
	if (iPinPlugin != NULL)
		{
		INFO_PRINTF2(_L("Requesting Train the Identity : %d"), iIdentityId);
		res = TrainIdentifyL();
		if (res)
			{
			iTempPinValue.Copy(iPinValue);
			if (DialogCancel)
				{
				PreferDialogInputL(KDialogCancel, EPinPluginTraining, dialogno);
				}
			else
				{
				PreferDialogInputL(KReTrainSuccess, EPinPluginTraining, dialogno);		
				}
			INFO_PRINTF2(_L("Requesting ReTrain the Identity : %d"), iIdentityId);
			res = TrainIdentifyL();
			if (res)
				{
				iPinValue.Copy(iTempPinValue);
				iIdentityId = identityId;
				if (DialogCancel)
					{
					PreferDialogInputL(KIdentifySuccess, EPinPluginIdentify, dialogno);	
					}
				else
					{
					PreferDialogInputL(KIdentifyFailure, EPinPluginIdentify, 3);		
					}		
				HBufC8* result = NULL;
				HBufC* pinVal = HBufC::NewLC(iPinValue.Length());
			   	pinVal->Des().Copy(iPinValue);
				INFO_PRINTF3(_L("Requesting Identify the Identity : %d with the pin : %S"), iIdentityId, pinVal);
				CleanupStack::PopAndDestroy(pinVal);
				res = DoIdentifyL(result);
				if(result != NULL)
					{
					INFO_PRINTF2(_L("The Identity : %d is Successfully identified"), iIdentityId);
					delete result;
					}
				else
					{
					INFO_PRINTF2(_L("IdentityId : %d is not identified"), identityId);
					}
				}
			}
		}
	else
		{
		iLogInfo.Copy( _L("Cannot Load PinPlugin") );
		return EFail;
		}
	UnLoadPlugin();
__UHEAP_MARKEND;
	
	return ReturnResultL(res);
	}
	
TVerdict CPinPluginTestActive::DoTestServerCancelL()
	{
	if(!ReadDetailsFromIni(iTestStep))
		{
		return EFail;
		}
	PreferDialogInputL(iExpectedOutput, EPinPluginTraining, 1);
	
	TBool res = ETrue;
	
__UHEAP_MARK;		// Check for memory leaks
	TIdentityId identityId = iIdentityId;
	iPinPlugin = LoadPluginL();
	if (iPinPlugin != NULL)
		{
		if (iPinProcessType.CompareF(KReTrainServerCancel) == 0)
			{
			INFO_PRINTF2(_L("Requesting Train the Identity : %d"), iIdentityId);
			res = TrainIdentifyL();
			}
		HBufC8* result = NULL;
		if (iPinProcessType.CompareF(KTrainServerCancel) == 0
			|| iPinProcessType.CompareF(KReTrainServerCancel) == 0)
			{
			iTempPinValue.Copy(iPinValue);
			INFO_PRINTF2(_L("Requesting Train the Identity : %d"), iIdentityId);
			iPinPlugin->Train(iIdentityId, result, iStatus);
			SetActive();
			}
		else if (iPinProcessType.CompareF(KIdentifyServerCancel) == 0)
			{
			INFO_PRINTF2(_L("Identify the Identity : %d"), iIdentityId);
			iPinPlugin->Identify(iIdentityId, KNullDesC(), result, iStatus);
			SetActive();
			}

		INFO_PRINTF1(_L("Cancel the process from the Authentication Server"));

		Cancel();
		
		if(iStatus.Int() != -3)
			{
			res = EFalse;
			}
		if (res && iPinProcessType.CompareF(KIdentifyServerCancel) != 0)
			{
			iIdentityId = identityId;
			if (iPinProcessType.CompareF(KTrainServerCancel) == 0)
				{
				_LIT(KPinValue, "123456");
				iPinValue.Copy(KPinValue);
				PreferDialogInputL(KIdentifyFailure, EPinPluginIdentify, 3);	
				}
			else
				{
				iPinValue.Copy(iTempPinValue);
				PreferDialogInputL(KIdentifySuccess, EPinPluginIdentify, 1);		
				}		
			HBufC8* result = NULL;
			HBufC* pinVal = HBufC::NewLC(iPinValue.Length());
		   	pinVal->Des().Copy(iPinValue);
			INFO_PRINTF3(_L("Requesting Identify the Identity : %d with the pin : %S"), iIdentityId, pinVal);
			CleanupStack::PopAndDestroy(pinVal);
			res = DoIdentifyL(result);
			if(result != NULL)
				{
				INFO_PRINTF2(_L("The Identity : %d is Successfully identified"), iIdentityId);
				delete result;
				}
			else
				{
				INFO_PRINTF2(_L("IdentityId : %d is not identified"), identityId);
				}
			}
		}
	else
		{
		iLogInfo.Copy( _L("Cannot Load PinPlugin") );
		return EFail;
		}
	UnLoadPlugin();

__UHEAP_MARKEND;
	
	return ReturnResultL(res);
	}
	
TVerdict CPinPluginTestActive::DoTestSequenceL()
	{
	PreferDialogInputL(KTrainSuccess, EPinPluginTraining, 1);
	
	TBool res = ETrue;
	
__UHEAP_MARK;		// Check for memory leaks
		
	iPinPlugin = LoadPluginL();
	if (iPinPlugin != NULL)
		{
		INFO_PRINTF2(_L("Requesting Train the Identity : %d"), iIdentityId);
		res = TrainIdentifyL();
		if (res)
			{
			PreferDialogInputL(KReTrainSuccess, EPinPluginTraining, 1);
			INFO_PRINTF2(_L("Requesting ReTrain the Identity : %d"), iIdentityId);
			HBufC8* result = NULL;
			res = DoTrainL(result);
			if (result != NULL)
				{
				delete result;
				}
			if (res)
				{
				TIdentityId id = iIdentityId;
				res = ForgetIdentifyL();
				if(res)
					{
					for (TInt i = 0; i <= 2; i++)
						{
						iIdentityId = id;
						INFO_PRINTF2(_L("Requesting Train the Identity : %d"), iIdentityId);
						if (i==2)
							{
							PreferDialogInputL(KTrainSuccess, EPinPluginTraining, 1);
							}
						else
							{
							iInfoMsg = EPinPluginTrainingSuccess;
							PreferDialogInputL(KDialogCancel, EPinPluginTraining, 1);
							}
						res = TrainIdentifyL();
						if (!res)
							{
							break;
							}
						}
					}
				}
			}
		}
	else
		{
		iLogInfo.Copy( _L("Cannot Load PinPlugin") );
		return EFail;
		}
	UnLoadPlugin();

__UHEAP_MARKEND;
	
	return ReturnResultL(res);
	}
	
TVerdict CPinPluginTestActive::DoTestOomL()
	{
 	if(!iTestStep.GetIntFromConfig(iTestStep.ConfigSection(), _L("oomtest"), iOomTest))
   		{
   		INFO_PRINTF1(_L("Test about which function is needed"));
   		return EFail;
   		}
	__UHEAP_MARK;
		TIdentityId id = 1234;
		HBufC8* result = NULL;
	switch(iOomTest)
		{
		case 1:
			{
			iPinPlugin = LoadPluginL();
			UnLoadPlugin();
			}
			break;
		case 2:
			{
			iPinPlugin = LoadPluginL();
			TInt err = iPinPlugin->DefaultData(id, result);
			CleanupStack::PushL(result);
			if (err == KErrNoMemory)
				{
				User::Leave(err);
				}
			err = iPinPlugin->Forget(id);
			if (err == KErrNoMemory)
				{
				User::Leave(err);
				}
			CleanupStack::PopAndDestroy(result);
			UnLoadPlugin();
			}
			break;
		case 3:
			{
			iPinPlugin = LoadPluginL();
			iStatus = KRequestPending;
			_LIT(KPinValue, "11111");
			iPinValue.Copy(KPinValue);
			PreferDialogInputL(KIdentifySuccess, EPinPluginIdentify, 1);
			iPinPlugin->Identify(id, KNullDesC(), result, iStatus);
			CleanupStack::PushL(result);
			SetActive();
			CActiveScheduler::Start();
			CleanupStack::PopAndDestroy(result);
			UnLoadPlugin();
			}
			break;
		case 4:
			{
			iPinPlugin = LoadPluginL();
			iStatus = KRequestPending;
			PreferDialogInputL(KTrainSuccess, EPinPluginTraining, 1);
			iPinPlugin->Train(id, result, iStatus);
			SetActive();
			CActiveScheduler::Start();
			if (result != NULL)
				{
				delete result;
				}
			TInt err = iPinPlugin->Forget(id);
			if (err == KErrNoMemory)
				{
				User::Leave(err);
				}
			UnLoadPlugin();
			}
			break;
		case 5:
			{
			iPinPlugin = LoadPluginL();
			iStatus = KRequestPending;
			PreferDialogInputL(KTrainSuccess, EPinPluginTraining, 1);
			iPinPlugin->Train(id, result, iStatus);
			SetActive();
			CActiveScheduler::Start();
			if (result != NULL)
				{
				delete result;
				}
			UnLoadPlugin();
			}
			break;
		case 6:
			{
			iPinPlugin = LoadPluginL();
			TInt err = iPinPlugin->Forget(id);
			if (err == KErrNoMemory)
				{
				User::Leave(err);
				}
			UnLoadPlugin();
			}
			break;
		}

	__UHEAP_MARKEND;
 	return EPass;
	}

CPinPlugin* CPinPluginTestActive::LoadPluginL()
	{
	 TEComResolverParams resolverParams;
	  TBufC8<16> pluginIdTxt;
	  
	  pluginIdTxt.Des().Format(_L8("%x"), KPinPluginImplementationUid);
	  pluginIdTxt.Des().UpperCase();
	  resolverParams.SetDataType(pluginIdTxt);
	  
	  //To load plugins from sources other than ROM the patch 
	  // data KEnablePostMarketAuthenticationPlugins must be set to True.
	  TUint32 enablePostMarketPlugin = KEnablePostMarketAuthenticationPlugins;

	#ifdef __WINS__

	  // Default SymbianOS behavior is to only load auth plugins from ROM.
	  enablePostMarketPlugin = 0;

	  // For the emulator allow the constant to be patched via epoc.ini
	  UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
	  (TAny*)"KEnablePostMarketAuthenticationPlugins", &enablePostMarketPlugin); // read emulator property (if present)

	#endif

	  TAny* plugin = 0;
	  TInt err = 0;
	  
	  if(enablePostMarketPlugin == 0) 
	  	{
	  	TRAP(err, plugin = 
		REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
											iDtor_ID_Key,
											resolverParams,
											KRomOnlyResolverUid));
	  	}
	  
	  else
	  	{
	  	TRAP(err, plugin = 
		REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
											iDtor_ID_Key,
											resolverParams));
	  	}
	  
	 if (err == KErrNotFound)
	    {
	    err = KErrAuthServNoSuchPlugin;  
	    }
	  User::LeaveIfError(err);
	    
	  return reinterpret_cast<CPinPlugin*>(plugin);
	
	}

void CPinPluginTestActive::UnLoadPlugin()
	{
	delete iPinPlugin;
	iPinPlugin = NULL;
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	REComSession::FinalClose();
	}
	
TVerdict CPinPluginTestActive::ReturnResultL(TBool aRes)
	{
	TVerdict result = EPass;
	DeleteFileL();
	if (!aRes)
		{
		result = EFail;
		}
	return result;
	}
	
TBool CPinPluginTestActive::TrainIdentifyL()
	{
	TBool res = ETrue;

	HBufC8* result = NULL;
	res = DoTrainL(result);
	if (res)
		{
		HBufC* pinVal = HBufC::NewLC(iPinValue.Length());
	   	pinVal->Des().Copy(iPinValue);
		INFO_PRINTF3(_L("Identify the Identity : %d using pin : %S"), iIdentityId, pinVal);
		HBufC8* result1 = NULL;
		TIdentityId identityId = iIdentityId;
		res = DoIdentifyL(result1);
		if(res && result1 != NULL)
			{
			if (result->Des().CompareF(result1->Des()) != 0)
				{
				INFO_PRINTF1( _L("Trained result is different from identification result"));
				res = EFalse;
				}
			else
				{
				INFO_PRINTF3(_L("The Identity : %d is Successfully identified with pin : %S"), iIdentityId, pinVal);
				}
			delete result1;
			}
		else
			{
			INFO_PRINTF2(_L("The Identity %d is not trained"), identityId);
			}
		CleanupStack::PopAndDestroy(pinVal);
		}
	if(result != NULL)
		{
		delete result;
		}
		
	return res;
	}

TBool CPinPluginTestActive::ForgetIdentifyL()
	{
	INFO_PRINTF2(_L("Requesting Forget the Identity : %d"), iIdentityId);
	TInt err = iPinPlugin->Forget(iIdentityId);
	
	TBool res = CheckResultL(err);
	if (res)
		{
		INFO_PRINTF2(_L("IdentityId :%d is successfully removed"), iIdentityId);
		INFO_PRINTF1(_L("Verify using Identify Process"));	
		HBufC8* result = NULL;
		res = DoIdentifyL(result);
		if(res && result != NULL)
			{
			INFO_PRINTF2(_L("The Identity : %d is Successfully identified"), iIdentityId);
			delete result;
			}
		}
	return res;	
	}
	
TBool CPinPluginTestActive::DoTrainL(HBufC8*& aResult)
	{
	TBool res = ETrue;
	iStatus = KRequestPending;
	
	iPinPlugin->Train(iIdentityId, aResult, iStatus);
	SetActive();
	CActiveScheduler::Start();
		
	res = VerifyResultL(iStatus.Int());
	return res;
	}
	
TBool CPinPluginTestActive::DoIdentifyL(HBufC8*& aResult)
	{
	TBool res = ETrue;
		
	iPinPlugin->Identify(iIdentityId, KNullDesC(), aResult, iStatus);
	if(!IsActive())
		SetActive();
	CActiveScheduler::Start();
	
	res = VerifyResultL(iStatus.Int());
	return res;
	}
	
TBool CPinPluginTestActive::VerifyResultL(TInt aOutputVal)
	{
	TBool result = ETrue;
	
	if ((iExpectedOutput.CompareF(KDialogCancel) == 0)
		&& (aOutputVal != KErrAuthServPluginCancelled))
		{
		INFO_PRINTF1(_L("RequestStatus is not completed with expected result"));
		result = EFalse;
		}
	else if ((iExpectedOutput.CompareF(KDialogQuit) == 0)
		&& (aOutputVal != KErrAuthServPluginQuit))
		{
		INFO_PRINTF1(_L("RequestStatus is not completed with expected result"));
		result = EFalse;
		}
	else if ((iExpectedOutput.CompareF(KTrainSuccess) == 0)
		&& (aOutputVal == KErrAuthServRegistrationFailed))
		{
		INFO_PRINTF2(_L("Authserver registration failed. Returned error code = %d"), aOutputVal);
		result = EFalse;
		}	
	else if ((iExpectedOutput.CompareF(KDialogCancel) != 0)
			&& (iExpectedOutput.CompareF(KDialogQuit) != 0)
			&& (iExpectedOutput.CompareF(KServerCancel) != 0)
			&& (aOutputVal != KErrNone))
		{
		INFO_PRINTF1(_L("RequestStatus is not completed with expected result"));
		result = EFalse;
		}
	if (result && aOutputVal == KErrNone)
		{
		TInt dialogNumber;
		TPinValue pinValue;
		TPinPluginInfoMessage displayedMsg;
		ReadOutputDetailsL(dialogNumber, pinValue, displayedMsg);
		TPtrC message;
		switch(displayedMsg)
	    	{
	    	case EPinPluginTrainingSuccess:
	    		message.Set(_L("EPinPluginTrainingSuccess"));
	    		break;
	    	case EPinPluginReTrainingSuccess:
	    		message.Set(_L("EPinPluginReTrainingSuccess"));
	    		break;
	    	case EPinPluginIdentificationSuccess:
	    		message.Set(_L("EPinPluginIdentificationSuccess"));	  
	    		break;
	    	case EPinPluginIdentificationFailure:
	    		message.Set(_L("EPinPluginIdentificationFailure"));
	    		break;
	    	}
		if(displayedMsg != iInfoMsg)
			{
			INFO_PRINTF2(_L("INFO :%S which is not expected"), &message);
			result = EFalse;
			}
		if (iCmdResult == EOk && dialogNumber != iDialogNumber)
			{
			INFO_PRINTF3(_L("Number of dialog displayed : %d differ from expected dialog: %d."), dialogNumber,iDialogNumber);
			result = EFalse;
			}
		if (displayedMsg == EPinPluginIdentificationFailure
			&& dialogNumber == 0)
			{
			INFO_PRINTF2(_L("INFO :%S because IdentityId is not available in the DB"), &message);
			}
		if (displayedMsg == EPinPluginIdentificationFailure
			&& dialogNumber == 3)
			{
			INFO_PRINTF2(_L("INFO :%S because wrong pin input for all retries and IdentityId exists in the DB"), &message);
			}
		if (displayedMsg == EPinPluginIdentificationFailure
			&& iIdentityId != KUnknownIdentity)
			{
			INFO_PRINTF2(_L("IdentityId was not reset for %S"), &message);
			}
		if (displayedMsg != EPinPluginIdentificationSuccess
			&& displayedMsg != EPinPluginIdentificationFailure)
			{
			iPinValue.Copy(pinValue);
			if (iInfoMsg == EPinPluginTrainingSuccess 
				|| iInfoMsg == EPinPluginReTrainingSuccess)
				{
				HBufC* pinVal = HBufC::NewLC(pinValue.Length());
			   	pinVal->Des().Copy(pinValue);
				INFO_PRINTF3(_L("The Identity %d is trained with pin number %S"), iIdentityId, pinVal);
				CleanupStack::PopAndDestroy(pinVal);
				PreferDialogInputL(KIdentifySuccess, EPinPluginIdentify, 1);
				}
			else
				{
				PreferDialogInputL(KIdentifyFailure, EPinPluginIdentify, 0);
				}
			}
		}
	if(aOutputVal != KErrNone)
		{
		if (aOutputVal == KErrAuthServPluginCancelled)
			{
			INFO_PRINTF1(_L("Cancel the process from the Dialog Notifier"));	
			}
		else if (aOutputVal == KErrAuthServPluginQuit)
			{
			INFO_PRINTF1(_L("Quit the process from the Dialog Notifier"));	
			}
		CheckResultL(aOutputVal);
		}
	return result;
	}
	
TBool CPinPluginTestActive::CheckResultL(TInt aResult)
	{
	_LIT(KPinValue, "123456");
	if (iExpectedOutput.CompareF(KSuccess) == 0
		&& aResult == KErrAuthServNoSuchIdentity)
		{
		INFO_PRINTF2(_L("IdentityId : %d is not identified when should be"), iIdentityId);
		return EFalse;
		}
	else if(iExpectedOutput.CompareF(KFailure) == 0
		&& aResult == KErrNone)
		{
		INFO_PRINTF2(_L(" IdentityId : %d is identified when should not be"), iIdentityId);
		return EFalse;
		}
	iPinValue.Copy(KPinValue);
	PreferDialogInputL(KIdentifyFailure, EPinPluginIdentify, 3);
	return ETrue;
	}
	
TBool CPinPluginTestActive::ReadDetailsFromIni(CPinPluginTestStep& aStep)
	{
	TPtrC pinVal;
	if(!aStep.GetStringFromConfig(aStep.ConfigSection(), _L("expectedoutput"), iExpectedOutput))
		{
		INFO_PRINTF1(_L("expectedoutput value is missing"));
		return EFalse;
		}
	// Optional input, if the input is not available by default the iIndex is set to 0.
	if(!aStep.GetIntFromConfig(aStep.ConfigSection(), _L("selectindex"), iIndex))
		{
		iIndex = 0;
		}
	// optional input, if not pinplugin use the default pinlength from the config file.
	aStep.GetIntFromConfig(aStep.ConfigSection(), _L("newpinlength"), iNewPinLength);

	// optional input, 
	aStep.GetStringFromConfig(aStep.ConfigSection(), _L("pinvalue"), pinVal);
	iPinValue.Copy(pinVal);

	// Optional input, if the input is not available by default the resultant output cmd will given
	// in the dialog No 1.
	if(!aStep.GetIntFromConfig(aStep.ConfigSection(), _L("outputatdialog"), iDialogNumber))
		{
		if (iExpectedOutput.CompareF(KIdentifyFailure) == 0)
			{
			iDialogNumber = 3;
			}
		else
			{
			iDialogNumber = 1;
			}
		}
	return ETrue;
	}
	
void CPinPluginTestActive::PreferDialogInputL(const TDesC& aExpectedOutput,
			TPinPluginDialogOperation aOp, TInt aDialogNo)
	{
	// iPinValue, iIndex, iNewPinLength is not changing for every operation. if
	// any changes occurs , we have to reset just before calling this function.
	iOperation = aOp;
	iDialogNumber = aDialogNo;
	iExpectedOutput.Set(aExpectedOutput);
		
	if (aExpectedOutput.CompareF(KTrainSuccess) == 0)
		{
		iInfoMsg = EPinPluginTrainingSuccess;
		}
	else if (aExpectedOutput.CompareF(KReTrainSuccess) == 0)
		{
		iInfoMsg = EPinPluginReTrainingSuccess;
		}
	else if (aExpectedOutput.CompareF(KIdentifySuccess) == 0)
		{
		iInfoMsg = EPinPluginIdentificationSuccess;
		}
	else if (aExpectedOutput.CompareF(KIdentifyFailure) == 0)
		{
		iInfoMsg = EPinPluginIdentificationFailure;
		}

	if (aExpectedOutput.CompareF(KDialogCancel) == 0)
		{
		iCmdResult = iCmdFinalResult = ECancel;
		}
	else if (aExpectedOutput.CompareF(KDialogQuit) == 0)
		{
		iCmdResult = iCmdFinalResult = EQuit;
		}
	else 
		{
		iCmdResult = iCmdFinalResult = EOk;
		}
		
	if (iDialogNumber > 1 && 
	   	aExpectedOutput.CompareF(KIdentifyFailure) != 0 &&
		aExpectedOutput.CompareF(KIdentifySuccess) != 0)
		{
		iCmdResult = ENext;
		}
	DeleteFileL();
	WriteOutputDetailsL();
	}
	
void CPinPluginTestActive::WriteOutputDetailsL()
	{
	RFileWriteStream stream;
	TInt err = stream.Create(iFs, KInputFile, EFileWrite | EFileShareExclusive);
	User::LeaveIfError(err);
	stream.PushL();

	MStreamBuf* streamBuf = stream.Sink();
	streamBuf->SeekL(MStreamBuf::EWrite, EStreamEnd);
	stream.WriteInt32L(iOperation);
	stream.WriteInt32L(iPinValue.Length());
	stream.WriteL(iPinValue);
	stream.WriteInt32L(iIndex);
	stream.WriteInt32L(iNewPinLength);
	stream.WriteInt32L(iCmdResult);
	iOperation = EPinPluginInfo;
	stream.WriteInt32L(iOperation);
	stream.WriteInt32L(iCmdFinalResult);
	stream.WriteInt32L(iDialogNumber);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	}

void CPinPluginTestActive::ReadOutputDetailsL(TInt& aDialogNumber, TPinValue& aPinValue, TPinPluginInfoMessage& aDisplayedMsg)
	{
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(iFs, KOutputFile, EFileRead | EFileShareExclusive));
	stream.PushL();

	MStreamBuf* streamBuf = stream.Source();
	TPinValue pinvalue;
	TInt pinValueSize;
	
	aDialogNumber = stream.ReadInt32L();
	
	pinValueSize = stream.ReadInt32L();
	HBufC8* pinValueBuf = HBufC8::NewMaxLC(pinValueSize);
	TPtr8 pinValuePtr(pinValueBuf->Des());
	stream.ReadL(pinValuePtr, pinValueSize);
	aPinValue.Copy(pinValuePtr);
	CleanupStack::PopAndDestroy(1, pinValueBuf);	

	aDisplayedMsg = static_cast<TPinPluginInfoMessage>(stream.ReadInt32L());
	
	CleanupStack::PopAndDestroy(&stream); 
	}
	
void CPinPluginTestActive::DeleteFileL()
	{
	CFileMan* fileMan = CFileMan::NewL(iFs);
	CleanupStack::PushL(fileMan);
	TInt err = fileMan->Delete(KInputFile);
	if ( err != KErrNotFound && err != KErrNone )
		{
		User::LeaveIfError(err);
		}
	err = fileMan->Delete(KOutputFile);
	if (err != KErrNotFound && err != KErrNone )
		{
		User::LeaveIfError(err);
		}
	CleanupStack::PopAndDestroy(fileMan);	
	}





