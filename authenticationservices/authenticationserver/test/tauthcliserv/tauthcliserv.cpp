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
* tauthdbserver.cpp
* server implementation
*
*/


/**
 @file 
*/

#include "tauthcliservstep.h"

_LIT(KServerName,"tauthcliserv");

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

	if(aStepName == KTStepClient)
		{
		testStep = new CTStepClient();
		}
	else if(aStepName == KTStepMgrClient)
		{
		testStep = new CTStepMgrClient();
		}
	else if(aStepName == KTStepFirstStart)
		{
		testStep = new CTStepFirstStart();
		}

	// authentication expression tests
	else if (aStepName == KTStepAuthExprBuild)
		{
		testStep = new CTStepAuthExprBuild();
		}
	else if (aStepName == KTStepBadExtAuthExpr)
		{
		testStep = new CTStepBadExtAuthExpr();
		}
	else if (aStepName == KTStepAuthExprHighVer)
		{
		testStep = new CTStepAuthExprHighVer();
		}
	else if (aStepName == KTStepAuthExprEval)
		{
		testStep = new CTStepAuthExprEval();
		}
#ifdef _DEBUG
	else if (aStepName == KTStepAuthExprTypePnc)
		{
		testStep = new CTStepAuthExprTypePnc();
		}
	else if (aStepName == KTStepAuthExprTypePncBadLeft)
		{
		testStep = new CTStepAuthExprTypePncBadLeft();
		}
	else if (aStepName == KTStepAuthExprTypePncBadRight)
		{
		testStep = new CTStepAuthExprTypePncBadRight();
		}
#endif

	return testStep;
	}
