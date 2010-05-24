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
*
*/


#include "authcommon_impl.h"
#include <scs/cleanuputils.h>

using namespace AuthServer;

const TInt CAuthExpressionImpl::KVersion = 1;

// -------- TAuthExpressionWrapper --------


TAuthExpressionWrapper::TAuthExpressionWrapper(CAuthExpression* aExpression)
/**
	This constructor stores the supplied expression pointer.
	
	@param	aExpression		A pointer to the supplied expression
							is stored in the TAuthExpressionWrapper
							object.  On exit, this object owns
							the expression.
 */
:	iPtr(aExpression)
	{
	// empty.
	}


TAuthExpressionWrapper::TAuthExpressionWrapper(TAuthPluginType aType)
/**
	Allocates a new instance of CAuthExpressionImpl which
	describes the supplied plugin type.  If there is not
	enough memory, the pointer is NULL on exit.
	
	@param	aType			Plugin type used to create new instance
							of CAuthExpressionImpl.
 */
:	iPtr(new CAuthExpressionImpl(aType))
	{
	// empty.
	}


TAuthExpressionWrapper::TAuthExpressionWrapper(TPluginId aPluginId)
/**
	Allocates a new instance of CAuthExpressionImpl which
	describes the supplied plugin ID.  If there is not
	enough memory, the pointer is NULL on exit.
	
	@param	aPluginId		Value which describes a specific plugin.
 */
:	iPtr(new CAuthExpressionImpl(aPluginId))
	{
	// empty.
	}

TAuthExpressionWrapper::TAuthExpressionWrapper()
/**
	Allocates a new instance of CAuthExpressionImpl which
	describes the default plugin ID.  If there is not
	enough memory, the pointer is NULL on exit.
	
 */
:	iPtr(new CAuthExpressionImpl())
	{
	// empty.
	}

// -------- factory functions --------


static CAuthExpressionImpl* AuthComb(
	CAuthExpressionImpl::TType aType, CAuthExpressionImpl* aLeft, CAuthExpressionImpl* aRight);


EXPORT_C TAuthExpressionWrapper AuthServer::AuthExpr(TAuthPluginType aType)
/**
	Returns a wrapper around a new authentication expression.
	
	@param	aType			Identifies a type of plugin.
	@return					Wrapper object for an instance
							of CAuthExpressionImpl.  The authentication
							expression is allocated on the heap,
							and is NULL if there is not
							enough memory.
 */
	{
	return TAuthExpressionWrapper(aType);
	}


EXPORT_C TAuthExpressionWrapper AuthServer::AuthExpr(TPluginId aPluginId)
/**
	Returns a wrapper around a new authentication expression.
	
	@param	aPluginId		Identifies a specific plugin.
	@return					Wrapper object for an instance
							of CAuthExpressionImpl.  The authentication
							expression is allocated on the heap,
							and is NULL if there is not
							enough memory.
 */
	{
	return TAuthExpressionWrapper(aPluginId);
	}

EXPORT_C TAuthExpressionWrapper AuthServer::AuthExpr()
/**
	Returns a wrapper around a new authentication expression.
	
	@return					Wrapper object for an instance
							of CAuthExpressionImpl.  The authentication
							expression is allocated on the heap,
							and is NULL if there is not
							enough memory.
 */
	{
	return TAuthExpressionWrapper();
	}

EXPORT_C TAuthExpressionWrapper AuthServer::AuthOr(CAuthExpression* aLeft, CAuthExpression* aRight)
/**
	Allocate a CAuthExpressionImpl node which combines the supplied
	left and right nodes as an OR operation.
	
	If either aLeft or aRight are NULL, or this operation fails
	to allocate the required memory, then any allocated memory
	is cleaned up NULL is returned.
	
	@param	aLeft			Left node.  This must be an instance
							of CAuthExpresionImpl.
	@param	aRight			Right node.  This must be an instance
							of CAuthExpresionImpl.
	@return					New wrapper around CAuthExpression, NULL
							if could not allocate, or if either the
							supplied nodes were NULL.
 */
	{
	CAuthExpressionImpl* leftImpl = static_cast<CAuthExpressionImpl*>(aLeft);
	CAuthExpressionImpl* rightImpl = static_cast<CAuthExpressionImpl*>(aRight);
	return AuthComb(CAuthExpressionImpl::EOr, leftImpl, rightImpl);
	}


