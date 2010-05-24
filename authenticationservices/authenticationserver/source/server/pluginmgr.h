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


/**
 @file
 @released
 @internalComponent
*/
#ifndef PLUGINMGR_H
#define PLUGINMGR_H

#include <authserver/authplugininterface.h>
namespace AuthServer 
{

/**
 * The plugin manager is responsible for managing the lifetime of plugin DLLs.
 **/

class CPluginMgr : public CBase
	{
public:
	static CPluginMgr* NewL();
	static CPluginMgr* NewLC();
	
	~CPluginMgr();

	/**
	 * Returns the requested plugin. If the plugin has not yet been loaded
	 * then it will be instantiated and cached prior to returning.
	 *
	 * @param aId the id of the plugin to return.
	 *
	 * @return the requested plugin
	 *
	 * @leave KErrAuthServerNoSuchPlugin
	 *
	 **/
	CAuthPluginInterface* PluginL(const TPluginId& aId);

    /**
	 * Returns the list of plugin implementations. The list is cached for use
	 * rather than reloading it in multiple places.
	 *
	 * @return the array of implementation info objects
	 *
	 **/
	const RImplInfoPtrArray& ImplementationsL();
	
	CAuthPluginInterface* ImplementationL(TInt aIndex);
	
	/**
	 * Forces all plugins to forget their training for id
	 *
	 * @param aId the id of the identitiy to forget.
	 *
	 **/
	void ForgetIdentityL(TIdentityId aId);
	
	/**
	 * Builds authentication plugins list.
	 **/
	void BuildAuthPluginsListL();
	
private:
	static TInt PluginCompare(const CAuthPluginInterface& lhs,
							  const CAuthPluginInterface& rhs);
							  
	static TInt FindById(const TPluginId* lhs,
						 const CAuthPluginInterface& rhs);
						 
	static 	TLinearOrder<CAuthPluginInterface> SPluginOrder;
	
	void ConstructL();
	
	
	/**
	 * Forces the plugin manager to free all plugin objects.
	 **/
	void ReleasePlugins();
	
	/**
	 * Forces the plugin manager to free the implemtation info array and
	 * reload it on the next call to Implementations.
	 **/
	void ReleaseImplInfo();
	
	/**
	 * Creates an implementation of the specified plugin.
	 * 
	 * @param aId Id of the plugin to be created.
	 *	
	 * @return the requested plugin
	 *	 
	 **/
	CAuthPluginInterface* CreatePluginImplementationL(const TPluginId &aId);
	
private:	
	mutable RPointerArray<CAuthPluginInterface> iPlugins;
	
	mutable RImplInfoPtrArray iImplArray;
	
	/**
	 * Maintain a list of plugin destructor keys.
	 * This is used to destroy all plugin instances when ReleasePlugins
	 * is called.
	 * */
	mutable RArray<TUid>iPluginDtorUids;
	
    };

} //namespace

#endif // PLUGINMGR_H
