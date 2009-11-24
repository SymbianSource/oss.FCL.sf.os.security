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
* CPinPluginDialog implementation 
*
*/


/**
 @file 
*/

#include "pinplugindialog.h"
#include <authserver/auth_srv_errs.h>

CPinPluginDialog::~CPinPluginDialog()
	{
	Deque();
	iNotifier.Close();
	}

CPinPluginDialog* CPinPluginDialog::NewL()
	{
	CPinPluginDialog* self = CPinPluginDialog::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CPinPluginDialog* CPinPluginDialog::NewLC()
	{
	CPinPluginDialog* self = new (ELeave) CPinPluginDialog();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPinPluginDialog::CPinPluginDialog(): CActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

void CPinPluginDialog::ConstructL()
	{
	User::LeaveIfError(iNotifier.Connect());
	}

void CPinPluginDialog::PinIdentify(const TInt aMinLength, const TInt aMaxLength, const TBool aRetry,
						TPinValue& aPinValue, TPinPluginDialogResult& aResult, TRequestStatus& aStatus)
	{
	iClientPinValue = &aPinValue;
	iClientPinDialogResult = &aResult;
	iState = KPinPluginIdentify;
	DoInitialize(EPinPluginIdentify, aStatus);
	TPinIdentifyInput pinInput;
	pinInput.iOperation = EPinPluginIdentify;
	pinInput.iRetry = aRetry;
	pinInput.iMinLength = aMinLength;
	pinInput.iMaxLength = aMaxLength;
	TPinIdentifyInputBuf pinInputBuf(pinInput);
	iPinIdentifyInputBuf.Copy(pinInputBuf);
	RequestDialog(iPinIdentifyInputBuf, iPinDialogOutputBuf);
	}



void CPinPluginDialog::PinTraining(const TPinPluginTrainingMessage aType, const RPointerArray<TPinValue>& aPinNumbers,
	                        const TInt aMinLength, const TInt aMaxLength, TInt& aIndex, TInt& aCurrentLength,
	                        TPinPluginDialogResult& aResult, TRequestStatus& aStatus)
	{
	iClientPinIndex = &aIndex;
	iClientPinDialogResult = &aResult;
	iClientNewPinLength = &aCurrentLength;
	iState = KPinPluginTraining;
	DoInitialize(EPinPluginTraining, aStatus);
	TPinTrainingInput pinInput;
	pinInput.iOperation = EPinPluginTraining;
	pinInput.iMessage = aType;
	pinInput.iMinLength = aMinLength;
	pinInput.iMaxLength = aMaxLength;
	pinInput.iCurrentPinLength = aCurrentLength;
	pinInput.iPinValue1 = *(aPinNumbers[0]);
	pinInput.iPinValue2 = *(aPinNumbers[1]);
	pinInput.iPinValue3 = *(aPinNumbers[2]);
	pinInput.iPinValue4 = *(aPinNumbers[3]);
	TPinTrainingInputBuf pinInputBuf(pinInput);
	iPinTrainingInputBuf.Copy(pinInputBuf);
	RequestDialog(iPinTrainingInputBuf, iPinDialogOutputBuf);
	}


void CPinPluginDialog::PinInfo(const TPinPluginInfoMessage aMessage, TRequestStatus& aStatus)
	{
	iState = KPinPluginInfo;
	DoInitialize(EPinPluginInfo, aStatus);
	TPinInfoInput pinInput;
	pinInput.iOperation = EPinPluginInfo;
	pinInput.iMessage = aMessage;
	TPinInfoInputBuf pinInputBuf(pinInput);
	iPinInfoInputBuf.Copy(pinInputBuf);
	RequestDialog(iPinInfoInputBuf, iPinDialogOutputBuf);
	}

void CPinPluginDialog::DoInitialize(const TPinPluginDialogOperation aOperation, TRequestStatus& aStatus)
	{
	iCurrentOperation = aOperation;

	aStatus = KRequestPending;
	iClientStatus = &aStatus;
	}


void CPinPluginDialog::RequestDialog(const TDesC8& aData, TDes8& aResponse)
	{
	iNotifier.StartNotifierAndGetResponse(iStatus, KUidPinPluginDialogNotifier,
											aData, aResponse);
	SetActive();
	}
	
void CPinPluginDialog::DoCancel()
	{
	iNotifier.CancelNotifier(KUidPinPluginDialogNotifier);
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}

void CPinPluginDialog::RunL()
	{
	switch (iState)
		{
		case KPinPluginInfo:
			{
			//ignore the output as Info Dialog won't wait for user inputs
			}
			break;
		case KPinPluginTraining:
			{
			TPinDialogOutput& pinDialogOutput = iPinDialogOutputBuf();
			*iClientPinIndex = pinDialogOutput.iIndex;
			*iClientNewPinLength = pinDialogOutput.iNewPinLength;
			*iClientPinDialogResult = pinDialogOutput.iResult;
			if(iStatus.Int() != KErrNone)
				{
				User::RequestComplete(iClientStatus, KErrAuthServRegistrationFailed);
				return;
				}
			}
			break;
		case KPinPluginIdentify:
			{
			TPinDialogOutput& pinDialogOutput = iPinDialogOutputBuf();
			*iClientPinValue = pinDialogOutput.iPinValue;
			*iClientPinDialogResult = pinDialogOutput.iResult;
			}
			break;
		default:
			{
			User::Leave(KErrNotSupported);
			}
			break;
		}
	User::RequestComplete(iClientStatus, iStatus.Int());
	}
	
TInt CPinPluginDialog::RunError(TInt aError)
	{
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, aError);
		}
	return KErrNone;
	}








