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
* Base64 encoding and decoding functions.
*
*/




/**
 @file
*/

#include "base64.h"

// Constants used by the Base64 algorithm
const TUint8 TBase64::iAlphabet[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
const TInt8 KImcvLookUpStartOffset = 43;
const TUint8 KImcvConvEquals = '=';
const TInt8 AsciiToBase64[80]=
	{
	 62, -1, -1, -1, 63, 52, 53, 54, 55, 56,
	 57, 58, 59, 60, 61, -1, -1, -1, 64, -1,
	 -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,
	  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
	 18, 19, 20, 21, 22, 23, 24, 25, -1, -1,
	 -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
	 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
	};
const TInt8 Base64ToAscii[65]=
	{
	 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
	 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
	 85, 86, 87, 88, 89, 90, 97, 98, 99,100,
	101,102,103,104,105,106,107,108,109,110,
	111,112,113,114,115,116,117,118,119,120,
	121,122, 48, 49, 50, 51, 52, 53, 54, 55,
	 56, 57, 43, 47, 61
	};

EXPORT_C TBase64::TBase64() :
	iShiftStored(0),
	iMaskShiftStored(ESix)
	{
	}

EXPORT_C void TBase64::Initialize()
	{
	iMaskShiftStored = ESix;
	}

//Encodes data to Base64.
EXPORT_C HBufC8* TBase64::EncodeLC(const TDesC8& aSrcString)
	{
	// Allocate and clear destination string
	TInt destLen = ((aSrcString.Length() - 1) / 3 + 1) * 4;
	HBufC8* destString = HBufC8::NewMaxLC(destLen);
	TPtr8 destPtr = destString->Des();

	// Initialise variables
	const TUint8* srcStringPtr = aSrcString.Ptr();
	const TUint8* srcStringEnd = aSrcString.Length() + srcStringPtr;
	TUint8* destStringPtr = (TUint8*)destString->Ptr();
	TUint8* destStringPtrBase = destStringPtr;

	TInt character = 0;
	TUint8 encodedChar = 0;
	TInt charStorage = 0;
 	TInt maskShift = EZero;

	while (srcStringPtr <= srcStringEnd)
		{
		// maskShift is used as a char read counter
		if(maskShift == ESix)
			{
			// If the 3rd char read is also the last char then the while loop
			// is broken on the next check.
			if(srcStringPtr == srcStringEnd)
				srcStringPtr++;
			maskShift = EZero;
			character = 0;
			}
		else
			{
			if (srcStringPtr == srcStringEnd)
				character=0;
			else
				character = *srcStringPtr;

			srcStringPtr++;
			// Shifts charStorage ready for the next char
			charStorage = charStorage << 8;
			maskShift += ETwo;
			}
		charStorage = charStorage | character;
		// Shifts the mask to the correct bit location
		// Masks (AND's) the valid bits from charStorage
		// Shifts the valid bits into the low order 8bits
		// Converts to BASE64 char, Casts the result to an unsigned
		// char (which it should be ?....I hope)
		encodedChar = (TUint8)Base64ToAscii[((charStorage >> maskShift) & ESixBitMask)];
		*destStringPtr++ = encodedChar;
		}

	// Check for not enough chars and pad if required
	if (maskShift == EFour)
		{
		*destStringPtr++ = KImcvConvEquals;
		*destStringPtr++ = KImcvConvEquals;
		}
	else
		if (maskShift == ESix)
			*destStringPtr++ = KImcvConvEquals;

	destPtr.SetLength((TInt)(destStringPtr - destStringPtrBase));
	return destString;
	}

//Decodes Base64-encoded data.
EXPORT_C HBufC8* TBase64::DecodeLC(const TDesC8& aSrcString, TBool& aNeedMoreData)
	{
	// The source string must be a multiple of 4 bytes long
	if (aSrcString.Length() & 0x3)
		{
		User::Leave(KErrCorrupt);
		}

	TInt decodedInt = 0;
	TInt8 offsetChar = 0;
	TUint8 decodedChar = 0;

    // Find out how long the decoded string will be.
    TInt outLen = aSrcString.Length() / 4 * 3;
    if (aSrcString[aSrcString.Length() - 1] == '=')
        outLen--;
    if (aSrcString[aSrcString.Length() - 2] == '=')
        outLen--;

	// Construct output buffer.
	HBufC8* destString = HBufC8::NewMaxLC(outLen);
	TPtr8 destPtr = destString->Des();
	destPtr.Zero();

	// Initialise variables
	const TUint8* srcStringPtr = aSrcString.Ptr();
	const TUint8* srcStringEnd = aSrcString.Length() + srcStringPtr;
	TUint8* destStringPtr = (TUint8*)destString->Ptr();
	TUint8* destStringPtrBase = destStringPtr;
	TInt maskShift = iMaskShiftStored;
	TInt shiftStorage = iShiftStored;

	// Main character process loop
	while (srcStringPtr<srcStringEnd)
		{
		offsetChar = (TInt8)(*srcStringPtr - KImcvLookUpStartOffset);
		srcStringPtr++;

		// Check for valid B64 character
		if ((offsetChar >= 0) && (offsetChar < 80))
			{
			// Read in next character and B64 decode
			decodedInt = AsciiToBase64[offsetChar];

			// Exits when a PAD char is reached
			if(decodedInt == EPadChar)
				{
				destPtr.SetLength((TInt)(destStringPtr - destStringPtrBase));
				aNeedMoreData = EFalse;
				return destString;
				}

			// Ensures the first 2 chars of 4 are received before processing
			if(maskShift == ESix)
				maskShift = EFour;
			else
				{
				shiftStorage = shiftStorage << ESix;
				shiftStorage = shiftStorage | decodedInt;
				decodedChar = (TUint8)((shiftStorage >> maskShift) & EEightBitMask);

				if((maskShift -= ETwo) < EZero)
					maskShift = ESix;

				*destStringPtr++ = decodedChar;
				}
			shiftStorage = decodedInt;
			}
		}
	iShiftStored = shiftStorage;
	iMaskShiftStored = maskShift;

	destPtr.SetLength((TInt)(destStringPtr - destStringPtrBase));
	aNeedMoreData = maskShift < ESix;
	return destString;
	}
	
