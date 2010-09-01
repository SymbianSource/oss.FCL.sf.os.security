/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
*/


#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include <asn1enc.h>
#include <asn1dec.h>
#include <x509cert.h>
#include <x509keys.h>

/////////////////////////////////////////////////////////////////////////////////
// CImportKey
/////////////////////////////////////////////////////////////////////////////////

CTestAction* CImportKey::NewL(RFs& aFs, 
							  CConsoleBase& aConsole, 
							  Output& aOut,
							  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CImportKey::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CImportKey::NewLC(RFs& aFs,
							   CConsoleBase& aConsole, 
							   Output& aOut,
							   const TTestActionSpec& aTestActionSpec)
	{
	CImportKey* self = new (ELeave) CImportKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CImportKey::~CImportKey()
{
	delete iKeyData;
	delete iImportFileName;
	if (iKeyInfo != NULL)
		{
		iKeyInfo->Release();
		}
}

CImportKey::CImportKey(RFs& aFs,CConsoleBase& aConsole, Output& aOut) :
	CKeyStoreTestAction(aFs, aConsole, aOut), iState(EImportKey)
	{
	}


void CImportKey::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	TInt err = KErrNone;
	TInt pos = 0;

	iImportFileName = HBufC8::NewL(256); // Reasonable max length of a file name
	TPtr8 temp(iImportFileName->Des());
	temp.Copy(Input::ParseElement(aTestActionSpec.iActionBody, KImportDataFile, KImportDataFileEnd, pos, err));
}

void CImportKey::SetKeyDataFileL(const TDesC8& aDes)
{
	RFs theFs;
	User::LeaveIfError(theFs.Connect());
	CleanupClosePushL(theFs);
		
//	First the private data path for this test	
	TFileName buf;
	//User::LeaveIfError(theFs.PrivatePath(buf));
	
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TFileName keyDataFile (sysDrive.Name());
	keyDataFile.Append(_L("\\tkeystore\\data\\"));

	
//	Now the filename itself
	buf.FillZ();
	buf.Copy(aDes);
	keyDataFile.Append(buf);
	
	RFile file;
	TInt r = file.Open(theFs, keyDataFile, EFileRead);
	if ( (r==KErrNotFound) || (r==KErrPathNotFound) )
	{//	Not on c:, try z:
		keyDataFile[0] = 'z';
		r = file.Open(theFs, keyDataFile, EFileRead);
	}

	User::LeaveIfError(r);

	CleanupClosePushL(file);

	TInt fileSize = 0;
	User::LeaveIfError(file.Size(fileSize));

	if (fileSize > 0)
		{
		ASSERT(!iKeyData);
		iKeyData = HBufC8::NewMaxL(fileSize);	
		TPtr8 data(iKeyData->Des());
		data.FillZ();
		User::LeaveIfError(file.Read(data, fileSize));
		}

	CleanupStack::PopAndDestroy(2); // file, theFs
}

void CImportKey::PerformAction(TRequestStatus& aStatus)
	{
	switch (iState)
		{
		case EImportKey:
			{
			TInt result = KErrNone;
				TRAP(result, SetKeyDataFileL(*iImportFileName));
			if (KErrNone != result) 
				{
				iState = EFinished;			
				TRequestStatus *status = &aStatus;
				User::RequestComplete(status,result);
				break;			
				}
                    
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			if(iKeyStoreImplLabel.Length() != 0)
				{
				SetKeyStoreIndex(keyStore);
				}		
				{
					keyStore->ImportKey(iKeyStoreImplIndex, iKeyData->Des(), iUsage, *iLabel, iAccessType,
										TTime(0), TTime(0), iKeyInfo, aStatus);
				} // else
			iState = EFinished;
			}		
			break;

		case EFinished:
			{
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus == iExpectedResult)
				{
				iResult = ETrue;
				}
			else
				{
				iResult = EFalse;
				}
			iActionState = EPostrequisite;
			}
			break;
		}
	}

void CImportKey::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    if (iState == EFinished)
        {
        keystore->CancelImportKey();
        }
    }

void CImportKey::Reset()
	{
	iState = EImportKey;
	if (iKeyInfo != NULL)
		{
		iKeyInfo->Release();
		iKeyInfo = NULL;
		}
	delete iKeyData;
	iKeyData = NULL;
	}

void CImportKey::DoReportAction()
{
	_LIT(KImporting, "Importing key...");
	iOut.writeString(KImporting);
	TPtr theLabel(iLabel->Des());
	iOut.writeString(theLabel);
	iOut.writeNewLine();
}


void CImportKey::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Key imported successfully\n");
			iConsole.Write(KSuccessful);
			iOut.writeString(KSuccessful);
			iOut.writeNewLine();
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Key import failure!!!\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}
			else
			{
				_LIT(KFailed, "Key import failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
			iOut.writeNewLine();
		}
	}
}

