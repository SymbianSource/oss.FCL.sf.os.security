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
* This file contains the implementation for CAuthExpression which is only visible
* to implementation, and not the the client code.
*
*/


/**
 @file 
 @released
 @internalComponent
*/
 
#ifndef AUTHEXPRESSION_IMPL_H
#define AUTHEXPRESSION_IMPL_H

#include <s32buf.h>
#include <authserver/authexpression.h>

namespace AuthServer {

// constants and operators used for processing authentication expression.

_LIT(KAuthOpAnd,"&");
_LIT(KAuthOpOr,"|");
_LIT(KAuthBiometric, "Biometric");
_LIT(KAuthDefault, "Default");
_LIT(KAuthKnowledge, "Knowledge");
_LIT(KAuthToken, "Token");
_LIT(KAuthPerformance, "Performance");
_LIT(KOpenBracket, "(");
_LIT(KCloseBracket, ")");
const TInt KMaxBufferSize = 2048;
    
class TSizeStream : public MStreamBuf
/**
	This subclass of MStreamBuf is used with
	RWriteStream to count how many bytes are
	required to externalize an object.
 */
	{
public:
	inline TSizeStream();
	inline TInt Size() const;

	// override MStreamBuf
	IMPORT_C virtual void DoWriteL(const TAny* /* aPtr */, TInt aLength);

private:
	/** Accumulated stream length in bytes. */
	TInt iSize;
	};


class CAuthExpressionImpl : public CAuthExpression
/**
	This class, which is not visible to clients,
	defines the variables and functions which are
	used by authentication expressions.
	
	Clients should use the CAuthExpression base class
	which provides type safety but provides no implementation,
	so authentication expressions can be re-implemented
	later without breaking BC.

	@internalComponent
 */
	{
public:
	/**
		The type of expression represented by an instance
		of this object, namely a plugin type, a plugin ID,
		or a complex AND expression, OR expression or a 
		NULL expression.
	 */
	enum TType {EPluginType, EPluginId, EAnd, EOr, ENull};
		
	CAuthExpressionImpl(TAuthPluginType aType);
	CAuthExpressionImpl(TPluginId aPluginId);
	CAuthExpressionImpl();
	CAuthExpressionImpl(TType aType, CAuthExpressionImpl* aLeft, CAuthExpressionImpl* aRight);

	IMPORT_C static CAuthExpressionImpl* NewL(RReadStream& aReadStream);

	virtual ~CAuthExpressionImpl();

	// implement CAuthExpression
	virtual void ExternalizeL(RWriteStream& aWriteStream) const;
	
	IMPORT_C static CAuthExpression* CreateAuthExprObjectL(const TDesC& aStrengthAliasString);
	
	IMPORT_C static void InsertSpaceBetweenOperatorsL(const TDesC& aAliasStringToBeProcessed, RBuf& aResultantAliasString);
	
	inline TType Type() const;
	inline const CAuthExpressionImpl* Left() const;
	inline const CAuthExpressionImpl* Right() const;
	inline TAuthPluginType PluginType() const;
	inline TPluginId PluginId() const;
	inline CAuthExpressionImpl* Parent() const;

private:
	/**
		Current authentication expression version.
		This is embedded in the externalized expression
		so the server can reject the expression if it
		uses an unsupported version.
	 */
	static const TInt KVersion;
	void Externalize2L(RWriteStream& aWriteStream) const;
	static CAuthExpressionImpl* New2L(RReadStream& aReadStream);
	static CAuthExpression* CreateAuthExpressionL(const TDesC& aAliasString);
	static CAuthExpression* CreateAuthExpressionL(RPointerArray<CAuthExpression>& aAuthExprArray, TPtrC aOperator);
	static CAuthExpression* EvaluateAliasStringL(const RBuf& aStrengthAliasString);
	
	
private:
	/**
		This expression's parent node.  This value is
		used to navigate the expression tree during
		evaluation.
	 */
	CAuthExpressionImpl* iParent;
	
	/** This expression's type. */
	TType iType;
	
	class TBinaryComb
	/**
		This class contains pointers to the left and
		right subexpressions when this expression is
		an AND or an OR.
	 */
		{
	public:
		/** Left subexpression.  This cannot be NULL. */
		CAuthExpressionImpl* iLeft;
		/** Right subexpression.  This cannot be NULL. */
		CAuthExpressionImpl* iRight;
		};

	union
		{
		/** This field is valid iff iType == EPluginType. */
		TAuthPluginType iPluginType;
		/** This field is valid iff iType == EPluginId. */
		TPluginId iPluginId;
		/** This field is valid iff iType == EAnd or iType == EOr. */
		TBinaryComb iComb;
		};
		
private:
	// Invariant and Panic are defined, but only as stub
	// functions for release mode.

	enum TPanic
	/**
		In debug builds the current thread can be halted
		with panic category "AUTHEXPR" and one of the
		following reasons to indicate that the object has
		become corrupt or that a caller has supplied an
		invalid argument to one of this class' functions.
	 */
		{
		ECtTyInvariant = 0x10,
		ECtIdInvariant = 0x20,
		ECt2BadComb = 0x30, ECt2NullLeft, ECt2NullRight, ECt2Invariant,
		ETyAInvariant = 0x40,
		ELfInvariant = 0x50, ELfNotComplex,
		ERgInvariant = 0x60, ERgNotComplex,
		EPTyInvariant = 0x70, EPTyNotPluginType,
		EPIdInvariant = 0x80, EPIdNotPluginId,
		EPPrInvariant = 0x90,
		EExtInvariant = 0xa0
		};
		

	IMPORT_C TBool Invariant() const;
	IMPORT_C static void Panic(TPanic aPanic);
	};

}	// namespace AuthServer {

#include "authexpression_impl.inl"

#endif	// #ifndef AUTHEXPRESSION_IMPL_H

