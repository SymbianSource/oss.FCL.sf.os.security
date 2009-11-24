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


#ifndef __TPINPLUGINDLG_H__
#define __TPINPLUGINDLG_H__

#include <e32base.h>
#include <pinplugindialogdefs.h>

const TUint KTPinPluginDlgNotiferUid = 0x200032E7;

#ifdef _T_PINPLUGINDLG_TEXTSHELL

#include <twintnotifier.h>

/** Method at ordinal 1 to get a list of notifiers from this dll. */
IMPORT_C CArrayPtr<MNotifierBase2>* NotifierArray();

#else

#include <eiknotapi.h>
#include <ecom/ImplementationProxy.h>
#define MNotifierBase2 MEikSrvNotifierBase2

IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);
CArrayPtr<MNotifierBase2>* NotifierArray();

#endif

/**
 It reads the Input datas from t_pinplugindlg_in.dat and constructs
 TInputDetails to use in CTestPinPluginDlgNotifier.
 */
class TInputDetails
	{
 public:
 	/**
 	 Constructs the object while the request is Train/ReTrain.
 	 */
	TInputDetails(TPinPluginDialogOperation aOp, TInt aIndex, TInt aNewPinLength, TPinPluginDialogResult aCommandOutput);
	/**
 	 Constructs the object while the request is Identify.
 	 */
	TInputDetails(TPinPluginDialogOperation aOp, TPinValue& aPinValue, TPinPluginDialogResult aCommandOutput);
	/** Returns the current process Dialog Notification Operation */
	TPinPluginDialogNotification Operation() const { return iOp; }
	/** Returns the current process Pin Value Selected/Entered. */
	const TPinValue& PinValue() const { return iPinValue; }
	/** Returns the current process selected index. */
	TInt Index() const { return iIndex; }
	/** Returns the current process New pin Length given. */
	TInt NewPinLength() const { return iNewPinLength; }
	/** Returns the current process Dialog output. */
	TPinPluginDialogResult CommandOutput() const { return iCommandOutput; }
 private:
	TPinPluginDialogNotification iOp;
	TPinValue iPinValue;
	TInt iIndex;
	TInt iNewPinLength;
	TPinPluginDialogResult iCommandOutput;
	TPinPluginInfoMessage iExpectedMsg;
	};

/**
 This is a test implementation of the pinplugin dialog notifier. This version
 responds to dialogs with data read from c:\t_pinplugindlg_in.dat. This contains
 the expected input for the dialog. It keeps a number of dialog displayed
 and pin selected for the Train/ReTrain or pin given in Identify Dialog and
 displayed info message in c:\t_pinplugindlg_out.dat.
 */
class CTestPinPluginDlgNotifier : public CBase, public MNotifierBase2
	{
public:
	static CTestPinPluginDlgNotifier* NewL();

private:
	void ConstructL();
	CTestPinPluginDlgNotifier();

	/**
	 Read the Number of Dialog displayed already from t_pinplugindlg_out.dat.
	 */
	TInt ReadDialogCountL();
	/**
	 Read the PinValue Selected/Entered in the last dialog request from
	 t_pinplugindlg_out.dat.
	 */
	TPinValue ReadPinValueL();
	/**
	 Write the dialog count to the output file t_pinplugindlg_out.dat.
	 
	 @param aCount		The Count of the Current Dialog.
	 */
	void WriteDialogCountL(TInt aCount);
	/**
	 Write the PinValue to the output file t_pinplugindlg_out.dat.

 	 @param aPinValue		The pinvalue to be write.
	 */
	void WritePinL(const TPinValue& aPinValue);
	/**
	 Write the info message to the output file t_pinplugindlg_out.dat.
	 
	 @param aMessage		The message to be write.
	 */
	void WriteMsgL(const TPinPluginInfoMessage& aMessage);
	
	/**
	 Read an input specification.
	 
	 @param aOperation		Consturct the InputDetails for the aOperation.
	 */
	TInputDetails* ReadInputDetailsL(TUint aOperation);
	
	/**
	 Construct the output Details from aDetail and complete the RMessagePtr2
	 with the details.
	 
	 @param aDetail		The input Details read from .dat file for this dialog
	 @param aBuffer		Data that can be passed from the client-side.
	 @param aReplySlot	Identifies which message argument to use for the reply.
	 @param aMessage	Encapsulates a client request.
	 */
	void DoSelectPinL(const TInputDetails& aDetail, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void DoIdentifyPinL(const TInputDetails& aDetail, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void DoInfoPinL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
public: 		// from MEikSrvNotifierBase2
	void DoStartL( const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage );

private:     	// from MEikSrvNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	
	virtual void StartL( const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage );
	virtual TPtrC8 StartL( const TDesC8& aBuffer );
	
	void Cancel();
	TPtrC8 UpdateL( const TDesC8& aBuffer );

private:
	RFs iFs;
	TNotifierInfo iInfo;
	};

#endif // __TPINPLUGINDLG_H__