EXPORT_C TAuthExpressionWrapper AuthServer::AuthAnd(CAuthExpression* aLeft, CAuthExpression* aRight)
/**
	Allocate a CAuthExpressionImpl node which combines the supplied
	left and right nodes as an AND operation.
	
	If either aLeft or aRight are NULL, or this operation fails
	to allocate the required memory, then the subexpressions are
	deleted and this function returns NULL.
	
	@param	aLeft			Left node.
	@param	aRight			Right node.
	@return					New wrapper around CAuthExpression, NULL
							if could not allocate, or if either the
							supplied nodes were NULL.
	@see AuthOr
 */
	{
	CAuthExpressionImpl* leftImpl = static_cast<CAuthExpressionImpl*>(aLeft);
	CAuthExpressionImpl* rightImpl = static_cast<CAuthExpressionImpl*>(aRight);
	return AuthComb(CAuthExpressionImpl::EAnd, leftImpl, rightImpl);
	}


static CAuthExpressionImpl* AuthComb(
	CAuthExpressionImpl::TType aType, CAuthExpressionImpl* aLeft, CAuthExpressionImpl* aRight)
/**
	Helper function for AuthOr and AuthAnd.  This function
	allocates the combining node, storing the combining method (AND
	or OR) and pointers to the left and right nodes.
	
	If aLeft or aRight are NULL on entry, or this function cannot
	allocate the required memory, then any previously allocated nodes
	are freed, and this function returns NULL.
	
	@param	aType			Type of expression, AND/OR
	@param	aLeft			Left node.
	@param	aRight			Right node.
	@return					New wrapper around CAuthExpression, NULL
							if could not allocate, or if either the
							supplied nodes were NULL.
	@see AuthAnd
	@see AuthOr
 */
	{
	CAuthExpressionImpl* compound = 0;
	
	if (aLeft == 0 || aRight == 0)
		goto failed;
	
	compound = new CAuthExpressionImpl(aType, aLeft, aRight);
	if (compound == 0)
		goto failed;
	
	return compound;
	
failed:
	delete aLeft;
	delete aRight;
	
	return 0;
	}


// -------- TSizeStream --------


EXPORT_C void TSizeStream::DoWriteL(const TAny* /* aPtr */, TInt aLength)
/**
	Override MStreamBuf by incrementing the
	accumulated size by aLength.
	
	@param	aLength			Length of data to write to stream.
 */
	{
	iSize += aLength;
	}


// -------- CAuthExpressionImpl --------


#ifdef _DEBUG
#define VAR_FOLLOWS(___c, ___v1, ___v2)	\
	(_FOFF(___c, ___v2) >= _FOFF(___c, ___v1) + sizeof(___c::___v1))
#endif


CAuthExpressionImpl::CAuthExpressionImpl(TAuthPluginType aType)
:	iType(EPluginType),
	iPluginType(aType)
/**
	Initialise this leaf node authentication expression
	as describing a plugin type.
	
	@param	aType			Identifies a type of plugin.
	@panic AUTHEXPR 16		This constructed object is internally
							inconsistent (debug only.)
 */
	{
#ifdef _DEBUG
	// non-aligned value to detect invalid node pointers
	__ASSERT_COMPILE(VAR_FOLLOWS(CAuthExpressionImpl, iPluginType, iComb.iRight));
	iComb.iRight = (CAuthExpressionImpl*)0xB51DE;
#endif

	__ASSERT_DEBUG(Invariant(), Panic(ECtTyInvariant));
	}


CAuthExpressionImpl::CAuthExpressionImpl(TPluginId aPluginId)
/**
	Initialise this leaf node authentication expression
	as describing a specific plugin.
	
	@param	aPluginId		Identifies a specific plugin.
	@panic	AUTHEXPR 32		This constructed object is internally
							inconsistent (debug only.)
 */
