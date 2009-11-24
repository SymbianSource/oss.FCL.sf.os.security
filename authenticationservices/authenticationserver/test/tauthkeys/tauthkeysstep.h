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


#ifndef TAUTHKEYSSTEP_H
#define TAUTHKEYSSTEP_H

#include <test/testexecutestepbase.h>
#include <test/testexecutelog.h>
#include "tauthkeysserver.h"

#include "encryptedprotectionkey.h"
#include "transientkey.h"  
#include "transientkey.h"  
#include "transientkeyinfo.h"

_LIT(KTStepPersist,	"PERSIST");
_LIT(KTStepEncryptKey,	"ENCRYPT_KEY");

class CTAuthKeysStepBase : public CTestStep
	{
protected:
	CTAuthKeysStepBase() {}
	~CTAuthKeysStepBase() {}
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

class CTStepPersist : public CTAuthKeysStepBase
	{
public:
	CTStepPersist();
	virtual TVerdict doTestStepL();
	};


class CTStepEncryptKey : public CTAuthKeysStepBase
	{
public:
	CTStepEncryptKey();
	virtual TVerdict doTestStepL();
	};

#endif	/* TAUTHKEYSSTEP_H */
