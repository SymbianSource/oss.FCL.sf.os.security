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
* tAuthSvrServer.cpp
* Test code for the TestExecute Server
*
*/



#include "tAuthSvrServer.h"
#include "tAuthSvrStep.h"


_LIT(KServerName,"tAuthSvrNC");	

CTAuthSvrServer* CTAuthSvrServer::NewL()
/**
 * @return - Instance of the test server
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTAuthSvrServer* server = new (ELeave) CTAuthSvrServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// server Logging.

	//server->StartL(KServerName); 
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}


// EKA2 much simpler
// Just an E32Main and a MainL()

LOCAL_C void MainL()
/**
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
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
	
	CTAuthSvrServer* server = NULL;
	// Create the CTestServer derived server	
	TRAPD(err,server = CTAuthSvrServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	CleanupStack::Pop(sched);
	delete server;
	delete sched;
	}


GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on exit
 */
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


CTestStep* CTAuthSvrServer::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	
	// Test step name constant in the test step header file
	// Created "just in time"
	if(aStepName == KTFirstStart)
		testStep = new CTFirstStart();
	else if (aStepName == KTAuthSvrCheck)
		testStep = new CTAuthSvrCheck();
	else if (aStepName == KTRegIdentity)
		testStep = new CTRegIdentity(*this);
	else if (aStepName == KTIdentifyMultiple)
		testStep = new CTIdentifyMultiple();
	else if (aStepName == KTStepCreateTestDb)
		testStep = new CTStepCreateTestDb();
	else if (aStepName == KTRetrainPlugin)
		testStep = new CTRetrainPlugin(*this);
	else if (aStepName == KTRemoveDbs)
		testStep = new CTRemoveDbs();
	else if (aStepName == KTRemoveIdentity)
		testStep = new CTRemoveIdentity(*this);
	else if (aStepName == KTForgetPlugin)
		testStep = new CTRemoveAuthMethod(*this);	
	else if (aStepName == KTAuthenticate)
		testStep = new CTAuthenticate(*this);
	else if (aStepName == KTSetPrefs)
		testStep = new CTSetAuthPrefs(*this);
	else
		{
		ERR_PRINTF1(_L("The test step name specified does not exist."));
		}
	
	return testStep;
	}

