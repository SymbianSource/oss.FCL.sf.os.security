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
* hash.cpp
* (c) 1999-2003 Symbian Ltd. All rights reserved
*
*/


/**
 @file
*/

#include <e32std.h>
#include <hash.h>
#include "sha1shim.h"

CMessageDigest::CMessageDigest(void):CBase()
{}

CMessageDigest::CMessageDigest(const CMessageDigest& /*aMD*/):CBase()
{}

CMessageDigest::~CMessageDigest(void)
{}

TInt CMessageDigest::GetExtension(TUint aExtensionId, TAny*& a0, TAny* a1)
	{
	return Extension_(aExtensionId, a0, a1);
	}


//////////////////////////////////////////////////////////////////
//	Factory class to create CMessageDigest derived objects
//////////////////////////////////////////////////////////////////
CMessageDigest* CMessageDigestFactory::NewDigestL(CMessageDigest::THashId aHashId)
{
	CMessageDigest* hash = NULL;
	switch (aHashId)
	{
	case (CMessageDigest::ESHA1):
		{
			hash = CSHA1Shim::NewL();
			break;
		}
	default:	
		User::Leave(KErrNotSupported);
	}

	return (hash);
}

CMessageDigest* CMessageDigestFactory::NewDigestLC(CMessageDigest::THashId aHashId)
{
	CMessageDigest* hash = CMessageDigestFactory::NewDigestL(aHashId);
	CleanupStack::PushL(hash);
	return (hash);
}

