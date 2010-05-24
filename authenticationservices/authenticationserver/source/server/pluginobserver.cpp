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
 @released
*/


#include <e32debug.h>
#include <s32mem.h>
#include "authserver_impl.h"
#include "log.h"

using namespace AuthServer;

/**
  CPluginObserver registers for ECOM notification when new authentication plugins are added 
  or removed and updates the plugin manager accordingly. 
 
  */

/** 
  Constructs a new CPluginObserver object and puts it on the cleanup stack.
  */

CPluginObserver* CPluginObserver::NewLC(CPluginMgr& aPluginMgr)
	{
	CPluginObserver* self = new (ELeave) CPluginObserver(aPluginMgr);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/** 
  Constructs a new CPluginObserver object.
  */

CPluginObserver* CPluginObserver::NewL(CPluginMgr& aPluginMgr)
	{
	CPluginObserver* self = CPluginObserver::NewLC(aPluginMgr);
	CleanupStack::Pop(self);
	return self;
	}

CPluginObserver::CPluginObserver(CPluginMgr& aPluginMgr) : CActive(EPriorityStandard), iPluginMgr(aPluginMgr)
	{
	
	}

CPluginObserver::~CPluginObserver()
	{
	Cancel();
		
	// Close our ECOM session
	if(iEcomSession)
		{
		iEcomSession->CancelNotifyOnChange(iStatus);
		iEcomSession->Close();
		REComSession::FinalClose();
		}
	}

void CPluginObserver::ConstructL()
	{
	// Add ourselves to the current active scheduler so we can get dynamic 
	// updates when authentication plugins are removed or new plugins are added
	CActiveScheduler::Add(this);
	
	iEcomSession = &REComSession::OpenL();
	
	// register for ECOM update notifications in case a new agent appears
	iEcomSession->NotifyOnChange(iStatus);
	SetActive();
	}
	
void CPluginObserver::DoCancel()
	{
	// Abort any update notification 
	iEcomSession->CancelNotifyOnChange(iStatus);
	}

void CPluginObserver::RunL()
	{
	// Leave if there has been an error
	User::LeaveIfError(iStatus.Int());
	
	// the ownership is with auth server, so dont delete it.
	iPluginMgr.BuildAuthPluginsListL();
	
	// request notification of any further changes
	iEcomSession->NotifyOnChange(iStatus);
	SetActive();
	}
	
TInt CPluginObserver::RunError(TInt aError)
    {
    _LIT(KAuthError, "Authserver Panic :");
    User::Panic(KAuthError, aError);
	    			
	return KErrNone;
	}
	
