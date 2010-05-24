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
* step_retrainplugin.cpp
* CTestStep derived implementation
*
*/

 
#include "tAuthSvrStep.h"

#include "authserver/authclient.h"
#include "authserver/authmgrclient.h"

using namespace AuthServer;

//====================================================================================================

CTRemoveDbs::CTRemoveDbs()
/**
 * Constructor
 */
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTRemoveDbs);
	}

CTRemoveDbs::~CTRemoveDbs()
/**
 * Destructor
 */
	{}
	
TVerdict CTRemoveDbs::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTAuthSvrStepBase::doTestStepPreambleL();
	
	//SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTRemoveDbs::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * 
 */
	{	
	//If any test step leaves or panics, the test step thread won't exit, further calls are made.
	// but the html log is updatead, so that subsequent failures could be traced here if necessary. 
	if (TestStepResult() != EPass)
		{
		INFO_PRINTF1(_L("There was an error in a previous test step"));
		//return TestStepResult();
		}

__UHEAP_MARK;		// Check for memory leaks

	SetTestStepResult(EPass);
	RemoveExistingDbL();
	
__UHEAP_MARKEND;
	return TestStepResult();
	}


TVerdict CTRemoveDbs::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//Call the parent postamble, releasing the file handle, etc
	CTStepActSch::doTestStepPostambleL();
	//INFO_PRINTF1(_L("Test Step Postamble"));
	return TestStepResult();
	}