:	iType(EPluginId),
	iPluginId(aPluginId)
	{
#ifdef _DEBUG
	// non-aligned value to detect invalid node pointers
	__ASSERT_COMPILE(VAR_FOLLOWS(CAuthExpressionImpl, iPluginId, iComb.iRight));
	iComb.iRight = (CAuthExpressionImpl*)0xB51DE;
#endif

	__ASSERT_DEBUG(Invariant(), Panic(ECtIdInvariant));
	}

CAuthExpressionImpl::CAuthExpressionImpl()
/**
	Initialise this leaf node authentication expression
	as describing a specific plugin.

	@panic	AUTHEXPR 32		This constructed object is internally
							inconsistent (debug only.)
 */
:	iType(ENull)
	{
#ifdef _DEBUG
	// non-aligned value to detect invalid node pointers
	__ASSERT_COMPILE(VAR_FOLLOWS(CAuthExpressionImpl, iPluginId, iComb.iRight));
	iComb.iRight = (CAuthExpressionImpl*)0xB51DE;
#endif

	__ASSERT_DEBUG(Invariant(), Panic(ECtIdInvariant));

	}


CAuthExpressionImpl::CAuthExpressionImpl(TType aType, CAuthExpressionImpl* aLeft, CAuthExpressionImpl* aRight)
/**
	Initialise a complex - AND or OR - expression.
	
	@param	aType			The type of combination.  The only
							allowed values are EAnd and EOr.
	@param	aLeft			Left expression.  This cannot be NULL.
	@param	aRight			Right expression.  This cannot be NULL.
	@panic	AUTHEXPR 48		Called with expression type that was
							neither AND nor OR (debug only.)
	@panic	AUTHEXPR 49		Called with NULL left node (debug only.)
	@panic	AUTHEXPR 50		Called with NULL right node (debug only.)
	@panic	AUTHEXPR 51		This object is internally inconsistent
							after construction (debug only.)
 */
:	iType(aType)
	{
	__ASSERT_DEBUG(aType == EAnd || aType == EOr, Panic(ECt2BadComb));
	__ASSERT_DEBUG(aLeft != 0, Panic(ECt2NullLeft));
	__ASSERT_DEBUG(aRight != 0, Panic(ECt2NullRight));
	
	iComb.iLeft = aLeft;
	iComb.iRight = aRight;
	aLeft->iParent = aRight->iParent = this;
	
	__ASSERT_DEBUG(Invariant(), Panic(ECt2Invariant));
	}


CAuthExpressionImpl::~CAuthExpressionImpl()
/**
	Deletes resources used by this expression.  If this is a complex
	expression then it deletes the subexpression nodes.
 */
	{
	// by construction iType is always correctly initialized
	// before this function is called.
	if (iType == EAnd || iType == EOr)
		{
		delete iComb.iLeft;
		delete iComb.iRight;
		}
	}


EXPORT_C void CAuthExpressionImpl::ExternalizeL(RWriteStream& aWriteStream) const
/**
	Write a persistent version of this object to the supplied
	stream.  This function is used to transfer the expression
	to the authentication server.
	
	@param	aWriteStream	Stream to write object to.
	@panic	AUTHEXPR 160	This object is internally inconsistent
							when this function is called.
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EExtInvariant));
	
	aWriteStream.WriteInt8L(KVersion);
	Externalize2L(aWriteStream);
	}


void CAuthExpressionImpl::Externalize2L(RWriteStream& aWriteStream) const
/**
	Helper function for ExternalizeL.  This function writes
	a persistent version of this expression object, including
	any subexpressions, to the supplied stream.
	
	@param	aWriteStream	Stream to write object to.
	@panic	AUTHEXPR 144	This object is internally inconsistent
							when this function is called.
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EExtInvariant));
	
	aWriteStream.WriteInt8L(iType);
	
	switch (iType)
		{
	case EAnd:
	case EOr:
		iComb.iLeft->Externalize2L(aWriteStream);
		iComb.iRight->Externalize2L(aWriteStream);
		break;
	
	case EPluginId:
		aWriteStream.WriteInt32L(iPluginId);
		break;
	
	case EPluginType:
		aWriteStream.WriteInt32L(iPluginType);
		break;
		
	case ENull:
		aWriteStream.WriteInt32L(iPluginId);
		break;
	default:
		// this case should not be reached because this
		// object has passed the invariant.
		break;
		}
	}


// restore the encoded authentication expression


EXPORT_C CAuthExpressionImpl* CAuthExpressionImpl::NewL(RReadStream& aReadStream)
/**
	Factory function reconstructs an authentication
	expression from the supplied stream.
	
	@param	aReadStream		Stream containing externalized
							authentication expression.  This must
							have been generated with CAuthExpressionImpl::ExternalizeL.	
	@return					Authorisation expression internalized
							from the supplied stream.
	@leave KErrNoMemory		Not enough memory to reconstruct the expression.
	@leave KErrInternalizeInvalidAuthExpr The supplied stream does not
							describe a valid authentication expression.
	@leave KErrAuthServUnsupportedExprVersion The supplied stream
							was created with a later version of CAuthExpressionImpl.
	@see CAuthExpressionImpl::ExternalizeL
 */
	{
	TInt8 ver = aReadStream.ReadInt8L();
	if (ver > KVersion)
		User::Leave(KErrAuthServUnsupportedExprVersion);
	
	return New2L(aReadStream);
	}
	
