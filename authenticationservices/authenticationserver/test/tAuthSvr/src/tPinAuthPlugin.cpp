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



#include <f32file.h>

#include "tPinAuthPlugin.h"
#include <e32svr.h>

#ifndef __INIPARSER_H__
	#include <cinidata.h>
#endif // __INIPARSER_H__


CTPinAuthPlugin::~CTPinAuthPlugin()
	{
	delete iClientMessage;
	}

CTPinAuthPlugin::CTPinAuthPlugin()
	{	
	// See ConstructL() for initialisation completion.
	}
		
/**
 Safely complete the initialization of the constructed object	
*/
void CTPinAuthPlugin::ConstructL()
	{
	iName.Set(KPluginNamePin);
	iDescription.Set(KPluginDescriptionPin);
	iMinEntropy = KEntropyPin;
	iFalsePositiveRate = KFalsePosPin;
  	iFalseNegativeRate = KFalseNegPin;
  	iClientMessage = HBufC::NewL(2048);
	ReloadAllFromIniFile();
	}
	
/**
  Sets information like iActiveState and iSupportsDefaultData from 
  the TEF ini file to the individual plugin database files
*/	
void CTPinAuthPlugin::ReloadAllFromIniFile()
	{
	// Set the file to be read, based on the Id of the plugin
	
	_LIT(KFileText, "\\tAuth\\tAuthSvr\\testdata\\Pin");
	_LIT(KFileSuffix, "Db.ini");					
	_LIT(KFormatFileName,"%S%x%S");
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> fileText(sysDrive.Name());
	fileText.Append(KFileText);
	iFileToRead.Format(KFormatFileName,&fileText, Id(), &KFileSuffix);

	//Read data from file to get initialisation information
	
	// Open and read in INI file
	// Default path to look for INI file is in 'c:\system\data' 
	// on target filesystem
	
	CIniData* file=NULL;
	TRAPD(r,file=CIniData::NewL(iFileToRead));
	
	//Put file on the cleanupstack if anything else in this function is going to leave, but nothing does.	
	
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &iFileToRead);
		}
	
	else
		{
		// Look for a key under a named section, retrieve text value	
		TPtrC result;		
		TBool ret;	//return value from FindVar functions
		ret=file->FindVar(_L("SectionOne"),_L("iSupportsDefaultData"),result);
		if(!ret)
			{
			RDebug::Print(_L("Key or section not found. iSupportsDefaultData default value used."));
			iSupportsDefaultData = ETrue;	// Default value
			}
		else
			{
			_LIT(KFalse,"false");
			// Create a lower case copy of the data that is read from the file
			TBuf<10> resultLowerCase;			
			resultLowerCase.Copy(result);
			resultLowerCase.LowerCase();
			iSupportsDefaultData = (resultLowerCase.FindF(KFalse) == KErrNotFound);
			}		
				
		// Set the active state of the plugin depending on the contents of the 
		// corresponding file, which has been set as a member variable
		ret =file->FindVar(_L("SectionOne"),_L("activeState"),result);
		if(!ret)
			{
			RDebug::Print(_L("Key or section not found. iActiveState default value of ETRUE used."));			
			iActiveState = ETrue;	// Default value
			}
		else
			{
			_LIT(KFalse,"false");
			// Create a lower case copy of the data that is read from the file
			TBuf<10> resultLowerCase;			
			resultLowerCase.Copy(result);
			resultLowerCase.LowerCase();
			
			// If the string 'false' was not found, we set the value to 'true' -the default value
			iActiveState = (resultLowerCase.FindF(KFalse) == KErrNotFound);
			}
		TRAPD(r,file->WriteToFileL());
		
		if (KErrNone != r)
			{
			RDebug::Print(_L("Error occurred while writing to the file."));
			TBuf<84> initInfoFile(sysDrive.Name());
			initInfoFile.Append(KInitInfoFile);
			RDebug::Print(_L("Filename = %S, KeyName = iSupportsDefaultData,"), &initInfoFile);
			RDebug::Print(_L("value = %S."), &result);
			}
		
		delete file;
		TRAP(r,file=CIniData::NewL(KAuthSvrPolicyFile));
		TPtrC displayMessage;
		ret=file->FindVar(_L("SectionOne"),_L("DisplayMessage"),displayMessage);
		if(displayMessage.Length() == 0)   //Value = 0
			{
			RDebug::Print(_L("DisplayMessage not passed."));
			iClientMessage = iClientMessage->ReAllocL(0);	// Default value
			TPtr ptr(iClientMessage->Des());
			ptr = KNullDesC();
			ret=file->SetValue(_L("SectionOne"),_L("DisplayMessage"),displayMessage);
			TRAP(r,file->WriteToFileL());
			}
		else
			{
			iClientMessage = iClientMessage->ReAllocL(displayMessage.Length());
			TPtr ptr(iClientMessage->Des());
			ptr = displayMessage;
			ret=file->SetValue(_L("SectionOne"),_L("DisplayMessage"),displayMessage);
			TRAP(r,file->WriteToFileL());
			}
		delete file;
		}
			
	}

