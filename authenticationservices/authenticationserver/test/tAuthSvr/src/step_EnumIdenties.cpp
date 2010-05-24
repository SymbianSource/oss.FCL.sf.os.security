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
* CTestStep derived implementation
*
*/

 
#include "tAuthSvrStep.h"
//#include <testexecutelog.h>

#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"
#include "authserver/identity.h"

//using namespace AuthServer;

CTEnumIdentities::CTEnumIdentities(CTAuthSvrServer& aParent): iParent(aParent)
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTEnumIdentities);
	}


TVerdict CTRegIdentity::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * 
 */
	{	
	INFO_PRINTF1(_L("In EnumIdentities Step"));
	SetTestStepResult(EPass);
	
	//If any test step leaves or panics, the test step thread exits and no further calls are made. 
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}

__UHEAP_MARK;		// Check for memory leaks

	InitAuthServerFromFile();
	AuthServer::RAuthMgrClient client4 = (iParent.Cl());	//This is an R class, so the copy constructor provides the actual handle
		
	//User::LeaveIfError(iParent.Cl().Connect());	
	//CleanupClosePushL(iParent.Cl());
	User::LeaveIfError(client4.Connect());	
	CleanupClosePushL(client4);
	
	//Call the Enumerate Identities API
	AuthServer::RIdentityIdArray idArray;	//Initialisation
	iParent.Cl().IdentitiesL(idArray);
	
	
	//AuthServer::CIdentity aIdentity1 = new (ELeave) AuthServer::CIdentity(AuthServer::KUnknown, 0);
	
	TUint32 id1 = 0;
	AuthServer::CProtectionKey *key1 (AuthServer::CProtectionKey::NewL(8));
		
 	AuthServer::CIdentity *aIdentity1 = new AuthServer::CIdentity(id1, key1);
 	TRequestStatus reqStatus = KRequestPending;	//for async calls
	
	TRAPD(err, iParent.Cl().RegisterIdentity(*aIdentity1, reqStatus));
		
	User::WaitForRequest(reqStatus); //delay
	//Check reqStatus
	if (reqStatus.Int() == KErrNone)	//The request completed ok
		{					
		INFO_PRINTF2(_L("Request completed ok. reqStatus = %d"), reqStatus.Int());
		SetTestStepResult(EPass);
		}
	else 	// The request failed
		{					
		INFO_PRINTF2(_L("Request failed. reqStatus = %d"), reqStatus.Int());
		SetTestStepResult(EFail);
		}
	
	//CleanupStack::PopAndDestroy(&(iParent.Cl());
	CleanupStack::PopAndDestroy(&client4);
	
__UHEAP_MARKEND;
	return TestStepResult();	
	}
