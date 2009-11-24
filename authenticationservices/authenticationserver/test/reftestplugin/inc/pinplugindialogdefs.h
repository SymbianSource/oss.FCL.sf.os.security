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


/**
 @file 
 @internalAll
 @test
*/

#ifndef __PINPLUGINDIALOGDEFS_H__
#define __PINPLUGINDIALOGDEFS_H__

/** PinPlugin Dialog Notifier Uid */
const TUid KUidPinPluginDialogNotifier = { 0x200032E6 };
/** Pinplugin Dialog Notification for the DialogOperation */
typedef TUint TPinPluginDialogNotification;


/**
 Defines the operations provided by the Pin Plugin dialog. 
 It is used to identify the operation when transferring over to the notifier plugin.
 */
enum TPinPluginDialogOperation
	{
	/** Displays the message about Training/Retraining/Identify process met success/failure */
	EPinPluginInfo = 0x100,
	/** Train/ReTrain Pin */
	EPinPluginTraining = 0x102,
	/** Identify Pin */
	EPinPluginIdentify = 0x104
	};
	
/** Pinplugin Dialog Operation Bit Mask */
const TUint KPinPluginDialogOperationMask = 0xFFFF;

/** 
 Pinplugin Info Message display about the result for few seconds
 without expecting the user input.
 */
enum TPinPluginInfoMessage 
	{
	/** PinPlugin TrainingSuccess Message */
	EPinPluginTrainingSuccess = 0,
	/** PinPlugin ReTrainingSuccess Message */
	EPinPluginReTrainingSuccess,
	/** PinPlugin IdentificationSuccess Message */
	EPinPluginIdentificationSuccess,
	/** PinPlugin IdentificationFailure Message */
	EPinPluginIdentificationFailure
	};

/** PinPlugin TrainingMessage display the process is Train/ReTrain */
enum TPinPluginTrainingMessage
	{
	/** ETraining */
	ETraining = 0x10,
	/** EReTraining */
	EReTraining
	};
	
/** PinPlugin DialogResult used to get the result of the Dialog */
enum TPinPluginDialogResult
	{
	/** Command OK */
	EOk = 0x01,
	/** Command CANCEL */
	ECancel,
	/** Command QUIT */
	EQuit,
	/** Command NEXT */
	ENext
	};

/** The max Pin length should not exceed 32, because this is the maximum
 *	size possible in the CEikSecretEditor class. */
const TInt KMaxPinLength = 32;

/** A Pin value */
typedef TBuf8<KMaxPinLength> TPinValue;


/**  Input data for the PinIdentify */
class TPinIdentifyInput
	{
public:
	TPinPluginDialogNotification iOperation;

	/** minimum pin length */
	TInt iMinLength;
	/** maximum pin length */
	TInt iMaxLength;
	/** retry true or not */
	TBool iRetry;
	};

/** Pin IdentifyInput Buffer */
typedef TPckgBuf<TPinIdentifyInput> TPinIdentifyInputBuf;

/** Input data for the PinTraining */
class TPinTrainingInput
	{
public:
	TPinPluginDialogNotification iOperation;
	TPinPluginTrainingMessage iMessage;
	
	/** minimum pin length */
	TInt iMinLength;
	/** maximum pin length */
	TInt iMaxLength;
	/** current pin length used for the pinvalues */
	TInt iCurrentPinLength;
	/** Random PinValue for Choice 1 */
	TPinValue iPinValue1;
	/** Random PinValue for Choice 2 */
	TPinValue iPinValue2;
	/** Random PinValue for Choice 3 */
	TPinValue iPinValue3;
	/** Random PinValue for Choice 4 */
	TPinValue iPinValue4;
	};

/** Pin TrainingInput Buffer */
typedef TPckgBuf<TPinTrainingInput> TPinTrainingInputBuf;

/** Input data for the PinInfo */
class TPinInfoInput
	{
public:
	TPinPluginDialogNotification iOperation;
	TPinPluginInfoMessage iMessage;
	};

/** The buffer holds the Message about the result of the Training/ReTraining success
    or Identification success/failure for few seconds without expecting the user input. */
typedef TPckgBuf<TPinInfoInput> TPinInfoInputBuf;


/** Output data for the PinIdentify, PinTraining */
class TPinDialogOutput
	{
public:
	/** iPinValue - To get the pinvalue when case is PinIdentify */
	TPinValue iPinValue;
	/** 
	 iIndex - To get the index of selected pin when case is PinTraining 
	 iIndex value should be within the range 0 - 3.
	 */
	TInt iIndex;
	/** iNewPinLength - To get the new pin length for the next dialog */
	TInt iNewPinLength; 
	/** iResult - To get the output command when case is PinTraining / PinIdentify */
	TPinPluginDialogResult iResult;
	};

/** The buffer holds the data for the TPinDialogOutput */
typedef TPckgBuf<TPinDialogOutput> TPinDialogOutputBuf;


#endif // __PINPLUGINDIALOGDEFS_H__
