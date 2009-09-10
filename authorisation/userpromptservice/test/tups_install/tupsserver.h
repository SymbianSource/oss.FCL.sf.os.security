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
* tswisserver.h
* SWIS test server
*
*/




/**
 @file
*/

#ifndef __TUPSSERVER_H__
#define __TUPSSERVER_H__

#include <f32file.h>
#include <test/testexecuteserverbase.h>

class CUpsTestServer : public CTestServer
	{
public:
	static CUpsTestServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs() {return iFs;}

private:
	RFs iFs;
	};

#endif // __TUPSSERVER_H__
