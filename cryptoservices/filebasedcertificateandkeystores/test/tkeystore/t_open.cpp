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


#include <e32base.h>
#include "t_keystore_actions.h"
#include "t_keystore_defs.h"
#include "t_input.h"
#include <asn1enc.h>
#include <asn1dec.h>
#include <x509cert.h>
#include <x509keys.h>
#include <asymmetrickeys.h>

/*static*/ CTestAction* COpenKey::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = COpenKey::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* COpenKey::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	COpenKey* self = new (ELeave) COpenKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

COpenKey::~COpenKey()
{
	iKeys.Close();
}

void COpenKey::PerformAction(TRequestStatus& aStatus)
{
	switch (iState)
	{	
		case EListKeysPreOpen:
		{//	Currently uses the first store, change to check the script for a specific store
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			
			iState = EOpenKey;
		}
		break;
		
		case EOpenKey:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			
			TInt keyCount = iKeys.Count();

			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					switch (iType)
						{
					case ERSASign:
						keyStore->Open(*keyInfo, iRSASigner, aStatus);
						break;
					case EDSASign:
						keyStore->Open(*keyInfo, iDSASigner, aStatus);
						break;
					case EDecrypt:
						keyStore->Open(*keyInfo, iDecryptor, aStatus);
						break;
					case EDH:
						keyStore->Open(*keyInfo, iDH, aStatus);
						break;
						}
					break;
					}
				}
			iState = EFinished;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
		}
		break;

		case EFinished:
			{
			HBufC* label = 0;
			iKeys.Close();
			if (aStatus.Int() == KErrNone)
				{
				switch (iType)
					{
				case ERSASign:
					if (iRSASigner)
						{
						label = iRSASigner->Label().AllocLC();
						iRSASigner->Release();
						}
					break;
				case EDSASign:
					if (iDSASigner)
						{
						label = iDSASigner->Label().AllocLC();
						iDSASigner->Release();
						}
					break;
				case EDecrypt:
					if (iDecryptor)
						{
						label = iDecryptor->Label().AllocLC();
						iDecryptor->Release();
						}
				case EDH:
					if (iDH)
						{
						label = iDH->Label().AllocLC();
						iDH->Release();
						}
					break;
					}
				if (*label != *iLabel)
					aStatus = KErrBadName;
				CleanupStack::PopAndDestroy(label);
				}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			if (aStatus.Int()==iExpectedResult)
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
		default:
			ASSERT(EFalse);	
	}

}

void COpenKey::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EOpenKey:
            keystore->CancelList();
            break;

        case EFinished:
            keystore->CancelOpen();
            break;

        default:
            break;
        }
    }

void COpenKey::Reset()
	{
  	iState = EListKeysPreOpen;
    iKeys.Close();
	if (iRSASigner)
		{
		iRSASigner->Release();
		iRSASigner = NULL;
		}
	if (iDSASigner)
		{
		iDSASigner->Release();
		iDSASigner = NULL;
		}
	if (iDecryptor)
		{
		iDecryptor->Release();
		iDecryptor = NULL;
		}
	if (iDH)
		{
		iDH->Release();
		iDH = NULL;
		}
	}

void COpenKey::DoReportAction()
{
	_LIT(KDeleting, "Opening...");
	iOut.writeString(KDeleting);
	iOut.writeNewLine();
}

