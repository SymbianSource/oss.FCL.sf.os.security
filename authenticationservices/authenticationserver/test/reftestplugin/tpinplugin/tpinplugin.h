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
*
*/


#include <test/TestExecuteServerBase.h>

class CPinPluginTestServer : public CTestServer
	{
public:
	/**
 	 Called inside the MainL() function to create and start the
 	 CTestServer derived server.
 	 
 	 @return - Instance of the test server
 	 */
	static CPinPluginTestServer* NewL();
	
	/**
	 Implementation of CTestServer pure virtual
 	 @return - A CTestStep derived instance
 	 */
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};

private:
	RFs iFs;
	};