void CTPinAuthPlugin::ReloadActiveStateFromIniFile() const
	{
	// Set the file to be read, based on the Id of the plugin
	
	_LIT(KFileText, "\\tAuth\\tAuthSvr\\testdata\\Pin");
	_LIT(KFileSuffix, "Db.ini");
	
	_LIT(KFormatFileName,"%S%x%S");
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<64> fileText(sysDrive.Name());
	fileText.Append(KFileText);
	iFileToRead.Format(KFormatFileName,&fileText, Id(), &KFileSuffix);

	//Read data from file to get initialisation information
	
	// Open and read in INI file
	// Default path to look for INI file is in 'c:\system\data' 
	// on target filesystem
	
	CIniData* file=NULL;
	TRAPD(r,file=CIniData::NewL(iFileToRead));
	
	//Put file on the cleanupstack if anything else in this function is going to leave, but nothing does.	
	
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &iFileToRead);
		}
	
	else
		{
		// Look for a key under a named section, retrieve text value	
		TPtrC result;		
		TBool ret=EFalse;	//return value from FindVar functions

		// Set the active state of the plugin depending on the contents of the 
		// corresponding file, which has been set as a member variable
		ret =file->FindVar(_L("SectionOne"),_L("activeState"),result);
		if(!ret)
			{
			RDebug::Print(_L("Key or section not found. iActiveState default value of ETRUE used."));			
			iActiveState = ETrue;	// Default value
			}
		else
			{
			_LIT(KFalse,"false");
			// Create a lower case copy of the data that is read from the file
			TBuf<10> resultLowerCase;
			resultLowerCase.Copy(result);
			resultLowerCase.LowerCase();
			if (resultLowerCase.FindF(KFalse) == KErrNotFound) 
				iActiveState = ETrue;	// The string 'false' was not found, so we set the value to 'true' -the default value
			else
				iActiveState = EFalse;
			}
		TRAPD(r,file->WriteToFileL());
		
		if (KErrNone != r)
			{
			RDebug::Print(_L("Error occurred while writing to the file."));
			TBuf<84> initInfoFile(sysDrive.Name());
			initInfoFile.Append(KInitInfoFile);
			RDebug::Print(_L("Filename = %S, KeyName = activeState,"), &initInfoFile);
			RDebug::Print(_L("value = %S."), &result);
			}
		delete file;
		}
	}


	
//Implement CAuthPluginInterface definitions

/**
 Normally, this would ask the user to input a 4-digit pin and then compare it to 
 the values that this plugin has been trained with. The result should be the 
 user that this PIN corresponds to.
 'aResult' should be the 'hash' of the stored PIN (which corresponds to the freshly entered PIN)
 but in this case, it's returned as it was 'unhashed'.
*/

void CTPinAuthPlugin::Identify(TIdentityId& aId, const TDesC& aClientMessage,
							   HBufC8*& aResult, TRequestStatus& aRequest)
	{
	ASSERT(IsActive());
	
	// We are simulating user input by reading from a file
	// (The data contained in this file has been freshly written by the 'client part in TestExecute')
	
	if(aClientMessage != *ClientMessage())
		{
		RDebug::Print(_L("Display String Mismatch !!!"));	
		}
	else
		{
		RDebug::Print(_L("Display String at the plugin matches the string sent !! "));
		}
	aRequest = KRequestPending;
		
	TBuf8<32> userInput;
	TRAPD(result1, IdReadUserInput(userInput));	//Get the data from the AuthSvrPolicy.ini file
	if (KErrNone != result1)
		{
		aRequest = result1;
		}
	else
		{
		// No errors, carry on					
		//Convert the 8-bit string to a 16-bit string for printing in RDebug
		HBufC* buf = 0;
		buf = HBufC::NewLC(32);
		buf->Des().Copy(userInput);
		_LIT(KMsgDebug1,"Identifying Userinput read from file = %S");    		
		RDebug::Print(KMsgDebug1, buf);
		CleanupStack::PopAndDestroy(buf);	//Memory cleanup. This string was only needed for the above Print function
			
		// Possibly, to allow for the cancellation or quitting, allow that to be the input string
		_LIT8(KCancelText, "Cancel");	
		_LIT8(KQuitText, "Quit");	
		if (userInput == KCancelText)
			{
			TRequestStatus* status  = &aRequest;
			User::RequestComplete(status, KErrAuthServPluginCancelled);
			//aResult will not be updated, as specified
			}
		else if (userInput == KQuitText)
			{
			TRequestStatus* status  = &aRequest;
			User::RequestComplete(status, KErrAuthServPluginQuit);
			//aResult will not be updated, as specified
			}
		else
			{
			//compare with the known set of inputs and see if this is one of them.	
			TInt result2 = CheckForStringPresence(aId, userInput, aRequest);
			if (KErrNone == result2)	//The above function did not leave
				{
				//See if the string was found.
				TInt length = userInput.Length();
				
				aResult = HBufC8::New(length);	
				aResult->Des().Format(_L8("%S"), &userInput);
				
				RDebug::Printf("aResult has been updated. aResult = %S, userInput = %S", 
									aResult, &userInput);
				TRequestStatus* status  = &aRequest;
				User::RequestComplete(status, result2);
			
				//Note: aRequest has been updated by the CheckForStringPresence function				
				}
			else if (KErrNotFound == result2)
				{
				RDebug::Printf("The data that the user input was not found (recognized)"
								"Error = %d", result2);
				
				TRequestStatus* status  = &aRequest;
				aId = KUnknownIdentity;
				User::RequestComplete(status, KErrNone);
				}			
			else
				{
				//Caters for file access errors etc
				RDebug::Printf("Error occurred while checking for the presence of the string"
								"in the list. Error = %d", result2);
				
				TRequestStatus* status  = &aRequest;
				aId = KUnknownIdentity;
				User::RequestComplete(status, KErrNone);
				}
			}
			//End of 'else' where the user did not cancel or quit the 'identify' process
		}		
	}
	
