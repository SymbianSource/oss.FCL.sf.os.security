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
* TAuthDbServer is the server for all authentication keys tests
*
*/


#ifndef TAUTHDBSERVER_H
#define TAUTHDBSERVER_H
#include <test/testexecuteserverbase.h>

class CTAuthDbServer : public CTestServer
	{
public:
	static CTAuthDbServer*	NewL();
	virtual CTestStep*      CreateTestStep(const TDesC& aStepName);
	RFs& Fs()	{return iFs;};

private:
	RFs	iFs;
	};

#endif	/* TAUTHDBSERVER_H */

