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
* server implementation
*
*/


/**
 @file 
*/

#include "tauthdbserver.h"
#include "tauthdbstep.h"

_LIT(KServerName,"tauthdb");

/**
 * Called inside the MainL() function to create and start the CTestServer 
 * derived server.
 * @return Instance of the test server
 */
CTAuthDbServer* CTAuthDbServer::NewL()
	{
	CTAuthDbServer * server = new (ELeave) CTAuthDbServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit Server Logging.

	server->ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CTAuthDbServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTAuthDbServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	CleanupStack::Pop(sched);
	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());	
	delete cleanup;
	return KErrNone;
    }

CTestStep* CTAuthDbServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTStepPersist2)
		{
		testStep = new CTStepPersist2();
		}
	else if (aStepName == KTStepCreateTestDb)
		{
		testStep = new CTStepCreateTestDb;
		}
	
	return testStep;
	}
