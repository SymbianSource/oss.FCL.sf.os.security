/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32base.h>
#include <utf.h>

#define STATIC_CAST(t,v) static_cast<t>(v)
#define CONST_CAST(t,v) const_cast<t>(v)
#define FOREVER for(;;)

const TUint KNotInBase64Alphabet=KMaxTUint;

enum TPanic
	{
	EPanicBad6BitNumber=1,
	EPanicBadUtf7Pointers1,
	EPanicBadUtf7Pointers2,
	EPanicBadUtf7Pointers3,
	EPanicBadUtf7Pointers4,
	EPanicBadUtf7Pointers5,
	EPanicBadUtf7Pointers6,
	EPanicBadUtf7Pointers7,
	EPanicBadUtf7Pointers8,
	EPanicBadUtf7Pointers9,
	EPanicBadUtf7Pointers10,
	EPanicBadUtf7Pointers11,
	EPanicNotInBase64Block,
	EPanicBadUnicodePointers1,
	EPanicBadUnicodePointers2,
	EPanicBadUnicodePointers3,
	EPanicBadUnicodePointers4,
	EPanicBadUnicodePointers5,
	EPanicBadUnicodePointers6,
	EPanicBadUnicodePointers7,
	EPanicBadUnicodePointers8,
	EPanicBadUnicodePointers9,
	EPanicBadUnicodePointers10,
	EPanicBadBitBufferState1,
	EPanicBadBitBufferState2,
	EPanicBadBitBufferState3,
	EPanicBadBitBufferState4,
	EPanicBadBitBufferState5,
	EPanicBadBitBufferState6,
	EPanicBadBitBufferState7,
	EPanicBadBitBufferState8,
	EPanicBadBitBufferState9,
	EPanicBadBitBufferState10,
	EPanicBadBitBufferState11,
	EPanicBadBitBufferState12,
	EPanicBadBitBufferState13,
	EPanicBadBitBufferState14,
	EPanicBadBitBufferState15,
	EPanicBadBitBufferState16,
	EPanicBadBitBufferState17,
	EPanicUnexpectedNumberOfLoopIterations,
	EPanicInitialEscapeCharacterButNoBase64,
	EPanicBase64SequenceDoesNotFallOnUnicodeCharacterBoundary,
	EPanicBadUtf8Pointers1,
	EPanicBadUtf8Pointers2,
	EPanicBadUtf8Pointers3,
	EPanicBadUtf8Pointers4,
	EPanicBadUtf8Pointers5,
	EPanicBadUtf8Pointers6,
	EPanicBadUtf8Pointers7,
	EPanicOutOfSyncUtf7Byte1,
	EPanicOutOfSyncUtf7Byte2,
	EPanicOutOfSyncBase64Decoding
	};

_LIT(KLitPanicText, "CHARCONV-UTF");

LOCAL_C void Panic(TPanic aPanic)
	{
	User::Panic(KLitPanicText, aPanic);
	}

inline TUint EscapeCharacterForStartingBase64Block(TBool aIsImapUtf7) {return aIsImapUtf7? '&': '+';}

inline TBool BitBufferContainsNonZeroBits(TUint aBitBuffer, TInt aNumberOfBitsInBuffer)
	{
	return (aBitBuffer&((1<<aNumberOfBitsInBuffer)-1))!=0;
	}






 

/** Converts Unicode text into UTF-8 encoding.

@param aUtf8 On return, contains the UTF-8 encoded output string.
@param aUnicode The Unicode-encoded input string.
@return The number of unconverted characters left at the end of the input 
descriptor, or one of the error values defined in TError. */
EXPORT_C TInt CnvUtfConverter::ConvertFromUnicodeToUtf8(TDes8& aUtf8, const TDesC16& aUnicode)
	{
	return ConvertFromUnicodeToUtf8(aUtf8, aUnicode, EFalse);
	}



