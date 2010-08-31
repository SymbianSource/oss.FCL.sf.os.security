/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tMiscAuthPlugin.h
* CTPluginUnknown and CTPluginBase are interface implementations for
* CAuthPluginInterface
*
*/



#ifndef T_PLUGIN_BASE_H
#define T_PLUGIN_BASE_H

//#include <ImplementationInformation.h>
//#include <TestExecuteStepBase.h>
//#include <TestExecuteServerBase.h>
//#include <TestExecuteStepBase.h>

#include "authPluginInterface.h"	//'authserver' folder is included in mmp file


//An implementation of the CAuthPluginInterface definition

using namespace AuthServer;

class CTPluginUnknown : public CBase, public MAuthPluginInterface
	{
public:
	// Two phase constructor
	// Standardised safe construction which leaves nothing the cleanup stack.
	static CTPluginUnknown* NewL();
	// Destructor
	~CTPluginUnknown();

	virtual void Identify(TIdentityId& aId,  HBufC8*& aResult, TRequestStatus& aRequest);	
	virtual void Cancel();
	virtual void Train(TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest);		

	virtual TBool IsActive() const;
	virtual TInt Forget(TIdentityId aId);
	virtual TInt DefaultData(TIdentityId aId, HBufC8*& aOutputBuf);
	virtual TPluginId Id() const;
	
		
private:
	CTPluginUnknown();
	void ConstructL();
	
	// Data read from file determining if this plugin supports default data
	TBool iSupportsDefaultData;
	TInt iNumTimesTrained;
	};

//=========================================================================

//=========================================================================	
//implementation_uid = 0x11113000 to 0x11113031 	//(50 of them)

class CTPluginBase : public CBase, public MAuthPluginInterface
	{
public:

	// Destructor
	~CTPluginBase();
	
	//Implementation of CAuthPluginInterface definitions using a PIN system
	virtual void Identify(TIdentityId& aId,  HBufC8*& aResult, TRequestStatus& aRequest);	
	virtual void Cancel();	
	virtual void Train(TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest);		

	virtual TBool IsActive() const;
	virtual TInt Forget(TIdentityId aId);
	virtual TInt DefaultData(TIdentityId aId, HBufC8*& aOutputBuf);
	virtual TPluginId Id() const;
	
	// Utility functions
	//TInt ReadUserInputL(TBuf<32> inputValue);
	//TInt CheckForStringPresenceL(TBuf<32> inputValue, TRequestStatus& requestValue);
	//TInt CheckForNewStringPresenceL(TIdentityId aId, TBuf<32> inputValue, TRequestStatus& requestValue);
	//TInt FindStringAndRemoveL(TIdentityId aId);

	virtual TPluginId GetId() const = 0;
	virtual TAuthPluginType GetType() const = 0;
		
//private:
	//CTPluginBase(const TPluginId aPluginId);
	CTPluginBase();
	void ConstructL();
	
	// Data read from file determining if this plugin supports default data
	TBool iSupportsDefaultData;	
	// Can be used to generate results that are multiples of the plugin IDs
	TInt iNumTimesTrained;
	};


// Constants used in the utility functions -File locations, etc
//_LIT(KInitInfoFile, "c:\\tAuthSvr\\initialisation_Info.ini");
//_LIT(KPinInputFile, "c:\\tAuthSvr\\pin_Input.ini");
//_LIT(KPinContentsFile, "c:\\tAuthSvr\\pin_Contents.ini");
//_LIT8(KDefaultData, "0000");
_LIT8(KDefaultData, "CCCCCCCCCCCCCCCCCCCC");

// Provide a consistent naming structure
typedef CTPluginUnknown CTPlugin11112FFF ;


//=========================================================================	
// The various classes that are spawned from this base class by inheriting the implementations besides
// the identify() function

// number is hex, type is one of the TAuthPluginType enums
#define DEF_PLUGIN(number, type) class CTPlugin##number : public CTPluginBase\
	{                                        \
public:                                      \
	typedef CTPlugin##number TMyType;        \
	TIdentityId GetId() const                \
	{ return 0x##number; }                   \
	TAuthPluginType GetType() const          \
	{ return type; }                         \
    static TMyType* NewL()                   \
    {										 \
	  TMyType* r = new (ELeave) TMyType();   \
	  r->ConstructL();						 \
	  return r;                              \
	}                                        \
private:                                     \
	}         

DEF_PLUGIN(11113000, EAuthKnowledge);
DEF_PLUGIN(11113001, EAuthKnowledge);
DEF_PLUGIN(11113002, EAuthKnowledge);
DEF_PLUGIN(11113003, EAuthBiometric);
DEF_PLUGIN(11113004, EAuthBiometric);
DEF_PLUGIN(11113005, EAuthBiometric);
DEF_PLUGIN(11113006, EAuthToken);
DEF_PLUGIN(11113007, EAuthToken);
DEF_PLUGIN(11113008, EAuthToken);
DEF_PLUGIN(11113009, EAuthToken);

#endif	/* T_PLUGIN_BASE_H */