CAuthExpressionImpl* CAuthExpressionImpl::New2L(RReadStream& aReadStream)
/**
	Helper function for NewL.  This recursively constructs the
	authentication expression after NewL has checked that it
	supports the encoded version.
	
	@param	aReadStream		Stream containing externalized
							authentication expression.  This must
							have been generated with CAuthExpressionImpl::ExternalizeL.	
	@return					Authorisation expression internalized
							from the supplied stream.
	@leave KErrNoMemory		Not enough memory to reconstruct the expression.
	@leave KErrInternalizeInvalidAuthExpr The supplied stream does not
							describe a valid authentication expression.
	@see CAuthExpressionImpl::ExternalizeL
 */
	{
	CAuthExpressionImpl::TType t;
	t = static_cast<CAuthExpressionImpl::TType>(aReadStream.ReadInt8L());
	
	switch(t)
		{
	case CAuthExpressionImpl::EAnd:
	case CAuthExpressionImpl::EOr:
		{
		CAuthExpressionImpl* left = CAuthExpressionImpl::New2L(aReadStream);
		CleanupStack::PushL(left);
		CAuthExpressionImpl* right = CAuthExpressionImpl::New2L(aReadStream);
		CleanupStack::PushL(right);
		CAuthExpressionImpl* complex = new(ELeave) CAuthExpressionImpl(t, left, right);
		CleanupStack::Pop(2, left);	// complex now owns left and right
		return complex;
		}
	
	case CAuthExpressionImpl::EPluginId:
		return new(ELeave) CAuthExpressionImpl(
			static_cast<TPluginId>(aReadStream.ReadInt32L()));
	
	case CAuthExpressionImpl::EPluginType:
		return new(ELeave) CAuthExpressionImpl(
			static_cast<TAuthPluginType>(aReadStream.ReadInt32L()));
	
		
	case CAuthExpressionImpl::ENull:
		return new(ELeave) CAuthExpressionImpl();	
	
	default:
		User::Leave(KErrAuthServInvalidInternalizeExpr);
		return 0;	// avoid 'return value expected' warning
		}
	}
	
/**
  Helper function which converts the string into a form that can 
  be processed as tokens by TLex.Essentially, this method parses and appends
  space in the input string when delimiters("(",")","&" and "|") are
  encountered, consequenty the resultant string thus obtained can be parsed
  using TLex.
 
  @param	aStringToBeProcessed	string to be processed.	
  @param	aResultantString		a buffer which does not already own any allocated memory, and is populated 
  									with a string resulting from processing aStringToBeProcessed.
 	 									
 */ 	

