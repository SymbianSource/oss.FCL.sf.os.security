/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include "authserver_client.h"
#include <authserver/authplugininterface.h>

using namespace AuthServer;

//EXPORT_C CAuthPluginInterface::CAuthPluginInterface()
//	{}

EXPORT_C CAuthPluginInterface::~CAuthPluginInterface()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
/*
EXPORT_C CAuthPluginInterface* CAuthPluginInterface::NewL()
	{// Hardcoded to be the PIN plugin by default
	
	const TUid KPINPluginUid = { 0x102740FD };
	TAny* defaultPlugin = REComSession::CreateImplementationL(KPINPluginUid, _FOFF(CAuthPluginInterface, iDtor_ID_Key));
	return (reinterpret_cast<CAuthPluginInterface*>(defaultPlugin));
	}
*/

EXPORT_C CAuthPluginInterface* CAuthPluginInterface::NewL(TPluginId aPluginId)
	{// Use the default ECOM resolver
/*	
	//TDesC8& aCue = aPluginId;
	//aCue = aPluginId;
	TBuf8<32> cueValue;
	//TDes8 cueValue;
	_LIT8(KFormatValue2, "%x");
	cueValue.Format(KFormatValue2, aPluginId);		

	TEComResolverParams resolverParams;
	resolverParams.SetDataType(cueValue);	//(aCue);
	
	resolverParams.SetWildcardMatch(ETrue);	//There might be problems with this
	
	//TAny* pluginInterface =	REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
	//_FOFF(CAuthPluginInterface,iDtor_ID_Key),NULL,resolverParams);
	TAny* pluginInterface =	REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
	_FOFF(CAuthPluginInterface,iDtor_ID_Key),resolverParams);


	return (reinterpret_cast<CAuthPluginInterface*>(pluginInterface));
*/
  TEComResolverParams resolverParams;
  TBufC8<16> pluginIdTxt;
  
  pluginIdTxt.Des().Format(_L8("%x"), aPluginId);
  
  resolverParams.SetDataType(pluginIdTxt);
  
  TAny* plugin = 0; 

  plugin = 
	REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
										_FOFF(CAuthPluginInterface,
											  iDtor_ID_Key),
										resolverParams,
										KRomOnlyResolverUid);	
	return reinterpret_cast<CAuthPluginInterface*>(plugin);										
	}


EXPORT_C void CAuthPluginInterface::ListImplementationsL(RImplInfoPtrArray& aImplInfoArray)
	{
	REComSession::ListImplementationsL(KCAuthPluginInterfaceUid, aImplInfoArray);
	}

const TPtrC& CAuthPluginInterface::GetName() const
	{
	return iName;
	}
	
const TPtrC& CAuthPluginInterface::GetDescription() const
	{
	return iDescription;
	}
	
TAuthPluginType CAuthPluginInterface::GetType() const
	{
	return iType;
	}

/** DLL Entry point */
//GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
//	{
//	return(KErrNone);
//	}

/*
//Defined in main.cpp
TBool E32Dll()
	{
	return (ETrue);
	}
*/
