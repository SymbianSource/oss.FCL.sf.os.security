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
* CTestStep derived implementation
*
*/


#include "tAuthSvrStep.h"
#include <test/testexecutelog.h>

#ifndef __INIPARSER_H__
	#include <cinidata.h>
#endif // __INIPARSER_H__

#include <s32file.h>
#include <f32file.h>
#include "authrepository.h"
using namespace AuthServer;

CTAuthSvrStepBase::CTAuthSvrStepBase()
	{
	}

TInt CTAuthSvrStepBase::writeToNamedConfig(CIniData* aFile, const TDesC& aSectName,const TDesC& aKeyName,const TPtrC& aResult)
	{
	TInt retValue = KErrNone;
	// Look for a key under a named section, retrieve text value	
		
	TInt ret = KErrNone;	//return value from FindVar functions
	ret=aFile->SetValue(aSectName, aKeyName ,aResult);
	if(ret != KErrNone)
		{			
		if(ret == KErrNotFound)
			{
			RDebug::Print(_L("Key not found. Unable to set value in ini file.  SectionName = %S, KeyName = %S, value = %S."),  &aSectName, &aKeyName, &aResult);
			}
		else
			{
			RDebug::Print(_L("Unable to set value in ini file. SectionName = %S, KeyName = %S, value = %S."), &aSectName, &aKeyName, &aResult);
			}							
		retValue = ret;
		}
	return retValue;	
	}

TInt CTAuthSvrStepBase::writeToNamedConfig(CIniData* aFile, const TDesC& aKeyName,const TPtrC& aResult)
	{
	
	TInt retValue = KErrNone;
	// Look for a key under no named section, retrieve text value	
	
	TInt ret = KErrNone;	//return value from FindVar functions
	ret=aFile->SetValue(aKeyName ,aResult);
	if(ret != KErrNone)
		{
		if(ret == KErrNotFound)
			{
			RDebug::Print(_L("Key not found. Unable to set value in ini file. KeyName = %S, value = %S."), &aKeyName, &aResult);
			}			
		else
			{
			RDebug::Print(_L("Unable to set value in ini file. KeyName = %S, value = %S."), &aKeyName, &aResult);
			}
		retValue = ret;
		}
	return retValue;	
	}
	
TInt CTAuthSvrStepBase::readFromNamedConfigL(const TDesC& aFileName, const TDesC& aSectName,const TDesC& aKeyName, HBufC*& aResult)
	{
	TInt retValue = KErrNone;	
	CIniData* file;
	file = CIniData::NewL(aFileName);	
	CleanupStack::PushL(file);
	
	// Look for a key under a named section, retrieve text value	
	TBool ret=EFalse;	//return value from FindVar functions	
	TPtrC resultPtr;
	
	ret=file->FindVar(aSectName, aKeyName ,resultPtr);	
	if(ret == EFalse)
		{		
		RDebug::Print(_L("Unable to read value in ini file. Filename: %S, SectionName = %S, value = %S."), &aFileName, &aSectName, &aResult);
		retValue = ret;
		}
	else
		{
		aResult = resultPtr.AllocL();
		CleanupStack::PushL(aResult);
		file->WriteToFileL(); //apply the implementation
		CleanupStack::Pop(aResult);
		}
	CleanupStack::PopAndDestroy(file);

	return retValue;
	
	}
	
TInt CTAuthSvrStepBase::readFromNamedConfigL(const TDesC& aFileName,const TDesC& aKeyName, HBufC*& aResult)
	{
	TInt retValue = KErrNone;	
	CIniData* file=NULL;
	file = CIniData::NewL(aFileName);	
	CleanupStack::PushL(file);

	TBool ret=EFalse;	//return value from FindVar functions
	
	TPtrC resultPtr;
	ret=file->FindVar(aKeyName, resultPtr);
	if(ret == EFalse)
		{
	    RDebug::Print(_L("Unable to read value in ini file. Filename: %S, SectionName = %S, value = %S."), &aFileName, &aKeyName, &aResult);
		retValue = ret;
		}
	else
		{			
		aResult = resultPtr.AllocL();
		CleanupStack::PushL(aResult);
		file->WriteToFileL(); //apply the implementation
		CleanupStack::Pop(aResult);
		}	
	CleanupStack::PopAndDestroy(file);

	return retValue;
	}



void CTAuthSvrStepBase::SetDefaultPluginIdL(TInt aDefault)
	{
	//Read the default plugin value from the configuration file.
	CRepository* repository = CRepository::NewL(KUidAuthServerRepository);
	CleanupStack::PushL(repository);
		
	User::LeaveIfError(repository->Set(KAuthDefaultPlugin, aDefault));
	
	//Retrieve from file
	TInt defaultPlugin(0);
	User::LeaveIfError(repository->Get(KAuthDefaultPlugin, defaultPlugin));
	INFO_PRINTF2(_L("Default plugin value set in cenrep to %d"),defaultPlugin);
	
	CleanupStack::PopAndDestroy(repository);
		
	}	