/** Converts Unicode text into UTF-8 encoding. 

Surrogate pairs can be input which will result in a valid 4 byte UTF-8 value.

The variant of UTF-8 used internally by Java differs slightly from standard 
UTF-8. The TBool argument controls the UTF-8 variant generated by this function.

@param aUtf8 On return, contains the UTF-8 encoded output string.
@param aUnicode A UCS-2 encoded input string.
@param aGenerateJavaConformantUtf8 EFalse for orthodox UTF-8. ETrue for Java 
UTF-8. The default is EFalse.
@return The number of unconverted characters left at the end of the input descriptor, 
or one of the error values defined in TError. */
TInt CnvUtfConverter::ConvertFromUnicodeToUtf8(TDes8& aUtf8, 
											   const TDesC16& aUnicode, 
											   TBool aGenerateJavaConformantUtf8)
	{
	if (aUnicode.Length() == 0)
		{
		aUtf8.SetLength(0);
		return 0;
		}
	if (aUtf8.MaxLength() == 0)
		{
		return aUnicode.Length();
		}
	
	TUint8* pUtf8 = CONST_CAST(TUint8*, aUtf8.Ptr());
	const TUint8* pointerToLastUtf8Byte = pUtf8 + (aUtf8.MaxLength() - 1);
	TBool inputIsTruncated = EFalse;
	const TUint16* pUnicode = aUnicode.Ptr();
	const TUint16* pointerToLastUnicodeCharacter = pUnicode + (aUnicode.Length() - 1);
	
	FOREVER
		{
		__ASSERT_DEBUG(pUtf8 <= pointerToLastUtf8Byte, Panic(EPanicBadUtf8Pointers1));
		__ASSERT_DEBUG(pUnicode <= pointerToLastUnicodeCharacter, Panic(EPanicBadUnicodePointers3));
	
		if (pUnicode[0] < 0x80)
			{
			// ascii - 1 byte
			
			// internally java is different since the \x0000 character is 
			// translated into \xC0 \x80.
			
			if ((aGenerateJavaConformantUtf8) && (pUnicode[0] == 0x0000))
				{
				if (pUtf8 == pointerToLastUtf8Byte)
					{
					pUtf8--;
					pUnicode--;
					break;			
					}
				*pUtf8++ = STATIC_CAST(TUint8, 0xc0);
				*pUtf8   = STATIC_CAST(TUint8, 0x80);	
				}
			else
				{
				*pUtf8 = STATIC_CAST(TUint8, pUnicode[0]);
				}
			}
		else if (pUnicode[0] < 0x800)
			{
			// U+0080..U+07FF - 2 bytes
			
			if (pUtf8 == pointerToLastUtf8Byte)
				{
				pUtf8--;
				pUnicode--;
				break;
				}
			
			*pUtf8++ = STATIC_CAST(TUint8, 0xc0|(pUnicode[0]>>6));
			*pUtf8   = STATIC_CAST(TUint8, 0x80|(pUnicode[0]&0x3f));
			
			}

		// check to see if we have a surrogate in the stream, surrogates encode code points outside
		// the BMP and are 4 utf-8 chars, otherwise what we have here is 3 utf-8 chars.

		else if (((pUnicode[0] & 0xfc00) == 0xd800) && !aGenerateJavaConformantUtf8)
			{
			// surrogate pair - 4 bytes in utf-8
			// U+10000..U+10FFFF
			
			__ASSERT_DEBUG(pUtf8 <= pointerToLastUtf8Byte, Panic(EPanicBadUtf8Pointers2));
			// is there enough space to hold the character
			if ((pointerToLastUtf8Byte - pUtf8) < 3)
				{
				pUtf8--;
				pUnicode--;
				break;  // no go to the exit condition
				}
			
			__ASSERT_DEBUG(pUnicode <= pointerToLastUnicodeCharacter, Panic(EPanicBadUnicodePointers4));
			if (pUnicode >= pointerToLastUnicodeCharacter)
				{
				pUtf8--;
				pUnicode--;
				inputIsTruncated = ETrue;
				break; // middle of a surrogate pair. go to end condition
				}
			
			if ((pUnicode[1] & 0xfc00) != 0xdc00)
				{
				return EErrorIllFormedInput;
				}
			
			// convert utf-16 surrogate to utf-32
			TUint ch = ((pUnicode[0] - 0xD800) << 10 | (pUnicode[1] - 0xDC00)) + 0x10000;
			
			// convert utf-32 to utf-8
            *pUtf8++ = STATIC_CAST(TUint8,0xf0 | (ch >> 18));   
            *pUtf8++ = STATIC_CAST(TUint8,0x80 | ((ch >> 12) & 0x3f));
            *pUtf8++ = STATIC_CAST(TUint8,0x80 | ((ch >> 6) & 0x3f));
            *pUtf8   = STATIC_CAST(TUint8,0x80 | (ch & 0x3f));
			
            // we consumed 2 utf-16 values, move this pointer
			pUnicode++;
			}		
		else
			{
			// 3 byte - utf-8, U+800..U+FFFF rest of BMP.
			
			if (pointerToLastUtf8Byte - pUtf8 < 2)
				{
				pUtf8--;
				pUnicode--;
				break;
				}
			*pUtf8++ = STATIC_CAST(TUint8, 0xe0|(pUnicode[0]>>12));
			*pUtf8++ = STATIC_CAST(TUint8, 0x80|((pUnicode[0]>>6)&0x3f));
			*pUtf8   = STATIC_CAST(TUint8, 0x80|(pUnicode[0]&0x3f));
			}
		
		if ((pUnicode == pointerToLastUnicodeCharacter) || (pUtf8 == pointerToLastUtf8Byte))
			{
			break;
			}
		
		pUtf8++;
		pUnicode++;
		
		}
	
	if ((pUnicode < aUnicode.Ptr()) && inputIsTruncated)
		{
		return EErrorIllFormedInput;
		}
	
	aUtf8.SetLength((pUtf8 - aUtf8.Ptr())+1);
	return pointerToLastUnicodeCharacter-pUnicode;
	}





 