void CTPinAuthPlugin::Cancel()
	{		
	}

void CTPinAuthPlugin::Train(TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest)
	{
	
	//Accept user input and update the list of inputs along with IDs.
	
ASSERT(IsActive());
	
	aRequest = KRequestPending;
	//TInt result = KErrNone;
	
	TBuf8<32> userInput;
	TRAPD(result1, TrainReadUserInput(userInput));
	if (KErrNone != result1)
		{
		_LIT(KMsgDebug2,"Training Userinput read Error: result = %d");    
		RDebug::Print(KMsgDebug2, result1);
		}
	else
		{
		// No errors, carry on		
		//Convert the 8-bit string to a 16-bit string for printing in RDebug
		HBufC* buf = 0;
		TRAPD(err, buf = HBufC::NewL(32));
		
		if(err == KErrNoMemory)
			{
			TRequestStatus* status  = &aRequest;
			User::RequestComplete(status, KErrNoMemory);
			return;
			}
		buf->Des().Copy(userInput);
		_LIT(KMsgDebug3, "--Training Userinput read from file = %S");
		RDebug::Print(KMsgDebug3, buf);
		delete buf;	//Memory cleanup. This string was only needed for the above Print function			
		
		// To allow for the cancellation or quitting, allow that to be the input string
		TRequestStatus aRequest2 = KRequestPending;
		_LIT8(KCancelText, "Cancel");
		_LIT8(KQuitText, "Quit");
		userInput.Trim();		
		if (userInput.CompareF(KCancelText) == 0)
			{ aRequest2 = KErrAuthServPluginCancelled; }		
		else if (userInput.CompareF(KQuitText) == 0)
			{ aRequest2 = KErrAuthServPluginQuit; }
		else
			{
			//compare with the present set of inputs and see if this is one of them.
			// update this list with this new user input, tagged to the aId
			TRAPD(result2, CheckForNewStringPresenceL(aId, userInput, aRequest2));			
			if (KErrNone != result2)
				{
				_LIT(KMsgDebug4, "Training Userinput read Error: results = %d");    
				RDebug::Print(KMsgDebug4, result2);
				}
			else if(KErrNone == aRequest2.Int())
				{
				// No errors, update aResult.
				// First allocate space for it since this variable will always be a 
				// null pointer coming into this function
				
				TRAPD(err1,aResult = HBufC8::NewL(userInput.Size()));	
				if(err1 == KErrNoMemory)
					{
					TRequestStatus* status  = &aRequest;
					RDebug::Printf("User::request complete with kerr no memory");
					User::RequestComplete(status, KErrNoMemory);	
					return;
					}
				*aResult = userInput;
				aRequest2 = KErrNone;
				}			
			else
				{
				_LIT(KMsgDebug5, "Error occurred during training. aRequest.Int() = %d");
				RDebug::Print(KMsgDebug5, aRequest2.Int());
				//aResult is not updated, nor is aId				
				}
			}
		// Complete the asynchronous function
		TRequestStatus* status  = &aRequest;
		User::RequestComplete(status, aRequest2.Int());
		}
	}

TBool CTPinAuthPlugin::IsActive () const
	{
	// There is never any user intervention required
	//return true;
	// As a future development note, this value may be read from a file (or a section in a file)
	
	ReloadActiveStateFromIniFile();
	return iActiveState;
	}
	
TInt CTPinAuthPlugin::Forget(TIdentityId aId)
	{
	// Open the list of userInputs that have been stored by this plugin
	// Then find the one that corresponds to this id
	// Then remove this string from the list and return the correct status value
	
	//Open the file
	TRAPD(result3, FindStringAndRemoveL(aId));
	return result3;
			
	}
	
