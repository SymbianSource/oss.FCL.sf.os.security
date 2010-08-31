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


#include "authserver_impl.h"


using namespace AuthServer;

const CAuthExpressionImpl::TType CEvaluator::KAnd = CAuthExpressionImpl::EAnd;
const CAuthExpressionImpl::TType CEvaluator::KOr = CAuthExpressionImpl::EOr;
const TInt CEvaluator::KRPNGranularity = 4;

// -------- (de)allocation --------


CEvaluator* CEvaluator::NewL(MEvaluatorPluginInterface* aPluginInterface, MEvaluatorClientInterface* aClientInterface)
/**
	Factory function allocates and initializes new
	instance of CEvaluator.
	
	@param	aPluginInterface	Used to invoke plugins.
	@param	aClientInterface	Used to notify clients when
								an evaluation has completed.
 */
	{
	CEvaluator* ev = new(ELeave) CEvaluator(aPluginInterface, aClientInterface);
	CleanupStack::PushL(ev);
	ev->ConstructL();
	CleanupStack::Pop(ev);
	return ev;
	}


CEvaluator::CEvaluator(MEvaluatorPluginInterface* aPluginInterface, MEvaluatorClientInterface* aClientInterface)
/**
	Constructor records the supplied plugin and client interfaces.
	
	@param	aPluginInterface	Used to invoke plugins.
	@param	aClientInterface	Used to notify clients when
								an evaluation has completed.
 */
:	CActive(CActive::EPriorityStandard),
	iPluginInterface(aPluginInterface),
	iClientInterface(aClientInterface)
//	,iRpnStack(0)
	{
	CActiveScheduler::Add(this);
	}


void CEvaluator::ConstructL()
/**
	Allocate resources (i.e. RPN stack) used by
	this object throughout its lifetime.
 */
	{
	iRpnStack = new(ELeave) CArrayFixFlat<TIdentityId>(KRPNGranularity);
	}


CEvaluator::~CEvaluator()
/**
	Free any resources (i.e. RPN stack) successfully
	allocated by this object.
 */
	{
	Cancel();
	delete iRpnStack;
	}


// -------- evaluation --------


void CEvaluator::Evaluate(const CAuthExpressionImpl* aExpr)
/**
	Evaluate the current expression.  This finds
	the leftmost leaf node from the supplied expression
	and calls the appropriate plugin.
 */
	{
	__ASSERT_DEBUG(!IsActive(),
					Panic(EBusy));	
	// iterate through left branches until find
	// a leaf node
	
	while (aExpr->Type() == KAnd || aExpr->Type() == KOr)
		{
		aExpr = aExpr->Left();
		}
	
	iCurrentNode = aExpr;
	
	if (aExpr->Type() == CAuthExpressionImpl::EPluginId)
		iPluginInterface->Evaluate(aExpr->PluginId(), iIdentity, aExpr->Type(), iStatus);
	else if (aExpr->Type() == CAuthExpressionImpl::ENull)
		{
		iPluginInterface->Evaluate(0, iIdentity, aExpr->Type(), iStatus);
		}
	else /* aExpr->Type() == CAuthExpression::EPluginType */
		iPluginInterface->Evaluate(aExpr->PluginType(), iIdentity, aExpr->Type(), iStatus);
	SetActive();
	}


void CEvaluator::RunL()
/**
	Implement CActive by recording the identity
	returned by the plugin.  If the individual
	plugin failed then fail the entire evaluation.
 */
	{
	switch (iStatus.Int())
	    {
	    case KErrAuthServPluginNotActive:
	        // don't break
	    case KErrAuthServPluginCancelled:
	        iIdentity = KUnknownIdentity;
	        // don't break
	    case KErrNone:
	        EvaluatedNode(iIdentity);
    		break;
    	default: 
    	    NotifyClientFailed(iStatus.Int());
    	    break;
	    }
	}


void CEvaluator::DoCancel()
/**
	Implement CActive.  This function does not
	currently do anything.
 */
	{
	iPluginInterface->CancelEvaluate();

	NotifyClientFailed(KErrCancel);
	}

void CEvaluator::EvaluatedNode(TIdentityId aIdentity)
/**
	This function is called when the plugin has
	returned an identity.
	
	If it is a root node then complete the client request.

	If it is a left node, then push the identity on the
	RPN stack and evaluate the right node.
	
	If it is a right node then combine it with the stacked
	left result using the parent node's operator.
 */
	{
	CAuthExpressionImpl* parent = iCurrentNode->Parent();
	
	// If parent node complete client request.
	// This path is only used if the expression contains
	// a single leaf node.  If the root node is complex
	// then EvaluateCompound() calls HaveFinalResult().
	if (parent == 0)
		{
		__ASSERT_DEBUG(RpnDepth() == 0, Panic(EENRpnStackNonZero));
		NotifyClientSucceeded(aIdentity);
		}
	// if left node then push identity and evaluate sibling
	else
		{	
		if (! PushIdentity(aIdentity))
			NotifyClientFailed(KErrNoMemory);
		else
			{
			const CAuthExpressionImpl* rhsExpr = parent->Right();
			// if just evaluated RHS then compound result
			if (rhsExpr == iCurrentNode)
				{
				EvaluateCompound(parent);
				}
			else if ( 	(aIdentity == KUnknownIdentity && parent->Type() == KAnd) || 
						(aIdentity != KUnknownIdentity && parent->Type() == KOr)
					)
				{
				if (! PushIdentity(aIdentity))
					NotifyClientFailed(KErrNoMemory);
				else
					EvaluateCompound(parent);
				}
			// can't be short-circuited so evaluate RHS
			else
				{
				Evaluate(rhsExpr);
				}
			}	// else (! PushIdentity(aIdentity))
		}
	}


