/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CTestStep derived implementation
*
*/

 
#include "tAuthSvrStep.h"


using namespace AuthServer;


CTIdentifyMultiple::CTIdentifyMultiple()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTIdentifyMultiple);
	}

CTIdentifyMultiple::~CTIdentifyMultiple()
/**
 * Destructor
 */
	{
	 TInt i = iPluginIds.Count();
	  while (i)
		{
		 REComSession::DestroyedImplementation(iPluginIds[--i]);
		}
	}
	
TVerdict CTIdentifyMultiple::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	
	//Put in an initial value for the userResult (PIN) that will be used as input during the Identify() function
	TPtrC userResultValue;
	if(GetStringFromConfig(ConfigSection(),_L("userResult"), userResultValue) != EFalse) // the tag 'userResult ' was present
		{
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		TBuf<128> authSvrPolicyFile (sysDrive.Name());
		authSvrPolicyFile.Append(KAuthSvrPolicyFile);
		
		CIniData* policyFile=NULL;
		policyFile=CIniData::NewL(authSvrPolicyFile);
		CleanupStack::PushL(policyFile);
		//Update AuthSvrPolicy.ini, section: SectionOne, value:EnteredPinValue
		writeToNamedConfig(policyFile, _L("SectionOne"), _L("EnteredPinValue"), userResultValue);
		policyFile->WriteToFileL();

		CleanupStack::PopAndDestroy(policyFile);

		}
	return TestStepResult();
	}
	
 
