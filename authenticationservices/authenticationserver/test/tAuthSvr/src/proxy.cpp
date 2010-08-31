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



#include <e32std.h>
#include <ecom/implementationproxy.h>	// ECOM header file

#include "tPinAuthPlugin.h"			// Class definition for one of the available implementations


// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
	{
		//Next line commented because of the use of parameterised classes
		//IMPLEMENTATION_PROXY_ENTRY(0x11113100,	CTPinAuthPlugin::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113100,	CTPinAuthPlugin11113100::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113101,	CTPinAuthPlugin11113101::NewL),		
		IMPLEMENTATION_PROXY_ENTRY(0x11113102,	CTPinAuthPlugin11113102::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113103,	CTPinAuthPlugin11113103::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113104,	CTPinAuthPlugin11113104::NewL)
	};

// Exported proxy function to resolve intstantiation methods for an ECOM plugin dll
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
