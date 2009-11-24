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


/**
 @file 
 @publishedPartner
 @released
*/

#include <e32base.h>

 /**
   This 8-bit variable is used in agent resolution. It is used to override AuthServer's 
   default resolution behaviour of only loading plugins from the ROM drive. 
   Compile time value is 0 - default behaviour. 
   When non-zero, Authserver will load all plugins visible to the ECOM service regardless of 
   the drive. 
   This variable is changed at ROM build time using the patchdata obey keyword.
   See Symbian OS Developer Library documentation on how to patch DLL data exports.
   */
   
IMPORT_C extern const TUint8 KEnablePostMarketAuthenticationPlugins;

