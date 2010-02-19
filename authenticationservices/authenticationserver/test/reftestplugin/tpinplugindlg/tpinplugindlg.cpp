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
* Pinplugindlg.cpp
*
*/


#include "tpinplugindlg.h"
#include <s32file.h>

const TInt KErrInvalidPinLength			= -1002;
_LIT(KPinPluginDailogPanicString,"CTestPinPluginDlgNotifier");

// Lib main entry point.

#ifdef _T_PINPLUGINDLG_TEXTSHELL
EXPORT_C CArrayPtr<MNotifierBase2>* NotifierArray()
#else
CArrayPtr<MNotifierBase2>* NotifierArray()
#endif
	{
	CArrayPtrFlat<MNotifierBase2>* subjects = NULL;
	CTestPinPluginDlgNotifier* notifier;
	TRAPD(err,
		subjects = new (ELeave) CArrayPtrFlat<MNotifierBase2>(1);
		CleanupStack::PushL(subjects);
		notifier = CTestPinPluginDlgNotifier::NewL();
		CleanupStack::PushL(notifier);
		subjects->AppendL(notifier);
		CleanupStack::Pop(2,subjects));	//notifier, subjects
	if (err != KErrNone)
		{
		subjects = NULL;
		}
	return subjects;
	}

// Ecom plugin implementation for UI notifier

#ifndef _T_PINPLUGINDLG_TEXTSHELL

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KTPinPluginDlgNotiferUid, NotifierArray)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return (ImplementationTable);
	}

#endif

// TInputDetails
//

TInputDetails::TInputDetails(TPinPluginDialogOperation aOp, TInt aIndex, TInt aNewPinLength, TPinPluginDialogResult aCommandOutput) :
	iOp(aOp), iPinValue(0), iIndex(aIndex), iNewPinLength(aNewPinLength), iCommandOutput(aCommandOutput)
	{
	}

TInputDetails::TInputDetails(TPinPluginDialogOperation aOp, TPinValue& aPinValue, TPinPluginDialogResult aCommandOutput) :
	iOp(aOp), iPinValue(aPinValue), iCommandOutput(aCommandOutput)
	{
	}


// CTestPinPluginDlgNotifier
//

_LIT(KInputFile, "C:\\t_pinplugindlg_in.dat");
_LIT(KOutputFile, "C:\\t_pinplugindlg_out.dat");

CTestPinPluginDlgNotifier* CTestPinPluginDlgNotifier::NewL()
	{
	CTestPinPluginDlgNotifier* self=new (ELeave) CTestPinPluginDlgNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestPinPluginDlgNotifier::CTestPinPluginDlgNotifier()
	{
	iInfo.iUid = KUidPinPluginDialogNotifier;
	iInfo.iChannel = TUid::Uid(0x00001200); // test uid
	iInfo.iPriority = ENotifierPriorityHigh;
	}

void CTestPinPluginDlgNotifier::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}

TInt CTestPinPluginDlgNotifier::ReadDialogCountL()
	{
	RFileReadStream stream;
	TInt err = stream.Open(iFs, KOutputFile, EFileRead | EFileShareExclusive);
	// If the file doesn't exist yet just return zero
	if (err == KErrNotFound)
		{
		return 0;
		}
	User::LeaveIfError(err);
	stream.PushL();
	TInt dialogCount = stream.ReadInt32L();
	CleanupStack::PopAndDestroy(&stream); 
	return dialogCount;
	}

TPinValue CTestPinPluginDlgNotifier::ReadPinValueL()
	{
	TPinValue pinValue;
	RFileReadStream stream;
	TInt err = stream.Open(iFs, KOutputFile, EFileRead | EFileShareExclusive);
	// If the file doesn't exist yet just return zero
	if (err == KErrNotFound)
		{
		pinValue.Copy(_L("0"));
		return pinValue;
		}
	User::LeaveIfError(err);
	stream.PushL();
	stream.ReadInt32L();
	TInt pinValueSize = stream.ReadInt32L();
	HBufC8* pinValueBuf = HBufC8::NewMaxLC(pinValueSize);
	TPtr8 pinValuePtr(pinValueBuf->Des());
	stream.ReadL(pinValuePtr, pinValueSize);
	pinValue.Copy(pinValuePtr);
	CleanupStack::PopAndDestroy(2, &stream);	
	return pinValue;
	}
	