TInt CTPinAuthPlugin::DefaultData(TIdentityId aId, HBufC8*& aOutputBuf)
	{
	
	// This implementation of the PIN plugin does support default data.
	// There will be cases where i don't want this available. Hence the addition of a 
	// new class member iSupportsDefaultData
	ReloadAllFromIniFile();
	
	TInt result = KErrNotSupported;
	TRequestStatus aRequest2 = KRequestPending;
	
	if (iSupportsDefaultData)	//Class member initialised in the constructor
		{
		TBufC8<16> defaultBuf(KDefaultData);
		//compare with the present set of inputs and see if this is one of them.
		// update this list with this new user input, tagged to the aId
		TRAPD(result2, CheckForNewStringPresenceL(aId, defaultBuf, aRequest2));			
		if (KErrNone != result2)
			{
			_LIT(KMsgDebug4, "Training Userinput read Error: results = %d");    
			RDebug::Print(KMsgDebug4, result2);
			}
		else if(KErrNone == aRequest2.Int())
			{
			// No errors, update aOutputBuf.
			// First allocate space for it since this variable will always be a 
			// null pointer coming into this function				
			
			TRAPD(resAlloc, (aOutputBuf = HBufC8::NewL(KDefaultData().Size())) );
			if (KErrNone != resAlloc)
				{
				_LIT(KMsgAllocFailed2,"Failed to allocate memory for updating aOutputBuf");
				RDebug::Print(KMsgAllocFailed2);
				result = resAlloc;
				}
			else
				{
				*aOutputBuf = KDefaultData;
				result = KErrNone;				
				}
			}			
		else
			{
			_LIT(KMsgDebug5, "Error occurred during training. aRequest2.Int() = %d");
			RDebug::Print(KMsgDebug5, aRequest2.Int());
			//aOutputBuf is not updated, nor is aId				
			}
		}
		
	return result;	
	}

TInt CTPinAuthPlugin::Reset(TIdentityId aId, const TDesC& aRegistrationData, HBufC8*& aResult)
	{
	// To enable testing return KErrNotSupported for one of the knowledge-based incarnations of this class
	if (Id() == 0x11113102)
		{
		return KErrNotSupported;
		}

	// If no registration data is supplied or if this plugin is configured as non-knowledge 
	// type plugin then reset is simply a forget operation else the registration data is 
	// used to retrain the plugin
	if ((aRegistrationData == KNullDesC) || (Id() == 0x11113101) || (Type() == EAuthBiometric) || (Type() == EAuthToken))
		{
		aResult = NULL;
		TRAPD(err, FindStringAndRemoveL(aId));
		return err;
		}

	// For EAuthKnowledge type
	// Convert the 16-bit string to a 8-bit string (pin)
	// Note that no unicode conversion is being done here since a pin cannot be in unicode
	RBuf8 pinBuf;
	TBuf8<32> newPin;
	TRAPD(err, pinBuf.CreateL(32);\
				CleanupClosePushL(pinBuf);\
				pinBuf.Copy(aRegistrationData.Left(32));\
				newPin = pinBuf;\
				CleanupStack::PopAndDestroy(&pinBuf));
	if (err != KErrNone)
		{
		return err;
		}

	// Log a message
	TBuf8<32> userInput;
	_LIT(KMsgDebug3, "Reset with registration data = %S");
	RDebug::Print(KMsgDebug3, &aRegistrationData);

	// Compare with the present set of inputs and see if this is one of them.
	// update this list with this new user input, tagged to the aId
	TRequestStatus request = KRequestPending;
	TRAP(err, CheckForNewStringPresenceL(aId, newPin, request));	
	
	if( KErrNoMemory == err)
		{
		User::LeaveNoMemory();
		}
	if (err != KErrNone)
		{
		RDebug::Print(_L("Reset error = %d"), err);
		return err;
		}
	else if (request.Int() == KErrNone)
		{
		// No errors, update aResult.
		TRAP(err, aResult = HBufC8::NewL(newPin.Size()));
		if (err != KErrNone)
			{
			return err;
			}
		*aResult = newPin;
		}
	else
		{
		_LIT(KMsgDebug5, "Error occurred during reset. request.Int() = %d");
		RDebug::Print(KMsgDebug5, request.Int());
		return request.Int();
		}

	return KErrNone;
	}

//--------------------------------------------------------------------------------

