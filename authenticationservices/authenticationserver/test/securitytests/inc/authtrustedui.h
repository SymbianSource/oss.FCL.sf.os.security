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
* authtrustedui.cpp
*
*/


#include "captestutility.h"
#include <authserver/authclient.h>
#include <authserver/authmgrclient.h>
#include <e32def.h>

class CRegActive;

class CAuthTrustedUISecTest : public CDefaultCapabilityTest
	{
public:
	static CAuthTrustedUISecTest* NewL();
	
private:
	CAuthTrustedUISecTest();
	
	void RunTestL();
	
	void DoTestL();

	void ConstructL();
	};