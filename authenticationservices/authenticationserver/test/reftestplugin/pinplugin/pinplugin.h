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
* CPinPlugin is an CAuthPluginInterface implementation.
*
*/


/**
 @file 
 @internalAll
 @test
*/


#ifndef PIN_PLUGIN_H
#define PIN_PLUGIN_H

#include <barsc.h>
#include <authserver/authplugininterface.h>
#include "pinpluginao.h"

/**
 An implementation of the CAuthPluginInterface definition
 */
class CPinPlugin : public AuthServer::CAuthPluginInterface
	{
public:
	/** static methods to constuct the object of the CPinPlugin */
	static CPinPlugin* NewL();
	static CPinPlugin* NewLC();
	/** Destructor */
	~CPinPlugin();

public:			//from CAuthPluginInterface
	void Identify(AuthServer::TIdentityId& aId, const TDesC& aClientMessage,
				  HBufC8*& aResult, TRequestStatus& aRequest);	
	void Train(AuthServer::TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest);		
	TInt Forget(AuthServer::TIdentityId aId);
	TInt DefaultData(AuthServer::TIdentityId aId, HBufC8*& aOutputBuf);
	TInt Reset(AuthServer::TIdentityId aIdentityId, const TDesC& aRegistrationData, HBufC8*& aResult);
	TBool IsActive() const;
	void Cancel();
	AuthServer::TPluginId Id() const;
	const TPtrC& Name() const;
	const TPtrC& Description() const;
	AuthServer::TEntropy MinEntropy() const;
	AuthServer::TPercentage FalsePositiveRate() const;
	AuthServer::TPercentage FalseNegativeRate() const;
	AuthServer::TAuthPluginType Type() const;
private:
	/** 
	  Second Phase Constructor
	  @panic EPinPanicInvalidConfigValues	if the resource file contains the invalid entries
	  */
	void ConstructL();
	/** Constructor */
	CPinPlugin();
	/**
	 Opens the Resource file and Read the values from it.
	 
	 @param aRetryCount		On return, it holds Retry Count value.
	 @param aPinSize		On return, it holds Default Pin Size.
	 @param aPinMinSize		On return, it holds Minimum Pin Size.
	 @param aPinMaxSize		On return, it holds Maximum Pin Size.
	 */
	void ReadResourceL(TInt& aRetryCount, TInt& aPinSize, TInt& aPinMinSize,
					TInt& aPinMaxSize);
	/**
	 Utility function to give a value for a particular resource Id.
	 
	 @param aResFile		The resource file to be read
	 @param aResourceID		The resource Id to be read from the resource file
	 @return				returns the value read from the resource file.
 	 */
	TInt ResourceValueL(RResourceFile& aResFile, const TInt aResourceID);

	void SetPluginStateL();
	
private:
	/** CPinpluginAO object to process the operation in AO */
	CPinPluginAO* iPinPluginAO;
	/** The name of plugin read from the resource file */
	HBufC* iPinName;
	/** The Description for the plugin read from the resource file */
	HBufC* iPinDescription;
protected:
	/// The name of the plugin. Derived classes should set this member.
	TPtrC iName;
	/// The plugin's description. Derived classes should set this member.
	TPtrC iDescription;
	/// The minumum entropy provided by the plugin. Derived classes should set
	/// this member.
	AuthServer::TEntropy iMinEntropy;
	/// The false positive rate of the plugin. Derived classes should set this
	/// member.
	AuthServer::TPercentage iFalsePositiveRate;
	/// The false negative rate of the plugin. Derived classes should set this
	/// member.
	AuthServer::TPercentage iFalseNegativeRate;
 	/// The type of the plugin. Derived classes should set this member.
	AuthServer::TAuthPluginType iType;
	/// the state of the plugin.
	TBool iActive;
	};

#endif	/* PIN_PLUGIN_H */