/**
 Utility method to capture the data that the user has input 
 (in the form of data stored in a file)
*/
TInt CTPinAuthPlugin::IdReadUserInput(TBuf8<32>& aInputValue)
	{

	TInt retValue;
	CIniData* file=NULL;
	TRAPD(r,file=CIniData::NewL(iFileToRead));
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &iFileToRead);
		retValue = r;
		}
	
	else
		{
		// Look for a key under a named section, retrieve text value	
		TPtrC result;		
		TBool ret=EFalse;	//return value from FindVar functions
		
		ret=file->FindVar(_L("SectionOne"),_L("IdEnteredPinValue"),result);
		if(!ret)
			{
			RDebug::Print(_L("Unable to find the key in the file %S. IdEnteredPinValue is not known, 9999 used. Error = %d"), &iFileToRead, ret);
			_LIT8(KEnteredPinValueErr, "9999");
			aInputValue = KEnteredPinValueErr;	// Default value
			retValue = KErrNotFound;						
			}
		else
			{
			//Later,include a check to ensure that only digits were entered
			aInputValue.Copy(result);
			aInputValue.Trim();			
			retValue = KErrNone;	//Not necessary
			}
		delete file;	//memory cleanup
		}
	
	return retValue;
	}

TInt CTPinAuthPlugin::TrainReadUserInput(TBuf8<32>& aInputValue)
	{

	TInt retValue;
	CIniData* file=NULL;
	TRAPD(r,file=CIniData::NewL(iFileToRead));
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &iFileToRead);
		retValue = r;
		}
	
	else
		{
		// Look for a key under a named section, retrieve text value	
		TPtrC result;		
		TBool ret=EFalse;	//return value from FindVar functions
		
		ret=file->FindVar(_L("SectionOne"),_L("TrainEnteredPinValue"),result);
		if(!ret)
			{
			RDebug::Print(_L("Unable to find the key in the file %S. TrainEnteredPinValue is not known, 9999 used. Error = %d"), &iFileToRead, ret);
			_LIT8(KEnteredPinValueErr, "9999");
			aInputValue = KEnteredPinValueErr;	// Default value
			retValue = KErrNotFound;						
			}
		else
			{
   			aInputValue.Copy(result);
			aInputValue.Trim();
			retValue = KErrNone;	//Not necessary
			}
		delete file;	//memory cleanup
		}
	
	return retValue;
	}

/**
 Utility method to compare the data that the user has input 
 with all the data that has been stored for users 
 Called by Identify()
 @return - success or failure value
*/
TInt CTPinAuthPlugin::CheckForStringPresence(TIdentityId& aId, TBuf8<32> aInputValue, TRequestStatus& aRequestValue)
	{

	TInt retValue = KErrNone;	
	// First format the aInputValue string so that it starts with a ':' and ends with a ','.
	_LIT8(KFormatValue2, ":%S,");
	TBuf8<32> aInputValue2;
	aInputValue2.Format(KFormatValue2, &aInputValue);

	//Convert the 8-bit string to a 16-bit string for printing in RDebug
	HBufC* buf = 0;
	buf = HBufC::New(32);
	buf->Des().Copy(aInputValue);		
	RDebug::Print(_L("Formatted string: %S"), buf);
	delete buf;	//Memory cleanup. This string was only needed for the above Print function

	TBuf8<500> pinFileContents1, pinFileContents2;
	// Read the contents of the file that contains all the ID/PIN combinations

	CIniData* file=NULL;
	TRAPD(r,file=CIniData::NewL(iFileToRead));
	if(KErrNoMemory == r)
		{
		delete file;
		RDebug::Print(_L("Unable to create CIniData object - No memory !! "));
		User::LeaveNoMemory();
		
		}
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &iFileToRead);
		retValue = r;
		}	
	else
		{
		// Look for a key under a named section, retrieve text value	
		TPtrC result;		
		TBool ret=EFalse;	//return value from FindVar functions
		
		ret=file->FindVar(_L("SectionOne"),_L("Identity&PinValues"),result);
		if(!ret)
			{
			RDebug::Print(_L("Unable to find the Identity&PinValues key in the file %S."), &iFileToRead);
			retValue = KErrNotFound;
			aRequestValue =KErrNotFound;
			}
		else
			{
			pinFileContents1.Copy(result);	
			
			TInt searchResult;
			searchResult = pinFileContents1.Find(aInputValue2);
			
			if (KErrNotFound == searchResult)
				{
				RDebug::Print(_L("Userinput not found in the pinFile %S. Identification error"), &iFileToRead);
				aRequestValue= KErrAuthServIdentityNotFound;
				retValue  = searchResult;
				}
			else if (searchResult)	// a positive value
				{				
				// Now find the userID by searching back through the string for the "," marker				
				pinFileContents2 = pinFileContents1.Left(searchResult);
				TInt userIdStartPos = pinFileContents2.LocateReverse(',');
								 
				//Extract this userId for use by the calling function
				TBuf8<50> p1 = pinFileContents2.Right(searchResult - userIdStartPos - 1);
				TLex8 input (p1);
				TRadix aRadix = EDecimal;
				input.Val(aId, aRadix);
				RDebug::Print(_L("UserId that is stored = %u"), aId);
				aRequestValue = KErrNone;
				retValue = KErrNone;
				}				
			else
				{
				RDebug::Print(_L("Unexpected error in the 'Find' function. Searchresult = %d"), searchResult);
				aRequestValue = searchResult;
				retValue = searchResult;				
				}//End check for key&section search in file				
			}//End check for safe file access
		delete file;
		}
	
	return retValue;
	}

