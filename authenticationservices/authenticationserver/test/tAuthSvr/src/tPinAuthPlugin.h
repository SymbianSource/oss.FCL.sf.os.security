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
* CTPinAuthPlugin is an interface implementation for CAuthPluginInterface
*
*/



#ifndef TPINAUTHPLUGIN_H
#define TPINAUTHPLUGIN_H

#include "authserver_client.h"
#include <authserver/authplugininterface.h>



//An implementation of the CAuthPluginInterface definition

using namespace AuthServer;
	
//implementation_uid = 0x10204F10
class CTPinAuthPlugin : public CAuthPluginInterface
	{
public:
	// Two phase constructor
	static CTPinAuthPlugin* NewL();
	// Destructor
	virtual ~CTPinAuthPlugin();
	//Implementation of CAuthPluginInterface definitions using a PIN system
	virtual void Identify(TIdentityId& aId, const TDesC& aClientMessage, HBufC8*& aResult, TRequestStatus& aRequest);	
	virtual void Cancel();	
	virtual void Train(TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest);		
	virtual TBool IsActive() const;
	virtual TInt Forget(TIdentityId aId);
	virtual TInt DefaultData(TIdentityId aId, HBufC8*& aOutputBuf);
	virtual TInt Reset(TIdentityId aId, const TDesC& aRegistrationData, HBufC8*& aResult);
	//virtual TPluginId Id() const;
	virtual const TPtrC& Name() const;
	virtual const TPtrC& Description() const;
	virtual AuthServer::TEntropy MinEntropy() const;
	virtual AuthServer::TPercentage FalsePositiveRate() const;
	virtual AuthServer::TPercentage FalseNegativeRate() const;
	virtual AuthServer::TAuthPluginType Type() const;
		
	// Utility functions
	TInt IdReadUserInput(TBuf8<32>& inputValue);
	TInt TrainReadUserInput(TBuf8<32>& inputValue);
	TInt CheckForStringPresence(TIdentityId& aId, TBuf8<32> aInputValue, TRequestStatus& aRequestValue);
	TInt CheckForNewStringPresenceL(TIdentityId aId, TBuf8<32> aInputValue, TRequestStatus& aRequestValue);
	TInt FindStringAndRemoveL(TIdentityId aId);
	TInt AddToGlobalDb (TIdentityId aId, TBuf8<32> aInputValue);
	TInt RemoveFromGlobalDb (TIdentityId aId);
	const HBufC* ClientMessage ();
	

public:
	mutable TBuf<70> iFileToRead;
	void ConstructL();
	CTPinAuthPlugin();
	void ReloadAllFromIniFile();
	void ReloadActiveStateFromIniFile() const;
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

private:
	// Data read from file determining if this plugin supports default data
	TBool iSupportsDefaultData;
	mutable TBool iActiveState;
	HBufC* iClientMessage;
	};


//=========================================================================	
// The various classes that are spawned from this base class by inheriting the implementations besides
// the Id() function (Uses the #define macro)

// number is hex, type is one of the TAuthPluginType enums
// Id() returns the Id of the plugin


#define NClass(id,type) class CTPinAuthPlugin##id : public CTPinAuthPlugin\
	{                                        		\
public:                                      		\
	typedef CTPinAuthPlugin##id TMyPinType;	    	\
	TPluginId Id() const 		               		\
		{ return 0x##id; }  	               		\
		const TPtrC& Name() const					\
		  {											\
		  return iName;								\
		  }											\
		const TPtrC& Description() const			\
		  {											\
		  return iDescription;						\
		  }											\
		AuthServer::TAuthPluginType Type() const 	\
		  {											\
		  return iType;								\
		  }											\
		AuthServer::TEntropy MinEntropy() const		\
		  {											\
		  return iMinEntropy;						\
		  }											\
		AuthServer::TPercentage FalsePositiveRate() const \
		  {												  \
		  return iFalsePositiveRate;					  \
		  }												  \
		AuthServer::TPercentage FalseNegativeRate() const \
		  {												  \
		  return iFalseNegativeRate;					  \
		  }												  \
	static TMyPinType* NewL()                   	\
    	{										 	\
        TMyPinType* r = new (ELeave) TMyPinType();	\
        CleanupStack::PushL(r);						\
	  	r->ConstructL();						 	\
	  	CleanupStack::Pop(r);						\
        r->iType = type;                            \
	  	return r;                              		\
		}											\
	}

NClass(11113100, EAuthKnowledge);
NClass(11113101, EAuthKnowledge);
NClass(11113102, EAuthKnowledge);
NClass(11113103, EAuthBiometric);
NClass(11113104, EAuthToken);

//=========================================================================		
// Constants used in the utility functions -File locations, etc
_LIT8(KDefaultData, "0000");
_LIT(KInitInfoFile, "\\tAuth\\tAuthSvr\\testdata\\initialisation_Info.ini");
_LIT(KPolicyFile, 	"\\tAuth\\tAuthSvr\\testdata\\AuthSvrPolicy.ini");

_LIT(KPluginNamePin, "Pin1");
_LIT(KPluginDescriptionPin, "Pin Plugin, can store and return known id and training data for each identity.");

static const TInt KEntropyPin = 999;
static const TInt KFalsePosPin = 100;
static const TInt KFalseNegPin = 10;

//From Step_Base.cpp
_LIT(KPluginIniSection,			"SectionOne");
_LIT(KAuthSvrPolicyFile, 		"\\tAuth\\tAuthSvr\\testdata\\AuthSvrPolicy.ini");
_LIT(KDefaultPluginTag, 		"DefaultPlugin");
_LIT(KEnteredPinTag, 			"EnteredPinValue");
_LIT(KPinDbTag,					"Identity&PinValues");
_LIT(KTotalDbTag,				"AllUserID&PinValues");
_LIT(KInitPinDatabaseValue,		",");



#endif	/* TPINAUTHPLUGIN_H */
