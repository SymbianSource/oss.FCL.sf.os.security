/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TestExecute Server testing pin plugin
*
*/


/**
 @file 
*/
#include "tpinplugin.h"
#include "tpinpluginteststep.h"

using namespace AuthServer;

CPinPluginTestServer* CPinPluginTestServer::NewL()
	{
	CPinPluginTestServer* server = new (ELeave) CPinPluginTestServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit
	// Server Logging.
	TParsePtrC serverName(RProcess().FileName());	
	server->ConstructL(serverName.Name());
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
	CPinPluginTestServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CPinPluginTestServer::NewL());
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

// Only a DLL on emulator for typhoon and earlier

GLDEF_C TInt E32Main()
/**
 @return - Standard Epoc error code on exit
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

// Create a thread in the calling process
// Emulator typhoon and earlier

CTestStep* CPinPluginTestServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	// This server creates just one step 
	// They are created "just in time" when the worker thread is created

	if(aStepName == KPinPluginTestStep)
		{
		testStep = new CPinPluginTestStep();
		}
	else if (aStepName == KCreateInputFile)
		{
		testStep = new CCreateFile();
		}
	else
		{
		ERR_PRINTF1(_L("The test step name specified does not exist."));
		}

	return testStep;
	}