void COpenKey::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			_LIT(KSuccessful, "Key open success\n");			
			buf.Format(KSuccessful);
			iConsole.Write(buf);
			iOut.writeString(buf);
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Key open failure %d!!!\n");
				buf.Format(KFailed, aError);
				iConsole.Write(buf);
				iOut.writeString(buf);
			}
			else
			{
				_LIT(KFailed, "Key open failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

COpenKey::COpenKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CKeyStoreTestAction(aFs, aConsole, aOut)
{}

void COpenKey::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	TInt pos = 0, err = 0;
	CKeyStoreTestAction::ConstructL(aTestActionSpec);
	SetKeyType(Input::ParseElement(aTestActionSpec.iActionBody, KOpenStart, KOpenEnd, pos, err));
	iState = EListKeysPreOpen;

	}
void COpenKey::SetKeyType(const TDesC8& aKeyType)
{
	if (aKeyType.Compare(KAlgRSA)==0)
	{
		iType = ERSASign;
	}
	else if (aKeyType.Compare(KAlgDSA)==0)
	{
		iType = EDSASign;
	}
	else if (aKeyType.Compare(KDecryptUsage)==0)
	{
		iType = EDecrypt;
	}
	else if (aKeyType.Compare(KAlgDH)==0)
	{
		iType = EDH;
	}
}


////////////////////////////////////
// CSign
////////////////////////////////////
/*static*/ CTestAction* CSign::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CSign::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* CSign::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CSign* self = new (ELeave) CSign(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CSign::~CSign()
{
	iKeys.Close();
	delete iReadText;
	delete iExportedPublicKey;
	delete iRSASignature;
	delete iDSASignature;
	delete iHash;
}

void CSign::PerformAction(TRequestStatus& aStatus)
{
	// Jump straight to final state if an error occured 
	if (aStatus.Int()!=KErrNone)
		{
		iState=EFinished;
		}
		
	switch (iState)
	{	
		case EListKeysPreOpen:
		{            
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			
			iState = EOpenKey;
		}
		break;
		
		case EOpenKey:
		{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
				ASSERT(keyStore);	//	Flag it up
				
				TInt keyCount = iKeys.Count();

				TInt i;
				for (i = 0; i < keyCount; i++)
					{
					CCTKeyInfo* keyInfo = iKeys[i];

					if (keyInfo->Label() == *iLabel)
						{
						switch (iType)
							{
						case ERSASign:
							keyStore->Open(*keyInfo, iRSASigner, aStatus);
							break;
						case EDSASign:
							keyStore->Open(*keyInfo, iDSASigner, aStatus);
							break;
						case EDH:
						case EDecrypt:
							break;	//	Nothing to do, for the compiler
							}
						break;
						}
					}
				iState = ESign;
				if (i == keyCount)
					{
					TRequestStatus* status = &aStatus;
					User::RequestComplete(status, KErrNotFound);
					}					
				}
		break;

		case ESign:
			{
			switch (iType)
				{
				case ERSASign:
				{
					if (iHash)
					{
					    if (iFailHashDigest)	//	Don't hash it, fail deliberately
							iRSASigner->Sign(*iReadText,iRSASignature,aStatus);
						else  // message gets signed by the keystore
							iRSASigner->SignMessage(*iReadText,iRSASignature,aStatus);
					}
					else
					{
						iRSASigner->Sign(*iReadText,iRSASignature,aStatus);
					}
				}
				break;
				case EDSASign:
				{
					if (iHash)
					{
					    if (iFailHashDigest)	//	Don't hash it, deliberately fail it
							iDSASigner->Sign(*iReadText,iDSASignature,aStatus);
						else  // message gets signed by the keystore
							iDSASigner->SignMessage(*iReadText,iDSASignature,aStatus);
					}
					else
					{
						iDSASigner->Sign(*iReadText,iDSASignature,aStatus);
					}
				}
				break;
				default:
					ASSERT(EFalse);
			}					
			iState = EExportPublic;					
		}
		break;

		case EExportPublic:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up				
			TInt keyCount = iKeys.Count();
			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					iExportHandle = keyInfo->Handle();

					switch (iType)
						{
					case ERSASign:
						keyStore->ExportPublic(iExportHandle, iExportedPublicKey,  aStatus);
						break;
					case EDSASign:
						keyStore->ExportPublic(iExportHandle, iExportedPublicKey,  aStatus); 
						break;
					case EDH:
					case EDecrypt:
						break;	//	Nothing to do, for the compiler
						}
					break;
					}
				}
			iState = EVerify;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
			}
			break;

		case EVerify:
			{
			TInt keyCount = iKeys.Count();
			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					iExportHandle = keyInfo->Handle();
					CX509SubjectPublicKeyInfo* ki = 
						CX509SubjectPublicKeyInfo::NewLC(*iExportedPublicKey);

					switch (iType)
						{
					case ERSASign:
						{
						TX509KeyFactory factory; 
						CRSAPublicKey* key = factory.RSAPublicKeyL(ki->KeyData());
						CleanupStack::PushL(key);
						
						CRSAPKCS1v15Verifier* verifier = NULL;
						
						if (iHash)	//	Must compare with hash of original data
						{
							verifier = CRSAPKCS1v15Verifier::NewLC(*key);
							iHash->Reset();
							iVerifyResult = verifier->VerifyL(iHash->Final(*iReadText), *iRSASignature);
						}
						else
						{
							verifier = CRSAPKCS1v15Verifier::NewLC(*key);
							iVerifyResult = verifier->VerifyL(*iReadText, *iRSASignature);
						}
												
						_LIT(KReturned, "Returned... ");
						iOut.writeString(KReturned);
						iOut.writeNewLine();

						CleanupStack::PopAndDestroy(2, key);
						}
						break;
					case EDSASign:
						{
						TX509KeyFactory factory;
						CDSAPublicKey* key = factory.DSAPublicKeyL(ki->EncodedParams(), ki->KeyData());
						CleanupStack::PushL(key);
						
						CDSAVerifier* verifier = CDSAVerifier::NewLC(*key);
						if (iHash)	//	Must compare with hash of original value
						{
							iHash->Reset();
							iVerifyResult = verifier->VerifyL(iHash->Final(*iReadText),*iDSASignature);
						}
						else
						{
							iVerifyResult = verifier->VerifyL(*iReadText,*iDSASignature);
						}
						_LIT(KReturned, "Returned... ");
						iOut.writeString(KReturned);
						iOut.writeNewLine();
						CleanupStack::PopAndDestroy(verifier);
						CleanupStack::PopAndDestroy(key);
						}
						break;
					default:
						ASSERT(EFalse);

						}
					CleanupStack::PopAndDestroy(ki);
					}
				}
			iState = EFinished;
			TRequestStatus* status = &aStatus;
			if (!iVerifyResult)		
				{
				_LIT(KVerifyFail, "**Verify failed**");
				iOut.writeString(KVerifyFail);
				iOut.writeNewLine();

				// Flat verify failed as KErrGeneral
				if (aStatus.Int() == KErrNone)
					{
					aStatus = KErrGeneral;
					}
				}
			User::RequestComplete(status, aStatus.Int());					
			}
			break;
		
		case EFinished:
		{
		iActionState = EPostrequisite;
		iResult = (aStatus.Int() == iExpectedResult);
			
			if ((aStatus.Int() != KErrNone)&&(aStatus!=KErrNoMemory))
				{
				_LIT(KExportFail," !Failed when exporting public key! ");
				iOut.writeString(KExportFail);
				}
			
			iKeys.Close();
			switch (iType)
				{
			case ERSASign:
				if (iRSASigner)
					iRSASigner->Release();
				break;
			case EDSASign:
				if (iDSASigner)
					iDSASigner->Release();
				break;
			case EDecrypt:
				if (iDecryptor)
					iDecryptor->Release();
			case EDH:
				if (iDH)
					iDH->Release();
				break;
				}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
		}
		break;
		default:
			ASSERT(EFalse);	
	}

}

