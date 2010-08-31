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
*
*/



#include <e32std.h>
#include <ecom/ImplementationProxy.h>	// ECOM header file
//#include <ImplementationInformation.h>

#include "tPluginBase.h"			// Class definition for with the available implementations


// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x11112FFF,	CTPlugin11112FFF::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113000,	CTPlugin11113000::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113001,	CTPlugin11113001::NewL),		
		IMPLEMENTATION_PROXY_ENTRY(0x11113002,	CTPlugin11113002::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113003,	CTPlugin11113003::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113004,	CTPlugin11113004::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113005,	CTPlugin11113005::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113006,	CTPlugin11113006::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113007,	CTPlugin11113007::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113008,	CTPlugin11113008::NewL),
		IMPLEMENTATION_PROXY_ENTRY(0x11113009,	CTPlugin11113009::NewL)
	};

// Exported proxy function to resolve intstantiation methods for an ECOM plugin dll
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
