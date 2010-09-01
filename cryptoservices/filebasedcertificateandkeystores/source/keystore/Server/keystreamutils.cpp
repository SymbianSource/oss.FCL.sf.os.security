/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "keystreamutils.h"
#include "asymmetrickeys.h"


#include <e32debug.h>

void ExternalizeL(const CRSAPublicKey& aKey, RWriteStream& aStream)
	{
	aStream << aKey.N() << aKey.E();
	}

void ExternalizeL(const CRSAPrivateKey& aData, RWriteStream& aStream)
	{
	aStream << aData.N();
	
	// Check the incoming RSA private key (standard or CRT)	
	TRSAPrivateKeyType keyType = aData.PrivateKeyType();
	aStream.WriteInt32L((TInt32)keyType);

	if (EStandard==keyType)
		{
		const CRSAPrivateKeyStandard& key = static_cast<const CRSAPrivateKeyStandard&>(aData);
		aStream << key.D();
		}
	else if (EStandardCRT==keyType)
		{
		const CRSAPrivateKeyCRT& key = static_cast<const CRSAPrivateKeyCRT&>(aData);
		aStream << key.P() << key.Q() << key.DP() << key.DQ() << key.QInv();
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

void ExternalizeL(const CDSAPublicKey& aKey, RWriteStream& aStream)
	{
	aStream << aKey.P() << aKey.Q() << aKey.G() << aKey.Y();
	}

void ExternalizeL(const CDSAPrivateKey& aKey, RWriteStream& aStream)
	{
	aStream << aKey.P() << aKey.Q() << aKey.G() << aKey.X();
	}

void CreateL(RReadStream& aStream, CRSAPublicKey*& aOut)
	{
	RInteger N, keyPublicExp;
	CreateLC(aStream, N);
	CreateLC(aStream, keyPublicExp);

	aOut = CRSAPublicKey::NewL(N, keyPublicExp);

	CleanupStack::Pop(2, &N); // keyPublicExp, N
	}

void CreateL(RReadStream& aStream, CRSAPrivateKey*& aOut)
	{
	RInteger privateN;
	CreateLC(aStream, privateN);
		
	TRSAPrivateKeyType keyType = EStandard;
	keyType = (TRSAPrivateKeyType)aStream.ReadInt32L();
	
	if (EStandard==keyType)
		{
		RInteger D;
		CreateLC(aStream, D);
	
		aOut = CRSAPrivateKeyStandard::NewL(privateN, D);

		CleanupStack::Pop(&D);
		}
	else if (EStandardCRT==keyType)
		{
		RInteger p, q, dP, dQ, qInv;
		CreateLC(aStream, p);
		CreateLC(aStream, q);
		CreateLC(aStream, dP);
		CreateLC(aStream, dQ);
		CreateLC(aStream, qInv);
				
		aOut = CRSAPrivateKeyCRT::NewL(privateN, p, q, dP, dQ, qInv);
		
		CleanupStack::Pop(5, &p);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(&privateN);
	}

void CreateL(RReadStream& aStream, CDSAPublicKey*& aOut)
	{
	RInteger P, Q, G, Y;
	CreateLC(aStream, P);
	CreateLC(aStream, Q);
	CreateLC(aStream, G);
	CreateLC(aStream, Y);

	aOut = CDSAPublicKey::NewL(P, Q, G, Y);

	CleanupStack::Pop(4, &P);
	}

void CreateL(RReadStream& aStream, CDSAPrivateKey*& aOut)
	{
	RInteger P, Q, G, X;
	CreateLC(aStream, P);
	CreateLC(aStream, Q);
	CreateLC(aStream, G);
	CreateLC(aStream, X);

	aOut = CDSAPrivateKey::NewL(P, Q, G, X);

	CleanupStack::Pop(4, &P);
	}



