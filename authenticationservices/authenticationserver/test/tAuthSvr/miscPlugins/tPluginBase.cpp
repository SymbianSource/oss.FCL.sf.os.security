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
* tMiscAuthPlugin.cpp
* Improvements: 
*
*/

 

#include <f32file.h>


#ifndef __INIPARSER_H__
	#include <cinidata.h>
#endif // __INIPARSER_H__

#include "tPluginBase.h"
#include <e32svr.h>
#include <authserver/auth_srv_errs.h>

TBool E32Dll()
{

	return ETrue;
}


// Construction and destruction functions
//CTPluginUnknown* CTPluginUnknown::NewL(const TPluginId aPluginId)
CTPluginUnknown* CTPluginUnknown::NewL()
 	{
 	CTPluginUnknown* self = new (ELeave) CTPluginUnknown();
 	CleanupStack::PushL(self);
 	self->ConstructL();
 	CleanupStack::Pop();
 	return self;
	}

CTPluginUnknown::~CTPluginUnknown()
	{}

//CTPluginUnknown::CTPluginUnknown(const TPluginId aPluginId)
CTPluginUnknown::CTPluginUnknown()
	{ // See ConstructL() for initialisation completion.
	}
		
void CTPluginUnknown::ConstructL()
// Safely complete the initialization of the constructed object	
	{
	
	//TUid iDtor_ID_Key;
	
	// Initialise member data
	/*
	iName = "Pin1";
	iDescription = "PIN Plugin";
	iMinEntropy = 0.8;
	iFalsePositiveRate = 1;
	iFalseNegativeRate = 1;
	iType = EAuthKnowledge;
	
	// TPtrC iName;
	// TPtrC iDescription;
	// TEntropy iMinEntropy;
	// TPercentage iFalsePositiveRate;
	// TPercentage iFalseNegativeRate;
	// TPluginType iType;
	*/
	
	iSupportsDefaultData = true;
	iNumTimesTrained = 0;
	}

	
//Implement CAuthPluginInterface definitions	
void CTPluginUnknown::Identify(TIdentityId& aId, HBufC8*& aResult, TRequestStatus& aRequest)
	{
	// We are simulating user input by reading from a file
	// (The data contained in this file has been freshly written by the 'client part in TestExecute')
	
	aRequest = KRequestPending;
	//aId = USER;
	aId = KUnknownIdentity;	//Unknown Id
	aResult = HBufC8::New(16);
	
	TRequestStatus* status  = &aRequest;
	
	if(aResult == NULL)
		{
		User::RequestComplete(status, KErrNoMemory);
		return;
		}
	
	aResult->Des().Format(_L8("%x"), aId);
	
		
	
	//aRequest = AuthServer::KIdCancel;	//KIdCancel not defined yet
	//aResult will not be updated, as specified
	
	
	//aRequest = AuthServer::KIdQuit;	//KIdQuit not defined yet
	//aResult will not be updated, as specified
	
	
	//aRequest = AuthServer::KIdUnknown;		
	
	//return aRequest.Int();
	
	User::RequestComplete(status, KErrNone);
	}

void CTPluginUnknown::Cancel()
	{	
	}	
	
void CTPluginUnknown::Train(TIdentityId /*aId*/, HBufC8*& /*aResult*/, TRequestStatus& aRequest)							
	{
	
	aRequest = KRequestPending;
	iNumTimesTrained++;
	//Possibly update aResult here, later for now we'll pretend that the plugin was cancelled
		
	TRequestStatus* status  = &aRequest;
	User::RequestComplete(status, KErrAuthServPluginCancelled);
	}
	
		
TBool CTPluginUnknown::IsActive() const
	{
	// There is never any user intervention required
	return true;
	
	// As a future development note, this value may be read from a file (or a section in a file)
	}
	
TInt CTPluginUnknown::Forget(TIdentityId /*aId*/)
	{
	
	//Dummy implementation	
	return KErrNone;	
	}
	
TInt CTPluginUnknown::DefaultData(TIdentityId /*aId*/, HBufC8*& aOutputBuf)
	{
	
	// This implementation of the PIN plugin does support default data.
	// There will be cases where i don't want this available. Hence the addition of a 
	// new class member iSupportsDefaultData
	
	TInt result = KErrNone;
	
	if (iSupportsDefaultData)	//Class member initialised in the constructor
		{
		//_LIT(KDefaultData, "0000");	// in tPluginBase.h
		aOutputBuf = HBufC8::New(KDefaultData().Size());
		if(aOutputBuf == NULL)
			{
			return KErrNoMemory;
			}
		*aOutputBuf = KDefaultData;		
		}
	else
		result = KErrNotSupported;
	
	return result;	
	}

