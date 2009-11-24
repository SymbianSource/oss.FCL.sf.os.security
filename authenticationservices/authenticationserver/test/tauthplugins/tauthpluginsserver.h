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
* TAuthPluginsServer is the server for all authentication plugins tests
*
*/


#ifndef TAUTHPLUGINSSERVER_H
#define TAUTHPLUGINSSERVER_H
#include <test/testexecuteserverbase.h>

class CTAuthPluginsServer : public CTestServer
	{
public:
	static CTAuthPluginsServer*	NewL();
	virtual CTestStep*			CreateTestStep(const TDesC& aStepName);
	RFs& Fs()	{return iFs;};

private:
	RFs								iFs;
	};

#endif	/* TAUTHPLUGINSSERVER_H */