void CSign::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EOpenKey:
            keystore->CancelList();
            break;

        case ESign:
            keystore->CancelOpen();
            break;

        case EExportPublic:
			switch (iType)
                {
				case ERSASign:
                    ASSERT(iRSASigner);
                    iRSASigner->CancelSign();
                    break;

                case EDSASign:
                    ASSERT(iDSASigner);
                    iDSASigner->CancelSign();
                    break;

                default:
                    break;
                }
            break;

        case EVerify:            
            keystore->CancelExportPublic();
            break;

        default:
            break;
        }
    }

void CSign::Reset()
	{
	iState = EListKeysPreOpen;
	iKeys.Close();
    if (iRSASigner)
        {
        iRSASigner->Release();
        iRSASigner = NULL;        
        }
    if (iDSASigner)
        {
        iDSASigner->Release();
        iDSASigner = NULL;        
        }
	delete iExportedPublicKey;
	iExportedPublicKey = NULL;
	delete iRSASignature;
	iRSASignature = NULL;
	delete iDSASignature;
	iDSASignature = NULL;
	}

void CSign::DoReportAction()
{
	_LIT(KSigning, "Signing... ");
	iOut.writeString(KSigning);
	iOut.writeNewLine();
}

void CSign::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			iOut.writeNewLine();
			_LIT(KSuccessful, "Sign success\n");			
			buf.Format(KSuccessful);
			iConsole.Write(buf);
			iOut.writeString(buf);
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Sign failure %d!!!\n");
				buf.Format(KFailed, aError);
				iConsole.Write(buf);
				iOut.writeString(buf);
			}
			else
			{
				_LIT(KFailed, "Sign failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

CSign::CSign(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
:	CKeyStoreTestAction(aFs, aConsole, aOut)
{}

void CSign::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	SetKeyType(Input::ParseElement(aTestActionSpec.iActionBody, KOpenStart));	
	SetDigestSignL(Input::ParseElement(aTestActionSpec.iActionBody, KSignDigestStart));
	SetSignText(Input::ParseElement(aTestActionSpec.iActionBody, KTextStart, KTextEnd));

	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;
	
	iState = EListKeysPreOpen;
}


void CSign::SetKeyType(const TDesC8& aKeyType)
{
	if (aKeyType.Compare(KAlgRSA)==0)
	{
		iType = ERSASign;
	}
	else if (aKeyType.Compare(KAlgDSA)==0)
	{
		iType = EDSASign;
	}
	else if (aKeyType.Compare(KDecryptUsage)==0)
	{
		iType = EDecrypt;
	}
	else if (aKeyType.Compare(KAlgDH)==0)
	{
		iType = EDH;
	}
}

void CSign::SetDigestSignL(const TDesC8& aSignDigestDesc)
{
	TLex8 lexer(aSignDigestDesc);
	TInt digest = 0;
	lexer.Val(digest);

	if (digest > 0)
		iHash = CMessageDigestFactory::NewDigestL(CMessageDigest::ESHA1);

	if (digest==2)
		iFailHashDigest = ETrue;
}

void CSign::SetSignText(const TDesC8& aText)
{
	iReadText = HBufC8::NewMax(aText.Size());
	if (iReadText)
	{
		TPtr8 theText(iReadText->Des());
		theText.FillZ();
		theText.Copy(aText);
	}
}

////////////////////////////////////
// CDecrypt
////////////////////////////////////
/*static*/ CTestAction* CDecrypt::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CDecrypt::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* CDecrypt::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CDecrypt* self = new (ELeave) CDecrypt(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CDecrypt::~CDecrypt()
{
	iKeys.Close();
	delete iReadText;
	delete iPlainText;
	delete iPublic;
}

void CDecrypt::PerformAction(TRequestStatus& aStatus)
{
	// Jump straight to final state if an error occured 
	if (aStatus.Int()!=KErrNone)
		{
		iState=EFinished;
		}

	switch (iState)
	{	
		case EListKeysPreOpen:
		{//	Currently uses the first store, change to check the script for a specific store
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			
			iState = EOpenKey;
		}
		break;
		
		case EOpenKey:
		{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			
			TInt keyCount = iKeys.Count();

			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					keyStore->Open(*keyInfo, iDecryptor, aStatus);
					break;
					}
				}
			iState = EExportPublic;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
		}
		break;

		case EExportPublic:
		{
			if (aStatus.Int()!=KErrNone)
				{
				_LIT(KSignFail," !Failed when opening! ");
				iOut.writeString(KSignFail);
				iState=EFinished;
				// need to set it to true so that test is true if some error was expected
				iVerifyResult=ETrue;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				break;
				}

			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			
			TInt keyCount = iKeys.Count();

			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					iExportHandle = keyInfo->Handle();
					
					keyStore->ExportPublic(iExportHandle, iPublic,  aStatus);
					break;
					}
				}
			iState = EDecrypt;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
		}
		break;

		case EDecrypt:
			{
			
			if (aStatus.Int()!=KErrNone)
				{
				iState=EFinished;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				break;
				}

			CX509SubjectPublicKeyInfo* ki = 
				CX509SubjectPublicKeyInfo::NewLC(*iPublic);

			TX509KeyFactory factory; 
			CRSAPublicKey* key = factory.RSAPublicKeyL(ki->KeyData());
			CleanupStack::PushL(key);
						
		//	Encrypt with public key
			CRSAPKCS1v15Encryptor* encryptor = CRSAPKCS1v15Encryptor::NewLC(*key);
			HBufC8* cipherText = HBufC8::NewLC(encryptor->MaxOutputLength());
			TPtr8 cipherTextPtr = cipherText->Des();

			encryptor->EncryptL(*iReadText, cipherTextPtr);
			
		//	Now decrypt again
			iPlainText = HBufC8::NewMaxL(100);
			iPlainTextPtr.Set(iPlainText->Des());
			iDecryptor->Decrypt(*cipherText, iPlainTextPtr, aStatus);
			
			CleanupStack::PopAndDestroy(cipherText);
			CleanupStack::PopAndDestroy(encryptor);
			CleanupStack::PopAndDestroy(key);
			CleanupStack::PopAndDestroy(ki);
			
			iState = EFinished;
			}
			break;

		case EFinished:
			{
			if (aStatus == KErrNone && (!iPlainText || 
										*iPlainText != *iReadText))
				{
				aStatus = KErrGeneral;	//	Decryption failed
				}
			
			iActionState = EPostrequisite;
			iResult = (aStatus.Int() == iExpectedResult);
			
			if (iDecryptor)
				{
				iDecryptor->Release();
				}
			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
		}
		break;
		default:
			ASSERT(EFalse);	
	}

}

