/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
*/
  
#ifndef PLUGINOBSERVER_H
#define PLUGINOBSERVER_H

#include <e32cmn.h>
#include <ecom/ecom.h>

namespace AuthServer
{
class CPluginMgr;

class CPluginObserver : public CActive
	{
public:

	static CPluginObserver* NewLC(CPluginMgr& aPluginMgr);
	static CPluginObserver* NewL(CPluginMgr& aPluginMgr);
	virtual ~CPluginObserver();	
	
private:

	// From CActive, used for dynamically updating the list of authentication plugins.
	virtual void DoCancel();
	virtual void RunL();
	TInt RunError(TInt aError);
	
	CPluginObserver(CPluginMgr& aPluginMgr);
	void ConstructL();

private:
	
	REComSession* iEcomSession;
	CPluginMgr& iPluginMgr;
	};
	
}	//namespace

#endif