/**
 Utility method to compare the data that the user has input 
 with all the data that has been stored for users 
 Used to find out if the input is unique. 
 -since KIdAmbiguous is no longer used, KIdCancel will be returned if the input is non-unique
 Called by Train() and DefaultData()
*/
TInt CTPinAuthPlugin::CheckForNewStringPresenceL(TIdentityId aId, TBuf8<32> aInputValue, TRequestStatus& aRequestValue)
	{	
	TBuf8<500> pinFileContents, pinFileContents2;
	TInt retValue = KErrNone;

	CIniData* file=NULL;
	TRAPD(r,file=CIniData::NewL(iFileToRead));
	if(KErrNoMemory == r)
		{
		User::LeaveNoMemory();
		}
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &iFileToRead);
		return r;
		}	
	
	
	// Look for a key under a named section, retrieve text value	
	TPtrC result;		
	TBool ret=EFalse;	//return value from FindVar functions
	aRequestValue = KErrAuthServIdentityNotFound;	//Initialisation		
			
	ret=file->FindVar(KPluginIniSection, KPinDbTag,result);
	if(!ret)
		{
		RDebug::Print(_L("Unable to find the %S key in the file %S."), &KPinDbTag, &iFileToRead);
		delete file;
		return KErrNotFound;			
		}
	
	//Copy the data from the named section in the file, and see if any previous user has used this input string
	pinFileContents.Copy(result);			
	
	TInt searchResultPin;
	TBuf8<50> searchStringPin;
	_LIT8(KFormat3, ":%S,");
	searchStringPin.Format(KFormat3, &aInputValue);
	searchResultPin = pinFileContents.Find(searchStringPin);
				
	if (KErrNotFound == searchResultPin)
		{				
		//check that the identity is unique, if so, update the Db 
		// with the new Id-PIN pair, as is done already
		// else update the PIN only, ie training data for that identity
		TInt searchResultId;
		TBuf8<50> searchStringId;
		_LIT8(KFormat4, ",%u:");
		searchStringId.Format(KFormat4, aId);
		
		searchResultId = pinFileContents.Find(searchStringId);
		
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		TBuf<80> policyFile;
		if (KErrNotFound == searchResultId)
			{									
			// Add this entry to the file
			// This is a simple system, expecting a format as below:
			// ",aId:inputPin,aId:inputPin,aId:inputPin,"				
			
			_LIT8(KFormat2, "%S%u:%S,");
			if (pinFileContents.Size() == 0)
				{
				pinFileContents.Append(',');	//Initialisation
				}
			pinFileContents2.Format(KFormat2, &pinFileContents, aId, &aInputValue);				
										
			//For the purpose of writing the data to a file, i'll create a 16-bit version of pinFileContents
			TInt ret2 = 0;
			HBufC* buf = 0;
			TRAPD(err,buf = HBufC::NewL(pinFileContents2.Length()));
				
			if(err == KErrNoMemory)
				{
				delete file;
				User::LeaveNoMemory();
				}
			buf->Des().Copy(pinFileContents2);												
			ret2 = file->SetValue(_L("SectionOne"),_L("Identity&PinValues"), *buf);
			
			TRAPD(r,file->WriteToFileL());
			
						
			if (KErrNone != r)
				{
				policyFile.Copy(sysDrive.Name());
				policyFile.Append(KPolicyFile);
				RDebug::Print(_L("Error occurred while writing to the file. Filename = %S, KeyName = AllUserID&PinValues, value = %S."), &policyFile, buf);
				}
			delete buf;		//Memory cleanup. This string was only needed for the above Print function
			
			if(KErrNone == ret2)
				{				
				aRequestValue = KErrNone;
				
				//Update the global database
				AddToGlobalDb(aId, aInputValue);
				}
			else
				{
				RDebug::Print(_L("Error occurred while writing data to file. Error = %d"), ret2);					
				aRequestValue = ret2;
				}
			
			}

		else
			{
			//The Identity has already been trained with this plugin, 
			// update the PIN (training data)
			
			//extract the string to the right of the end of the aId
			pinFileContents2 = pinFileContents.Mid(searchResultId + searchStringId.Size());
			
			//Find the end of the PIN (Training data)
			_LIT8(KPinEndMarker, ",");
			TBufC8<5> bufPinEndMarker(KPinEndMarker);					
			TInt pinEndPos = pinFileContents2.Find(bufPinEndMarker);
			
			//Replace this with the new PIN
			pinFileContents.Replace((searchResultId + searchStringId.Size()),
									 pinEndPos, aInputValue);	
			
			//For the purpose of writing the data to a file, i'll create a 16-bit version of pinFileContents
			TInt ret2 = 0;
			HBufC* buf = 0;
			TRAPD(err1,buf = HBufC::NewL(pinFileContents.Length()));
			if(err1 == KErrNoMemory)
				{
				delete file;
				User::LeaveNoMemory();
				}
			buf->Des().Copy(pinFileContents);
								
			ret2 = file->SetValue(_L("SectionOne"),_L("Identity&PinValues"), *buf);
			
			TRAPD(r,file->WriteToFileL());
			
			if (KErrNone != r)
				{
				policyFile.Copy(sysDrive.Name());
				policyFile.Append(KPolicyFile);
				RDebug::Print(_L("Error occurred while writing to the file. Filename = %S, KeyName = AllUserID&PinValues, value = %S."), &policyFile, buf);
				}
			delete buf;		//Memory cleanup. This string was only needed for the above Print function
			
			if(KErrNone ==ret2)
				{				
				aRequestValue = KErrNone;
				
				//Update the global database
				AddToGlobalDb(aId, aInputValue);
				}
			else
				{
				RDebug::Print(_L("Error occurred while writing data to file. Error = %d"), ret2);					
				aRequestValue = ret2;
				}
			}
		delete file;
		return ret;
		}
	if (searchResultPin)	// a positive value
		{
		// A user has already tried to train this plugin using the given data.
		// The pinFileContents are not updated, to avoid duplication
		
		aRequestValue = KErrAuthServPluginCancelled;
		delete file;
		return searchResultPin;
		}	
	
	RDebug::Print(_L("Unexpected error in the 'Find' function. SearchresultPin = %d"), searchResultPin);
	retValue = searchResultPin;

	delete file;
		
	return retValue;
	}

	
