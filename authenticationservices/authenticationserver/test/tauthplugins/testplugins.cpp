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
*
*/


#include "testplugins.h"
#include <ecom/ecom.h>
#include <f32file.h>
#include "consts.h"

using namespace AuthServer;

TBool E32Dll()
  {
  return (ETrue);
  }

CTestPlugin1::CTestPlugin1()  : iActive(false)
  {
  iName.Set(KPluginName);
  iDescription.Set(KPluginDescription);
  iType = EAuthKnowledge;
  iMinEntropy = KEntropy;
  iFalsePositiveRate = KFalsePos;
  iFalseNegativeRate = KFalseNeg;

  RFs fs;
  fs.Connect();
  TEntry file;
  
  _LIT(KActiveFile, "\\testplugin_active\\");
  TDriveUnit sysDrive (fs.GetSystemDrive());
  TBuf<128> activeFile (sysDrive.Name());
  activeFile.Append(KActiveFile);
  iActive = fs.Entry(activeFile, file);;
  fs.Close();
  //RDebug::Printf("Plugin started, active = %d", iActive);
  iActive = iActive != KErrNotFound;
  //RDebug::Printf("Plugin started, active = %d", iActive);
  }

void CTestPlugin1::Identify(TIdentityId& aId, const TDesC& /*aClientMessage*/,
							HBufC8*& aResult, TRequestStatus& aRequest)
    {
	aId = 22;
	aResult = HBufC8::New(20);
	
	TRequestStatus *stat = &aRequest;
	
	if(aResult == NULL)
			{
			User::RequestComplete(stat, KErrNoMemory);
			return;
			}
	
	*aResult = KIdentifyData;
	User::RequestComplete(stat, KErrNone);
    }
  
void CTestPlugin1::Train(TIdentityId /*aId*/, HBufC8*& aResult,
						 TRequestStatus& aRequest)
    {
	aResult = HBufC8::New(20);
	
	TRequestStatus* status = &aRequest;
	
	if(aResult == NULL)
		{
		User::RequestComplete(status, KErrNoMemory);
		return;
		}

	*aResult = KTrainData;
	User::RequestComplete(status, KErrNone);
			
	}


void CUnknownPlugin::Identify(TIdentityId& aId, const TDesC& /*aClientMessage*/,
							  HBufC8*& aResult, TRequestStatus& aRequest)
    {
	aId = KUnknownIdentity;
	aResult = HBufC8::New(0);
	
	TRequestStatus *stat = &aRequest;
	
	if(aResult == NULL)
		{
		User::RequestComplete(stat, KErrNoMemory);
		return;
		}
	
	User::RequestComplete(stat, KErrNone);
    }
  
void CUnknownPlugin::Train(TIdentityId /*aId*/, HBufC8*& aResult,
						 TRequestStatus& aRequest)
    {
	aResult = HBufC8::New(0);
	TRequestStatus* status = &aRequest;
	
	if(aResult == NULL)
		{
		User::RequestComplete(status, KErrNoMemory);
		return;
		}
	
	User::RequestComplete(status, KErrAuthServPluginCancelled);
	}

TBool CTestPlugin1::IsActive() const 
    {
	return iActive;
	}


TInt CTestPlugin1::Forget(TIdentityId /*aId*/)
    {
	return KErrNone;
    }
TInt CTestPlugin1::DefaultData(TIdentityId /*aId*/, HBufC8*& /*aOutputBuf*/)
    {
	return KErrNotSupported;
    }

TInt CTestPlugin1::Reset(TIdentityId /*aId*/, const TDesC& /*aRegistrationData*/, HBufC8*& /*aResult*/)
	{
	return KErrNotSupported;
	}

const TImplementationProxy ImplementationTable[] =
  {
	IMPLEMENTATION_PROXY_ENTRY(0x10274104, CTestPlugin1::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10274105, CBlockPlugin::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10274106, CInactivePlugin::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10274107, CUnknownPlugin::NewL)
  };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
  {
  aTableCount = sizeof(ImplementationTable) /
	sizeof(TImplementationProxy);
  return ImplementationTable;
  }

const TPtrC& CTestPlugin1::Name() const
  {
  return iName;
  }
const TPtrC& CTestPlugin1::Description() const
  {
  return iDescription;
  }
TAuthPluginType CTestPlugin1::Type() const 
  {
  return iType;
  }

TEntropy CTestPlugin1::MinEntropy() const
  {
  return iMinEntropy;
  }

TPercentage CTestPlugin1::FalsePositiveRate() const
  {
  return iFalsePositiveRate;
  }

TPercentage CTestPlugin1::FalseNegativeRate() const
  {
  return iFalseNegativeRate;
  }


//Unknown Plugin

const TPtrC& CUnknownPlugin::Name() const
  {
  return iName;
  }
const TPtrC& CUnknownPlugin::Description() const
  {
  return iDescription;
  }
AuthServer::TAuthPluginType CUnknownPlugin::Type() const 
  {
  return iType;
  }

AuthServer::TEntropy CUnknownPlugin::MinEntropy() const
  {
  return iMinEntropy;
  }

AuthServer::TPercentage CUnknownPlugin::FalsePositiveRate() const
  {
  return iFalsePositiveRate;
  }

AuthServer::TPercentage CUnknownPlugin::FalseNegativeRate() const
  {
  return iFalseNegativeRate;
  }



//Inactive Plugin

const TPtrC& CInactivePlugin::Name() const
  {
  return iName;
  }
const TPtrC& CInactivePlugin::Description() const
  {
  return iDescription;
  }
AuthServer::TAuthPluginType CInactivePlugin::Type() const 
  {
  return iType;
  }

AuthServer::TEntropy CInactivePlugin::MinEntropy() const
  {
  return iMinEntropy;
  }

AuthServer::TPercentage CInactivePlugin::FalsePositiveRate() const
  {
  return iFalsePositiveRate;
  }

AuthServer::TPercentage CInactivePlugin::FalseNegativeRate() const
  {
  return iFalseNegativeRate;
  }

