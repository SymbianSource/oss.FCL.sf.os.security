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
* authreadedvicedata.cpp
*
*/



#include "authreaddevicedata.h"

#include <authserver/authclient.h>
#include <authserver/authmgrclient.h>
#include <e32def.h>

_LIT(KAuthReadDeviceDataSecName, "Auth ReadDeviceData APIs test");

using namespace AuthServer;

CAuthReadDeviceDataSecTest* CAuthReadDeviceDataSecTest::NewL()
	{
	CAuthReadDeviceDataSecTest* self=new(ELeave) CAuthReadDeviceDataSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAuthReadDeviceDataSecTest::CAuthReadDeviceDataSecTest()
	{
	SetCapabilityRequired(ECapabilityReadDeviceData);
	}
	
void CAuthReadDeviceDataSecTest::ConstructL()
	{
	SetNameL(KAuthReadDeviceDataSecName);
	}

void CAuthReadDeviceDataSecTest::RunTestL()
	{
	AuthServer::RAuthMgrClient authMgrClient;	
	User::LeaveIfError(authMgrClient.Connect());
	CleanupClosePushL(authMgrClient);
	
	TInt err(0);
	
	AuthServer::RIdentityIdArray idList;
	TRAP(err, authMgrClient.IdentitiesL(idList));
	idList.Close();
	CheckFailL(err, _L("AuthServer::IdentitiesL()."));
	
	CleanupStack::PopAndDestroy(&authMgrClient);
	}
