/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
// base64.h
//

/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Symbian Foundation License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Base64 encoding and decoding functions.
* 
*
*/





/**
 @file
 @internalComponent
*/

#ifndef __BASE64_H__
#define __BASE64_H__

#include <e32base.h>

/**
 * This class performs Base64 encoding and decoding. The implementation
 * has been obtained from messaging.
 */
class TBase64
	{
public:
	/**
	 * Constructs the codec object.
	 */
	IMPORT_C TBase64();

	/**
	 * Initializes the codec.
	 */
	IMPORT_C void Initialize();

	/**
	 * Encodes binary data using base64 encoding.
	 * @param aSrcString Data to encode.
	 * @return Newly allocated buffer containing base64 encoding of
	 *     the data (on the cleanup stack).
	 * @note The function leaves in case of an error.
	 */
	IMPORT_C HBufC8* EncodeLC(const TDesC8& aSrcString);

	/**
	 * Decodes base64-encoded binary data.
	 * @param aSrcString Data to decode.
	 * @param aNeedMoreData Receives ETrue if the encoded string is
	 *     not long enough to decode in full, EFalse otherwise.
	 * @return Newly allocated buffer containing decoded binary data
	 *     (on the cleanup stack).
	 * @note The function leaves in case of an error.
	 */
	IMPORT_C HBufC8* DecodeLC(const TDesC8& aSrcString, TBool& aNeedMoreData);

private:
	enum {EPadChar = 64};
	enum EMaskValues {ESixBitMask = 0x3F, EEightBitMask = 0xFF};
	enum EMaskShiftValues {ESix = 6, EFour = 4, ETwo = 2, EZero = 0};
	static const TUint8 iAlphabet[];
	TInt iShiftStored;
	TInt iMaskShiftStored;
	};

#endif // __BASE64_H__