EXPORT_C void CAuthExpressionImpl::InsertSpaceBetweenOperatorsL(const TDesC& aStringToBeProcessed, RBuf& aResultantString)
	{
	TChar ch = 0;
	_LIT(KSeparator, " ");
	HBufC* strBuffer = HBufC::NewLC(KMaxBufferSize);
	TPtr strPtr(strBuffer->Des());
	
	// length of 2 KSeparator and 1 ch.
	const TInt KAddLength = 3;
	
	for(TInt i = 0; i < aStringToBeProcessed.Length(); ++i)
		{
		ch = aStringToBeProcessed[i];
		
		// check whether appending 3 characters(2 KSeparator and 1 ch) to
		// aResultantAliasString exceeds the Maxlength.
		TInt resultingLen = strPtr.Length() + KAddLength;
		if(resultingLen > KMaxBufferSize)
			{
			strBuffer->ReAllocL(resultingLen);
			}
				
		if(ch == '(' || ch == ')' || ch == '&' || ch == '|')
			{
			strPtr.Append(KSeparator);
			strPtr.Append(ch);
			strPtr.Append(KSeparator);
			}
			
		else
			{
			strPtr.Append(ch);
			}
		}
	
	CleanupStack::Pop(strBuffer);
	aResultantString.Assign(strBuffer);
	}
		
/**
  Creates CAuthExpression object from a string defining an authentication.
  strength.The ownership of CAuthExpression object is transferred to the caller.
 
  @param aStrengthAliasString a string defining an authentication.
  strength obtained from the authserver cenrep file.
 
  @return CAuthExpression object.
 
  @leave KErrAuthServInvalidAliasStringExpression, when aStrengthAliasString contains tokens other than pluginId, pluginType
  and operators '&', '|', '(' and ')'		
 
 */  
    
 EXPORT_C CAuthExpression* CAuthExpressionImpl::CreateAuthExprObjectL(const TDesC& aStrengthAliasString)
 	{
   	RBuf resultantAliasString;
   	CleanupClosePushL(resultantAliasString);
     	
   	// tokenize aStrengthAliasString, to facilitate parsing using TLex. 
   	InsertSpaceBetweenOperatorsL(aStrengthAliasString, resultantAliasString);
		
    // facilitates ordering of expression evaluation via brackets.
    CAuthExpression* authExpression = EvaluateAliasStringL(resultantAliasString);
    CleanupStack::PopAndDestroy(); 

    return authExpression;
    }
    
  
 /**
  This method facilitates ordering of alias string expression (containing pluginIds or pluginTypes
  or a combination of both) evaluation via brackets.The ownership of CAuthExpression object is 
  transferred to the caller.
 
  @param aStrengthAliasString	an alias string to be parsed and processed.This must be enclosed within brackets.
 
  @return CAuthExpression object.
 
  @leave KErrAuthServInvalidAliasStringExpression, when aStrengthAliasString contains tokens other than pluginId, pluginType
  						and operators('&', '|') and brackets.		
 
 */ 
    
  CAuthExpression* CAuthExpressionImpl::EvaluateAliasStringL(const RBuf& aStrengthAliasString)
  	{
  	TLex input(aStrengthAliasString);

  	// array of operators (& and |) and brackets.
  	const TInt KGranularity = 8;
  	CArrayFixFlat<TPtrC>* opStack = new(ELeave) CArrayFixFlat<TPtrC>(KGranularity);
   	CleanupStack::PushL(opStack);
   	
   	// array of CAuthExpression objects.
    RPointerArray<CAuthExpression> authExpressionArray;
    CleanupResetAndDestroyPushL(authExpressionArray);
   	
    for (TPtrC token = input.NextToken(); token.Size() > 0 ; 
         token.Set(input.NextToken()))
    	{
    	TInt count = 0;
    	TInt index = 0;
    	if(token.CompareF(KAuthOpOr) == 0)
    		{
    		count = opStack->Count();
    		index = count - 1;
    		// only when the previous element in the opStack is not "&" , will token be added on the opStack.
    		if(index >= 0 && opStack->At(index) != KAuthOpAnd)
    			{
    			opStack->AppendL(token);
    			}
    				
    		else
    			{
    			// atleast 1 element in opStack is "&",hence iterate through opStack and   
    			// keep creating auth expression object until the loop ends.
    			while(index >= 0 && opStack->At(index) == KAuthOpAnd)
    				{
    				CAuthExpression* authExpr = CreateAuthExpressionL(authExpressionArray, opStack->At(index));
    				opStack->Delete(index);
    				opStack->Compress();
    				CleanupStack::PushL(authExpr);
					authExpressionArray.AppendL(authExpr);
					CleanupStack::Pop(authExpr);
					--index;
    				}
    					
    			opStack->AppendL(token);	
    			}	
    		} //if(token.CompareF(KAuthOpOr) == 0)
    			
    	else if(token.CompareF(KAuthOpAnd) == 0 || token.CompareF(KOpenBracket) == 0)
    		{
    		opStack->AppendL(token);
    		}
    				
    	else if(token.CompareF(KCloseBracket) == 0)
    		{
    		// when the previous element in the opStack is "(", delete it.
    		count = opStack->Count();
    		index = count - 1;
    		TInt exprCount = authExpressionArray.Count();
    			
    		if(index >= 0 && opStack->At(index) == KOpenBracket)
    			{
    			opStack->Delete(index);
    			opStack->Compress();
    			}
    			
    		// the previous element could be an operator/operators of same precedence.	
    		else
    			{
    			while(index >= 0 && opStack->At(index) != KOpenBracket)
					{
					CAuthExpression* expr = CreateAuthExpressionL(authExpressionArray, opStack->At(index));
					CleanupStack::PushL(expr);
					authExpressionArray.AppendL(expr);
					CleanupStack::Pop(expr);
					opStack->Delete(index);	
					opStack->Compress();
					--index;
					}	//while
						
				if(index == -1)
					{
					User::Leave(KErrAuthServInvalidAliasStringExpression);
					}
						
				opStack->Delete(index);
				opStack->Compress();
    			}
			}	//else if(token.CompareF(KCloseBracket) == 0)
    		
    	//when the token is neither "(","|","&" and ")" , it must be a plugin id or plugin Type,
    	//hence create a CAuthExpression object.
    	else
    		{
    		CAuthExpression* expression = CreateAuthExpressionL(token);
    		CleanupStack::PushL(expression);
			authExpressionArray.AppendL(expression);
			CleanupStack::Pop(expression);
    		}
		}	//for loop
    
  // since authExpressionArray will eventually contain only 1 authExpression pointer,
  // remove it from the array, as the ownership of this pointer is transferred to the caller.
  __ASSERT_ALWAYS((authExpressionArray.Count() == 1), User::Leave(KErrAuthServInvalidAliasStringExpression));
   		
  CAuthExpression* authExpression = authExpressionArray[0];
  authExpressionArray.Remove(0);
  
  CleanupStack::PopAndDestroy(2, opStack);	//opStack and authExpressionArray
  return authExpression;
  
  }
        
