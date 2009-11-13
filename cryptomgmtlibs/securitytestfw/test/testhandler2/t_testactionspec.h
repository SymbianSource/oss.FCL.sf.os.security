/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_TESTACTIONSPEC_H__
#define __T_TESTACTIONSPEC_H__

#include <e32base.h>

class CConsoleBase;
class Output;

/**
 * TODO
 */
class TTestActionSpec
	{
public:
	TTestActionSpec();
	TInt Init(const TDesC8& aInput, TInt& aPos, CConsoleBase& aConsole, Output& aOut, TInt& aBitFlag);
	void HardcodedInit(const TDesC8& aInput);

public:
	TPtrC8 iActionName;
	TPtrC8 iActionType;
	TPtrC8 iActionGroup;
	TPtrC8 iActionBody;
	/**
	 * This member indicates the expected result of the action.
	 */
	TPtrC8 iActionResult;
	};

#endif
