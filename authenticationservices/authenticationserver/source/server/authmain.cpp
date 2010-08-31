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
* AuthServer startup code implementation
*
*/


/**
 @file 
*/

#include "authserver_impl.h"

using namespace AuthServer;

namespace
{

/**
 * Perform all server initialisation, in particular creation of the
 * scheduler and server and then run the scheduler
 */
static CScsServer* NewAuthServerLC()
/**
	This factory function is called by SCS.  It allocates
	the server object and leaves it on the cleanup stack.

	@return					New initialized instance of CScsTestServer.
							On return this is on the cleanup stack.
 */
	{
	return AuthServer::CAuthServer::NewLC();
	}

}

/**
 * Server process entry-point
 **/
TInt E32Main()
	{
	TInt err = StartScsServer(NewAuthServerLC);

	/**
     *  err can either be KErrNone (AuthServer is started successfully) or  
     *  KErrAlreadyExists(AuthServer is currently running). Panic for all other
     *  cases.
	 **/
		
	__ASSERT_DEBUG(err == KErrNone || err == KErrAlreadyExists, User::Panic(KAuthServerShortName, err));
	return err;
	}

