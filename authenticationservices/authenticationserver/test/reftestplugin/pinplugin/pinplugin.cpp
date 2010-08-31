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
* CPinPlugin implementation
*
*/


/**
 @file 
*/


#include "pinplugin.h"
#include <bautils.h>
#include <barsread.h>
#include <e32math.h>
#include <pinpluginconfig.rsg>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <f32file.h>

using namespace AuthServer;

_LIT(KPinPluginResourceFile, "z:\\Resource\\pinplugin\\pinpluginconfig.rsc");

const TPluginId KPinPluginImplementationUid = 0x200032E5;

CPinPlugin* CPinPlugin::NewL()
 	{
 	CPinPlugin* self = CPinPlugin::NewLC();
 	CleanupStack::Pop();
 	return self;
	}
	
CPinPlugin* CPinPlugin::NewLC()
	{
	CPinPlugin* self = new(ELeave) CPinPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CPinPlugin::~CPinPlugin()
	{
	delete iPinPluginAO;
	delete iPinName;
	delete iPinDescription;
	}

CPinPlugin::CPinPlugin()
	{	
	iActive = ETrue;
	}
		
void CPinPlugin::ConstructL()
	{
	TInt retryCount, pinSize, pinMinSize, pinMaxSize;
	TReal val;
	
	// Verifies the UID/SID of the Authentication server
	// to make sure pinplugin is loaded from Authentication server.  
	RProcess process;
        
	ReadResourceL(retryCount, pinSize, pinMinSize, pinMaxSize);

	__ASSERT_ALWAYS(retryCount >= 0 && pinSize >= 0 && pinMinSize >= 0 &&
	 		pinMaxSize >= 0 && pinSize >= pinMinSize && pinSize <= pinMaxSize,
	 		User::Panic(KPinPluginPanicString(), EPinPanicInvalidConfigValues));

	iName.Set(iPinName->Des());
	iDescription.Set(iPinDescription->Des());
	Math::Pow10(val, pinMinSize);
	// since pinMinSize is always a Int value, val never have a floating point
	// in future if entropy is changed to have real value also, it will work
	// now in our case, compiler do the default truncation.
	iMinEntropy = val;
	iFalsePositiveRate = 0;
	iFalseNegativeRate = 0;
	iType = AuthServer::EAuthKnowledge;
	 
	//Create a new implementation object
	iPinPluginAO = CPinPluginAO::NewL(pinSize, pinMinSize, pinMaxSize, retryCount);
	
	SetPluginStateL();
	
	}
	
	
//Implement CAuthPluginInterface definitions

void CPinPlugin::Identify(TIdentityId& aId,const TDesC& aClientMessage,
						  HBufC8*& aResult, TRequestStatus& aRequest)
	{
	iPinPluginAO->Identify(aId, aClientMessage, aResult, aRequest);
	}
	
void CPinPlugin::Cancel()
	{
	iPinPluginAO->Cancel();		
	}

void CPinPlugin::Train(TIdentityId aId, HBufC8*& aResult, TRequestStatus& aRequest)
	{
	iPinPluginAO->Train(aId, aResult, aRequest);
	}

TInt CPinPlugin::Forget(TIdentityId aId)
	{
	return iPinPluginAO->Forget(aId);
	}

TBool CPinPlugin::IsActive() const
	{
	return iActive;
	}
	
TInt CPinPlugin::DefaultData(TIdentityId aId, HBufC8*& aOutputBuf)
	{
	return iPinPluginAO->DefaultData(aId, aOutputBuf);
	}

TInt CPinPlugin::Reset(TIdentityId aIdentityId, const TDesC& aRegistrationData, HBufC8*& aResult)
	{
	TRAPD(err, iPinPluginAO->ResetL(aIdentityId, aRegistrationData, aResult));
	return err;
	}

TPluginId CPinPlugin::Id() const
	{
	return KPinPluginImplementationUid;
	}

void CPinPlugin::ReadResourceL(TInt& aRetryCount, TInt& aPinSize, TInt& aPinMinSize,
							TInt& aPinMaxSize)
	{
	RFs fs;
	TFileName fileName(KPinPluginResourceFile);
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	BaflUtils::NearestLanguageFile(fs,fileName);
	RResourceFile resourceFile;
	resourceFile.OpenL(fs,fileName);
	CleanupClosePushL(resourceFile);
	resourceFile.ConfirmSignatureL(0);
	resourceFile.Offset();
	
	aRetryCount = ResourceValueL(resourceFile, R_IDENTIFY_DIALOG_RETRY_COUNT);
	aPinSize = ResourceValueL(resourceFile, R_PIN_SIZE);
	aPinMinSize = ResourceValueL(resourceFile, R_PIN_MIN_SIZE);
	aPinMaxSize = ResourceValueL(resourceFile, R_PIN_MAX_SIZE);

	HBufC8* res = resourceFile.AllocReadLC(R_PLUGIN_INFO);
	TResourceReader reader;
	reader.SetBuffer(res);
	iPinName = reader.ReadTPtrC().AllocL();
	iPinDescription = reader.ReadTPtrC().AllocL();
	CleanupStack::PopAndDestroy(3, &fs);
	}

TInt CPinPlugin::ResourceValueL(RResourceFile& aResFile, const TInt aResourceID)
	{
	TResourceReader reader;
	HBufC8* res = aResFile.AllocReadLC(aResourceID);
	reader.SetBuffer(res);
	TInt val = reader.ReadInt8();
	CleanupStack::PopAndDestroy(res);
	return val;
	}
	
void CPinPlugin::SetPluginStateL()
	{
	// identifies the pinplugins state by looking for a file 
	// in system drive.
	_LIT(KPinPlugin,"pinplugin_inactive.txt");
	TFileName filename;
	filename.Copy(KPinPlugin); // convert from 8 -> 16 bit descriptor
			
	TDriveUnit sysDrive = RFs::GetSystemDrive();
	TDriveName sysDriveName (sysDrive.Name());
	filename.Insert(0,sysDriveName);
	filename.Insert(2,_L("\\"));
		
	RFs fs;
	User::LeaveIfError(fs.Connect());
	TEntry entry;
	TInt err = fs.Entry(filename, entry);
	if(err == KErrNone)
		{
		iActive = EFalse;
		}
	else if( err == KErrNotFound )
		{
		iActive = ETrue;
		}
	else
		{
		User::LeaveIfError(err);
		}
	}

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KPinPluginImplementationUid,	CPinPlugin::NewL)
	};

// Exported proxy function to resolve intstantiation methods for an ECOM plugin dll
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}


const TPtrC& CPinPlugin::Name() const
  {
  return iName;
  }
const TPtrC& CPinPlugin::Description() const
  {
  return iDescription;
  }
TAuthPluginType CPinPlugin::Type() const 
  {
  return iType;
  }

TEntropy CPinPlugin::MinEntropy() const
  {
  return iMinEntropy;
  }

TPercentage CPinPlugin::FalsePositiveRate() const
  {
  return iFalsePositiveRate;
  }

TPercentage CPinPlugin::FalseNegativeRate() const
  {
  return iFalseNegativeRate;
  }




