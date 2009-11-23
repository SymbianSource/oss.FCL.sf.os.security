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
* CAuthTransaction implementation
*
*/


/**
 @file 
*/

#include <s32file.h>
#include <s32mem.h>
#include "authtransaction.h"
#include "authexpression_impl.h"
#include "authserveripc.h"

using namespace AuthServer;


CAuthTransaction* CAuthTransaction::NewL(const RMessage2& aMsg,
										 TBool aClientKey, 
										 TInt aClientSid,
										 TBool aWithString,
									 	 const TDesC& aClientMessage,
									 	 const CAuthExpressionImpl* aExpr)
	{
	CAuthTransaction* me =
		new (ELeave) CAuthTransaction(aMsg, aClientKey, aClientSid, aWithString,
									  aClientMessage, aExpr);
	return me;
	}
	

CAuthTransaction::CAuthTransaction(const RMessage2& aMsg, TBool aClientKey, 
									TInt aClientSid, TBool aWithString,
									 const TDesC& aClientMessage, const CAuthExpressionImpl* aExpr) :
	iMsg(aMsg), iClientKey(aClientKey),
	iClientSid(aClientSid), iWithString(aWithString),
	iClientMessage(aClientMessage),	iExpr(aExpr)
	{
	 
	if(iClientSid == 0)
		{
		iClientSid = iMsg.SecureId();
		}
	}
	
HBufC8*& CAuthTransaction::AddPluginL(TPluginId& aPluginId,
									 TIdentityId& aIdentityId)
	{
	TPluginResult* result  = new (ELeave) TPluginResult;
	CleanupStack::PushL(result);
	
	Mem::FillZ(result, sizeof(TPluginResult));
	result->iPlugin = aPluginId;
	result->iId = &aIdentityId;
	iRecords.AppendL(result);
	CleanupStack::Pop(result);
	return result->iData;
	}


TPluginId CAuthTransaction::LastPluginId() const
	{
	return iRecords[iRecords.Count()-1]->iPlugin; 
	}
	
CAuthTransaction::~CAuthTransaction()
	{
	delete iExpr;
	TInt count = iRecords.Count();
	for (TInt i = 0 ; i < count ; ++i)
	    {
		delete iRecords[i]->iData;
		delete iRecords[i];
	    }
	
	iRecords.Close();
	
	}
