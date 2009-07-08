/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* DES Tables
*
*/




/**
 @file 
 @internalComponent 
*/
 
#ifndef __DESTABLES_H__
#define __DESTABLES_H__

#include <e32std.h>

namespace DES_TABLE
{
/* initial permutation IP */
const TUint8 ip[] = {
	   58, 50, 42, 34, 26, 18, 10,  2,
	   60, 52, 44, 36, 28, 20, 12,  4,
	   62, 54, 46, 38, 30, 22, 14,  6,
	   64, 56, 48, 40, 32, 24, 16,  8,
	   57, 49, 41, 33, 25, 17,  9,  1,
	   59, 51, 43, 35, 27, 19, 11,  3,
	   61, 53, 45, 37, 29, 21, 13,  5,
	   63, 55, 47, 39, 31, 23, 15,  7
};

/* final permutation IP^-1 */
const TUint8 fp[] = {
	   40,  8, 48, 16, 56, 24, 64, 32,
	   39,  7, 47, 15, 55, 23, 63, 31,
	   38,  6, 46, 14, 54, 22, 62, 30,
	   37,  5, 45, 13, 53, 21, 61, 29,
	   36,  4, 44, 12, 52, 20, 60, 28,
	   35,  3, 43, 11, 51, 19, 59, 27,
	   34,  2, 42, 10, 50, 18, 58, 26,
	   33,  1, 41,  9, 49, 17, 57, 25
};

/* expansion operation matrix */
const TUint8 ei[] = {
	   32,  1,  2,  3,  4,  5,
		4,  5,  6,  7,  8,  9,
		8,  9, 10, 11, 12, 13,
	   12, 13, 14, 15, 16, 17,
	   16, 17, 18, 19, 20, 21,
	   20, 21, 22, 23, 24, 25,
	   24, 25, 26, 27, 28, 29,
	   28, 29, 30, 31, 32,  1
};

const TUint32 sbox[8][64] = {
{
0x01010400,0x00000000,0x00010000,0x01010404, 0x01010004,0x00010404,0x00000004,0x00010000,
0x00000400,0x01010400,0x01010404,0x00000400, 0x01000404,0x01010004,0x01000000,0x00000004,
0x00000404,0x01000400,0x01000400,0x00010400, 0x00010400,0x01010000,0x01010000,0x01000404,
0x00010004,0x01000004,0x01000004,0x00010004, 0x00000000,0x00000404,0x00010404,0x01000000,
0x00010000,0x01010404,0x00000004,0x01010000, 0x01010400,0x01000000,0x01000000,0x00000400,
0x01010004,0x00010000,0x00010400,0x01000004, 0x00000400,0x00000004,0x01000404,0x00010404,
0x01010404,0x00010004,0x01010000,0x01000404, 0x01000004,0x00000404,0x00010404,0x01010400,
0x00000404,0x01000400,0x01000400,0x00000000, 0x00010004,0x00010400,0x00000000,0x01010004},
{
0x80108020,0x80008000,0x00008000,0x00108020, 0x00100000,0x00000020,0x80100020,0x80008020,
0x80000020,0x80108020,0x80108000,0x80000000, 0x80008000,0x00100000,0x00000020,0x80100020,
0x00108000,0x00100020,0x80008020,0x00000000, 0x80000000,0x00008000,0x00108020,0x80100000,
0x00100020,0x80000020,0x00000000,0x00108000, 0x00008020,0x80108000,0x80100000,0x00008020,
0x00000000,0x00108020,0x80100020,0x00100000, 0x80008020,0x80100000,0x80108000,0x00008000,
0x80100000,0x80008000,0x00000020,0x80108020, 0x00108020,0x00000020,0x00008000,0x80000000,
0x00008020,0x80108000,0x00100000,0x80000020, 0x00100020,0x80008020,0x80000020,0x00100020,
0x00108000,0x00000000,0x80008000,0x00008020, 0x80000000,0x80100020,0x80108020,0x00108000},
{
0x00000208,0x08020200,0x00000000,0x08020008, 0x08000200,0x00000000,0x00020208,0x08000200,
0x00020008,0x08000008,0x08000008,0x00020000, 0x08020208,0x00020008,0x08020000,0x00000208,
0x08000000,0x00000008,0x08020200,0x00000200, 0x00020200,0x08020000,0x08020008,0x00020208,
0x08000208,0x00020200,0x00020000,0x08000208, 0x00000008,0x08020208,0x00000200,0x08000000,
0x08020200,0x08000000,0x00020008,0x00000208, 0x00020000,0x08020200,0x08000200,0x00000000,
0x00000200,0x00020008,0x08020208,0x08000200, 0x08000008,0x00000200,0x00000000,0x08020008,
0x08000208,0x00020000,0x08000000,0x08020208, 0x00000008,0x00020208,0x00020200,0x08000008,
0x08020000,0x08000208,0x00000208,0x08020000, 0x00020208,0x00000008,0x08020008,0x00020200},
{
0x00802001,0x00002081,0x00002081,0x00000080, 0x00802080,0x00800081,0x00800001,0x00002001,
0x00000000,0x00802000,0x00802000,0x00802081, 0x00000081,0x00000000,0x00800080,0x00800001,
0x00000001,0x00002000,0x00800000,0x00802001, 0x00000080,0x00800000,0x00002001,0x00002080,
0x00800081,0x00000001,0x00002080,0x00800080, 0x00002000,0x00802080,0x00802081,0x00000081,
0x00800080,0x00800001,0x00802000,0x00802081, 0x00000081,0x00000000,0x00000000,0x00802000,
0x00002080,0x00800080,0x00800081,0x00000001, 0x00802001,0x00002081,0x00002081,0x00000080,
0x00802081,0x00000081,0x00000001,0x00002000, 0x00800001,0x00002001,0x00802080,0x00800081,
0x00002001,0x00002080,0x00800000,0x00802001, 0x00000080,0x00800000,0x00002000,0x00802080},
{
0x00000100,0x02080100,0x02080000,0x42000100, 0x00080000,0x00000100,0x40000000,0x02080000,
0x40080100,0x00080000,0x02000100,0x40080100, 0x42000100,0x42080000,0x00080100,0x40000000,
0x02000000,0x40080000,0x40080000,0x00000000, 0x40000100,0x42080100,0x42080100,0x02000100,
0x42080000,0x40000100,0x00000000,0x42000000, 0x02080100,0x02000000,0x42000000,0x00080100,
0x00080000,0x42000100,0x00000100,0x02000000, 0x40000000,0x02080000,0x42000100,0x40080100,
0x02000100,0x40000000,0x42080000,0x02080100, 0x40080100,0x00000100,0x02000000,0x42080000,
0x42080100,0x00080100,0x42000000,0x42080100, 0x02080000,0x00000000,0x40080000,0x42000000,
0x00080100,0x02000100,0x40000100,0x00080000, 0x00000000,0x40080000,0x02080100,0x40000100},
{
0x20000010,0x20400000,0x00004000,0x20404010, 0x20400000,0x00000010,0x20404010,0x00400000,
0x20004000,0x00404010,0x00400000,0x20000010, 0x00400010,0x20004000,0x20000000,0x00004010,
0x00000000,0x00400010,0x20004010,0x00004000, 0x00404000,0x20004010,0x00000010,0x20400010,
0x20400010,0x00000000,0x00404010,0x20404000, 0x00004010,0x00404000,0x20404000,0x20000000,
0x20004000,0x00000010,0x20400010,0x00404000, 0x20404010,0x00400000,0x00004010,0x20000010,
0x00400000,0x20004000,0x20000000,0x00004010, 0x20000010,0x20404010,0x00404000,0x20400000,
0x00404010,0x20404000,0x00000000,0x20400010, 0x00000010,0x00004000,0x20400000,0x00404010,
0x00004000,0x00400010,0x20004010,0x00000000, 0x20404000,0x20000000,0x00400010,0x20004010},
{
0x00200000,0x04200002,0x04000802,0x00000000, 0x00000800,0x04000802,0x00200802,0x04200800,
0x04200802,0x00200000,0x00000000,0x04000002, 0x00000002,0x04000000,0x04200002,0x00000802,
0x04000800,0x00200802,0x00200002,0x04000800, 0x04000002,0x04200000,0x04200800,0x00200002,
0x04200000,0x00000800,0x00000802,0x04200802, 0x00200800,0x00000002,0x04000000,0x00200800,
0x04000000,0x00200800,0x00200000,0x04000802, 0x04000802,0x04200002,0x04200002,0x00000002,
0x00200002,0x04000000,0x04000800,0x00200000, 0x04200800,0x00000802,0x00200802,0x04200800,
0x00000802,0x04000002,0x04200802,0x04200000, 0x00200800,0x00000000,0x00000002,0x04200802,
0x00000000,0x00200802,0x04200000,0x00000800, 0x04000002,0x04000800,0x00000800,0x00200002},
{
0x10001040,0x00001000,0x00040000,0x10041040, 0x10000000,0x10001040,0x00000040,0x10000000,
0x00040040,0x10040000,0x10041040,0x00041000, 0x10041000,0x00041040,0x00001000,0x00000040,
0x10040000,0x10000040,0x10001000,0x00001040, 0x00041000,0x00040040,0x10040040,0x10041000,
0x00001040,0x00000000,0x00000000,0x10040040, 0x10000040,0x10001000,0x00041040,0x00040000,
0x00041040,0x00040000,0x10041000,0x00001000, 0x00000040,0x10040040,0x00001000,0x00041040,
0x10001000,0x00000040,0x10000040,0x10040000, 0x10040040,0x10000000,0x00040000,0x10001040,
0x00000000,0x10041040,0x00040040,0x10000040, 0x10040000,0x10001000,0x10001040,0x00000000,
0x10041040,0x00041000,0x00041000,0x00001040, 0x00001040,0x00040040,0x10000000,0x10041000}
};


/* 32-bit permutation function P used on the output of the S-boxes */
const TUint8 p32i[] = {
	   16,  7, 20, 21,
	   29, 12, 28, 17,
		1, 15, 23, 26,
		5, 18, 31, 10,
		2,  8, 24, 14,
	   32, 27,  3,  9,
	   19, 13, 30,  6,
	   22, 11,  4, 25
};

/* permuted choice table (key) */
const TUint8 pc1[] = {
	   57, 49, 41, 33, 25, 17,  9,
		1, 58, 50, 42, 34, 26, 18,
	   10,  2, 59, 51, 43, 35, 27,
	   19, 11,  3, 60, 52, 44, 36,

	   63, 55, 47, 39, 31, 23, 15,
		7, 62, 54, 46, 38, 30, 22,
	   14,  6, 61, 53, 45, 37, 29,
	   21, 13,  5, 28, 20, 12,  4
};

/* number left rotations of pc1 */
const TUint8 totrot[] = {1,2,4,6,8,10,12,14,15,17,19,21,23,25,27,28};

/* permuted choice key (table) */
const TUint8 pc2[] = {
	   14, 17, 11, 24,  1,  5,
		3, 28, 15,  6, 21, 10,
	   23, 19, 12,  4, 26,  8,
	   16,  7, 27, 20, 13,  2,
	   41, 52, 31, 37, 47, 55,
	   30, 40, 51, 45, 33, 48,
	   44, 49, 39, 56, 34, 53,
	   46, 42, 50, 36, 29, 32
};
}	//	namespace DES_TABLE
#endif	//	__DESTABLES_H__
