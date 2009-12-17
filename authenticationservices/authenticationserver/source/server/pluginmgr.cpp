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
* Implementation of CPluginMgr
*
*/


/**
 @file
*/

#include "authserver_impl.h"
#include <authserver/authpatchdata.h>
#include <u32hal.h>
#include <e32svr.h> 
using namespace AuthServer;

TLinearOrder<CAuthPluginInterface> CPluginMgr::SPluginOrder(PluginCompare);

TInt CPluginMgr::PluginCompare(const CAuthPluginInterface& aLhs,
									  const CAuthPluginInterface& aRhs)
  {
  return aLhs.Id() - aRhs.Id();
  }

TInt CPluginMgr::FindById(const TPluginId* aId,
						 const CAuthPluginInterface& aRhs)
  {
  return *aId - aRhs.Id();
  }


CPluginMgr* CPluginMgr::NewLC()
  {
  CPluginMgr* me = new (ELeave) CPluginMgr();
  CleanupStack::PushL(me);
  me->ConstructL();
  return me;
  } 

CPluginMgr* CPluginMgr::NewL()
  {
  CPluginMgr* me = CPluginMgr::NewLC();
  CleanupStack::Pop(me);
  return me;
  }

CPluginMgr::~CPluginMgr()
  {
  ReleasePlugins();
  ReleaseImplInfo();
  }


void CPluginMgr::ConstructL()
  {
  BuildAuthPluginsListL();
  }

CAuthPluginInterface* CPluginMgr::PluginL(const TPluginId& aId)
  {
  TInt idx = iPlugins.FindInOrder<>(aId, CPluginMgr::FindById);
  CAuthPluginInterface* plugin = 0;
  if (idx == KErrNotFound)
	{
	  plugin = CreatePluginImplementationL(aId);
	  iPlugins.InsertInOrder(plugin, SPluginOrder);
	}
  else
	{
	plugin = iPlugins[idx];
	}
  
  return plugin;
  }

CAuthPluginInterface* CPluginMgr::ImplementationL(TInt aIndex)
	{
	if( aIndex < 0 || aIndex > iPlugins.Count()-1 )
		{
		User::Leave(KErrArgument);
		}
	return iPlugins[aIndex];
	}

const RImplInfoPtrArray& CPluginMgr::ImplementationsL()
    {
    if (iImplArray.Count() == 0) 
        {
        //To load plugins from sources other than ROM the patch 
        // data KEnablePostMarketAuthenticationPlugins must be set to True.
        TUint32 enablePostMarketPlugin = KEnablePostMarketAuthenticationPlugins;

        #ifdef __WINS__

        // Default SymbianOS behavior is to only load auth plugins from ROM.
        enablePostMarketPlugin = 0;

        // For the emulator allow the constant to be patched via epoc.ini
        UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
        (TAny*)"KEnablePostMarketAuthenticationPlugins", &enablePostMarketPlugin); // read emulator property (if present)

        #endif
          
        if(enablePostMarketPlugin == 0)
        	{
         	TEComResolverParams resolverParams;
           	REComSession::ListImplementationsL(KCAuthPluginInterfaceUid,
        									 resolverParams,
        									 KRomOnlyResolverUid,
        									 iImplArray);
        									 
            }
            
         else
         	{
         	REComSession::ListImplementationsL(KCAuthPluginInterfaceUid, iImplArray);
          	}
        }
    return iImplArray;
    }

void CPluginMgr::ReleasePlugins()
  {
  TInt i = iPlugins.Count();
  while (i)
	{
	delete iPlugins[--i];
	}
  
  i = iPluginDtorUids.Count();
  
  while(i)
	  {
	  REComSession::DestroyedImplementation(iPluginDtorUids[--i]);
	  }
  iPlugins.Reset();
  iPluginDtorUids.Reset();
  }

void CPluginMgr::ForgetIdentityL(TIdentityId aId)
  {
  ImplementationsL();
  
  TInt i = iImplArray.Count();
  while (i)
	{
	PluginL(iImplArray[--i]->ImplementationUid().iUid)->Forget(aId);
	}
  }

void CPluginMgr::ReleaseImplInfo()
  {
  TInt i = iImplArray.Count();
  while (i)
	{
	  delete iImplArray[--i];
	}
  
  iImplArray.Reset();
  }
  
void CPluginMgr::BuildAuthPluginsListL()
	{
	ReleasePlugins();
  	ReleaseImplInfo();
  	
	ImplementationsL();
  	CAuthPluginInterface* plugin = 0;
  	for( TInt count=0; count < iImplArray.Count(); ++count)
		{
		plugin = CreatePluginImplementationL(iImplArray[count]->ImplementationUid().iUid);

		CleanupStack::PushL(plugin);
		User::LeaveIfError(iPlugins.InsertInOrder(plugin, SPluginOrder));
		CleanupStack::Pop(plugin);
		}
	}

CAuthPluginInterface* CPluginMgr::CreatePluginImplementationL(const TPluginId &aId)
	{
	TEComResolverParams resolverParams;
	TBufC8<16> pluginIdTxt;
	pluginIdTxt.Des().Format(_L8("%x"), aId);
	pluginIdTxt.Des().UpperCase();
	resolverParams.SetDataType(pluginIdTxt);
		  
	//To load plugins from sources other than ROM the patch 
	// data KEnablePostMarketAuthenticationPlugins must be set to True.
	TUint32 enablePostMarketPlugin = KEnablePostMarketAuthenticationPlugins;

#ifdef __WINS__

	// Default SymbianOS behavior is to only load auth plugins from ROM.
	enablePostMarketPlugin = 0;

	// For the emulator allow the constant to be patched via epoc.ini
	UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
	(TAny*)"KEnablePostMarketAuthenticationPlugins", &enablePostMarketPlugin); // read emulator property (if present)

#endif

	TAny* plugin = 0;
	TInt err = 0;
	TUid dtor_ID_Key = TUid::Null();
	if(enablePostMarketPlugin == 0) 
	  	{
	  	TRAP(err, plugin = 
		REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
											dtor_ID_Key,
											resolverParams,
											KRomOnlyResolverUid));
	  	}
		  
	  else
	  	{
	  	TRAP(err, plugin = 
		REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
											dtor_ID_Key,
											resolverParams));
	  	}
		  
	 if (err == KErrNotFound)
	    {
	    err = KErrAuthServNoSuchPlugin;  
	    }
	 
	 //Add key to plugin destructor keys list .
	 TInt err2 =  iPluginDtorUids.Append(dtor_ID_Key);
	 
	 if(KErrNoMemory == err2)
		 {
		 CAuthPluginInterface* authPlugin = reinterpret_cast<CAuthPluginInterface*>(plugin);
		 delete authPlugin;
		 REComSession::DestroyedImplementation(dtor_ID_Key);
		 User::LeaveNoMemory();
		 }
	 User::LeaveIfError(err);

	return reinterpret_cast<CAuthPluginInterface*>(plugin);
	}

