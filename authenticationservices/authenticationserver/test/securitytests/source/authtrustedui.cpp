/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "authtrustedui.h"

_LIT(KAuthTrustedUISecName, "Auth TrustedUI APIs test");

using namespace AuthServer;

class CRegActive : public CActive
    {
	public:
    CRegActive(RAuthMgrClient& aClient, CIdentity*& aResult) : 
         CActive(EPriorityNormal),
         iFirstTime(true),
         iClient(aClient), 
         iResult(aResult),
         iErr(KErrNone)
        {
        CActiveScheduler::Add(this);
        }
        
    void doReg()
        {
        SetActive();		
		iClient.RegisterIdentityL(iResult, KAuthTrustedUISecName, iStatus);
		}
        
    void DoCancel() 
        {
        }
        
    void RunL() 
    	{
        iErr = iStatus.Int();
        CActiveScheduler::Stop();
		}
        
    TInt RunError(TInt aError)
    	{
    	iErr = aError;
	   	CActiveScheduler::Stop();
    	return KErrNone;
    	}
        
    TBool iFirstTime;
    RAuthMgrClient& iClient;
    CIdentity*& iResult;
    TInt iErr;
    };
    

CAuthTrustedUISecTest* CAuthTrustedUISecTest::NewL()
	{
	CAuthTrustedUISecTest* self=new(ELeave) CAuthTrustedUISecTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAuthTrustedUISecTest::CAuthTrustedUISecTest()
	{
	SetCapabilityRequired(ECapabilityTrustedUI);
	}
	
void CAuthTrustedUISecTest::ConstructL()
	{
	SetNameL(KAuthTrustedUISecName);
	}

void CAuthTrustedUISecTest::RunTestL()
	{
	TInt err(0);
	
	TRAP(err, DoTestL());
	CheckFailL(err, _L("AuthServer::RegisterIdentityL()."));
	}
	
void CAuthTrustedUISecTest::DoTestL()
	{
	AuthServer::RAuthMgrClient authMgrClient;	
	User::LeaveIfError(authMgrClient.Connect());
	CleanupClosePushL(authMgrClient);
	
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	CIdentity* identity = 0;
	CRegActive active(authMgrClient, identity);
    active.doReg();
	CActiveScheduler::Start();	
	
	CleanupStack::PopAndDestroy(2, &authMgrClient);
	User::LeaveIfError(active.iErr);
	}

