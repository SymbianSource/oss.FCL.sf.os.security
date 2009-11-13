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


#ifndef __SECTCBUTIL_H__
#define __SECTCBUTIL_H__

/**
 @file
 @internalTechnology
 @released
 */

#include <f32file.h>
 
namespace SecTcbUtil
	{
	/**
	Determines the preferred internal, persistent and read-writable drive (system drive) 
	by calling RFs GetSystemDrive API. 
	
	@return The drive number of the system drive
	*/
	IMPORT_C TDriveNumber GetSystemDrive();
	
	IMPORT_C TChar GetSystemDriveChar(TDriveNumber aDriveNumber);
	}

#endif