/**
  Creates CAuthExpression object from an alias string containing either 
  plugin Id  or plugin type.The ownership of the object is transferred to the caller.
 
  @param aAliasString	string containing either plugin Id or plugin Type.
 
  @return CAuthExpression object.
 
  @leave KErrAuthServInvalidAliasStringExpression, if the operands or operators are invalid strings.	
 
  @leave KErrNoMemory, if there was insufficient memory to allocate the CAuthExpression object.
 */ 	
  CAuthExpression* CAuthExpressionImpl::CreateAuthExpressionL(const TDesC& aAliasString)
  	{
   	TLex  input(aAliasString);
   	CAuthExpression* expression = 0;
    TPluginId plugin = 0;
    	
  	for (TPtrC token = input.NextToken(); token.Size() > 0 ; 
         token.Set(input.NextToken()))
       {
    	TLex lexer(token);
    	if(token.CompareF(KAuthBiometric) == 0)
    		{
    		expression = AuthExpr(EAuthBiometric);
    		}
    	else if(token.CompareF(KAuthKnowledge) == 0)
    		{
    		expression = AuthExpr(EAuthKnowledge);
    		}
    	else if(token.CompareF(KAuthToken) == 0)
    		{
    		expression = AuthExpr(EAuthToken);
    		}
    	else if(token.CompareF(KAuthPerformance) == 0)
    		{
    		expression = AuthExpr(EAuthPerformance);
    		}
    	else if(token.CompareF(KAuthDefault) == 0)
    		{
    		expression = AuthExpr(EAuthDefault);
    		}	
       	else if(lexer.Val(plugin, EHex) == KErrNone)
    		{
    		expression = AuthExpr(plugin);
    		}	
    	else
    		{
    		User::Leave(KErrAuthServInvalidAliasStringExpression);
    		}
       }
     		

    if(expression == NULL)
		{
		User::Leave(KErrNoMemory);
		}

    return expression;    	
   	}
 
