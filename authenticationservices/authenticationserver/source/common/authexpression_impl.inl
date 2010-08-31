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


#ifndef AUTHEXPRESSION_IMPL_INL
#define AUTHEXPRESSION_IMPL_INL

#include "authexpression_impl.h"

namespace AuthServer {

// -------- TSizeStream --------


inline TSizeStream::TSizeStream()
/**
	Initialise the accumulated stream size to zero.
 */
:	iSize(0)
	{
	// empty.
	}


inline TInt TSizeStream::Size() const
/**
	Accessor function returns the accumulated
	stream size in bytes.
	
	@return					Accumulated stream size in bytes.
 */
	{
	return iSize;
	}


// -------- CAuthExpressionImpl --------


inline CAuthExpressionImpl::TType CAuthExpressionImpl::Type() const
/**
	Accessor function returns this expression type, i.e.
	whether it is a plugin ID, a plugin type, an AND expression,
	or an OR expression.
	
	@return					This expression's type.
	@panic	AUTHEXPR 64		This object is internally inconsistent
							when the function is called (debug only.)
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(ETyAInvariant));
	return iType;
	}


inline const CAuthExpressionImpl* CAuthExpressionImpl::Left() const
/**
	Accessor function returns this expression's left node.
	
	@return					A non-modifiable pointer to
							this expression's left node.
	@panic	AUTHEXPR 80		This object is internally inconsistent	
							when this function is called (debug only.)
	@panic	AUTHEXPR 81		This object is not an AND or OR expression
							(debug only.)
	@see Right
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(ELfInvariant));
	__ASSERT_DEBUG(iType == EAnd || iType == EOr, Panic(ELfNotComplex));
	return iComb.iLeft;
	}

	
inline const CAuthExpressionImpl* CAuthExpressionImpl::Right() const
/**
	Accessor function returns this expression's left node.
	
	@return					A non-modifiable pointer to
							this expression's right node.
	@panic	AUTHEXPR 96		This object is not an AND or OR expression
							(debug only.)
	@see Left
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(ERgInvariant));
	__ASSERT_DEBUG(iType == EAnd || iType == EOr, Panic(ERgNotComplex));
	return iComb.iRight;
	}


inline TAuthPluginType CAuthExpressionImpl::PluginType() const
/**
	Accessor function returns this expression's plugin type.
	
	@return					This expression's plugin type.
	@panic	AUTHEXPR 112	This object is internally inconsistent
							when this function is called (debug only.)
	@panic	AUTHEXPR 113	This object does not describe a
							plugin type (debug only.)
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EPTyInvariant));
	__ASSERT_DEBUG(iType == EPluginType, Panic(EPTyNotPluginType));
	return iPluginType;
	}


inline TPluginId CAuthExpressionImpl::PluginId() const
/**
	Accessor function returns this expression's plugin ID.
	
	@return					This expression's plugin ID.
	@panic AUTHEXPR 128		This object is internally inconsistent
							when this function is called (debug only.)
	@panic AUTHEXPR 129		This object does not identify a
							specific plugin (debug only.)
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EPIdInvariant));
	__ASSERT_DEBUG(iType == EPluginId, Panic(EPIdNotPluginId));
	return iPluginId;
	}


inline CAuthExpressionImpl* CAuthExpressionImpl::Parent() const
/**
	Accessor function returns this node's parent.
	
	@return					This node's parent.  This is
							NULL for a root node.
	@panic AUTHEXPR 144		This object is internally inconsistent
							when this function is called (debug only.)
	@internalComponent
 */
	{
	__ASSERT_DEBUG(Invariant(), Panic(EPPrInvariant));

	return iParent;
	}



}	// namespace AuthServer {


#endif	// #ifndef AUTHEXPRESSION_IMPL_INL