/** Converts text encoded using the Unicode transformation format UTF-8 into the 
Unicode UCS-2 character set.

@param aUnicode On return, contains the Unicode encoded output string.
@param aUtf8 The UTF-8 encoded input string
@return The number of unconverted bytes left at the end of the input descriptor, 
or one of the error values defined in TError. */
EXPORT_C TInt CnvUtfConverter::ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8)
	{
	return ConvertToUnicodeFromUtf8(aUnicode, aUtf8, EFalse);
	}

static void UpdateUnconvertibleInfo(TInt& aNumberOfUnconvertibleCharacters,
		TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter, TUint8 aIndex)
	{
	if (aNumberOfUnconvertibleCharacters<=0)
		{
		aIndexOfFirstByteOfFirstUnconvertibleCharacter = aIndex;
		}
	++aNumberOfUnconvertibleCharacters;
	}

/** Converts text encoded using the Unicode transformation format UTF-8 into the 
Unicode UCS-2 character set.

@param aUnicode On return, contains the Unicode encoded output string.
@param aUtf8 The UTF-8 encoded input string
@param aGenerateJavaConformantUtf8 EFalse for orthodox UTF-8. ETrue for Java 
@return The number of unconverted bytes left at the end of the input descriptor, 
or one of the error values defined in TError. */
TInt CnvUtfConverter::ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8, TBool aGenerateJavaConformantUtf8)
	{
	TInt dummyUnconverted, dummyUnconvertedIndex;
	return ConvertToUnicodeFromUtf8(aUnicode, aUtf8, aGenerateJavaConformantUtf8, dummyUnconverted, dummyUnconvertedIndex);
	}

/** Converts text encoded using the Unicode transformation format UTF-8 into the 
Unicode UCS-2 character set. Surrogate pairs can be created when a valid 4 byte UTF-8 is input.

The variant of UTF-8 used internally by Java differs slightly from standard 
UTF-8. The TBool argument controls the UTF-8 variant generated by this function.

@param aUnicode On return, contains the Unicode encoded output string.
@param aUtf8 The UTF-8 encoded input string
@param aGenerateJavaConformantUtf8 EFalse for orthodox UTF-8. ETrue for Java 
UTF-8. The default is EFalse.
@param aNumberOfUnconvertibleCharacters On return, contains the number of bytes 
which were not converted.
@param aIndexOfFirstByteOfFirstUnconvertibleCharacter On return, the index 
of the first byte of the first unconvertible character. For instance if the 
first character in the input descriptor (aForeign) could not be converted, 
then this parameter is set to the first byte of that character, i.e. zero. 
A negative value is returned if all the characters were converted.
@return The number of unconverted bytes left at the end of the input descriptor, 
or one of the error values defined in TError. */

