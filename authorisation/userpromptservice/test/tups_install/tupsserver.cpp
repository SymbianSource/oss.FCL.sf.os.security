/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tups_installserver.cpp
*
*/


/**
 * @file
 *
 * SWIS test server implementation
 */

#include "tupsserver.h"
#include "tupsstep.h"

_LIT(KServerName, "tups_install");

/**
 * Called inside the MainL() function to create and start the CTestServer 
 * derived server.
 * @return Instance of the test server
 */
CUpsTestServer* CUpsTestServer::NewL()
	{
	CUpsTestServer *server = new(ELeave) CUpsTestServer();
	CleanupStack::PushL(server);
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
	CActiveScheduler::Install(sched);
	CUpsTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err, server = CUpsTestServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	delete sched;
	}

/**
 * Server entry point
 * @return Standard Epoc error code on exit
 */
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
 * Implementation of CTestServer pure virtual
 * @return A CTestStep derived instance
 */
CTestStep* CUpsTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step but create as many as you want
	// They are created "just in time" when the worker thread is created
	// install steps
	if (aStepName == KUpsInstallStep)	// Install with file name
		testStep = new CUpsInstallStep(CUpsInstallStep::EUseFileName);
// Uninstall
	else if (aStepName == KUpsUnInstallStep)
		testStep = new CUpsUninstallStep(CUpsUninstallStep::EByUid);
	else if (aStepName == KUpsUnInstallAugStep)
		testStep = new CUpsUninstallStep(CUpsUninstallStep::EByPackage);
	
	return testStep;

	}