TPluginId CTPluginUnknown::Id() const
	{
	return 0x11112FFF;
	}



//=========================================================================

//=========================================================================

// Construction and destruction functions
/*CTPluginBase* CTPluginBase::NewL()
 	{
	CTPluginBase* self = new (ELeave) TMyType();
 	CleanupStack::PushL(self);
 	self->ConstructL();
 	CleanupStack::Pop();
 	return self;
	}
*/

CTPluginBase::~CTPluginBase()
	{}

CTPluginBase::CTPluginBase()
	{
	// See ConstructL() for initialisation completion.
	}

void CTPluginBase::ConstructL()
// Safely complete the initialization of the constructed object	
	{
	
	//TUid iDtor_ID_Key;
	
	
	/*
	iName = "Pin1";
	iDescription = "PIN Plugin";
	iMinEntropy = 0.8;
	iFalsePositiveRate = 1;
	iFalseNegativeRate = 1;
	iType = EAuthKnowledge;
	
	// TPtrC iName;
	// TPtrC iDescription;
	// TEntropy iMinEntropy;
	// TPercentage iFalsePositiveRate;
	// TPercentage iFalseNegativeRate;
	// TPluginType iType;
	*/
	
	iType = GetType();
	
	iSupportsDefaultData = true;
	iNumTimesTrained = 0;
	}

	
//Implement CAuthPluginInterface definitions

void CTPluginBase::Identify(TIdentityId& aId, HBufC8*& aResult, TRequestStatus& aRequest)
	{
	// We are simulating user input by reading from a file
	// (The data contained in this file has been freshly written by the 'client part in TestExecute')
	
//	aRequest = KRequestPending;
//	aId = USER;
	//delete aResult;
//	aResult = HBufC8::NewL(16);
	
	//aResult->Des().Format(_L8("%x"), aResult);
//	aResult->Des().Format(_L8("%d"), aId);
	
	
	//aRequest = AuthServer::KIdCancel;	//KIdCancel not defined yet
	//aResult will not be updated, as specified
	
	
	//aRequest = AuthServer::KIdQuit;	//KIdQuit not defined yet
	//aResult will not be updated, as specified
	
	
	//aRequest = AuthServer::KIdUnknown;		
	
//	aRequest = KIdSuccess;
	//return aRequest.Int();

	aRequest = KRequestPending;
	aId = GetId();
	aResult = HBufC8::New(16);
	
	TRequestStatus* status = &aRequest;
	if(aResult == NULL)
		{
		User::RequestComplete(status, KErrNoMemory);
		return;
		}
	
	aResult->Des().Format(_L8("%x"), aId);
	User::RequestComplete(status, KErrNone);
	}


void CTPluginBase::Cancel()
	{	
	}

void CTPluginBase::Train(TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest)							
	{
	
	aRequest = KRequestPending;
	iNumTimesTrained++;
	aResult = HBufC8::New(16);
	TRequestStatus* status  = &aRequest;
	if(aResult == NULL)
		{
		User::RequestComplete(status, KErrNoMemory);
		return;
		}
	TInt number = iNumTimesTrained*GetId();
	aResult->Des().Format(_L8("%x"), aId);
	
	User::RequestComplete(status, KErrNone);
	}


TBool CTPluginBase::IsActive() const
	{
	// There is never any user intervention required
	return true;
	
	// As a future development note, this value may be read from a file (or a section in a file)
	}

TInt CTPluginBase::Forget(TIdentityId /*aId*/)
	{
	
	//Dummy implementation	
	return KErrNone;
	
	//Improvement
	// If (NumTimesTrained == 0) 
	//	return AuthServer::KIdUnknown;	// This is what is returned for the CTUnknownPlugin class
			
	}

TInt CTPluginBase::DefaultData(TIdentityId /*aId*/, HBufC8*& aOutputBuf)
	{
	
	// This implementation of the PIN plugin does support default data.
	// There will be cases where i don't want this available. Hence the addition of a 
	// new class member iSupportsDefaultData
	
	TInt result = KErrNone;
	
	if (iSupportsDefaultData)	//Class member initialised in the constructor
		{
		//_LIT(KDefaultData, "0000");	// in tPinAuthPlugin.h
		aOutputBuf = HBufC8::New(KDefaultData().Size());
		if(aOutputBuf == NULL)
			{
			return KErrNoMemory;
			}
		*aOutputBuf = KDefaultData;
		}
	else
		result = KErrNotSupported;
	
	return result;	
	}

TPluginId CTPluginBase::Id() const
	{
	return GetId();
	}
