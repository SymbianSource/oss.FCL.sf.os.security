/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Prints debug information.
*
*/


/**
 @file
 @internalComponent
*/
 
#ifndef __AUTHSERVER_LOG_H__
#define __AUTHSERVER_LOG_H__

#include <e32debug.h>

namespace AuthServ
{

#ifdef _DEBUG

#define DEBUG_PRINTF(a) {AuthServ::DebugPrintf(__LINE__, __FILE__, a);}
#define DEBUG_PRINTF2(a, b) {AuthServ::DebugPrintf(__LINE__, __FILE__, a, b);}
#define DEBUG_PRINTF3(a, b, c) {AuthServ::DebugPrintf(__LINE__, __FILE__, a, b, c);}
#define DEBUG_PRINTF4(a, b, c, d) {AuthServ::DebugPrintf(__LINE__, __FILE__, a, b, c, d);}
#define DEBUG_PRINTF5(a, b, c, d, e) {AuthServ::DebugPrintf(__LINE__, __FILE__, a, b, c, d, e);}

#define DEBUG_CODE_SECTION(a) TRAP_IGNORE({ a; }) 

// UTF-8 overload of the DebufPrintf method. Should be used by default,
// since it's cheaper both in CPU cycles and stack space.

inline void DebugPrintf(TInt aLine, char* aFile, TRefByValue<const TDesC8> aFormat, ...)
	{
	VA_LIST list;
	VA_START(list, aFormat);
	
	TTime now;
	now.HomeTime();
	
	TBuf8<1024> buffer;
	_LIT8(KAuthServLogPrefix, "[authserver] ");
	_LIT8(KAuthServLineFileFormat, "%Ld Line: % 5d, File: %s -- ");
	buffer.Append(KAuthServLogPrefix);
	buffer.AppendFormat(KAuthServLineFileFormat, now.Int64(), aLine, aFile);
	buffer.AppendFormatList(aFormat, list);
	buffer.Append(_L8("\r\n"));
	
	RDebug::RawPrint(buffer);
	
	VA_END(list);
	}
	
// Unicode DebufPrintf overload

inline void DebugPrintf(TInt aLine, char* aFile, TRefByValue<const TDesC16> aFormat, ...)
	{
	VA_LIST list;
	VA_START(list, aFormat);
	
	TTime now;
	now.HomeTime();
	
	TBuf8<256> header;
	_LIT8(KAuthServLogPrefix, "[authserver] ");
	_LIT8(KAuthServLineFileFormat, "%Ld Line: % 5d, File: %s -- ");
	header.Append(KAuthServLogPrefix);
	header.AppendFormat(KAuthServLineFileFormat, now.Int64(), aLine, aFile);
	
	TBuf<1024> buffer;
	buffer.Copy(header);
	buffer.AppendFormatList(aFormat, list);
	buffer.Append(_L("\r\n"));
	
	RDebug::RawPrint(buffer);
	
	VA_END(list);
	}

#else

#define DEBUG_PRINTF(a)
#define DEBUG_PRINTF2(a, b)
#define DEBUG_PRINTF3(a, b, c)
#define DEBUG_PRINTF4(a, b, c, d)
#define DEBUG_PRINTF5(a, b, c, d, e)

#define DEBUG_CODE_SECTION(a)

#endif


} // namespace AuthServ

#endif // __AUTHSERVER_LOG_H__