/**
 Utility function to remove the training data that is associated with a given Id
 Called by the Forget function
 @return - KIdSuccess, KIdUnknown or some error code  
 */ 
TInt CTPinAuthPlugin::FindStringAndRemoveL (TIdentityId aId)
	{
	TBuf8<500> pinFileContents, pinFileContents2;
	TInt retValue = KErrAuthServNoSuchIdentity;		//init
	CIniData* file=NULL;
	//First read the list of userId and PIN combinations from a file
	TRAPD(r,file=CIniData::NewL(iFileToRead));
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &iFileToRead);
		retValue = r;
		}	
	else
		{
		// Look for a key under a named section, retrieve text value	
		TPtrC result;		
		TBool ret=EFalse;	//return value from FindVar functions
				
		ret=file->FindVar(KPluginIniSection, KPinDbTag,result);
		if(!ret)
			{
			RDebug::Print(_L("Config error: Unable to find the %S key in the file %S."), &KPinDbTag, &iFileToRead);
			retValue = KErrNotFound;						
			}
		else
			{
			//Copy the data from the named section in the file, and see if any previous user has used this input string
			pinFileContents.Copy(result);				
			
			//Format the searchstring as defined in the file so that it starts with a ',' and ends with a ':'.
			_LIT8(KFormatValue3, ",%u:");			
			
			TBuf8<32> searchString;
			searchString.Format(KFormatValue3, aId);

			//For the purpose of printing the data to a file, i'll create a 16-bit version of pinFileContents
			HBufC* buf = 0;
			buf = HBufC::NewL(searchString.Length());
			buf->Des().Copy(searchString);
			RDebug::Print(_L("Formatted searchString used in 'FindStringAndRemove(): %S"), buf);
			delete buf;

			TInt userIdStartPos;	//The offset of the searchString within pinFileContents
									//i.e the position of the '.' just before the identityId
			userIdStartPos = pinFileContents.Find(searchString);
			if (KErrNotFound == userIdStartPos)
				{
				RDebug::Print(_L("FindStringAndRemoveL: The specified ID was not found in the pinFile"));
				retValue = KErrAuthServNoSuchIdentity;	//KIdUnknown;
				}
			else
				{
				
				//Find where the string (including PIN) ends
				TLex8 aLex = TLex8(pinFileContents);
				TInt length(0);
				aLex.Inc(userIdStartPos+1);
				while(aLex.Peek() != ',') 
					{
					aLex.Inc();
					length++;
					}
	
				//remove the found (total) string from the list (including the starting ',')
				pinFileContents.Delete(userIdStartPos,length+1);
				
				//Update the file
				//For the purpose of writing the data to a file, i'll create a 16-bit version of pinFileContents
				TInt ret2 = 0;
				HBufC* buf;
				buf = HBufC::NewL(pinFileContents.Length());	
				buf->Des().Copy(pinFileContents);
								
				ret2 = file->SetValue(KPluginIniSection, KPinDbTag, *buf);
				
				TRAPD(r,file->WriteToFileL());
				TDriveUnit sysDrive (RFs::GetSystemDrive());
				TBuf<2> sysDriveName (sysDrive.Name());
				TBuf<84> policyFile;
								
				if (KErrNone != r)
					{
					policyFile.Copy(sysDriveName);
					policyFile.Append(KPolicyFile);
					RDebug::Print(_L("Error occurred while writing to the file. Filename = %S, KeyName = AllUserID&PinValues, value = %S."), &policyFile, buf);
					}
				delete buf;		//Memory cleanup. This string was only needed for the above Print function
				
				if (KErrNone !=ret2)
					{
					policyFile .Copy(sysDriveName);
					policyFile.Append(KAuthSvrPolicyFile);
					RDebug::Print(_L("Error occurred in WriteToFileL(). Error = %d. Filename= %S"), ret2, &policyFile);
					retValue = ret2;
					}
				else
					{
					retValue = KErrNone;		
					}				
				}			
			}//End check for key&section search in file
		delete file;
		}//End check for safe file access
	
	return retValue;
	}


