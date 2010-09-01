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
*
*/


#include "tbackuprestore.h"
#include "tbackuprestorestep.h"

_LIT(KServerName,"tups_backuprestore");

CTBackupRestoreServer* CTBackupRestoreServer::NewL()
	{
	CTBackupRestoreServer * server = new (ELeave) CTBackupRestoreServer();
	CleanupStack::PushL(server);
	
	// Either use a StartL or ConstructL, the latter will permit Server Logging.

	//server->StartL(KServerName); 
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
	CTBackupRestoreServer* server = NULL;
	TRAPD(err,server = CTBackupRestoreServer::NewL());
	if(!err)
		{
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
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

CTestStep* CTBackupRestoreServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;

	if(aStepName == KTStepInstall)
		{
		testStep = new CTStepInstall();
		}
	else if(aStepName == KTStepUninstall)
		{
		testStep = new CTStepUninstall();
		}
	else if(aStepName == KTStepBackup)
		{
		testStep = new CTStepBackup();
		}
	else if(aStepName == KTStepRestore)
		{
		testStep = new CTStepRestore();
		}

	return testStep;
	}
