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
* CTestStep derived implementation
*
*/


 
#include "tAuthSvrStep.h"

#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"

CTAuthSvrCheck::CTAuthSvrCheck()
/**
  Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTAuthSvrCheck);
	}

TVerdict CTAuthSvrCheck::doTestStepL()
/**
  @return - TVerdict code
  Override of base class pure virtual
  
 */
	{	
	
__UHEAP_MARK;		// Check for memory leaks

	SetTestStepResult(EPass);
	
	InitAuthServerFromFileL();	// Set things like 'iSupportsDefaultData' and 'DefaultPlugin'

	//-----------------------------------------------------------------------------------------------------
	//Connect to the AuthServer
	AuthServer::RAuthClient authClient1;
	//User::LeaveIfError(authClient1.Connect());
	
	TInt connectVal = authClient1.Connect();
	if (KErrNotFound == connectVal)
		{
		//Retry after a delay
		TTimeIntervalMicroSeconds32 timeInterval = 2000;	//2 Milliseconds
		User::After(timeInterval);
		connectVal = authClient1.Connect();
		}
	if (KErrNone != connectVal)
		{
		ERR_PRINTF2(_L("Unable to start a session or other connection error. Err = %d"), connectVal);
		User::LeaveIfError(connectVal);		
		}
	CleanupClosePushL(authClient1);
	
	//Check that the default identity has been created
	//Call the Enumerate Identity API
	AuthServer::RIdentityIdArray idList;
	TRAPD(res1, authClient1.IdentitiesL(idList));
	CleanupClosePushL(idList);
	
	if(KErrNone != res1)
		{
		ERR_PRINTF2(_L("authClient::IdentitiesL() performed a Leave with code %d"), res1 );
		}
	else
		{
		//Retrieve the expected information from the ini file
		TInt expNumIdentities;
	
		if(GetIntFromConfig(ConfigSection(),_L("NumIdentities"), expNumIdentities) != EFalse) // the tag 'NumIdentities' was present
			{
			//Do the compare
			if (idList.Count() != expNumIdentities)
				{
				ERR_PRINTF3(_L("expNumIdentities differs from expected, Expected = %d, Number of Identities = %d"), expNumIdentities, idList.Count());			
				SetTestStepResult(EFail);
				}
			}
		
		// Check the status of a plugin compared to what was expected
		CheckSpecifiedPluginStatusConnectedL(authClient1);
		}
	CleanupStack::PopAndDestroy(&idList);
	CleanupStack::PopAndDestroy(&authClient1); 

__UHEAP_MARKEND;
	return TestStepResult();	
	}
