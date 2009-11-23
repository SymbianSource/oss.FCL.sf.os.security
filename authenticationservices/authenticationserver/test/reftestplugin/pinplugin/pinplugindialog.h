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
* CPinPluginDialog declaration
*
*/


/**
 @file 
*/

#ifndef PIN_PLUGINDIALOG_H
#define PIN_PLUGINDIALOG_H

#include <e32base.h>
#include <pinplugindialogdefs.h>

/**
 Definition of the pin plugin dialog interface 
 */
class CPinPluginDialog : public CActive
	{
public:
	/**
	 Prompts the user to enter a pin. 
	 
	 @param aMinLength	The minimum length of the pin.
	 @param aMaxLength	The maximum length of the pin.
	 @param aRetry		Indicates whether the user is retrying.
	 @param aPinValue	On return, It holds the pin that the user entered.
	 @param aResult		On return, It holds output of the dialog
	 @param aStatus		An asynchronous request status object.
	 */
	void PinIdentify(const TInt aMinLength, const TInt aMaxLength, const TBool aRetry, TPinValue& aPinValue,
	                       TPinPluginDialogResult& aResult, TRequestStatus& aStatus);

	/**
	 Prompts the user to select a pin from the list.
	  
	 @param aType			Information about the pin to train/retrain
	 @param aPinNumbers		array of pin number to display
	 @param aMinLength		The minimum length of the pin.
	 @param aMaxLength		The maximum length of the pin.
	 @param aIndex			On return, It holds the index of the selected pin.
	 @param aCurrentLength	By default, It has the current length of the pin
	 						On return, It holds the new length of the pin if user changes.
 	 @param aResult			On return, It holds output of the dialog.
 	 @param aStatus			An asynchronous request status object. 
	 */
	void PinTraining(const TPinPluginTrainingMessage aType, const RPointerArray<TPinValue>& aPinNumbers,
	                        const TInt aMinLength, const TInt aMaxLength, TInt& aIndex, TInt& aCurrentLength,
	                        TPinPluginDialogResult& aResult, TRequestStatus& aStatus);

	/**
	 Display the message to the user. 
	  
	 @param aMessage	Information about the PIN to display.
	 @param aStatus		An asynchronous request status object.	
	 */
	 void PinInfo(const TPinPluginInfoMessage aMessage,TRequestStatus& aStatus);
	
public:
	/**
	 Destructor for the CPinPluginDialog class
	 */
	~CPinPluginDialog();
	/** static methods to constuct the object of the CPinPluginDialog */
	static CPinPluginDialog* NewL();
	static CPinPluginDialog* NewLC();
private:
	CPinPluginDialog();
	void ConstructL();
	/**
	 Helper function to perform basic initialization
	 */
	void DoInitialize(const TPinPluginDialogOperation aOperation, TRequestStatus& aStatus);
	/**
	 Helper function to Issue Notifier request for the Dialog.
	 */
	void RequestDialog(const TDesC8& aData, TDes8& aResponse);

private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	enum TState 
		{ 	
		KPinPluginInfo,
		KPinPluginTraining,
		KPinPluginIdentify
		};

	TPinPluginDialogOperation iCurrentOperation;
	TRequestStatus* iClientStatus;
	RNotifier iNotifier;
	TState iState;

	TPinIdentifyInputBuf iPinIdentifyInputBuf;
	TPinTrainingInputBuf iPinTrainingInputBuf;
	TPinInfoInputBuf iPinInfoInputBuf;
	TPinDialogOutputBuf iPinDialogOutputBuf;
	TInt* iClientPinIndex;
	TPinValue* iClientPinValue;
	TInt* iClientNewPinLength;
	TPinPluginDialogResult* iClientPinDialogResult;
	};


#endif /* PIN_PLUGINDIALOG_H */

