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


#include "authreaduserdata.h"

#include <authserver/authclient.h>
#include <authserver/authmgrclient.h>
#include <e32def.h>

_LIT(KAuthReadUserDataSecName, "Auth ReadUserData APIs test");

using namespace AuthServer;

CAuthReadUserDataSecTest* CAuthReadUserDataSecTest::NewL()
	{
	CAuthReadUserDataSecTest* self=new(ELeave) CAuthReadUserDataSecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAuthReadUserDataSecTest::CAuthReadUserDataSecTest()
	{
	SetCapabilityRequired(ECapabilityReadUserData);
	}
	
void CAuthReadUserDataSecTest::ConstructL()
	{
	SetNameL(KAuthReadUserDataSecName);
	}

void CAuthReadUserDataSecTest::RunTestL()
	{
	AuthServer::RAuthMgrClient authMgrClient;	
	User::LeaveIfError(authMgrClient.Connect());
	CleanupClosePushL(authMgrClient);

	TInt err(0);
	CAuthExpression* authExpr = AuthExpr(EAuthKnowledge);
	CleanupStack::PushL(authExpr);
	
	TUid clientSid = {0};
	
	AuthServer::CIdentity* identity = NULL;
	TRAP(err, identity = authMgrClient.AuthenticateL(*authExpr, 0, EFalse, clientSid, EFalse, KNullDesC()));
	CheckFailL(err, _L("AuthServer::AuthenticateL : Client Specific Key set to false."));
	
	TRAP(err, identity = authMgrClient.AuthenticateL(*authExpr, 0, ETrue, clientSid, ETrue, KNullDesC()));
	CheckFailL(err, _L("AuthServer::AuthenticateL : Identity String set to true"));
	
	clientSid.iUid = 12345678;	
	TRAP(err, identity = authMgrClient.AuthenticateL(*authExpr, 0, ETrue, clientSid, EFalse, KNullDesC()));
	CheckFailL(err, _L("AuthServer::AuthenticateL : ClientSid greater than zero."));
	
	if(identity != NULL)
		{
		delete identity;
		}
		
	TIdentityId id = 5110;
	HBufC* idString = NULL;
	TRAP(err, idString = authMgrClient.IdentityStringL(id));
	CheckFailL(err, _L("AuthServer::IdentityStringL()."));
	
	if(idString != NULL)
		{
		delete idString;
		}
	
	CleanupStack::PopAndDestroy(2, &authMgrClient);
	}

	
	