/* of note: conformance.  Unicode standard 5.0 section 3.9, table 3-7
 * Well formed UTF-8 Byte Sequences, full table.
 * +----------------------------------------------------------------+
 * | Code Points        | 1st byte | 2nd byte | 3rd byte | 4th byte |
 * +--------------------+----------+----------+----------+----------+
 * | U+0000..U+007F     | 00..7D   |          |          |          |  1 byte, ascii
 * | U+0080..U+07FF     | C2..DF   | 80..BF   |          |          |  2 bytes, error if 1st < 0xC2 
 * | U+0800..U+0FFF     | E0       | A0..BF   | 80..BF   |          |  3 bytes, 1st == 0xE0, error if 2nd < 0xA0
 * | U+1000..U+CFFF     | E1..EC   | 80..BF   | 80..BF   |          |  normal
 * | U+D000..U+D7FF     | ED       | 80..9F   | 80..BF   |          |  3 bytes, 1st == 0xED, error if 2nd > 0x9F
 * | U+E000..U+FFFF     | EE..EF   | 80..BF   | 80..BF   |          |  normal
 * | U+10000..U+3FFFF   | F0       | 90..BF   | 80..BF   | 80..BF   |  4 bytes, 1st == 0xf0, error if 2nd < 0x90
 * | U+40000..U+FFFFF   | F1..F3   | 80..BF   | 80..BF   | 80..BF   |  normal
 * | U+100000..U+10FFFF | F4       | 80..8F   | 80..BF   | 80..BF   |  4 bytes, 1st == 0xF4, error if 2nd > 0x8F
 * +--------------------+----------+----------+----------+----------+
 * 
 * As a consequence of the well-formedness conditions specified in table 3-7,
 * the following byte values are disallowed in UTF-8: C0-C1, F5-FF.
 */
