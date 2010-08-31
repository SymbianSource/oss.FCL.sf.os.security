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
* CAuthTransaction class definition
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef AUTHTRANSACTION_H
#define AUTHTRANSACTION_H

#include <e32cmn.h>
#include <e32base.h>
#include "authserver/authtypes.h"

namespace AuthServer 
{

class CAuthExpressionImpl;

/**
 * This class holds state information about the current asynchronous
 * authentication taking place.
 **/
class CAuthTransaction : public CBase
	{
public:
	struct TPluginResult
		{
		TPluginId iPlugin;
		TIdentityId* iId;		
		HBufC8* iData;
	    }; 

	typedef RPointerArray<TPluginResult> RResultArray;
	
	static CAuthTransaction* NewL(const RMessage2& aMsg,
								  TBool aClientKey, TInt aClientSid,
								  TBool aWithString, const TDesC& aClientMessage,
								  const CAuthExpressionImpl* aExpr);
	
	HBufC8*& AddPluginL(TPluginId& aPluginId,
						TIdentityId& aIdentityId);
					   
	const RMessagePtr2& Message() { return iMsg; }

	RResultArray& Results() { return iRecords; }
	
	TBool ClientKey() { return iClientKey; }

	TBool WithString() { return iWithString; }
	
	TInt ClientSid()  { return iClientSid;	}

	const HBufC* ClientMessage() {return static_cast<const HBufC*>(&iClientMessage); }

	TPluginId LastPluginId() const;
	
	~CAuthTransaction();
	
private:

	CAuthTransaction(const RMessage2& aMsg, TBool aClientKey, TInt aClientSid,
					 TBool aWithString, const TDesC& aClientMessage,
					 const CAuthExpressionImpl* aExpr);
					 
	void ConstructL(const RMessage2& aMessage);
	
	/// the message to complete at the end
    const RMessagePtr2         iMsg;
    
	/// are we returning a client key?
	TBool                      iClientKey;
	   
	TInt					   iClientSid;
       
	/// are we returning a client key?
	TBool                      iWithString;
	
    /// the displayable text string to the plugin
    const TDesC&              	iClientMessage;
    
	// the authentication expression being used
	const CAuthExpressionImpl* iExpr;
	
	/// A list of results returned by each plugin called
    RResultArray               iRecords;
    
    };

} //namespace

#endif // AUTHTRANSACTION_H