void CDecrypt::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EOpenKey:
            keystore->CancelList();
            break;

        case EExportPublic:
            keystore->CancelOpen();
            break;
            
        case EDecrypt:
            keystore->CancelExportPublic();
            break;

        case EFinished:
            ASSERT(iDecryptor);
            iDecryptor->CancelDecrypt();
            break;
            
        default:
            break;
        }
    }

void CDecrypt::Reset()
	{
	iState = EListKeysPreOpen;
	iKeys.Close();
    if (iDecryptor)
        {
        iDecryptor->Release();
        iDecryptor = NULL;
        }
	delete iPlainText;
	iPlainText = NULL;
	delete iPublic;
	iPublic = NULL;
	}

void CDecrypt::DoReportAction()
{
	_LIT(KSigning, "Decrypting... ");
	iOut.writeString(KSigning);
	iOut.writeNewLine();
//	iOut.writeString(*iReadText);
//	iOut.writeNewLine();
}

void CDecrypt::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			iOut.writeNewLine();
			_LIT(KSuccessful, "Decrypt success\n");			
			buf.Format(KSuccessful);
			iConsole.Write(buf);
			iOut.writeString(buf);
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Decrypt failure %d!!!\n");
				buf.Format(KFailed, aError);
				iConsole.Write(buf);
				iOut.writeString(buf);
			}
			else
			{
				_LIT(KFailed, "Decrypt failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

CDecrypt::CDecrypt(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
		:	CKeyStoreTestAction(aFs, aConsole, aOut), iPlainTextPtr(0,0)
{}

void CDecrypt::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	SetSignText(Input::ParseElement(aTestActionSpec.iActionBody, KTextStart));
	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;

	iState = EListKeysPreOpen;
}

void CDecrypt::SetSignText(const TDesC8& aText)
{
	iReadText = HBufC8::NewMax(aText.Size());
	if (iReadText)
	{
		TPtr8 theText(iReadText->Des());
		theText.FillZ();
		theText.Copy(aText);
	}
}

////////////////////////////////////
// CDerive
////////////////////////////////////

/*static*/ CTestAction* CDerive::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CTestAction* self = CDerive::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
}

/*static*/ CTestAction* CDerive::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, const TTestActionSpec& aTestActionSpec)
{
	CDerive* self = new (ELeave) CDerive(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
}

CDerive::~CDerive()
	{
	iKeys.Close();
	iN.Close();
	iG.Close();
	if (iDH)
		{
		iDH->Release();
		}
	delete iPublicKey;
	delete iOutput;
	delete iRemote;
	}

void CDerive::PerformAction(TRequestStatus& aStatus)
	{
	// Jump straight to final state if an error occured 
	if (aStatus.Int()!=KErrNone)
		{
		iState=EFinished;
		}
 
	switch (iState)
		{	
		case EListKeysPreOpen:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			if (keyStore)
				keyStore->List(iKeys, iFilter, aStatus);
			
			iState = EOpenKey;
			}
			break;
		
		case EOpenKey:
			{
			CUnifiedKeyStore* keyStore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
			ASSERT(keyStore);	//	Flag it up
			
			TInt keyCount = iKeys.Count();

			TInt i;
			for (i = 0; i < keyCount; i++)
				{
				CCTKeyInfo* keyInfo = iKeys[i];

				if (keyInfo->Label() == *iLabel)
					{
					keyStore->Open(*keyInfo, iDH, aStatus);
					break;
					}
				}
			iState = EExportPublic;
			if (i == keyCount)
				{
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, KErrNotFound);
				}
			}
			break;

		case EExportPublic:
			{
			ASSERT(iDH);
			iDH->PublicKey(iN, iG, iPublicKey, aStatus);
			iState = EDerive;
			}
			break;

		case EDerive:
			{
			if (aStatus.Int()!=KErrNone)
				{
				iState=EFinished;
				TRequestStatus* status = &aStatus;
				User::RequestComplete(status, aStatus.Int());
				break;
				}

			ASSERT(iPublicKey);
			iDH->Agree(iRemote->PublicKey(), iOutput, aStatus);
			
			iState = EFinished;
			}
			break;

		case EFinished:
			{
			if (aStatus.Int()==KErrNone)
				{
				CDH* dh = CDH::NewLC(iRemote->PrivateKey());
				const HBufC8* output = dh->AgreeL(*iPublicKey);
				CleanupStack::PopAndDestroy(dh);
			
				if (aStatus == KErrNone && (!iOutput || 
											*iOutput != *output))
					{
					aStatus = KErrGeneral;	//	Agree failed
					}

				delete const_cast<HBufC8*>(output);
				}
			
			iActionState = EPostrequisite;
			iResult = (aStatus.Int() == iExpectedResult);

			if (aStatus != KErrNone)
				{
				_LIT(KSignFail," !Failed when agreeing key!");
				iOut.writeString(KSignFail);
				}

			TRequestStatus* status = &aStatus;
			User::RequestComplete(status, aStatus.Int());
			}
			break;
		default:
			ASSERT(EFalse);	
		}
	}

