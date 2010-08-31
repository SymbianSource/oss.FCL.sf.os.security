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
* CAuthServerSession class definition
*
*/


/**
 @file 
 @released
 @internalComponent
*/
#ifndef AUTHSERVERSESSION_H
#define AUTHSERVERSESSION_H

#include "authserver.h"
#include <authserver/auth_srv_errs.h>
#include <scs/scsserver.h>

namespace AuthServer
{

/**
 * This enum is used to make IPC indexes used as arguments in function calls
 * less obscure.
 * @internalComponent
 * @released
 */
enum TIpcIndexes
	{
	EIpcArgument0 = 0,  ///< IPC indexes arguments 0
	EIpcArgument1,      ///< IPC indexes arguments 1
	EIpcArgument2,      ///< IPC indexes arguments 2
	EIpcArgument3       ///< IPC indexes arguments 3
	};


/**
 * @internalComponent
 * @released
 */
class CAuthServerSession : public CScsSession
	{
public:
	void CreateL();
	static CAuthServerSession* NewL(CAuthServer &aServer);
private:
	~CAuthServerSession();
	
	/**
	 * Handle a client request.
	 * Leaving is handled by CAuthServer::ServiceError() which reports
	 * the error code to the client
	 **/
	TBool DoServiceL(TInt aFunction, const RMessage2& aMessage);
	
	/**
	 * Handle an error from CAuthServerSession::ServiceL().
	 * A bad descriptor error implies a badly programmed client, so panic it;
	 * otherwise use the default handling (report the error to the client)
	 **/
	void ServiceError(const RMessage2& aMessage,TInt aError);

 	void PanicClient(const RMessagePtr2& aMessage, TAuthServerPanic aPanic);

 	CAuthServerSession(CAuthServer &aServer);
 	
	CAuthServer* iAuthServer; 
	};
	
	
} //namespace

#endif // AUTHSERVERSESSION_H
