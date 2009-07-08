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


#include <sectcbutil.h>
#include <f32file.h>
#include <hal.h>

EXPORT_C TDriveNumber SecTcbUtil::GetSystemDrive()
	{
	TDriveNumber sysDrive = RFs::GetSystemDrive();
	return sysDrive;
	}

EXPORT_C TChar SecTcbUtil::GetSystemDriveChar(TDriveNumber /*aDriveNumber*/)
	{
	TChar driveChar;	
	RFs::DriveToChar(GetSystemDrive(), driveChar);
	return driveChar;
	}