TInt CnvUtfConverter::ConvertToUnicodeFromUtf8(TDes16& aUnicode, const TDesC8& aUtf8, TBool aGenerateJavaConformantUtf8,
		TInt& aNumberOfUnconvertibleCharacters, TInt& aIndexOfFirstByteOfFirstUnconvertibleCharacter)
	{	
	aUnicode.SetLength(0);
	
	if ((aUtf8.Length() == 0) || (aUnicode.MaxLength() == 0))
		{
		return aUtf8.Length();
		}

	TUint16*           pUnicode = CONST_CAST(TUint16*, aUnicode.Ptr());
	const TUint16* pLastUnicode = pUnicode + (aUnicode.MaxLength() - 1);
	const TUint8*         pUtf8 = aUtf8.Ptr();   
	const TUint8*     pLastUtf8 = pUtf8 + (aUtf8.Length() - 1);
	const TUint16 replacementcharacter = 0xFFFD;
	TUint currentUnicodeCharacter;
	TUint sequenceLength;

	
	FOREVER
		{
		TBool illFormed=EFalse;
		
		__ASSERT_DEBUG(pUnicode <= pLastUnicode, Panic(EPanicBadUnicodePointers8));
		__ASSERT_DEBUG(pUtf8 <= pLastUtf8, Panic(EPanicBadUtf8Pointers3));
		
		sequenceLength = 1;
		
		// ascii - optimisation (i.e. it isn't a sequence)
		if (pUtf8[0] < 0x80)
			{
			currentUnicodeCharacter = pUtf8[0];
			}
		else
			{
			// see if well formed utf-8, use table above for reference	
			if ((pUtf8[0] >= 0xc2) && (pUtf8[0] <= 0xdf))
				{
				// 0xc1-0xc2 are not valid bytes
				sequenceLength = 2;
				}
			else if ((pUtf8[0] & 0xf0) == 0xe0)
				{
				sequenceLength = 3;
				}
			else if ((pUtf8[0] >= 0xf0) && (pUtf8[0] < 0xf5))
				{
				// 0xf5-0xff, are not valid bytes
				sequenceLength = 4;
				}
			else if ((pUtf8[0] == 0xc0) && aGenerateJavaConformantUtf8)
				{
				if ((pUtf8 == pLastUtf8) || (pUtf8[1] == 0x80))
					{
					// either we've split the 0xc0 0x80 (i.e. 0xc0 is
					// the last character in the string) or we've
					// discovered a valid 0xc0 0x80 sequence.  
					sequenceLength = 2;
					}
				}
			
			/* checking to see if we got a valid sequence */
			if (sequenceLength == 1)
				{
				// bad value in the leading byte, 0xc0-0xc1,0x5f-0xff for example
				currentUnicodeCharacter = replacementcharacter;
				UpdateUnconvertibleInfo(aNumberOfUnconvertibleCharacters,
						aIndexOfFirstByteOfFirstUnconvertibleCharacter,	pUtf8-aUtf8.Ptr());
				}
			else
				{
				// this is a check to see if the sequence goes beyond the input 
				// stream.  if its not the first and only character in the input
				// stream this isn't an error, otherwise it is.
				if ((pUtf8 + sequenceLength - 1) >  pLastUtf8)
					{
					// check to see if this sequence was the first character
					if ((pUnicode - aUnicode.Ptr()) == 0)
						{
						return EErrorIllFormedInput;
						}
					break;
					}			
				
				currentUnicodeCharacter = pUtf8[0] & (0x7F>>sequenceLength);
			
				/* check the trailing bytes, they should begin with 10 */
				TUint i = 1;

				do
					{
					if ((pUtf8[i] & 0xc0) == 0x80)
						{
						// add the trailing 6 bits to the current unicode char
						currentUnicodeCharacter = (currentUnicodeCharacter <<6 ) | (pUtf8[i] & 0x3F);
						}
					else
						{
						// ill formed character (doesn't have a lead 10)
						currentUnicodeCharacter = replacementcharacter;
						UpdateUnconvertibleInfo(aNumberOfUnconvertibleCharacters,
								aIndexOfFirstByteOfFirstUnconvertibleCharacter,	pUtf8-aUtf8.Ptr());
						illFormed=ETrue;
						break; 
						}
					i++;
					}
				while (i < sequenceLength);
				}
				
			/* conformance check.  bits of above table for reference.
			 * +----------------------------------------------------------------+
			 * | Code Points        | 1st byte | 2nd byte | 3rd byte | 4th byte |
			 * +--------------------+----------+----------+----------+----------+
			 * | U+0800..U+0FFF     | E0       | A0..BF   | 80..BF   |          |  3 bytes, 1st == 0xE0, 2nd < 0xA0
			 * | U+D000..U+D7FF     | ED       | 80..9F   | 80..BF   |          |  3 bytes, 1st == 0xED, 2nd > 0x9F
			 * | U+10000..U+3FFFF   | F0       | 90..BF   | 80..BF   | 80..BF   |  4 bytes, 1st == 0xf0, 2nd < 0x90
			 * | U+100000..U+10FFFF | F4       | 80..8F   | 80..BF   | 80..BF   |  4 bytes, 1st == 0xF4, 2nd > 0x8F
			 * +--------------------+----------+----------+----------+----------+
			 */
			
			if (currentUnicodeCharacter != replacementcharacter)
				{
				if (sequenceLength == 3)
					{
					if ((pUtf8[0] == 0xE0) && (pUtf8[1] < 0xA0))
						{
						currentUnicodeCharacter = replacementcharacter;
						UpdateUnconvertibleInfo(aNumberOfUnconvertibleCharacters,
								aIndexOfFirstByteOfFirstUnconvertibleCharacter,	pUtf8-aUtf8.Ptr());
						illFormed=ETrue;
						}
					else if ((pUtf8[0] == 0xED) && (pUtf8[1] > 0x9F))
						{
						currentUnicodeCharacter = replacementcharacter;
						UpdateUnconvertibleInfo(aNumberOfUnconvertibleCharacters,
								aIndexOfFirstByteOfFirstUnconvertibleCharacter,	pUtf8-aUtf8.Ptr());
						illFormed=ETrue;
						}
					}
				else if (sequenceLength == 4)
					{
					if ((pUtf8[0] == 0xF0) && (pUtf8[1] < 0x90))
						{
						currentUnicodeCharacter = replacementcharacter;
						UpdateUnconvertibleInfo(aNumberOfUnconvertibleCharacters,
								aIndexOfFirstByteOfFirstUnconvertibleCharacter,	pUtf8-aUtf8.Ptr());
						illFormed=ETrue;
						}
					else if ((pUtf8[0] == 0xF4) && (pUtf8[1] > 0x8F))
						{
						currentUnicodeCharacter = replacementcharacter;
						UpdateUnconvertibleInfo(aNumberOfUnconvertibleCharacters,
								aIndexOfFirstByteOfFirstUnconvertibleCharacter,	pUtf8-aUtf8.Ptr());
						illFormed=ETrue;
						}
					}
				
				
				/* last conformance check - Unicode 5.0 section 3.9 D92 Because surrogate code points
				 * are not Unicode scalar values, any UTF-8 byte sequence that would map to code 
				 * points D800..DFFF is ill formed */
				
				if ((currentUnicodeCharacter >= 0xD800) && (currentUnicodeCharacter <= 0xDFFF))
					{
					currentUnicodeCharacter = replacementcharacter;
					UpdateUnconvertibleInfo(aNumberOfUnconvertibleCharacters,
							aIndexOfFirstByteOfFirstUnconvertibleCharacter,	pUtf8-aUtf8.Ptr());
					illFormed=ETrue;
					}	
				}
				// end conformance check
			}

		// would this character generate a surrogate pair in UTF-16?
		if (currentUnicodeCharacter > 0xFFFF)
			{
			// is there enough space to hold a surrogate pair in the output?
			if (pUnicode >= pLastUnicode)
				{
				break; // no, end processing.
				}
			
			TUint surrogate = (currentUnicodeCharacter>>10) + 0xD7C0;
			*pUnicode++ = STATIC_CAST(TUint16, surrogate);
					
			surrogate = (currentUnicodeCharacter & 0x3FF) + 0xDC00;
			*pUnicode++ = STATIC_CAST(TUint16, surrogate);			
			}
		else
			{
			*pUnicode++ = STATIC_CAST(TUint16, currentUnicodeCharacter);
			}
		
		// move the input pointer
		if (currentUnicodeCharacter != replacementcharacter)
			{
			pUtf8 += sequenceLength;
			}
		else if(illFormed == EFalse)
			{
			pUtf8 += (sequenceLength);
			}
		else
			{
			// we had a character we didn't recognize (i.e. it was invalid)
			// so move to the next character in the input
			pUtf8++;
			}
		
		if ((pUtf8 > pLastUtf8) || (pUnicode > pLastUnicode))
			{ 
			break;  // we've either reached the end of the input or the end of output
			}
		}

	aUnicode.SetLength(pUnicode - aUnicode.Ptr());
	return (pLastUtf8 - pUtf8 + 1);
	}

