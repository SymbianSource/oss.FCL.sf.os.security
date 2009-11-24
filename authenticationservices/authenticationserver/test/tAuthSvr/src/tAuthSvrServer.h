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
* tAuthSvrServer is the base class for all authSvr tests
*
*/



#ifndef T_AUTHSVR_SERVER_H
#define T_AUTHSVR_SERVER_H

#include <test/testexecuteserverbase.h>
#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"



class CTAuthSvrServer : public CTestServer
	{
public:
	static CTAuthSvrServer*		NewL();	// First phase constructor
	// Base class pure virtual
	virtual CTestStep*			CreateTestStep(const TDesC& aStepName);
	//RFs& Fs()	{return iFs;};
	//AuthServer::RAuthClient Cl() {return client3;};
	AuthServer::RAuthMgrClient Cl() {return client2;};	
	//RAuthClient will be tested by way of inheritance
	
private:
	//AuthServer::RAuthClient client3;	//variable that will be shared by the steps
	AuthServer::RAuthMgrClient client2;	//variable that will be shared by the steps
	//RFs		iFs;
	
	};

#endif	/* T_AUTHSVR_SERVER_H */
