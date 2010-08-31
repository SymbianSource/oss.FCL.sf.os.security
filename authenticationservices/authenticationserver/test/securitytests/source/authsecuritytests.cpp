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
*
*/


#include "captestutility.h"
#include "authwriteuserdata.h"
#include "authreaduserdata.h"
#include "authreaddevicedata.h"
#include "authwritedevicedata.h"
#include "authtrustedui.h"
#include "authreaduseranddevicedata.h"

#include <s32file.h>

// Factory function
MCapabilityTestFactory* CapabilityTestFactoryL()
	{
	CDefaultCapabilityTestFactory* factory=new(ELeave) CDefaultCapabilityTestFactory();

	factory->AddTestL(CAuthReadUserDataSecTest::NewL());
	factory->AddTestL(CAuthReadDeviceDataSecTest::NewL());
	factory->AddTestL(CAuthWriteUserDataSecTest::NewL());
	factory->AddTestL(CAuthWriteDeviceDataSecTest::NewL());
	factory->AddTestL(CAuthReadUserAndDeviceDataSecTest::NewL());
	factory->AddTestL(CAuthTrustedUISecTest::NewL());
	
	return factory;
	}

EXPORT_C MCapabilityTestFactory* CapabilityTestFactory()
	{
	MCapabilityTestFactory* factory=NULL;
	TRAP_IGNORE(factory = CapabilityTestFactoryL());
	return factory;
	}


