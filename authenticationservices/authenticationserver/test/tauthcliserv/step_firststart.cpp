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
*
*/


#include "tauthcliservstep.h"
#include <test/testexecutelog.h>

#include "authserver/authmgrclient.h"
#include "authserver/identity.h"

#include <s32file.h>

using namespace AuthServer;

CTStepFirstStart::CTStepFirstStart()
	{
	SetTestStepName(KTStepFirstStart);
	}

TVerdict CTStepFirstStart::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		INFO_PRINTF1(_L("Previous step failed"));

		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks
    SetTestStepResult(EFail);
    
	TBool res = ETrue;
	TBool tmp = ETrue;
	TInt  err = KErrNone;

 	User::SetJustInTime(ETrue);

    TRAP(err, RemoveExistingDbL());
    
	RAuthClient client;
	err = client.Connect();
	CleanupClosePushL(client);

    TEST(tmp = (err == KErrNone));
    res = tmp && res;
    INFO_PRINTF2(_L("Connect result = %d"), err);
    
	RIdentityIdArray ids;
	client.IdentitiesL(ids); 
	
	TEST(tmp = ids.Count() == 1);
    res = tmp && res;

	HBufC* str = client.IdentityStringL(ids[0]);
	CleanupStack::PushL(str);
	
	ids.Close();
	
	SetTestStepResult(res ? EPass : EFail);

	CleanupStack::PopAndDestroy(2, &client);
__UHEAP_MARKEND;	
	return TestStepResult();
	}

