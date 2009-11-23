/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Utility functions for copying and streaming RArrays. 
* RPointerArray utilities
*
*/

template <class T>
void ExternalizePointerArrayL(const RPointerArray<T>& aArray, RWriteStream& aStream)
	{
	aStream.WriteInt32L(aArray.Count());
	for (TInt i = 0; i < aArray.Count(); ++i)
		{
		TTraits<T>::WriteToStreamL(*aArray[i], aStream);
		}
	}

template <class T>
void InternalizePointerArrayL(RPointerArray<T>& aArray, RReadStream& aStream)
	{
	TInt count = aStream.ReadInt32L();
	for (TInt i = 0; i < count; ++i)
		{
		T* item = TTraits<T>::ReadFromStreamLC(aStream);
		User::LeaveIfError(aArray.Append(item));
		CleanupStack::Pop();
		}
	}

/////////////////////////////////////////////////////////////////////
// RArray utilities
/////////////////////////////////////////////////////////////////////
template <class T>
void ExternalizeArrayL(const RArray<T>& aArray, RWriteStream& aStream)
	{
	aStream.WriteInt32L(aArray.Count());
	for (TInt i = 0; i < aArray.Count(); ++i)
		{
		aStream.WriteL(TPckgC<T>(aArray[i]));
		}
	}

template <class T>
void InternalizeArrayL(RArray<T>& aArray, RReadStream& aStream)
	{
	TInt count = aStream.ReadInt32L();
	for (TInt i = 0; i < count; ++i)
		{
		T item;
		TPckg<T> itemPckg(item);
		aStream.ReadL(itemPckg);
		User::LeaveIfError(aArray.Append(item));
		}
	}