/**
  Creates CAuthExpression object from an array of authexpression objects and an operator.
  The ownership of the object is transferred to the caller.
 
  @param aAuthExprArray		an array of authexpression objects. 
  @param aOperator			an authexpression operator ( '&' or '|').
  
  @return CAuthExpression object.
  @leave KErrAuthServInvalidAliasStringExpression, if the operands or operators are invalid strings.
 
 */  	
 CAuthExpression* CAuthExpressionImpl::CreateAuthExpressionL(RPointerArray<CAuthExpression>& aAuthExprArray, TPtrC aOperator)
  	{
  	// aAuthExprArray must contain minimum of 2 elements.
  	__ASSERT_ALWAYS((aAuthExprArray.Count() >= 2), User::Leave(KErrAuthServInvalidAliasStringExpression));
  	
  	CAuthExpression* authExpression = 0;
   	TInt exprIndex = aAuthExprArray.Count() - 1;
   	
  	if(aOperator.CompareF(KAuthOpAnd) == 0)
  		{
  		authExpression = AuthAnd(aAuthExprArray[exprIndex], aAuthExprArray[exprIndex-1]);
  		}
    	
  	else if(aOperator.CompareF(KAuthOpOr) == 0)
  		{
  		authExpression = AuthOr(aAuthExprArray[exprIndex], aAuthExprArray[exprIndex-1]);
  		}
  		
    else
    	{
    	User::Leave(KErrAuthServInvalidAliasStringExpression);
    	}
	
	// array elements are not deleted since the ownership is transferred to authExpression,
	aAuthExprArray.Remove(exprIndex);
	aAuthExprArray.Remove(exprIndex-1);
	aAuthExprArray.Compress();
	
	return authExpression;
  	}


#ifndef _DEBUG
// In release mode provide stub functions for Panic() and Invariant().

EXPORT_C void CAuthExpressionImpl::Panic(TPanic /* aPanic */)
	{
	// empty.
	}

EXPORT_C TBool CAuthExpressionImpl::Invariant() const
	{
	return ETrue;
	}

#else

EXPORT_C void CAuthExpressionImpl::Panic(TPanic aPanic)
/**
	This function is defined for debug builds only.
	It halts the current thread when an invalid
	argument is supplied to one of CAuthExpressionImpl's functions.
	
	The current thread is panicked with category "AUTHEXPR"
	and the supplied reason.
	
	@param	aPanic			Panic reason.
 */
	{
	_LIT(KPanicCat, "AUTHEXPR");
	User::Panic(KPanicCat, aPanic);
	}

EXPORT_C TBool CAuthExpressionImpl::Invariant() const
/**
	This function is defined for debug builds, and
	checks that the object is internally consistent.
	
	The node's type must be a supported value - AND, OR,
	plugin ID, or plugin type.
	
	If this is a combining node then recursively ensure
	that both the left and right nodes are internally
	consistent.
 */
	{
	// this node's parent sees it as a child
	if (iParent != 0 && !(iParent->iComb.iLeft == this || iParent->iComb.iRight == this))
		return EFalse;
	
	switch (iType)
		{
	case EPluginId:
	case EPluginType:
		return ETrue;
		
	case EAnd:
	case EOr:
		// these will fault, as opposed to fail the invariant,
		// if the pointers are invalid, but so would the original
		// call to this function if the this pointer were invalid.
		
		// this node's children see it as the parent
		// don't use Parent() because that asserts on the
		// subexpression, whereas this function should
		// return EFalse.
		if (iComb.iLeft->iParent != this)
			return EFalse;
		
		if (iComb.iRight->iParent != this)
			return EFalse;
		
		return iComb.iLeft->Invariant() && iComb.iRight->Invariant();
	
	case ENull:
		return ETrue;
		
	default:
		// unrecognized node type, so fail invariant
		return EFalse;
		}
	}
	
#endif	// #ifdef _DEBUG


