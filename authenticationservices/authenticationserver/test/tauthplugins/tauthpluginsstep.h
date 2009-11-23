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


#ifndef TAUTHPLUGINSSTEP_H
#define TAUTHPLUGINSSTEP_H

#include <test/testexecutestepbase.h>
#include "tauthpluginsserver.h"

_LIT(KTStepInterface, "INTERFACE");

class CTAuthPluginsStepBase : public CTestStep
	{
protected:
	CTAuthPluginsStepBase() {}
	~CTAuthPluginsStepBase() {}
	virtual TVerdict doTestStepPreambleL()
	{
	iFs.Connect();
 	return TestStepResult();
	}
	virtual TVerdict doTestStepPostambleL()
	{
	iFs.Close();
	return TestStepResult();
	}
	
protected:
	RFs		iFs;
	};


class CTStepInterface : public CTAuthPluginsStepBase
	{
public:
	CTStepInterface();
	~CTStepInterface();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	
private:
	CActiveScheduler* iActiveScheduler;
	};

#endif	/* TAUTHPLUGINSSTEP_H */
