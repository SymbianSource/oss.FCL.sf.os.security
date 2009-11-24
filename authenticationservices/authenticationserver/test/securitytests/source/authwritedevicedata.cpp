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


#include "authwritedevicedata.h"
#include <authserver/authclient.h>
#include <authserver/authmgrclient.h>
#include <scs/cleanuputils.h>
#include <e32def.h>

_LIT(KAuthWriteDeviceDataSecName, "Auth WriteDeviceData APIs test");

using namespace AuthServer;

CAuthWriteDeviceDataSecTest* CAuthWriteDeviceDataSecTest::NewL()
	{
	CAuthWriteDeviceDataSecTest* self=new(ELeave) CAuthWriteDeviceDataSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAuthWriteDeviceDataSecTest::CAuthWriteDeviceDataSecTest()
	{
	SetCapabilityRequired(ECapabilityWriteDeviceData);
	}
	
void CAuthWriteDeviceDataSecTest::ConstructL()
	{
	SetNameL(KAuthWriteDeviceDataSecName);
	}

void CAuthWriteDeviceDataSecTest::RunTestL()
	{
	AuthServer::RAuthMgrClient authMgrClient;	
	User::LeaveIfError(authMgrClient.Connect());
	CleanupClosePushL(authMgrClient);

	TInt err(0);
	TPluginId pluginId = 536883941;
			
	TRAP(err, authMgrClient.SetPreferredTypePluginL(EAuthKnowledge, pluginId));
	CheckFailL(err, _L("AuthServer::SetPreferredTypePluginL()."));

	TIdentityId identity = 420; // Any thing but 0 since client does sanity check
	
	TRAP(err, authMgrClient.RemoveIdentityL(identity));
	CheckFailL(err, _L("AuthServer::RemoveIdentityL()."));
		
	TRAP(err, authMgrClient.ResetIdentityL(identity, _L("")));
	CheckFailL(err, _L("AuthServer::ResetIdentityL() 1st overload."));

	TRAP(err, authMgrClient.ResetIdentityL(identity, EAuthKnowledge, _L("")));
	CheckFailL(err, _L("AuthServer::ResetIdentityL() 2nd overload."));

	RArray<TPluginId> pluginList;
	CleanupClosePushL(pluginList);
	RPointerArray<const HBufC> regDataList;
	CleanupResetAndDestroyPushL(regDataList);
	pluginList.AppendL(pluginId);
	regDataList.AppendL(KNullDesC().AllocL());
	TRAP(err, authMgrClient.ResetIdentityL(identity, pluginList, regDataList));
	CheckFailL(err, _L("AuthServer::ResetIdentityL() 3rd overload."));
	CleanupStack::PopAndDestroy(2, &pluginList);

	CleanupStack::PopAndDestroy(&authMgrClient);
	}
	
