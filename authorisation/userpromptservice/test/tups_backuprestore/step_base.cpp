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


#include "tbackuprestorestep.h"
#include <test/testexecutelog.h>

#include <f32file.h>

_LIT(KMetaDataFileName, "\\tswi\\tbackuprestore\\metadata%x");
_LIT(KBackupFileName, "\\tswi\\tbackuprestore\\backup%x.%u");

_LIT(KExpectedResultKeyFormat, "expectedresult%d");
const TInt KResultKeyMaxLength = 16;
const TInt KResultKeyMaxNum = 99;


CTBackupRestoreStepBase::~CTBackupRestoreStepBase()
	{
	iFs.Close();
	}

CTBackupRestoreStepBase::CTBackupRestoreStepBase()
	{
	}

TVerdict CTBackupRestoreStepBase::doTestStepPreambleL()
	{
	User::LeaveIfError(iFs.Connect());
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTBackupRestoreStepBase::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TBool CTBackupRestoreStepBase::Exists(TPtrC aFileName)
	{
	TEntry entry;
	TInt err = iFs.Entry(aFileName, entry);
	INFO_PRINTF3(_L("Entry of %S returned %d"), &aFileName, err);
	return err == KErrNone;
	}
	
TBool CTBackupRestoreStepBase::ModifyFile()
	{
	TBuf<20> token;
	token.Format(_L("MODIFY%d.%d"), iCurrentPackageNumber, iCurrentFileNumber);
	TInt doIt = 0;
	TBool found = GetIntFromConfig(ConfigSection(),token, doIt);
	return (found && doIt != 0);
	}

void CTBackupRestoreStepBase::CopyFileL(TPtrC aSourceFileName, TPtrC aDestFileName)
	{
	INFO_PRINTF3(_L("Copy %S to %S"), &aSourceFileName, &aDestFileName);
	RFile infile;
	RFile outfile;
	User::LeaveIfError(infile.Open(iFs,aSourceFileName,EFileRead));
	CleanupClosePushL(infile);
	if (KErrNone != outfile.Create(iFs,aDestFileName,EFileWrite))
		{
		User::LeaveIfError(outfile.Open(iFs, aDestFileName, EFileWrite));
		}
		
	CleanupClosePushL(outfile);
	
	HBufC8* buffer = HBufC8::NewLC(1024);
	TPtr8 ptr = buffer->Des();
	
	User::LeaveIfError(infile.Read(ptr));
	while(ptr.Length() != 0)
		{
		User::LeaveIfError(outfile.Write(*buffer));
		User::LeaveIfError(infile.Read(ptr));
		}
	
	CleanupStack::PopAndDestroy(3, &infile); // buffer, outfile
	}

TBool CTBackupRestoreStepBase::NextPackage()
	{
	iCurrentPackageNumber++;
	iCurrentFileNumber=0;
	iDataFileNumber=0;
	if (PackageUID().iUid != 0)
		{
		INFO_PRINTF2(_L("Package %d"), iCurrentPackageNumber);
		return ETrue;
		}
	return EFalse;
	}

TUid CTBackupRestoreStepBase::PackageUID()
	{
	TUid uid;
	TInt tmp;
	TBuf<20> token;
	token.Format(_L("UID%d"), iCurrentPackageNumber);
	if(GetHexFromConfig(ConfigSection(),token, tmp) == EFalse)
		{
		tmp = 0;
		}
	uid.iUid = tmp;
	return uid;
	}

TBool CTBackupRestoreStepBase::NextFile()
	{
	iCurrentFileNumber++;
	if (InstalledFileName().Length() != 0)
		{
		INFO_PRINTF2(_L("File %d"), iCurrentFileNumber);
		return ETrue;
		}
	return EFalse;
	}
	
TBool CTBackupRestoreStepBase::NextDataFile()
	{
	iDataFileNumber++;
	if (DataFileName().Length() != 0)
		{
		return ETrue;
		}
	return EFalse;
	}

TPtrC CTBackupRestoreStepBase::SISFileNameL()
	{
	TBuf<20> token;
	token.Format(_L("SIS%d"), iCurrentPackageNumber);
	TPtrC sisfile;
	if (GetStringFromConfig(ConfigSection(),token, sisfile) == EFalse)
		{
		User::Leave(KErrNotFound);
		}
	return sisfile;
	}

TPtrC CTBackupRestoreStepBase::WrongInstalledFileName()
	{
	TBuf<20> token;
	token.Format(_L("WrongFile%d.%d"), iCurrentPackageNumber, iCurrentFileNumber);
	TPtrC filename;
	GetStringFromConfig(ConfigSection(),token, filename);
	return filename;
	}

TPtrC CTBackupRestoreStepBase::InstalledFileName()
	{
	TBuf<20> token;
	token.Format(_L("FILE%d.%d"), iCurrentPackageNumber, iCurrentFileNumber);
	TPtrC filename;
	GetStringFromConfig(ConfigSection(),token, filename);
	return filename;
	}

TPtrC CTBackupRestoreStepBase::DataFileName()
	{
	TBuf<20> token;
	token.Format(_L("DATA%d.%d"), iCurrentPackageNumber, iDataFileNumber);
	TPtrC filename;
	GetStringFromConfig(ConfigSection(),token, filename);
	return filename;
	}



TFileName CTBackupRestoreStepBase::BackupFileName()
	{
	TFileName backupFileName;
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> fileName = sysDrive.Name();
	fileName.Append(KBackupFileName);
	backupFileName.Format(fileName, PackageUID().iUid, iCurrentFileNumber);
	return backupFileName;
	}
	
void CTBackupRestoreStepBase::TouchL(const TDesC& aFileName)
	{
	iFs.MkDirAll(aFileName);
	RFile file;
	TRAP_IGNORE(file.Replace(iFs, aFileName, EFileWrite));
	file.Close();
	}

void CTBackupRestoreStepBase::WriteMetaDataL(HBufC8* aMetaData)
	{
	RFile metadatafile;
	TBuf<256> metadataFileName;
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> fileName = sysDrive.Name();
	fileName.Append(KMetaDataFileName);
	metadataFileName.Format(fileName, PackageUID().iUid);

	User::LeaveIfError(metadatafile.Replace(iFs, metadataFileName, EFileWrite));
	
	CleanupClosePushL(metadatafile);
	User::LeaveIfError(metadatafile.Write(*aMetaData));
	CleanupStack::PopAndDestroy(&metadatafile);
	}

HBufC8*	CTBackupRestoreStepBase::ReadMetaDataL(TInt aBytesToCrop)
	{
	RFile metadatafile;
	TBuf<256> metadataFileName;
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	TBuf<128> fileName = sysDrive.Name();
	fileName.Append(KMetaDataFileName);
	metadataFileName.Format(fileName, PackageUID().iUid);

	User::LeaveIfError(metadatafile.Open(iFs, metadataFileName, EFileRead));
	CleanupClosePushL(metadatafile);
	
	TInt length(0);
	metadatafile.Seek(ESeekEnd,length);
	length -= aBytesToCrop;
	TInt dummy(0);
	metadatafile.Seek(ESeekStart,dummy);
	
	HBufC8* metadata = HBufC8::NewL(length);
	TPtr8 ptr = metadata->Des();
	TInt err = metadatafile.Read(ptr, length);
	if (err != KErrNone)
		{
		delete metadata;
		User::Leave(err);
		}
	CleanupStack::PopAndDestroy(&metadatafile);
	return metadata;
	}

TInt CTBackupRestoreStepBase::MetaDataCropLength()
	{
	TInt cropLength(0);
	TBuf<24> token;
	token.Format(_L("METADATACROPBYTES%d"), iCurrentPackageNumber);
	if(GetIntFromConfig(ConfigSection(),token, cropLength))
		{
		INFO_PRINTF3(_L("Ignoring last %d bytes from metadata for package %d."),
						cropLength, iCurrentPackageNumber);
		}
	return cropLength;	
	}

TChar CTBackupRestoreStepBase::ExpectedDriveL()
	{
	TBuf<20> token;
	token.Format(_L("INSTALLDRIVE%d"), iCurrentPackageNumber);
	TPtrC driveString;
	if (GetStringFromConfig(ConfigSection(),token, driveString) == EFalse)
		{
		User::Leave(KErrNotFound);
		}
	TChar sisDrive(0);
	if (driveString.Length() > 0)
		{
		sisDrive = driveString[0];
		}
	return sisDrive;
	}


void CTBackupRestoreStepBase::CheckExpectedResultCodesL()
	{
	RArray<TInt> expectedResultCodes;
	CleanupClosePushL(expectedResultCodes);
	TInt result = KErrNone;
	TBuf<KResultKeyMaxLength> resultKey;
	TBool found = ETrue;

	for (TInt i = 1; i <= KResultKeyMaxNum && found; i++)
		{
		resultKey.Format(KExpectedResultKeyFormat, i);
		found = GetIntFromConfig(ConfigSection(), resultKey, result);
		if (found)
			{
			INFO_PRINTF2(_L("Acceptable result code %d"), result);
			TInt err = expectedResultCodes.InsertInOrder(result);
			if (err != KErrNone && err != KErrAlreadyExists)
				{
				User::Leave(err);
				}
			}
		}

	if (expectedResultCodes.Count() > 0)
		{
		// A result code matching one of the possible expected results will
		// be converted to EPass, otherwise the result will be EFail.
		if (expectedResultCodes.FindInOrder(TestStepResult()) == KErrNotFound)
			{
			ERR_PRINTF2(_L("Result code %d is not expected"), TestStepResult());
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("Result code %d is in accepted list"), TestStepResult());
			SetTestStepResult(EPass);
			}
		}
	CleanupStack::PopAndDestroy(&expectedResultCodes);
	}