void CEvaluator::EvaluateCompound(const CAuthExpressionImpl* aParent)
/**
	This function is called by EvaluatedNode when
	both the left and right identities are on the
	RPN stack.
	
	Those two values are replaced with a single value
	from the combining operator.
 */
	{
	__ASSERT_DEBUG(RpnDepth() >= 2, Panic(EECRpnStackTooLow));
	TIdentityId rhs = PopIdentity();
	TIdentityId& lhs = LastIdentity();	// replace this with result
	
	switch (aParent->Type())
		{
	case KAnd:
		// unknown if left and right don't match.
		// These are both unknown for short-circuit.
		if (lhs != rhs)
			lhs = KUnknownIdentity;
		break;
	
	case KOr:
		// only take right value if left unknown.
		// Both same non-unknown for short-circuit.
		if (lhs == KUnknownIdentity)
			lhs = rhs;
		break;
	
	default:
		__ASSERT_DEBUG(EFalse, Panic(EECBadParentType));
		break;
		}
	
	// combine the parent complex node with _its_ sibling
	CAuthExpressionImpl* parent2 = aParent->Parent();

	// if parent2 is the root node, then complete the client request
	if (parent2 == 0)
		{
		__ASSERT_DEBUG(RpnDepth() == 1, Panic(EECRpnStackNotOneAtRoot));
		// reset stack after notifying client because
		// lhs is a reference to the top (only) item.
		NotifyClientSucceeded(lhs);
		}
	
	// if parent is parent2's left node then evaluate parent's right
	// sibling.  At this point parent's result is on the stack.
	else if (parent2->Left() == aParent)
		{
		// (lhs == unknown && type == AND) || (lhs != unknown && type == OR)
		if ((lhs == KUnknownIdentity) == (parent2->Type() == KAnd))
			{
			if (! PushIdentity(lhs))
				NotifyClientFailed(KErrNoMemory);
			else
				EvaluateCompound(parent2);
			}
		
		else
			Evaluate(parent2->Right());
		}
	
	// parent must have been parent2's right child so combine
	// its result with its left sibling.
	else
		{
		__ASSERT_DEBUG(parent2->Right() == aParent, Panic(EECBadRightParent));
		EvaluateCompound(parent2);
		}
	}


// -------- client notification --------


void CEvaluator::NotifyClientSucceeded(TIdentityId aIdentityId)
/**
	Notify the client that the evaluation has completed
	successfully, and free the RPNStack.
	
	@param	aIdentityId		Identified user.  This can be KUnknownIdentity.
 */
	{
	iClientInterface->EvaluationSucceeded(aIdentityId);
	ResetRpnStack();
	}


void CEvaluator::NotifyClientFailed(TInt aReason)
/**
	Notify the client that the evaluation has failed
	and free the RPN stack.
	
	@param	aReason			Symbian OS error code.
 */
	{
	iClientInterface->EvaluationFailed(aReason);
	ResetRpnStack();
	}


// -------- RPN stack --------


TBool CEvaluator::PushIdentity(TIdentityId aIdentity)
/**
	Append the supplied entity to the RPN stack.
	
	@param	aIdentity		Function to append to the RPN stack.
	@return					zero if could not append the item,
							i.e. OOM, non-zero otherwise.
 */
	{
	TRAPD(r, iRpnStack->AppendL(aIdentity));
	return r == KErrNone;
	}


TIdentityId CEvaluator::PopIdentity()
/**
	Remove the last identity from the RPN stack
	and return it.
	
	@return					The identity which was the last
							item on the RPN stack when this
							function was called.
	@pre The RPN stack contains at least one identity.
 */
	{
	TInt lastIndex = iRpnStack->Count() - 1;
	TIdentityId id = iRpnStack->At(lastIndex);
	
	// This function will not leave because the array
	// if not being grown.  It is only trapped to satisfy
	// leavescan.
	
	TRAP_IGNORE(iRpnStack->ResizeL(lastIndex));
	return id;
	}


TIdentityId& CEvaluator::LastIdentity()
/**
	Return a reference to the last item on the
	RPN stack.  This function is defined so
	ReplaceComplexIdentity() can efficiently
	put a result on the RPN stack without reallocating.
	
	@return					Reference to last item on the RPN stack.
	@pre The RPN stack contains at least one identity.
 */
	{
	TInt lastIndex = iRpnStack->Count() - 1;
	return iRpnStack->At(lastIndex);	
	}


void CEvaluator::ResetRpnStack()
/**
	Clear the RPN stack.
 */
	{
	iRpnStack->Reset();
	}


#ifdef _DEBUG


TInt CEvaluator::RpnDepth() const
/**
	Returns the depth of the RPN stack.  This is
	defined for debug builds only to ensure the
	stack depth is within an acceptable range.
 */
	{
	return iRpnStack->Count();
	}


void CEvaluator::Panic(CEvaluator::TPanic aPanic)
/**
	Halt the current thread with the category "AUTHEVAL"
	and the supplied reason.
	
	@param	aPanic			Panic reason.
 */
	{
	_LIT(KPanicCat, "AUTHEVAL");
	User::Panic(KPanicCat, aPanic);
	}


#endif	// #ifdef _DEBUG


