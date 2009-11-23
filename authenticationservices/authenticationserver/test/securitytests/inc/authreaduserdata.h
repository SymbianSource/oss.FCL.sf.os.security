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
* authreaduserdata.cpp
*
*/


#include "captestutility.h"

class CAuthReadUserDataSecTest : public CDefaultCapabilityTest
	{
public:
	static CAuthReadUserDataSecTest* NewL();

private:
	CAuthReadUserDataSecTest();
	
	void RunTestL();

	void ConstructL();
	};