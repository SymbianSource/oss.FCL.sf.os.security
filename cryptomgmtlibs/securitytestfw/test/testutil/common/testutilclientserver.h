/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* SisRegistry - shared client/server definitions
*
*/




/**
 @file 
 @test
 @internalComponent
*/

#include <e32std.h>

_LIT(KTestUtilServerName,"testutilsdpserver");
_LIT(KTestUtilServerImg,"testutilsdpserver");		// DLL/EXE name
const TUid KServerUid3={0x101F7295};

const TInt KMaxTestUtilMessage=100;

enum TTestUtilMessages
	{
	ECopy,
	EMove,
	EDelete,
	EMkDirAll,
	ERmDir,
	EFileExists,
	ELock,
	EUnlock,
	EFormat,
	EMount,
	EUnMount,
	ESetReadOnly,
	EGetFileHandle,
	EWatchFile,
	EWatchFileCancel,
	EGetNumFiles
	};