/** Given a sample text this function attempts to determine whether or not
 *  the same text is encoded using the UTF-8 standard encoding scheme.

@param TInt a confidence level, given at certain value.  if the given sample
			is UTF-8 this value will not be changed (unless > 100) then its
			set to 100.  Otherwise if the same isn't UTF-8, its set to 0.
@param TDesC8 sample text.
UTF-8. The default is EFalse.
@return void
 */

/* of note: conformance.  Unicode standard 5.0 section 3.9, table 3-7
 * Well formed UTF-8 Byte Sequences, full table.
 * +----------------------------------------------------------------+
 * | Code Points        | 1st byte | 2nd byte | 3rd byte | 4th byte |
 * +--------------------+----------+----------+----------+----------+
 * | U+0000..U+007F     | 00..7D   |          |          |          |  1 byte, ascii
 * | U+0080..U+07FF     | C2..DF   | 80..BF   |          |          |  2 bytes, error if 1st < 0xC2 
 * | U+0800..U+0FFF     | E0       | A0..BF   | 80..BF   |          |  3 bytes, 1st == 0xE0, error if 2nd < 0xA0
 * | U+1000..U+CFFF     | E1..EC   | 80..BF   | 80..BF   |          |  normal
 * | U+D000..U+D7FF     | ED       | 80..9F   | 80..BF   |          |  3 bytes, 1st == 0xED, error if 2nd > 0x9F
 * | U+E000..U+FFFF     | EE..EF   | 80..BF   | 80..BF   |          |  normal
 * | U+10000..U+3FFFF   | F0       | 90..BF   | 80..BF   | 80..BF   |  4 bytes, 1st == 0xf0, error if 2nd < 0x90
 * | U+40000..U+FFFFF   | F1..F3   | 80..BF   | 80..BF   | 80..BF   |  normal
 * | U+100000..U+10FFFF | F4       | 80..8F   | 80..BF   | 80..BF   |  4 bytes, 1st == 0xF4, error if 2nd > 0x8F
 * +--------------------+----------+----------+----------+----------+
 * 
 * As a consequence of the well-formedness conditions specified in table 3-7,
 * the following byte values are disallowed in UTF-8: C0-C1, F5-FF.
 * 
 * Code Rules:
 *   R1: If the string contains any non-UTF-8 characters the returned confidence
 *       is 0.  Valid UTF-8 combinations are listed in the above table.
 *   R2: Otherwise if the string starts with a UTF-8 BOM (byte order mark) in  
 *       the (see ) the returned confidence is 95.
 *   R3: Otherwise the confidence returned is based upon the sample string 
 *       length.
 *   R4: If the sample string is under 75 characters, the confidence is set to 
 *       75.
 */
