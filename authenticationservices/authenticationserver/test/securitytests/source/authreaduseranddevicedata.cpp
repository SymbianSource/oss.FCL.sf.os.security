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



#include "authreaduseranddevicedata.h"

#include <authserver/authclient.h>
#include <authserver/authmgrclient.h>
#include <e32def.h>

_LIT(KAuthReadUserAndDeviceDataSecName, "Auth ReadUserAndDeviceDeviceData APIs test");

using namespace AuthServer;

CAuthReadUserAndDeviceDataSecTest* CAuthReadUserAndDeviceDataSecTest::NewL()
	{
	CAuthReadUserAndDeviceDataSecTest* self=new(ELeave) CAuthReadUserAndDeviceDataSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAuthReadUserAndDeviceDataSecTest::CAuthReadUserAndDeviceDataSecTest()
	{
	SetCapabilityRequired(ECapabilityReadUserData);
	SetCapabilityRequired(ECapabilityReadDeviceData);	
	}
	
void CAuthReadUserAndDeviceDataSecTest::ConstructL()
	{
	SetNameL(KAuthReadUserAndDeviceDataSecName);
	}

void CAuthReadUserAndDeviceDataSecTest::RunTestL()
	{
	AuthServer::RAuthMgrClient authMgrClient;	
	User::LeaveIfError(authMgrClient.Connect());
	CleanupClosePushL(authMgrClient);

	TInt err(0);
	
	RIdAndStringArray idWithString;
	TRAP(err, authMgrClient.IdentitiesWithStringL(idWithString));
	idWithString.Close();
	CheckFailL(err, _L("AuthServer::IdentitiesWithStringL()."));
	
	CleanupStack::PopAndDestroy(&authMgrClient);
	}