void CTestPinPluginDlgNotifier::WriteDialogCountL(TInt aCount)
	{
	RFileWriteStream stream;
	TInt err = stream.Replace(iFs, KOutputFile, EFileWrite | EFileShareExclusive);
	if (err == KErrNotFound)
		{
		err = stream.Create(iFs, KOutputFile, EFileWrite | EFileShareExclusive);
		}
	User::LeaveIfError(err);
	stream.PushL();
	stream.WriteInt32L(aCount);
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream); 
	}

void CTestPinPluginDlgNotifier::WritePinL(const TPinValue& aPinValue)
	{
	TInt dialogCount = ReadDialogCountL();

	RFileWriteStream stream;
	TInt err = stream.Replace(iFs, KOutputFile, EFileWrite | EFileShareExclusive);
	if (err == KErrNotFound)
		{
		err = stream.Create(iFs, KOutputFile, EFileWrite | EFileShareExclusive);
		}
	User::LeaveIfError(err);
	stream.PushL();
	stream.WriteInt32L(dialogCount);
	stream.WriteInt32L(aPinValue.Length());
	stream.WriteL(aPinValue);
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream); 
	}
	
void CTestPinPluginDlgNotifier::WriteMsgL(const TPinPluginInfoMessage& aMessage)
	{
	TInt dialogCount = ReadDialogCountL();
	TPinValue pinValue = ReadPinValueL();
	
	RFileWriteStream stream;
	TInt err = stream.Replace(iFs, KOutputFile, EFileWrite | EFileShareExclusive);
	if (err == KErrNotFound)
		{
		err = stream.Create(iFs, KOutputFile, EFileWrite | EFileShareExclusive);
		}
	User::LeaveIfError(err);
	stream.PushL();
	stream.WriteInt32L(dialogCount);
	stream.WriteInt32L(pinValue.Length());
	stream.WriteL(pinValue);
	stream.WriteInt32L(aMessage);
	stream.CommitL();
	CleanupStack::PopAndDestroy(&stream); 
	}
	
TInputDetails* CTestPinPluginDlgNotifier::ReadInputDetailsL(TUint aOperation)
	{
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(iFs, KInputFile, EFileRead | EFileShareExclusive));
	stream.PushL();

	TPinValue pinvalue;
	TInt index, newLength,dialogNumber;
	TInt pinValueSize;

	TPinPluginDialogOperation op = static_cast<TPinPluginDialogOperation>(stream.ReadInt32L());
	
	pinValueSize = stream.ReadInt32L();
	HBufC8* pinValueBuf = HBufC8::NewMaxLC(pinValueSize);
	TPtr8 pinValuePtr(pinValueBuf->Des());
	stream.ReadL(pinValuePtr, pinValueSize);
	pinvalue.Copy(pinValuePtr);
	
	index = stream.ReadInt32L();
	if (index < 0 && index > 3)
		{
		index = 0;
		}
	newLength = stream.ReadInt32L();
	
	TPinPluginDialogResult outputCommand = static_cast<TPinPluginDialogResult>(stream.ReadInt32L());
	TPinPluginDialogOperation op1 = static_cast<TPinPluginDialogOperation>(stream.ReadInt32L());
	TPinPluginDialogResult finalOutputCommand = static_cast<TPinPluginDialogResult>(stream.ReadInt32L());
	dialogNumber = stream.ReadInt32L();
	if (op != aOperation)
		{
		op = op1;
		}
	else
		{
		// Update dialog count here so test code can see how many dialogs were
		// requested if there were more than expected
		TInt dialogCount = ReadDialogCountL() + 1;
		WriteDialogCountL(dialogCount);
		if (dialogNumber == dialogCount)
			{
			outputCommand = finalOutputCommand;
			}
		}
	
	TInputDetails* inputDetail = NULL;
	if (op == EPinPluginTraining)
		{
		inputDetail = new (ELeave) TInputDetails(op, index, newLength, outputCommand);
		}
	else if (op == EPinPluginIdentify) 
		{
		inputDetail = new (ELeave) TInputDetails(op, pinvalue, outputCommand);
		}
	CleanupStack::PopAndDestroy(1, pinValueBuf);	
	CleanupStack::PopAndDestroy(&stream); 

	return inputDetail;
	}


