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


#include "tauthcliservstep.h"

using namespace AuthServer;


void CTAuthcliservStepBase::WaitForServerToReleaseDb()
/**
	Wait for the server to shut down and so
	release the authentication database.
 */
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


void CTAuthcliservStepBase::RemoveExistingDbL()
/**
	Remove any existing database file.
 */
	{
	WaitForServerToReleaseDb();
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName sysDriveName (sysDrive.Name());
	
	TFileName dbName(KDbName);
	dbName[0] = 'A' + sysDrive;	
	TInt r = iFs.Delete(dbName);
	User::LeaveIfError(r);
	
	//Reset the data 'AllUserID&PinValues' in AuthSvrPolicy.ini & the 
	// individual plugin Db files
	//_LIT(KInitPinDatabaseValue, ",");
	TBufC<16> initPinDbBuffer(KInitPinDatabaseValue);
	TPtr valueToWrite = initPinDbBuffer.Des();
	
	TBuf<64> fileName (sysDriveName);
	fileName.Append(KAuthSvrPolicyFile);
	writeToNamedConfig(fileName, KTotalDbTag, valueToWrite);
	
	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3100File);
	writeToNamedConfig(fileName, KPinDbTag, valueToWrite);

	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3101File);
	writeToNamedConfig(fileName, KPinDbTag, valueToWrite);

	fileName.Copy(sysDriveName);
	fileName.Append(KPlugin3102File);
	writeToNamedConfig(fileName, KPinDbTag, valueToWrite); 
	}

TInt CTAuthcliservStepBase::writeToNamedConfig(const TDesC &aFileName, const TDesC &aKeyName,const TPtrC &aResult)
	{
	
	TInt retValue = KErrNone;
	CIniData* file=NULL;
	TRAPD(r,file=CIniData::NewL(aFileName));
	
	if(r!=KErrNone)
		{
		RDebug::Print(_L("Unable to create CIniData object. Error = %d, File = %S"), r, &aFileName);
		retValue = r;
		}
	
	else
		{
		// Look for a key under no named section, retrieve text value	
		
		TInt ret = KErrNone;	//return value from FindVar functions
		ret=file->SetValue(aKeyName ,aResult);
		if(ret != KErrNone)
			{
			if(ret == KErrNotFound)
				{
				RDebug::Print(_L("Key not found. Unable to set value in ini file. Filename: %S, KeyName = %S, value = %S."), &aFileName, &aKeyName, &aResult);
				}			
			else
				{
				RDebug::Print(_L("Unable to set value in ini file. Filename: %S, KeyName = %S, value = %S."), &aFileName, &aKeyName, &aResult);
				}
			retValue = ret;
			}
		TRAPD(r,file->WriteToFileL());
		if (KErrNone != r)
			{
			RDebug::Print(_L("Error occurred while writing to the file. Filename = %S, KeyName = %S, value = %S."), &aFileName, &aKeyName, &aResult);
			}
		delete file;
		}
	return retValue;	
	}