/**
 Utility function to Update the Global Db with the training data 
 that is associated with a given Id. The update will not happen if 
 the identity is already present in this list, i.e. it's been pu in 
 by another plugin
 Called by the CheckForNewStringPresence function
 @return - KErrNone, or some error code
*/
TInt CTPinAuthPlugin::AddToGlobalDb (TIdentityId aId, TBuf8<32> aInputValue)
	{
	
	TBuf8<500> totalDbFileContents, totalDbFileContents2;
	TInt retValue = KErrNone;
	CIniData* file=NULL;
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<128> authSvrPolicyFile(sysDriveName);
	authSvrPolicyFile.Append(KAuthSvrPolicyFile);
	
	TRAPD(r,file=CIniData::NewL(authSvrPolicyFile));	
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &authSvrPolicyFile);
		retValue = r;
		}	
	else
		{
		// Look for a key under a named section, retrieve text value	
		TPtrC result;		
		TBool ret=EFalse;	//return value from FindVar functions		
				
		ret=file->FindVar(KPluginIniSection, KTotalDbTag,result);
		if(!ret)
			{
			RDebug::Print(_L("Unable to find the %S key in the file %S."), &KTotalDbTag, &authSvrPolicyFile);
			retValue = KErrNotFound;		
			}
		else
			{
			//Copy the data from the named section in the file, and see if any previous user has used this input string
			totalDbFileContents.Copy(result);
			TBuf8<20> aidString;
			aidString.Num(aId, EDecimal);			
			TInt searchResult;			
			searchResult = totalDbFileContents.Find(aidString);
			
			if (KErrNotFound == searchResult)
				{
				// Add this entry to the file
				// This is a simple system, expecting a format as below:
				// ",aId:inputPin,aId:inputPin,aId:inputPin,"				
				_LIT8(KFormat2, "%S%u:%S,");
				if (totalDbFileContents.Size() == 0)
					{					
					totalDbFileContents.Append(',');	//Initialisation
					}
				totalDbFileContents2.Format(KFormat2, &totalDbFileContents, aId, &aInputValue);				
					
				//For the purpose of writing the data to a file, i'll create a 16-bit version of pinFileContents
				TInt ret2 = 0;
				HBufC* buf = 0;
				TRAPD(err,buf = HBufC::NewL(totalDbFileContents2.Length()));
				if(err == KErrNoMemory)
					{
					delete file;
					return KErrNoMemory;
					}
				buf->Des().Copy(totalDbFileContents2);								
				ret2 = file->SetValue(KPluginIniSection, KTotalDbTag, *buf);
				
				TRAPD(r,file->WriteToFileL());
									
				if (KErrNone != r)
					{
					TBuf<80> policyFile(sysDriveName);
					policyFile.Append(KPolicyFile);
					RDebug::Print(_L("Error occurred while writing to the file. Filename = %S, KeyName = AllUserID&PinValues, value = %S."), &policyFile, buf);
					}
				delete buf;	//Memory cleanup. This string was only needed for the above Print function
				
				if (KErrNone !=ret2)
					{
					RDebug::Print(_L("Error occurred in SetValue(). Error = %d. Filename= %S"), ret2, &authSvrPolicyFile);
					retValue = ret2;
					}
				}
			else
				{
				// A user has already tried to train this plugin using the given data.
				// The pinFileContents are not updated, to avoid duplication				
				retValue = KErrNone;													
				}//End aidString search in the totalDbFileContents string (Find())
			}//End check for key&section search in file (FindVar())
			delete file;
		}//End check for safe file access
	return retValue;
	}

const HBufC* CTPinAuthPlugin::ClientMessage()
	{
	return iClientMessage;
	}

const TPtrC& CTPinAuthPlugin::Name() const
  {
  return iName;
  }
const TPtrC& CTPinAuthPlugin::Description() const
  {
  return iDescription;
  }
AuthServer::TAuthPluginType CTPinAuthPlugin::Type() const 
  {
  return iType;
  }

AuthServer::TEntropy CTPinAuthPlugin::MinEntropy() const
  {
  return iMinEntropy;
  }

AuthServer::TPercentage CTPinAuthPlugin::FalsePositiveRate() const
  {
  return iFalsePositiveRate;
  }

AuthServer::TPercentage CTPinAuthPlugin::FalseNegativeRate() const
  {
  return iFalseNegativeRate;
  }


