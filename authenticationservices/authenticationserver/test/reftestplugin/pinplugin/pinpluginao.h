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
* CPinPluginAO declaration
*
*/


/**
 @file 
 @internalComponent
 @released
*/

#ifndef PIN_PLUGINAO_H
#define PIN_PLUGINAO_H

#include "pinplugindialog.h"
#include "pinplugindb.h"
#include "pinpanic.h"

class CPinPluginAO : public CActive
	{
public:
	/** static methods to constuct the object of the CPinpluginAO */
	static CPinPluginAO* NewL(TInt aPinSize, TInt aPinMinSize, TInt aPinMaxSize, TInt aRetryCount);
	static CPinPluginAO* NewLC(TInt aPinSize, TInt aPinMinSize, TInt aPinMaxSize, TInt aRetryCount);
	/** Destructor */
	~CPinPluginAO();
public: //CAuthPluginInterface
	void Identify(AuthServer::TIdentityId& aId, const TDesC& aClientMessage,
				  HBufC8*& aResult, TRequestStatus& aRequest);
	void Train(AuthServer::TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest);
	TInt Forget(AuthServer::TIdentityId aId);
	TInt DefaultData(AuthServer::TIdentityId aId, HBufC8*& aOutputBuf);
	void ResetL(AuthServer::TIdentityId aId, const TDesC& aRegistrationData, HBufC8*& aResult);

private:			//Active
	void DoCancel();
	void RunL();	
	TInt RunError(TInt aError);
private:
	/** Constructor */
	CPinPluginAO(TInt aPinSize, TInt aPinMinSize, TInt aPinMaxSize, TInt aRetryCount);
	
	/** Second Phase Constructor */
	void ConstructL();
	
	/**
	 Issue a Request for Identify Dialog upto iRetryCount times to 
	 PinPluginDialog. If the pinvalue received is wrong for all the
	 retries, reset the identityId as KUnknownIdentity.
	 */
	void IdentifyId();
	
	/**
 	 Issue a Request for Train Dialog to PinPluginDialog. 
 	 It will generate 4 random pin of length iNewPinSize and send 
 	 the pin Numbers and iNewPinSize to PinpluginDialog. Once it completed
 	 it have the selected pin index and new pin size which is used to generate
 	 another set of pin numbers for the next dialogs if required.

	 @panic EPinPanicInvalidPinLength	if plugin receives invalid pinlength fron Dialog Notifier
	 */
	void TrainIdL();
	
	/**
 	 It will generate KeyHash for the pin number given in the Identify Dialog.
 	 and verify the keyHash and Id with DB entries. If same, the state will be
 	 changed to EInfo and message is set to EPinPluginIdentificationSuccess 
 	 which is used to display info message to the user. 
 	 If not, same state will be maintained so that it will ask another Identify
 	 dialog to the user.
 	 */
	void IdentifyResultL();
	
	/**
	 It will add/update the DB with the pin selected in Pin Train/ReTrain Dialog.
	 
	 @panic EPinPanicIncorrectIndex		if plugin receives index values otherthan 0 -3
	 */
	void AddTrainResultToDBL();
	
	/**
 	 It will generate two KeyHash for the pin value using SHA1 Algorithm
 	 Intermediate result will return to the Authentication server whereas
 	 final result is stored in the DB.
 	  
	 @param aPinValue		The Pin number for which Hash value required
	 @param aIdentityKey	aIdentityKey is updated with intermediate KeyHash generated.
	 @return				It will return the final KeyHash Value generated
	 */
	HBufC8* GenerateKeyHashL(TPinValue& aPinValue, HBufC8*& aIdentityKey);
	
	/**
	 It will generate four Unique Pins which is not used any of the
	 existing identities.
	 
	 @param aPinList		On return, it holds the array of Pin numbers generated
	 @param idKeyList		On return, it holds the array of intermediate KeyHash 
	 						values for the Pin Numbers in the aPinList
	 @param idKeyHashList	On return, it holds the array of Final KeyHash values
	 						for the Pin Numbers in the aPinList
 	 */
	void GenerateUniquePinsL(RPointerArray<TPinValue>& aPinList, RPointerArray<HBufC8>& idKeyList,
								 RPointerArray<HBufC8>& idKeyHashList);
private:
	enum TProcessState
		{
		EIdentify,
		EIdentifyResult,
		ETrain,
		ETrainResult,
		EInfo,
		EFinished
		};
	CPinPluginDialog* iPinPluginDialog;
	CPinPluginDB* iPinPluginDb;
	TRequestStatus* iRequestStatus;
	TProcessState iState;
	TInt iIndex;
	TPinValue iPinValue;
	AuthServer::TIdentityId iIdentityId;
	AuthServer::TIdentityId* iIdentityIdPtr;
	HBufC8** iResult;
	TPinPluginDialogResult* iDialogResult;
	TPinPluginInfoMessage iMessage;
	
	TInt iRetryCount;
	TInt iRetryRefCount;
	TInt iPinSize;
	TInt iPinMinSize;
	TInt iPinMaxSize;
	TInt iNewPinSize;
		
	RPointerArray<TPinValue> iPinList;
	RPointerArray<HBufC8> iIdKeyList;
	RPointerArray<HBufC8> iIdKeyHashList;
	
	//Displayable text string
	const HBufC* iClientMessage;

	};

#endif /* PIN_PLUGINAO_H */
	