void IsCharacterSetUTF8(TInt& aConfidenceLevel, const TDesC8& aSample)
	{

	TInt sampleLength = aSample.Length();
	
	if (sampleLength == 0)
		{
		aConfidenceLevel = 89;
		return;
		}
	TInt bytesRemaining  = 0;
	TUint sequenceLength  = 0;
	
	aConfidenceLevel = sampleLength;

	const TUint8* buffer = &aSample[0];

	if (sampleLength < 95)
		{
		// check for the BOM
		if ((sampleLength >= 3) && 
			((buffer[0] == 0xEF) &&
			 (buffer[1] == 0xBB) &&
			 (buffer[2] == 0xBF)) 
			) 
			{
			aConfidenceLevel = 95;
			}
		else if (sampleLength < 75)
			{
			aConfidenceLevel = 75;
			}
		}
	
	for (TInt index = 0;index != sampleLength;index++)
		{
		
		if (bytesRemaining > 0)
			{
			// bytesRemaining > 0, means that a byte representing the start of a 
			// multibyte sequence was encountered and the bytesRemaining is the 
			// number of bytes to follow. 
			
			if ((buffer[index] & 0xc0) == 0x80) 
				{
				// need to check for ill-formed sequences -- all are in the 2nd byte
				
				if ((sequenceLength == 3) && (bytesRemaining == 2))
					{
					if ((buffer[index - 1] == 0xe0) && (buffer[index] < 0xa0))
						{
						aConfidenceLevel = 0;
						break;
						}
					else if ((buffer[index - 1] == 0xed) && (buffer[index] > 0x9f))
						{
						aConfidenceLevel = 0;
						break;
						}
					}
				else if ((sequenceLength == 4) && (bytesRemaining == 3))
					{
					if ((buffer[index - 1] == 0xf0) && (buffer[index] < 0x90))
						{
						aConfidenceLevel = 0;
						break;
						}
					else if ((buffer[index - 1] == 0xf4) && (buffer[index] > 0x8f))
						{
						aConfidenceLevel = 0;
						break;
						}
					}
				
				--bytesRemaining;
				continue;
				}
			else
				{
				aConfidenceLevel = 0;
				break;
				}
			}
		
		if (bytesRemaining == 0)
			{
			if (buffer[index] < 0x80)
				{
				// The value of aSample[index] is in the range 0x00-0x7f
				//UTF8 maintains ASCII transparency. So it's a valid
				//UTF8. Do nothing, check next value.
				continue;
				}
			else if ((buffer[index] >= 0xc2) && (buffer[index] < 0xe0))
				{
				// valid start of a 2 byte sequence (see conformance note)
				sequenceLength = 2;
				bytesRemaining = 1;
				}
			else if ((buffer[index] & 0xf0) == 0xe0)
				{
				// valid start of a 3 byte sequence
				sequenceLength = 3;
				bytesRemaining = 2;
				}
			else if ((buffer[index] >= 0xf0) && (buffer[index] < 0xf5))
				{
				// valid start of a 4 byte sequence (see conformance note)
				sequenceLength = 4;
				bytesRemaining = 3;
				}	
			else
				{
				// wasn't anything expected so must be an illegal/irregular UTF8 coded value
				aConfidenceLevel = 0;
				break;
				}
			}
		} // for 
	
	aConfidenceLevel = (aConfidenceLevel > 0)? ((aConfidenceLevel > 100)? 100: aConfidenceLevel): 0;
	}

// End of file