void CTestPinPluginDlgNotifier::DoSelectPinL(const TInputDetails& aDetail, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	const TPinTrainingInput& input = reinterpret_cast<const TPinTrainingInput&>(*aBuffer.Ptr());

	TPinDialogOutput output;
	output.iIndex = aDetail.Index();
	TInt pinLength = aDetail.NewPinLength();
	if (pinLength == 0)
		{
		output.iNewPinLength = input.iCurrentPinLength;
		}
	else
		{
		output.iNewPinLength = pinLength;
		}
	output.iResult = aDetail.CommandOutput();
	
	TPinValue pinValue;
	switch(output.iIndex)
		{
		case 0:
			pinValue = input.iPinValue1;
			break; 
		case 1:
			pinValue = input.iPinValue2;
			break; 
		case 2:
			pinValue = input.iPinValue3;
			break; 
		case 3:
			pinValue = input.iPinValue4;
			break; 
		}
	WritePinL(pinValue);
	TPinDialogOutputBuf outputBuf(output);
	aMessage.WriteL(aReplySlot, outputBuf);
	}

void CTestPinPluginDlgNotifier::DoIdentifyPinL(const TInputDetails& aDetail, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	const TPinIdentifyInput& input = reinterpret_cast<const TPinIdentifyInput&>(*aBuffer.Ptr());

	TPinDialogOutput output;
	const TPinValue& pinInput = reinterpret_cast<const TPinValue&>(aDetail.PinValue());
	TInt pinLength = pinInput.Length();
	if (pinLength >= input.iMinLength && pinLength <= input.iMaxLength)
		{
		output.iPinValue = pinInput;
		}
	else
		{
		User::Leave(KErrInvalidPinLength);	
		}
	
	WritePinL(pinInput);
	output.iResult = aDetail.CommandOutput();
	TPinDialogOutputBuf outputBuf(output);
	aMessage.WriteL(aReplySlot, outputBuf);
	}

void CTestPinPluginDlgNotifier::DoInfoPinL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	const TPinInfoInput& input = reinterpret_cast<const TPinInfoInput&>(*aBuffer.Ptr());
	WriteMsgL(input.iMessage);
	
	TPinDialogOutput output;
	TPinDialogOutputBuf outputBuf(output);
	aMessage.WriteL(aReplySlot, outputBuf);
	}

void CTestPinPluginDlgNotifier::Release()
	{
	delete this;
	}

CTestPinPluginDlgNotifier::TNotifierInfo CTestPinPluginDlgNotifier::RegisterL()
	{
	return iInfo;
	}

CTestPinPluginDlgNotifier::TNotifierInfo CTestPinPluginDlgNotifier::Info() const
	{
	return iInfo;
	}

void CTestPinPluginDlgNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	TRAPD(err, DoStartL(aBuffer, aReplySlot, aMessage));
	aMessage.Complete(err);
	}
	
void CTestPinPluginDlgNotifier::DoStartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	TUint operation = *reinterpret_cast<const TInt *>(aBuffer.Ptr()) & KPinPluginDialogOperationMask;
	
	TInputDetails* detail = NULL;

	TRAPD(err, detail = ReadInputDetailsL(operation));
	
	User::LeaveIfError(err);

	CleanupStack::PushL(detail);
	
	switch (operation)
		{
		case EPinPluginTraining:
			DoSelectPinL(*detail, aBuffer, aReplySlot, aMessage);
			break;

		case EPinPluginIdentify:
			DoIdentifyPinL(*detail, aBuffer, aReplySlot, aMessage);
			break;
		case EPinPluginInfo:
			DoInfoPinL(aBuffer, aReplySlot, aMessage);
			break;
		default:
			User::Panic(_L("CTestPinPluginDlgNotifier"), 0);
		}
	CleanupStack::PopAndDestroy(detail);
	}

TPtrC8 CTestPinPluginDlgNotifier::StartL( const TDesC8& /*aBuffer*/ )
	{
	User::Panic(_L("CTestPinPluginDlgNotifier"), 0);
	return TPtrC8(KNullDesC8);
	}

void CTestPinPluginDlgNotifier::Cancel()
	{
	// Don't think there is much we can do here. If a client deletes the
	// client-side pinplugin dialog instance, after calling a method that 
	// displays a dialog, this will not get called until the user dismisses
	// the dialog. We can't do anything then.
	}

TPtrC8 CTestPinPluginDlgNotifier::UpdateL( const TDesC8& /*aBuffer*/ )
	{
	User::Panic(KPinPluginDailogPanicString, 0);
	return NULL;
	}


