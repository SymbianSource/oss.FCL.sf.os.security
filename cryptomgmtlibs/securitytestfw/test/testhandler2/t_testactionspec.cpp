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


#include "t_testactionspec.h"
#include "t_testsetup.h"
#include "tScriptSetup.h"
#include "t_input.h"
#include "tScriptTests.h"

_LIT8(KTrue, "true");
_LIT8(KFalse, "false");

_LIT8(KExOOMStart, "<exoom>");
_LIT8(KExOOMEnd, "</exoom>");
_LIT8(KInOOMStart, "<inoom>");
_LIT8(KInOOMEnd, "</inoom>");
_LIT8(KExCancelStart, "<excancel>");
_LIT8(KExCancelEnd, "</excancel>");
_LIT8(KInCancelStart, "<incancel>");
_LIT8(KInCancelEnd, "</incancel>");
_LIT8(KSkippedStart, "<skipped>");
_LIT8(KSkippedEnd, "</skipped>");
_LIT8(KSmokeStart, "<smoke>");
_LIT8(KSmokeEnd, "</smoke>");
_LIT8(KInteractiveStart, "<interactive>");
_LIT8(KInteractiveEnd, "</interactive>");

TTestActionSpec::TTestActionSpec()
	{
	}

TInt TTestActionSpec::Init(const TDesC8& aInput, 
						   TInt& aPos, 
						   CConsoleBase& /*aConsole*/,
						   Output& aOut,
						   TInt& aBitFlag)
	{
	TInt err = KErrNone;
	iActionName.Set(Input::ParseElement(aInput, KActionNameStart, KActionNameEnd, 
		aPos, err));
	if (err != KErrNone)
		{
		aOut.writeString(_L("Error couldn't find actionname in test case spec"));
		aOut.writeNewLine();
		return err;
		}
	
	iActionType.Set(Input::ParseElement(aInput, KActionTypeStart, KActionTypeEnd, aPos, err));
	if (err != KErrNone)
		{
		aOut.writeString(_L("Error couldn't find actiontype in test case spec"));
		aOut.writeNewLine();
		return err;
		}
	
	iActionGroup.Set(Input::ParseElement(aInput, KActionGroupingStart, KActionGroupingEnd, aPos, err));
	if (err == KErrNone)
		{
		//Do parsing of the returned groupings string
		TInt relativePos=0;
		TPtrC8 excludeOOM = Input::ParseElement(iActionGroup, KExOOMStart, KExOOMEnd, relativePos, err);
		relativePos=0;
		TPtrC8 includeOOM = Input::ParseElement(iActionGroup, KInOOMStart, KInOOMEnd, relativePos, err);
		relativePos=0;
		TPtrC8 excludeCancel = Input::ParseElement(iActionGroup, KExCancelStart, KExCancelEnd, relativePos, err);
		relativePos=0;
		TPtrC8 includeCancel = Input::ParseElement(iActionGroup, KInCancelStart, KInCancelEnd, relativePos, err);
		relativePos=0;
		TPtrC8 skipped = Input::ParseElement(iActionGroup, KSkippedStart, KSkippedEnd, relativePos, err);
		relativePos=0;
		TPtrC8 smoketest = Input::ParseElement(iActionGroup, KSmokeStart, KSmokeEnd, relativePos, err);
		relativePos=0;
		TPtrC8 interactive = Input::ParseElement(iActionGroup, KInteractiveStart, KInteractiveEnd, relativePos, err);

		if (excludeOOM==KTrue)
			{
			aBitFlag|=EXOOM;
			}
		else if (excludeOOM==KFalse)
			{
			aBitFlag&= (~EXOOM);
			}
		
		if (includeOOM==KTrue)
			{
			aBitFlag|=INOOM;
			}
		else if (includeOOM==KFalse)
			{
			aBitFlag&= (~INOOM);
			}

		if (excludeCancel==KTrue)
			{
			aBitFlag|=EXCANCEL;
			}
		else if (excludeCancel==KFalse)
			{
			aBitFlag&= (~EXCANCEL);
			}

		if (includeCancel==KTrue)
			{
			aBitFlag|=INCANCEL;
			}
		else if (includeCancel==KFalse)
			{
			aBitFlag&= (~INCANCEL);
			}

		if (skipped==KTrue)
			{
			aBitFlag|=SKIP;
			}
		else if (skipped==KFalse)
			{
			aBitFlag&= (~SKIP);
			}
	
		if (smoketest==KTrue)
			{
			aBitFlag|=SMOKE;
			}
		else if (smoketest==KFalse)
			{
			aBitFlag&= (~SMOKE);
			}

		if (interactive==KTrue)
			{
			aBitFlag|=INTER;
			}
		else if (interactive==KFalse)
			{
			aBitFlag&= (~INTER);
			}
		}
	
	iActionBody.Set(Input::ParseElement(aInput, KActionBodyStart, KActionBodyEnd, aPos, err));
	iActionResult.Set(Input::ParseElement(aInput, KActionResultStart, KActionResultEnd, aPos, err));
	
	return KErrNone; 
	}

void TTestActionSpec::HardcodedInit(const TDesC8& aInput)
	{
	iActionName.Set(aInput);
	iActionType.Set(KNullDesC8);
	iActionGroup.Set(KNullDesC8);
	iActionBody.Set(KNullDesC8);
	iActionResult.Set(KNullDesC8);
	}