void CDerive::PerformCancel()
    {
    CUnifiedKeyStore* keystore = CSharedKeyStores::TheUnifiedKeyStores().operator[](iKeystore);
    ASSERT(keystore);
    
    switch (iState)
        {
        case EOpenKey:
            keystore->CancelList();
            break;

        case EExportPublic:
            keystore->CancelOpen();
            break;
            
        case EDerive:
            keystore->CancelExportPublic();
            break;

        case EFinished:
            ASSERT(iDH);
            iDH->CancelAgreement();
            break;
            
        default:
            break;
        }
    }

void CDerive::Reset()
	{
	iState = EListKeysPreOpen;
	iKeys.Close();
	if (iDH)
		{
		iDH->Release();
		iDH = NULL;
		}
	delete iPublicKey;
	iPublicKey = NULL;
	delete iOutput;
	iOutput = NULL;
	}

void CDerive::DoReportAction()
{
	_LIT(KSigning, "Deriving key... ");
	iOut.writeString(KSigning);
	iOut.writeNewLine();
}

void CDerive::DoCheckResult(TInt aError)
{
	if (iFinished)
	{
		TBuf<256> buf;
		if (aError == KErrNone)
		{
			iOut.writeNewLine();
			_LIT(KSuccessful, "Derive success\n");			
			buf.Format(KSuccessful);
			iConsole.Write(buf);
			iOut.writeString(buf);
			iOut.writeNewLine();
		}
		else
		{
			if (aError!=iExpectedResult)
			{
				_LIT(KFailed, "!!!Derive failure %d!!!\n");
				buf.Format(KFailed, aError);
				iConsole.Write(buf);
				iOut.writeString(buf);
			}
			else
			{
				_LIT(KFailed, "Derive failed, but expected\n");
				iConsole.Write(KFailed);
				iOut.writeString(KFailed);
			}

			iOut.writeNewLine();
		}
	}
}

CDerive::CDerive(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
		:	CKeyStoreTestAction(aFs, aConsole, aOut)
{}

void CDerive::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CKeyStoreTestAction::ConstructL(aTestActionSpec);

	// Set parameters
	HBufC8* nData = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<n>"));
	CleanupStack::PushL(nData);
	iN = RInteger::NewL(*nData);
	CleanupStack::PopAndDestroy(nData);
	HBufC8* gData = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<g>"));
	CleanupStack::PushL(gData);
	iG = RInteger::NewL(*gData);
	CleanupStack::PopAndDestroy(gData);
	
	// Generate 'remote' key pair
	RInteger n = RInteger::NewL(iN);
	CleanupStack::PushL(n);
	RInteger g = RInteger::NewL(iG);
	CleanupStack::PushL(g);
	iRemote = CDHKeyPair::NewL(n, g);
	CleanupStack::Pop(2); // n, g
	
	iFilter.iPolicyFilter = TCTKeyAttributeFilter::EAllKeys;
	
	iState = EListKeysPreOpen;
}