TVerdict CTIdentifyMultiple::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * 
 */
	{	
	//INFO_PRINTF1(_L("In IdentifyMultiple Step"));
	SetTestStepResult(EPass);
	
	//If any test step leaves or panics, the test step thread exits and no further calls are made. 
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

__UHEAP_MARK;		// Check for memory leaks

	//-----------------------------------------------------------------------------------------------------
	//Connect to the AuthServer
	
	
	AuthServer::RAuthClient authClient1;
	TInt connectVal = authClient1.Connect();
	if (KErrNotFound == connectVal)
		{
		//Retry after a delay
		TTimeIntervalMicroSeconds32 timeInterval = 2000;	//2 Milliseconds
		User::After(timeInterval);
		connectVal = authClient1.Connect();
		}
	if (KErrNone != connectVal)
		{
		ERR_PRINTF2(_L("Unable to start a session or other connection error. Err = %d"), connectVal);
		User::LeaveIfError(connectVal);		
		}	
	CleanupClosePushL(authClient1);


/*
	//Call the Enumerate Identities API
	TInt ret = KErrNone;
	AuthServer::RIdentityIdArray idArray;	//Initialisation
	TRAP(ret, authClient1.IdentitiesL(idArray));		// This will need to be updated if the function returns errors like KErrServerTerminated, etc 
			
	//Retrieve the expected information from the ini file
	TInt expNumIdentities = 0;	
	if(GetIntFromConfig(ConfigSection(),_L("NumIdentities"), expNumIdentities) != EFalse) // the tag 'NumIdentities' was present
		{//Do the compare
		if (idArray.Count() != expNumIdentities)
			{
			ERR_PRINTF2(_L("expNumIdentities differs from expected, Number of Identities = %d"), idArray.Count());			
			SetTestStepResult(EFail);
			}
		}
*/
///*
	//Run through the list of plugins and call the Identify function on each
	//Get all implementations using  CAuthPluginInterface::ListImplementationsL()	
	RImplInfoPtrArray infoArray;
	// Note that a special cleanup function is required to reset and destroy
	// all items in the array, and then close it.
	TCleanupItem cleanup(CleanupEComArray, &infoArray);
	CleanupStack::PushL(cleanup);
	REComSession::ListImplementationsL(KCAuthPluginInterfaceUid, infoArray);
	
	TInt numPlugins = infoArray.Count();
	INFO_PRINTF2(_L("Number of plugins: %d"), numPlugins);	
	for (TInt i = 0;  i < numPlugins; i++)
		{
		//Retrieves default data for each
		//TPtrC8 dataType = infoArray[i] ->DataType();
		//TDesC displayName = infoArray[i] ->DisplayName();
		
		TInt data = HexString2Num(infoArray[i]->DataType());
		INFO_PRINTF2(_L("Implementation Id: 0x%x"),infoArray[i]->ImplementationUid().iUid);
		INFO_PRINTF2(_L("Display Name: %S"),&infoArray[i]->DisplayName());
		INFO_PRINTF2(_L("Default Data: %x"),data);
	    //INFO_PRINTF2(_L("Rom Only: %d"),impl[i]->RomOnly());
		//INFO_PRINTF2(_L("Rom Based: %d"),impl[i]->RomBased());
		   
		//....
		}	
	// Clean up
	CleanupStack::PopAndDestroy(&infoArray); //infoArray, results in a call to CleanupEComArray
//*/

	
	//Read a cue from the ini file. A specific one in each section; the expected data is the same as the cue, so this minimizes
	// the number of times the file is read
	TInt fileCuePluginIdValue = 0;
	TPluginId cuePluginIdValue = 0;
	
	//User::SetJustInTime(ETrue);
		
	if(GetHexFromConfig(ConfigSection(),_L("pluginId"), fileCuePluginIdValue) != EFalse) // the tag 'pluginId' was present
		{
		cuePluginIdValue = fileCuePluginIdValue;
			
		CAuthPluginInterface* plugin = 0;
		TEComResolverParams resolverParams;
		TBufC8<16> pluginIdTxt;
			  
		pluginIdTxt.Des().Format(_L8("%x"), cuePluginIdValue);
			  
		resolverParams.SetDataType(pluginIdTxt);
			  
		TAny* tempPlugin = 0; 
		TUid Dtor_ID_Key = TUid::Null();
		tempPlugin = 
		REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
		    							  	Dtor_ID_Key,
											resolverParams,
											KRomOnlyResolverUid);	
		
		plugin = reinterpret_cast<CAuthPluginInterface*>(tempPlugin);
		CleanupStack::PushL(plugin);
		iPluginIds.Append(Dtor_ID_Key);
		//Call the identify function for this plugin
		HBufC8* result1 = 0;

		TRequestStatus reqStatus1 = KErrNone;
		TIdentityId userId = 0;
		
		plugin->Identify(userId, KNullDesC(), result1, reqStatus1);
		CleanupStack::PushL(result1);
		
		//if (reqStatus1.Int() == CAuthPluginInterface::KIdSuccess)
		if (reqStatus1.Int() == KErrNone)
			{
			//Check that this is what was expected		
			TBuf8<32> cueValue;		
			_LIT8(KFormatValue2, "%x");
			cueValue.Format(KFormatValue2, cuePluginIdValue);		
			
			TInt userIdValue = 0;
			//Check to see if there was a value in the testExecute 'ini' file to compare with						
			if(GetHexFromConfig(ConfigSection(),_L("userId"), userIdValue) != EFalse) // the tag 'userId' was present
				{	
				if(userId != userIdValue)
					{
					ERR_PRINTF3(_L("Id value returned differs from expected, Returned userId value= %d, Expected Id value = %d"), userId, userIdValue);
					SetTestStepResult(EFail);				
					}
				}

			TInt userResultValue = 0;				
			if(GetIntFromConfig(ConfigSection(),_L("userResult"), userResultValue) != EFalse) // the tag 'userId' was present
			
				{
				//Convert result1 to an integer
				TPtr8 ptr = result1->Des();		// Convert the HBufC8 to a TPtr8				
				TUint32 result2;
				TLex8 input (ptr);
				TRadix aRadix = EDecimal;
				input.Val(result2, aRadix);
				RDebug::Print(_L("Integer representation of result1 = %d, result1 itself = %S"), result2, result1);
								
				if(result2 != userResultValue)
					{
					_LIT(KErrMsg1, "result value returned differs from expected, Returned userId value= %d, \n Returned data value = %d. Expected data value = %d");						
					ERR_PRINTF4(KErrMsg1, userId, result2, userResultValue);					
					SetTestStepResult(EFail);				
					}
				}	
			//In this case, we're expecting the 'data' returned to be the same as the pluginId value (cueValue)
			else if (result1->Compare(cueValue) != 0)
				{
				TPtr8 ptr = result1->Des();		// Convert the HBufC8 to a TPtr8				
				TUint32 result2;
				TLex8 input (ptr);
				TRadix aRadix = EDecimal;
				input.Val(result2, aRadix);
				
				ERR_PRINTF3(_L("result value returned differs from expected, Returned userId value= %d, Returned data value = %d"), userId, result2);
				SetTestStepResult(EFail);		
				}
			else
				{
				//No errors returned. The process was successful
				SetTestStepResult(EPass);
				}				
			}		
		else
			{
			ERR_PRINTF2(_L("An error occurred in the Identity function. RequestStatus = %d"), reqStatus1.Int());
			SetTestStepResult(EFail);		
			}
		
		CleanupStack::PopAndDestroy(result1);
		CleanupStack::PopAndDestroy(plugin);
		}			
	
	CleanupStack::PopAndDestroy(&authClient1);	// authClient1	
	
	REComSession::FinalClose(); 	//garbage collect the last previously destroyed implementation and close the REComSession if no longer in use
	
 	__UHEAP_MARKEND;
	return TestStepResult();	
	}