/**
 Read data from testexecute ini file that will be used to initialise the Authentication Server
*/
TInt CTAuthSvrStepBase::InitAuthServerFromFileL()
	{

	//Set a default plugin using an ini file. Read this value from a file if it is specified	
	//Set this to be the default plugin (update the policy file) if the test case specifies it (in the script's ini file).
	
	TPtrC defaultPluginFromFile;
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<128> fileName (sysDriveName);
	fileName.Append(KAuthSvrPolicyFile);

	CIniData* policyFile=NULL;
	TRAPD(r,policyFile=CIniData::NewL(fileName));
	CleanupStack::PushL(policyFile);

	fileName.Copy(sysDriveName);
	fileName.Append(KPluginIniFile);
	
	CIniData* pluginFile=NULL;
	TRAP(r,pluginFile=CIniData::NewL(fileName));
	CleanupStack::PushL(pluginFile);

	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3100File);
	
	CIniData* p3100File=NULL;
	TRAP(r,p3100File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3100File);

	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3101File);
	
	CIniData* p3101File=NULL;
	TRAP(r,p3101File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3101File);

	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3102File);
	CIniData* p3102File=NULL;
	TRAP(r,p3102File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3102File);

	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3103File);

	CIniData* p3103File=NULL;
	TRAP(r,p3103File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3103File);

	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3104File);
	
	CIniData* p3104File=NULL;
	TRAP(r,p3104File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3104File);
	
	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin4100File);
	
	CIniData* p4100File=NULL;
	TRAP(r,p4100File=CIniData::NewL(fileName));
	CleanupStack::PushL(p4100File);
	
	if (GetStringFromConfig(ConfigSection(),_L("defaultPlugin"), defaultPluginFromFile) != EFalse) // the tag 'defaultPlugin' was present
		{ 
		//Write this value to the policy file
		writeToNamedConfig(policyFile, KDefaultPluginTag, defaultPluginFromFile);
	//	SetDefaultPluginIdL(defaultPluginFromFile);
		}
	TInt defaultPluginId(0);
	if (GetIntFromConfig(ConfigSection(),_L("defaultPluginId"), defaultPluginId) != EFalse) // the tag 'defaultPlugin' was present
		{ 
		//Write this value to the policy file
		//writeToNamedConfig(policyFile, KDefaultPluginTag, defaultPluginFromFile);
		SetDefaultPluginIdL(defaultPluginId);
		}
	//Set whether a plugin supports default data. Read this value from a file if it is specified
	_LIT(KTrueText,"true");
	_LIT(KFalseText,"false");
	
	TBool iSupportsDefaultDataFromFile(ETrue);	//Default case - the plugin supports default data
	
	TBufC<16> trueBuf1(KTrueText);
	TBufC<16> falseBuf1(KFalseText);
	TPtr valueToWrite = trueBuf1.Des();			//Default case - the plugin supports default data
	if (GetBoolFromConfig(ConfigSection(),_L("iSupportsDefaultData"), iSupportsDefaultDataFromFile) != EFalse) // the tag 'isTrusted' was present
		{
		//Set the value that will be picked up by the constructor of the plugin		
		if (iSupportsDefaultDataFromFile)
			{ valueToWrite = trueBuf1.Des();	}
		else
			{ valueToWrite = falseBuf1.Des();	}			
		writeToNamedConfig(pluginFile, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);	
		writeToNamedConfig(p3100File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p3101File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p3102File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p3103File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p4100File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		}
	else
		{
		INFO_PRINTF1(_L("The iSupportsDefaultData tag was not specified in the testexecute ini file."));
		//Use the default value of TRUE		
		valueToWrite = trueBuf1.Des();
		writeToNamedConfig(pluginFile, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p3100File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p3101File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p3102File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p3103File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		writeToNamedConfig(p4100File, KPluginIniSection, _L("iSupportsDefaultData"), valueToWrite);
		}
	
	//Set a value for the training input that will be used by the CTPinAuthPlugin
	//in the file AuthSvrPolicy.ini
	TPtrC userPinInputFromFile;
	TPtrC supportsDefaultFromFile;
	TPtrC activeStateFromFile;

	if (GetStringFromConfig(ConfigSection(),_L("Pin11113100InputId"), userPinInputFromFile) != EFalse) // the tag 'Pin11113100Input' was present
		{ writeToNamedConfig(p3100File, KIdEnteredPinTag, userPinInputFromFile); }	
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113101InputId"), userPinInputFromFile) != EFalse) // the tag 'Pin11113101Input' was present
		{ writeToNamedConfig(p3101File, KIdEnteredPinTag, userPinInputFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113102InputId"), userPinInputFromFile) != EFalse) // the tag 'Pin11113102Input' was present
		{ writeToNamedConfig(p3102File, KIdEnteredPinTag, userPinInputFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113103InputId"), userPinInputFromFile) != EFalse) // the tag 'Pin11113103Input' was present
		{ writeToNamedConfig(p3103File, KIdEnteredPinTag, userPinInputFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113104InputId"), userPinInputFromFile) != EFalse) // the tag 'Pin11113104Input' was present
		{ writeToNamedConfig(p3104File, KIdEnteredPinTag, userPinInputFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin10234100InputId"), userPinInputFromFile) != EFalse) // the tag 'Pin10234100Input' was present
		{ writeToNamedConfig(p4100File, KIdEnteredPinTag, userPinInputFromFile); }
	
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113100InputTrain"), userPinInputFromFile) != EFalse) // the tag 'Pin11113100Input' was present
		{ writeToNamedConfig(p3100File, KTrainEnteredPinTag, userPinInputFromFile); }	
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113101InputTrain"), userPinInputFromFile) != EFalse) // the tag 'Pin11113101Input' was present
		{ writeToNamedConfig(p3101File, KTrainEnteredPinTag, userPinInputFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113102InputTrain"), userPinInputFromFile) != EFalse) // the tag 'Pin11113102Input' was present
		{ writeToNamedConfig(p3102File, KTrainEnteredPinTag, userPinInputFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113103InputTrain"), userPinInputFromFile) != EFalse) // the tag 'Pin11113103Input' was present
		{ writeToNamedConfig(p3103File, KTrainEnteredPinTag, userPinInputFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113104InputTrain"), userPinInputFromFile) != EFalse) // the tag 'Pin11113104Input' was present
		{ writeToNamedConfig(p3104File, KTrainEnteredPinTag, userPinInputFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin10234100InputTrain"), userPinInputFromFile) != EFalse) // the tag 'Pin10234100Input' was present
		{ writeToNamedConfig(p4100File, KTrainEnteredPinTag, userPinInputFromFile); }
		
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113100Default"), supportsDefaultFromFile) != EFalse) // the tag 'Pin11113102Default' was present
		{ writeToNamedConfig(p3100File, KSupportsDefaultTag, supportsDefaultFromFile); }	
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113101Default"), supportsDefaultFromFile) != EFalse) // the tag 'Pin11113101Default' was present
		{ writeToNamedConfig(p3101File, KSupportsDefaultTag, supportsDefaultFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113102Default"), supportsDefaultFromFile) != EFalse) // the tag 'Pin11113102Default' was present
		{ writeToNamedConfig(p3102File, KSupportsDefaultTag, supportsDefaultFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113104Default"), supportsDefaultFromFile) != EFalse) // the tag 'Pin11113104Default' was present
		{ writeToNamedConfig(p3104File, KSupportsDefaultTag, supportsDefaultFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin10234100Default"), supportsDefaultFromFile) != EFalse) // the tag 'Pin10234100Default' was present
		{ writeToNamedConfig(p4100File, KSupportsDefaultTag, supportsDefaultFromFile); }

	if (GetStringFromConfig(ConfigSection(),_L("Pin11113103Default"), supportsDefaultFromFile) != EFalse) // the tag 'Pin11113103Default' was present
		{ writeToNamedConfig(p3103File, KSupportsDefaultTag, supportsDefaultFromFile); }
	else 
		{ writeToNamedConfig(p3103File, KSupportsDefaultTag, falseBuf1.Des()); }	

	
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113100Active"), activeStateFromFile) != EFalse) // the tag 'Pin11113100Active' was present
		{ writeToNamedConfig(p3100File, KActiveStateTag, activeStateFromFile); }	
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113101Active"), activeStateFromFile) != EFalse) // the tag 'Pin11113101Active' was present
		{ writeToNamedConfig(p3101File, KActiveStateTag, activeStateFromFile); }
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113102Active"), activeStateFromFile) != EFalse) // the tag 'Pin11113102Active' was present
		{ writeToNamedConfig(p3102File, KActiveStateTag, activeStateFromFile); }	
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113104Active"), activeStateFromFile) != EFalse) // the tag 'Pin11113104Active' was present
		{ writeToNamedConfig(p3104File, KActiveStateTag, activeStateFromFile); }	
	if (GetStringFromConfig(ConfigSection(),_L("Pin10234100Active"), activeStateFromFile) != EFalse) // the tag 'Pin10234100Active' was present
		{ writeToNamedConfig(p4100File, KActiveStateTag, activeStateFromFile); }	
		
	if (GetStringFromConfig(ConfigSection(),_L("Pin11113103Active"), activeStateFromFile) != EFalse) // the tag 'Pin11113103Active' was present
		{ writeToNamedConfig(p3103File, KActiveStateTag, activeStateFromFile); }	
	else 
		{ writeToNamedConfig(p3103File, KActiveStateTag, falseBuf1.Des()); }	
	
	// ... Other PinPluginDb's can be added
	//Else the data is set in the 'Defaultdata() function'

	policyFile->WriteToFileL();
	pluginFile->WriteToFileL();
	p3100File->WriteToFileL();
	p3101File->WriteToFileL();
	p3102File->WriteToFileL();
	p3103File->WriteToFileL();
	p3104File->WriteToFileL();
	p4100File->WriteToFileL();

	CleanupStack::PopAndDestroy(8,policyFile);
	return KErrNone;
	}

/**
 Read plugin input train data from testexecute ini file , when aInitTrainInput
 is set to true, or plugin input id when set to false.This data will be used to 
 initialise the Authentication Server.
  
*/	
TInt CTAuthSvrStepBase::InitPluginDataFromFileL(TBool aInitTrainInput)
	{
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<128> fileName (sysDriveName);
	fileName.Append(KPlugin3100File);
	
	CIniData* p3100File = NULL;
	TRAPD(r,p3100File = CIniData::NewL(fileName));
	if(r != KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &fileName);
		return r;
		}
		
	CleanupStack::PushL(p3100File);
	
	TPtrC userPinInputFromFile;
	
	if(aInitTrainInput)
		{
		if (GetStringFromConfig(ConfigSection(),_L("PinInputTrain"), userPinInputFromFile) != EFalse) // the tag 'Pin11113100Input' was present
			{ writeToNamedConfig(p3100File, KTrainEnteredPinTag, userPinInputFromFile); }
	
		}
		
	else
		{
		if (GetStringFromConfig(ConfigSection(),_L("PinInputRetrain"), userPinInputFromFile) != EFalse) // the tag 'Pin11113100Input' was present
			{ writeToNamedConfig(p3100File, KIdEnteredPinTag, userPinInputFromFile); }
		
		}
		
	p3100File->WriteToFileL();
	CleanupStack::PopAndDestroy(p3100File);
	return KErrNone;
	}

/**
 Wait for the server to shut down and so release the authentication database.
*/
void CTAuthSvrStepBase::WaitForServerToReleaseDb()
	{
	TFullName name;
	for (;;)
		{
		TFindServer find(::KAuthServerName);
		if (find.Next(name) != KErrNone)
			{
			break;
			}

		// Wait for server to terminate
		User::After(1 * 1000 * 1000);
		}
	}
	
/**
 	Remove any existing database file.
*/
void CTAuthSvrStepBase::RemoveExistingDbL()
	{
	WaitForServerToReleaseDb();

	TInt r = KErrNone;
	
	TBuf<80> dbName(AuthServer::KDbName);
	dbName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());				
	while ((r = iFs.Delete(dbName)) == KErrInUse)
		{
		User::After(50000);
		}
	if ((KErrPathNotFound == r) || (KErrNotFound == r))  //-12 or -1
		{
		INFO_PRINTF1(_L("The database file was absent when I tried to delete it. \
			Ignoring..."));
		r = KErrNone;	//Not Important
		}
		User::LeaveIfError(r);

	
		
	TBuf<80> pinDbName(_L("!:\\private\\102740FC\\plugin.store"));
	pinDbName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());				
	while ((r = iFs.Delete(pinDbName)) == KErrInUse)
		{
		User::After(50000);
		}
	
	if ((KErrPathNotFound == r) || (KErrNotFound == r))  //-12 or -1
		{
		INFO_PRINTF1(_L("The pin plugin database file was absent when I tried to delete it. \
					Ignoring..."));
		r = KErrNone;	//Not Important
		}
	User::LeaveIfError(r);
			
	TBuf<80> pinDbName1(_L("!:\\t_pinplugindlg_in.dat"));
	pinDbName1[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());				
	while ((r = iFs.Delete(pinDbName1)) == KErrInUse)
		{
	User::After(50000);
		}
			
	if ((KErrPathNotFound == r) || (KErrNotFound == r))  //-12 or -1
		{
		INFO_PRINTF1(_L("The pin plugin database file (t_pinplugindlg_in.dat)was absent when I tried to delete it. \
						Ignoring..."));
		r = KErrNone;	//Not Important
	}
	User::LeaveIfError(r);
		
			
	TBuf<80> pinDbName2(_L("!:\\t_pinplugindlg_out.dat"));
	pinDbName2[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());				
	while ((r = iFs.Delete(pinDbName2)) == KErrInUse)
		{
		User::After(50000);
		}
				
	if ((KErrPathNotFound == r) || (KErrNotFound == r))  //-12 or -1
		{
		INFO_PRINTF1(_L("The pin plugin database file (t_pinplugindlg_out.dat)was absent when I tried to delete it. \
						Ignoring..."));
		r = KErrNone;	//Not Important
		}
	User::LeaveIfError(r);
		
	
	//Reset the data 'AllUserID&PinValues' in AuthSvrPolicy.ini & the 
	// individual plugin Db files
	TBufC<16> initPinDbBuffer(KInitPinDatabaseValue);
	TPtr valueToWrite = initPinDbBuffer.Des();
	
	TBufC<16> initEnteredPinBuffer(KDefaultInput);
	TPtr defaultInputValue = initEnteredPinBuffer.Des();
	
	TBuf<2> noIdentityVal;
	_LIT16(KFormatLastAuth,"%u");
	
	noIdentityVal.Format(KFormatLastAuth, KErrNone); //generates:

	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<128> fileName (sysDriveName);
	fileName.Append(KAuthSvrPolicyFile);
	
	CIniData* policyFile=NULL;
	TRAP(r,policyFile=CIniData::NewL(fileName));
	CleanupStack::PushL(policyFile);
	
	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3100File);
	
	CIniData* p3100File=NULL;
	TRAP(r,p3100File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3100File);
	
	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3101File);
	
	CIniData* p3101File=NULL;
	TRAP(r,p3101File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3101File);
	
	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3102File);
	
	CIniData* p3102File=NULL;
	TRAP(r,p3102File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3102File);
	
	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3103File);
	
	CIniData* p3103File=NULL;
	TRAP(r,p3103File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3103File);
	
	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3104File);

	CIniData* p3104File=NULL;
	TRAP(r,p3104File=CIniData::NewL(fileName));
	CleanupStack::PushL(p3104File);

	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin4100File);

	CIniData* p4100File=NULL;
	TRAP(r,p4100File=CIniData::NewL(fileName));
	CleanupStack::PushL(p4100File);
	
	writeToNamedConfig(policyFile, KLastAuthIdTag, noIdentityVal);
	writeToNamedConfig(policyFile, KTotalDbTag, valueToWrite);
	writeToNamedConfig(p3100File, KPinDbTag, valueToWrite); 
	writeToNamedConfig(p3101File, KPinDbTag, valueToWrite); 
	writeToNamedConfig(p3102File, KPinDbTag, valueToWrite); 
	writeToNamedConfig(p3103File, KPinDbTag, valueToWrite); 
	writeToNamedConfig(p3104File, KPinDbTag, valueToWrite);
	writeToNamedConfig(p4100File, KPinDbTag, valueToWrite);
	writeToNamedConfig(p3100File, KIdEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3101File, KIdEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3102File, KIdEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3103File, KIdEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3104File, KIdEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p4100File, KIdEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3100File, KTrainEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3101File, KTrainEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3102File, KTrainEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3103File, KTrainEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p3104File, KTrainEnteredPinTag, defaultInputValue);
	writeToNamedConfig(p4100File, KTrainEnteredPinTag, defaultInputValue);
	
	policyFile->WriteToFileL();
	p3100File->WriteToFileL();
	p3101File->WriteToFileL();
	p3102File->WriteToFileL();
	p3103File->WriteToFileL();
	p3104File->WriteToFileL();
	p4100File->WriteToFileL();

	CleanupStack::PopAndDestroy(7,policyFile);
	}
	
/**
 	Utility function coverting HexString to Integer
*/
TInt CTAuthSvrStepBase::HexString2Num(const TDesC8& aStr)
  {
  	TInt data = 0;
	TInt char2hex = 0;
	TInt c = 0;
	for (TInt i = 0 ; i < aStr.Length() ; ++i)
	  {
	  c = aStr[i];
	  
	  if (c < 'A')
	    {
	    char2hex='0';
	    }
	  else if (c < 'a')
	    {
	    char2hex='A';
	    }
	  else
	    {
	    char2hex='a';
	    }
	  data = data << 4;      
	  data += aStr[i]-char2hex; 
	  }
	return data;
  }


/**
 Utility function to Update the Global Db by removing the training data 
 that is associated with a given Id. The removal will not happen if 
 the identity is not present in this list, however, this is not expected
 Called at the same time as RAuthMgrClient::RemoveIdentityL(aId)
 Returns KErrNone, or some error code
*/
TInt CTAuthSvrStepBase::RemoveIdFromGlobalDbL (TUint32 aId)
	{
	
	TBuf8<500> totalDbFileContents, totalDbFileContents2;
	TInt retValue = KErrNone;
	CIniData* file=NULL;
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<128> fileName (sysDriveName);
	fileName.Append(KAuthSvrPolicyFile);
	
	TRAPD(r,file=CIniData::NewL(fileName));	
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &fileName);
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
			RDebug::Print(_L("Unable to find the %S key in the file %S."), &KTotalDbTag, &fileName);
			retValue = KErrNotFound;		
			}
		else
			{
			//Copy the data from the named section in the file, and see if any previous user has used this input string
			totalDbFileContents.Copy(result);
			
			//Format the searchstring as defined in the file so that it starts with a ',' and ends with a ':'.
			_LIT8(KFormatValue3, ",%d:");
			TBuf8<20> searchString;
			searchString.Format(KFormatValue3, aId);
			TInt searchResult;			
			searchResult = totalDbFileContents.Find(searchString);
			
			if (searchResult)	// the tag was found
			{				
				//Extract the rest of the pinFileContents string before the start of the PIN (before the ':')
				totalDbFileContents2 = totalDbFileContents.Left(searchResult);
				
				//The userId starts at the first ',' from the end of pinFileContents2, 
				// where p2 is the portion of p1 to the left of searchResult
				TInt userIdStartPos = totalDbFileContents2.LocateReverse(',');
				
				//Find where the PIN ends
				TInt PinEndPos = searchResult + searchString.Length() - 1;
								
				//remove the found (total) string from the list
				totalDbFileContents.Delete(userIdStartPos, (PinEndPos - userIdStartPos) );
				
				//Update the file
				//For the purpose of writing the data to a file, i'll create a 16-bit version of pinFileContents
					TInt ret2 = 0;
					HBufC* buf;
					buf = HBufC::NewL(500);	
					buf->Des().Copy(totalDbFileContents);
								
				ret2 = file->SetValue(KPluginIniSection, KTotalDbTag, *buf);
				
				TRAPD(r,file->WriteToFileL());
				if (KErrNone != r)
					{
					RDebug::Print(_L("Error occurred while writing to the file. Filename = %S, KeyName = AllUserID&PinValues, value = %S."), &fileName, buf);
					}
				delete buf;		//Memory cleanup. This string was only needed for the above Print function
				
				if (KErrNone !=ret2)
					{
					RDebug::Print(_L("Error occurred in WriteToFileL(). Error = %d. Filename= %S"), ret2, &fileName);						
					retValue = ret2;
					}
				else
					{
					retValue = KErrNone;		
					}				
				}
				else // The identity was not found in the Db. 											
				{				
				RDebug::Print(_L("RemoveIdFromGlobalDb: The specified ID was not found in the totalDbFile"));
				retValue = KErrAuthServIdentityNotFound;									
				}
			}//End check for key&section search in file
		delete file;
		}//End check for safe file access
	return retValue;
	}

/**
Returns EPass or EFail
*/
TVerdict CTAuthSvrStepBase::checkAndSetUserNameL(AuthServer::RAuthClient& authClient1, TInt idPosition)
	{	
	TBool retValue = ETrue;
	TPtrC initNameValue;
	TPtrC finalNameValue;
	//Check that the user has specified the initial and later userNames for the identity 
	
	if( (GetStringFromConfig(ConfigSection(),_L("finalUserName"), finalNameValue)) 
			!= EFalse) // the tags 'initUserName' and finalUserName are present
		{
		//Get a list of all the present identities
		RIdentityIdArray ids;
		authClient1.IdentitiesL(ids);
		CleanupClosePushL(ids);
		
		//Check the userName of particular identity that was mentioned in the function call
		HBufC* idName = authClient1.IdentityStringL(ids[idPosition]);
		CleanupStack::PushL(idName);
		
		//Change the userName to what is desired
		authClient1.SetIdentityStringL(ids[idPosition], finalNameValue);
		
		//Compare to the expected final value of the userName
		
		HBufC* idNameFinal = authClient1.IdentityStringL(ids[idPosition]);
		
		if (idNameFinal->Compare(finalNameValue) != 0)
			{
			INFO_PRINTF4(_L("Id %d has the Final name %S instead of %S"), idPosition, idNameFinal, &finalNameValue);
			retValue = EFalse;
			}
		else
			{
			INFO_PRINTF4(_L("Id %d had the initial name %S and the Final name %S"), idPosition, idName, idNameFinal);	
			retValue = ETrue;
			}		
		delete idNameFinal;			
		CleanupStack::PopAndDestroy(2, &ids);	//idName, ids
		}
	else
		{
		INFO_PRINTF1(_L("finalUserName tag is absent in the script file"));		
		}
		
	if (retValue)
		{SetTestStepResult(EPass);}
	else
		{SetTestStepResult(EFail);}	
	
	return TestStepResult();
	}


//
void CTAuthSvrStepBase::CheckSpecifiedPluginStatusL ()
	{
	//If the pluginId is quoted in the ini file, check it's training status
	TInt plugin1IdValue = 0;
	if (GetHexFromConfig(ConfigSection(),_L("plugin1Id"), plugin1IdValue) != EFalse) // the tag 'pluginId1' was present
		{
		AuthServer::RAuthClient ac;
		User::LeaveIfError(ac.Connect());
		CleanupClosePushL(ac);
		// Get the status of the specified plugin
		TInt plugin1StatusValue;
		if (GetIntFromConfig(ConfigSection(),_L("plugin1Status"), plugin1StatusValue) != EFalse) 
			{
			TAuthTrainingStatus plugin1Status;// = plugin1StatusValue;
			switch (plugin1StatusValue)
				{
				case EAuthUntrained:
					plugin1Status = EAuthUntrained;
					break;
				case EAuthTrained:
					plugin1Status = EAuthTrained;
					break;
				case EAuthFullyTrained:
					plugin1Status = EAuthFullyTrained;
					break;
				default:
					ERR_PRINTF2(_L("Error receiving a value of the pluginIdStatus. Value in file : %d "), plugin1StatusValue);
					plugin1Status = EAuthUntrained;
					break;
				}				
			
			
			RPluginDescriptions pluginList1;
			TCleanupItem cleanup(CleanupEComArray, &pluginList1);
			CleanupStack::PushL(cleanup);				
			ac.PluginsL(pluginList1);
						
			TInt numPlugins = pluginList1.Count();
			for(TInt i = 0; i < numPlugins; i++)
				{
				TInt presentPluginIdVal = pluginList1[i]->Id();
								
				if (presentPluginIdVal == plugin1IdValue)
					{
					TInt presentPluginStatus = pluginList1[i]->TrainingStatus();
					if (presentPluginStatus == plugin1Status)
						{
						INFO_PRINTF3(_L("PluginId=%x has the expected status =%i"), presentPluginIdVal, presentPluginStatus);
						SetTestStepResult(EPass);
						}
					else
						{
						ERR_PRINTF4(_L("PluginId=%x has status =%i, instead of %i"), 
							presentPluginIdVal, presentPluginStatus, plugin1Status);
						SetTestStepResult(EFail);
						}
					break;	//Don't check any other plugins	
					}
				else
					{
					SetTestStepResult(EFail);	//After this, check others
					}
				}
			if(numPlugins == 0) 				
				{
				ERR_PRINTF1(_L("No plugins in the system"));
				SetTestStepResult(EFail);					
				}
				
			if (TestStepResult() == EFail)
				{
				ERR_PRINTF3(_L("PluginId=%x does not have the expected status %i"), 
					plugin1IdValue, plugin1StatusValue);
				}
				
			CleanupStack::PopAndDestroy(&pluginList1); //infoArray, results in a call to CleanupEComArray
			}
		else
			{
			INFO_PRINTF1(_L("plugin1Status was not specified in the ini file"));
			}
		CleanupStack::PopAndDestroy(&ac);	// ac	
		}
	else
		{
		INFO_PRINTF1(_L("plugin1Id was not specified in the ini file"));	
		}
	}



enum TOp
    {
    ENone =0,
    EOr = 1,
    EAnd = 2
    };
    
enum TState
    {
    EFirstToken  = 0,
    ESecondToken = 1,
    EThirdToken  = 2,
    ENumTokens = 3
    };
    
AuthServer::CAuthExpression* CTAuthSvrStepBase::CreateAuthExprLC (const TDesC& aString)
    {
   
    TLex  input(aString);
    
    _LIT(KKnowledge,"K");    
    _LIT(KBiometric,"B");
    _LIT(KToken,"T");  
    _LIT(KDefault,"D");
    _LIT(KAnd,"&");
    _LIT(KOr,"|");
    _LIT(KUnexpected, "Unexpected '%S' token while parsing authexpression");
    
    AuthServer::CAuthExpression* result = 0;
    TState state = EFirstToken;
    TOp op = ENone;
    TBool err;
    TPluginId plugin = 0;
    
    if (aString == KNullDesC)
       	{
       	result = AuthExpr();
       	CleanupStack::PushL(result);
       	return result;
       	}
    for (TPtrC token = input.NextToken(); token.Size() > 0 ; 
         token.Set(input.NextToken()))
        { 
        AuthServer::CAuthExpression* newExpr = 0; 
        INFO_PRINTF3(_L("Token %S Size = %d"), &token, token.Size()); 
        if (token.Length() > 1)
            {
            TLex lexer(token);
            if (lexer.Val(plugin,EHex) == KErrNone)
                {
                err = (state != EFirstToken && state != EThirdToken );
                if (err) ERR_PRINTF2(KUnexpected, &token);
                newExpr = AuthExpr(plugin);
                }
            else
                {
                err = true;
                ERR_PRINTF2(KUnexpected, &token);
                }
            }
        else if (token.CompareF(KKnowledge) == 0)
            {
            err = (state != EFirstToken && state != EThirdToken );
            if (err) ERR_PRINTF2(KUnexpected, &token);
            newExpr = AuthExpr(EAuthKnowledge);
            }
        else if (token.CompareF(KBiometric) == 0)
            {   
            err = (state != EFirstToken && state != EThirdToken );
            if (err) ERR_PRINTF2(KUnexpected, &token);
            newExpr = AuthExpr(EAuthBiometric);    
            }
        else if (token.CompareF(KToken) == 0)
            {
            err = (state != EFirstToken && state != EThirdToken );
            if (err) ERR_PRINTF2(KUnexpected, &token);
            newExpr = AuthExpr(EAuthToken);
            }
        else if (token.CompareF(KDefault) == 0)
            {
            err = (state != EFirstToken && state != EThirdToken );
            if (err) ERR_PRINTF2(KUnexpected, &token);
            newExpr = AuthExpr(EAuthDefault);
            }
        else if (token.CompareF(KAnd) == 0)
            {
            err = state != ESecondToken;
            if (err) ERR_PRINTF2(KUnexpected, &token);
            op = EAnd;
            }
        else if (token.CompareF(KOr) == 0)
            {
            err = state != ESecondToken;
            if (err) ERR_PRINTF2(KUnexpected, &token);
            op = EOr;
            }
        else 
            {
            err = true;
            ERR_PRINTF2(_L("Unknown '%S' token while parsing authexpression"), &token);
            }
            
        if (err) 
            {
            User::Leave(KErrGeneral);
            }
        switch (state)    
            {
            case EFirstToken:
                if ( result == 0 && op == ENone)
                    {
                    result = newExpr;
                    CleanupStack::PushL(result);
                    }
                else 
                    {
                    //err?
                    }
                state = ESecondToken;
                break;
            case ESecondToken:
                state = EThirdToken;
                break;
            case EThirdToken:
                if (op == EOr)
                    {
                    CleanupStack::Pop(result);
                    result =  AuthOr(result, newExpr);
                    CleanupStack::PushL(result);
    
                    }
                else
                    {
                    CleanupStack::Pop(result);
                    result =  AuthAnd(result, newExpr);
                    CleanupStack::PushL(result);
                    }
                op = ENone;
                state = ESecondToken;
            }
        }
    if (state == EThirdToken)
        {
        ERR_PRINTF1(_L("Unexpected end of stream while parsing authexpression"));
        User::Leave(KErrGeneral);
        }
    return result;
    }
    
void CTAuthSvrStepBase::CheckSpecifiedPluginStatusConnectedL (AuthServer::RAuthClient& ac)
	{
	//If the pluginId is quoted in the ini file, check it's training status
	TInt plugin1IdValue = 0;
	if (GetHexFromConfig(ConfigSection(),_L("plugin1Id"), plugin1IdValue) != EFalse) // the tag 'pluginId1' was present
		{
		// Get the status of the specified plugin
		TInt plugin1StatusValue = 0;
		if (GetIntFromConfig(ConfigSection(),_L("plugin1Status"), plugin1StatusValue) != EFalse) 
			{
			TAuthTrainingStatus plugin1Status;// = plugin1StatusValue;
			switch (plugin1StatusValue)
				{
				case EAuthUntrained:
					plugin1Status = EAuthUntrained;
					break;
				case EAuthTrained:
					plugin1Status = EAuthTrained;
					break;
				case EAuthFullyTrained:
					plugin1Status = EAuthFullyTrained;
					break;
				default:
					ERR_PRINTF2(_L("Error receiving a value of the pluginIdStatus. Value in file : %d "), plugin1StatusValue);
					plugin1Status = EAuthUntrained;
					break;
				}				
			
			
			RPluginDescriptions pluginList1;
			TCleanupItem cleanup(CleanupEComArray, &pluginList1);
			CleanupStack::PushL(cleanup);						
			ac.PluginsL(pluginList1);
						
			TInt numPlugins = pluginList1.Count();
			for(TInt i = 0; i < numPlugins; i++)
				{
				TInt presentPluginIdVal = pluginList1[i]->Id();
								
				if (presentPluginIdVal == plugin1IdValue)
					{
					TInt presentPluginStatus = pluginList1[i]->TrainingStatus();
					if (presentPluginStatus == plugin1Status)
						{
						INFO_PRINTF3(_L("PluginId=%x has the expected status =%i"), presentPluginIdVal, presentPluginStatus);
						SetTestStepResult(EPass);
						}
					else
						{
						ERR_PRINTF4(_L("PluginId=%x has status =%i, instead of %i"), 
							presentPluginIdVal, presentPluginStatus, plugin1Status);
						SetTestStepResult(EFail);
						}
					break;	//Don't check any other plugins	
					}
				else
					{
					SetTestStepResult(EFail);	//After this, check others
					}
				}
			if(numPlugins == 0) 				
				{
				ERR_PRINTF1(_L("No plugins in the system"));
				SetTestStepResult(EFail);					
				}
				
			if (TestStepResult() == EFail)
				{
				ERR_PRINTF3(_L("PluginId=%x does not have the expected status %i"), 
					plugin1IdValue, plugin1StatusValue);
				}
				
			CleanupStack::PopAndDestroy(&pluginList1); //infoArray, results in a call to CleanupEComArray
			}
		else
			{
			INFO_PRINTF1(_L("plugin1Status was not specified in the ini file"));
			}		
		}
	else
		{
		INFO_PRINTF1(_L("plugin1Id was not specified in the ini file"));	
		}	
	}

TBool CTAuthSvrStepBase::CheckPluginStatusAllL (AuthServer::RAuthClient& ac)
	{
	TBool retValue = EFalse;
	
	//Get a list of all the present plugins
	RPluginDescriptions pluginList1;
	TCleanupItem cleanup(CleanupEComArray, &pluginList1);
	CleanupStack::PushL(cleanup);	
	ac.PluginsL(pluginList1);
	
	//Get a list of all the present identities
	RIdentityIdArray ids;
	ac.IdentitiesL(ids);
	CleanupClosePushL(ids);
	TInt actualNumIdsFromServer = ids.Count();
	CleanupStack::PopAndDestroy(&ids);
	
	//Get the number of identities that has been trained for each of my 3 plugins
	TInt numIdsTrained;

	TInt numPlugins = pluginList1.Count();
	for(TInt i = 0; i < numPlugins; i++)
		{
		TInt presentPluginStatus = pluginList1[i]->TrainingStatus();
		TInt presentPluginId = pluginList1[i]->Id();
		TInt expectedStatus = 0;
		
		TDriveUnit sysDrive (RFs::GetSystemDrive());
		TDriveName sysDriveName (sysDrive.Name());
		TBuf<128> fileName (sysDriveName);
		
		switch (presentPluginId)
			{
			case 0x11113100:
				{
				fileName.Append(KPlugin3100File);
				numIdsTrained = GetNumIdsFromPluginDbL(fileName);
				break;
				}
			
			case 0x11113101:
				{
				fileName.Append(KPlugin3101File);
				numIdsTrained = GetNumIdsFromPluginDbL(fileName);
				break;
				}
				
			case 0x11113102:
				{
				fileName.Append(KPlugin3102File);
				numIdsTrained = GetNumIdsFromPluginDbL(fileName);
				break;
				}
				
			case 0x10234100:
				{
				fileName.Append(KPlugin4100File);
				numIdsTrained = GetNumIdsFromPluginDbL(fileName);
				break;
				}
				
			case 0x200032E5:
				{
				numIdsTrained = actualNumIdsFromServer - 1;
				}
				
			default:				
				numIdsTrained = KErrNotFound;
				break;
			}
		if (KErrNotFound != numIdsTrained)
			{
			//We are only considering the 3 specified Ids
			if (0 == numIdsTrained)
				{ expectedStatus = EAuthUntrained; }
			else if(actualNumIdsFromServer == numIdsTrained)
				{ expectedStatus = EAuthFullyTrained; }
			else if (actualNumIdsFromServer > numIdsTrained)
				{ expectedStatus = EAuthTrained; }
			else 
				{
				ERR_PRINTF2(_L("Error!! PluginId=%x has an unexpected Number of Ids in the Database = %i"), 
						numIdsTrained);
				ERR_PRINTF2(_L("Actual number of Ids registered by the authServer = %i"), 
						actualNumIdsFromServer);				
				}
			
			//Final check
			if (presentPluginStatus != expectedStatus)
				{
				ERR_PRINTF4(_L("Present PluginStatus of plugin %x is not as expected. It is %i instead of %i"), 
							presentPluginId, presentPluginStatus, expectedStatus);
				retValue = EFalse;
				}
			else
				{
				retValue = ETrue;	
				}
			}
		}//End of for loop
	CleanupStack::PopAndDestroy(&pluginList1); //infoArray, results in a call to CleanupEComArray
	return retValue;	
	}

TInt CTAuthSvrStepBase::GetNumIdsFromPluginDbL(const TDesC& aFileName)
	{
	TBuf8<500> pluginDbFileContents;
	TInt numIds = 0;	
	CIniData* file=NULL;
	file = CIniData::NewL(aFileName);	
	CleanupStack::PushL(file);
	
	// Look for a key under a named section, retrieve text value	
	TBool ret=EFalse;	//return value from FindVar functions	
	TPtrC resultPtr;
	
	ret=file->FindVar(KPluginIniSection, KPinDbTag, resultPtr);	
	if(ret == EFalse)
		{		
		RDebug::Print(_L("Unable to read value in ini file. Filename: %S, SectionName = %S, value = %S."), &aFileName, &KPluginIniSection, &KPinDbTag);
		numIds = ret;
		}
	else
		{
		pluginDbFileContents.Copy(resultPtr);
		_LIT8(KMarker, ":");
		TBuf8<3> searchString;		
		searchString = KMarker;
		
		TInt dBLen = pluginDbFileContents.Length();
		for (TInt i = 0; i < dBLen; i++)
			{
			if (pluginDbFileContents[i] == ':')
				{
				numIds++;
				}
			}//End for
		}//End check
	CleanupStack::PopAndDestroy(file);
	return numIds;
	}

void CTAuthSvrStepBase::CreatePinPluginInputFileL(TPinPluginDialogOperation aOperation, TPinValue aPinValue)
	{
	RFileWriteStream stream;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TInt err = stream.Replace(fs, KInputFile, EFileWrite | EFileShareExclusive);
	User::LeaveIfError(err);
	stream.PushL();
	
	MStreamBuf* streamBuf = stream.Sink();
	streamBuf->SeekL(MStreamBuf::EWrite, EStreamEnd);
	stream.WriteInt32L(aOperation);
	stream.WriteInt32L(aPinValue.Length());
	stream.WriteL(aPinValue);
	stream.WriteInt32L(0);
	stream.WriteInt32L(4);
	TPinPluginDialogResult cmdResult = EOk;
	stream.WriteInt32L(cmdResult);
	aOperation = EPinPluginInfo;
	stream.WriteInt32L(aOperation);
	TPinPluginDialogResult cmdFinalResult = EOk;
	stream.WriteInt32L(cmdFinalResult);
	stream.WriteInt32L(2);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream
	
	}

TPinValue CTAuthSvrStepBase::GetPinFromOutputFileL()
	{
	RFileReadStream stream;
	RFs fs;
	TPinValue pinValue;
	
	User::LeaveIfError(fs.Connect());
	TInt err = stream.Open(fs, KOutputFile, EFileRead | EFileShareExclusive);
	if(err == KErrNotFound)
		{
		pinValue.Copy(_L("11111"));
		return pinValue;
		}
	
	User::LeaveIfError(err);
	stream.PushL();

	MStreamBuf* streamBuf = stream.Source();
	
	stream.ReadInt32L();
	TInt pinValueLength = stream.ReadInt32L();
	HBufC8* pinValueBuf = HBufC8::NewMaxLC(pinValueLength);
	TPtr8 pinValuePtr(pinValueBuf->Des());
	stream.ReadL(pinValuePtr, pinValueLength);
	pinValue.Copy(pinValuePtr);
	
	CleanupStack::PopAndDestroy(2, &stream);	

	return pinValue;
	}

AuthServer::TIdentityId CTAuthSvrStepBase::getLastAuthId()
/**
	Retrieve the last Identity that was authenticated from the Global Db file
	@return		The last authenticated Identity
 */
	{
	TIdentityId lastId;
	lastId = 0;
		
	HBufC* valueFromFile;
	TInt ret = 0;
	TInt err = 0;
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> authSvrPolicyFile (sysDrive.Name());
	authSvrPolicyFile.Append(KAuthSvrPolicyFile);
	
	TRAP(err, ret = readFromNamedConfigL(authSvrPolicyFile, KLastAuthIdTag, valueFromFile));
	
	if (KErrNotFound == ret)
		{		
		ERR_PRINTF3(_L("Error while getting last AuthId. Tag %S not found in %S file"), KLastAuthIdTag, &authSvrPolicyFile);
		}
	else
		{		
		TLex input (*valueFromFile);
		TRadix aRadix = EDecimal;
		input.Val(lastId, aRadix);
		RDebug::Print(_L("UserId that is retrieved from file = %u"), lastId);
		}
		
	delete valueFromFile;		
	
	return lastId;
	}
	
	
void CTAuthSvrStepBase::ClientMessage(TBool aIsMessage, const TDesC& aDisplayString)
	{
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	TBuf<128> fileName (sysDriveName);
	
	fileName.Append(KAuthSvrPolicyFile);
	CIniData* file=NULL;
	TRAPD(r,file=CIniData::NewL(fileName));	
	CleanupStack::PushL(file);
	if(r!=KErrNone)
	{
	RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &fileName);
	}	
	
	if(aIsMessage)
		{
		TBool ret=file->SetValue(KPluginIniSection, KDisplayMessage, aDisplayString);
		file->WriteToFileL();
		}
	else
		{
		TBool ret=file->SetValue(KPluginIniSection, KDisplayMessage, KNullDesC());	
		}
	file->WriteToFileL();
	CleanupStack::PopAndDestroy(file);
	}

//Implementation of member functions for CTStepActSch

TVerdict CTStepActSch::doTestStepPreambleL()
/**
	Override CTestStep by allocating an active scheduler which is available to the subclass.	
	@return					This test step's result, which isn't actually used by ThreadStepExecutionL.
 */
	{
	CTAuthSvrStepBase::doTestStepPreambleL();
	iActSchd = new(ELeave) CActiveScheduler;
	return TestStepResult();
	}


TVerdict CTStepActSch::doTestStepPostambleL()
/**
	Override CTestStep by deleting the active scheduler which allocated in doTestStepPreambleL.
	@return					This test step's result, which isn't actually used by ThreadStepExecutionL.
 */
	{
	CTAuthSvrStepBase::doTestStepPostambleL();
	delete iActSchd;
	return TestStepResult();
	}

/**
 Utility clean up function used when working with locally declared arrays
 */
void CleanupEComArray(TAny* aArrayOfPlugins)
	{	
	(static_cast<RImplInfoPtrArray*> (aArrayOfPlugins))->ResetAndDestroy();
	(static_cast<RImplInfoPtrArray*> (aArrayOfPlugins))->Close();	
	}
