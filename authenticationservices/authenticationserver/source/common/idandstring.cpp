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
* CIdAndString implementation
*
*/


/**
 @file 
*/

#include "authcommon_impl.h"
#include "authserver/idandstring.h"

using namespace AuthServer;

EXPORT_C CIdAndString* CIdAndString::NewLC(TIdentityId aId, const TDesC& aString)
	{
	CIdAndString* me = new (ELeave) CIdAndString();
	CleanupStack::PushL(me);
	me->ConstructL(aId, aString);
	return me;
	}

EXPORT_C void CIdAndString::ConstructL(TIdentityId aId, const TDesC& aString)
    {
    iId = aId;
	iString = aString.AllocL();
	}

	
EXPORT_C CIdAndString::~CIdAndString() { delete iString; }

EXPORT_C CIdAndString* CIdAndString::NewLC(RReadStream& aStream)
	    {
		CIdAndString* me = new (ELeave) CIdAndString();
		CleanupStack::PushL(me);
		me->InternalizeL(aStream);
		return me;
		}
EXPORT_C void CIdAndString::ExternalizeL(RWriteStream& aStream) const
	    {
		  aStream << iId;
		  aStream << *iString;
	    }

void CIdAndString::InternalizeL(RReadStream& aStream) 
	{
	aStream >> iId;
	if (iString)
		{
		delete iString;
		iString = 0;
		}
	iString = HBufC::NewL(aStream,255);
	}
	
EXPORT_C TIdentityId CIdAndString::Id() const
  {
  return iId;
  }
EXPORT_C HBufC* CIdAndString::String() const
  {
  return iString;
  }
