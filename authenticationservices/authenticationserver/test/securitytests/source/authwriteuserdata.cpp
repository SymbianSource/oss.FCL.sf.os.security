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


#include "authwriteuserdata.h"
#include <e32def.h>

_LIT(KAuthWriteUserDataSecName, "Auth WriteUserData APIs test");

using namespace AuthServer;

CAuthWriteUserDataSecTest* CAuthWriteUserDataSecTest::NewL()
	{
	CAuthWriteUserDataSecTest* self=new(ELeave) CAuthWriteUserDataSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAuthWriteUserDataSecTest::CAuthWriteUserDataSecTest()
	{
	SetCapabilityRequired(ECapabilityWriteUserData);
	}
	
void CAuthWriteUserDataSecTest::ConstructL()
	{
	SetNameL(KAuthWriteUserDataSecName);
	}

void CAuthWriteUserDataSecTest::RunTestL()
	{
	AuthServer::RAuthMgrClient authMgrClient;	
	User::LeaveIfError(authMgrClient.Connect());
	CleanupClosePushL(authMgrClient);
	
	TInt err(0);
	TIdentityId id = 34567890;
		
	TRAP(err, authMgrClient.SetIdentityStringL(id, KAuthWriteUserDataSecName));
	CheckFailL(err, _L("AuthServer::SetIdentityStringL()."));
	
	TRAP(err, DoTestL(authMgrClient));
	CheckFailL(err, _L("AuthServer::TrainPlugin()."));
	
	TPluginId pluginId = 11113100;
	TRAP(err, authMgrClient.ForgetPluginL(id, pluginId));
	CheckFailL(err, _L("AuthServer::ForgetPluginL()."));
		
	CleanupStack::PopAndDestroy(&authMgrClient);
	}
	
void CAuthWriteUserDataSecTest::DoTestL(AuthServer::RAuthMgrClient& aAuthMgrClient)
	{
	TRequestStatus status;
	TIdentityId id = 34567890;
	TPluginId pluginId = 11113100;
	
	aAuthMgrClient.TrainPlugin(id, pluginId, status);
	User::WaitForRequest(status);
	
	User::LeaveIfError(status.Int());
	}
