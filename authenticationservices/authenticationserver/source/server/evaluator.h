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
* Evaluator mix-in classes implementation
*
*/


/**
 @file 
 @released
 @internalComponent 
*/
 
#ifndef AUTHSERVER_EVALUATOR_H
#define AUTHSERVER_EVALUATOR_H

#include "authexpression_impl.h"

namespace AuthServer {

class MEvaluatorPluginInterface
/**
	CEvaluator uses this interface to invoke a plugin,
	which can be specified by type or ID.
	
	@see CEvaluator
 */
	{
public:
	/**
		Invoke the specified plugin and record the identity
		which was returned.
		
		@param	aPluginId		Plugin to use.
		@param	aIdentityId		On successful completion this
								object is set to the returned
								identity.
		@param	aType			The type of plugin represented 
								by this instance.						
		@param	aStatus			On completion (successful or
								otherwise) this object is
								completed with the error code.
	 */
	virtual void Evaluate(TPluginId aPluginId, TIdentityId& aIdentityId, 
						  CAuthExpressionImpl::TType aType, TRequestStatus& aStatus) = 0;

	/**
	    Cancel the plugin currently being invoked.
	 **/
	virtual void CancelEvaluate() = 0;
	
	/**
		Invoke a plugin of the requested type and record
		the identity which was returned.
		
		@param	aPluginType		Type of plugin to use.  The
								implementation maps this to a
								single, specific plugin.
		@param	aIdentityId		On successful completion this
								object is set to the returned
								identity.
		@param	aType			The type of plugin represented 
								by this instance.						
		@param	aStatus			On completion (successful or
								otherwise) this object is
								completed with the error code.
	 */
	virtual void Evaluate(TAuthPluginType aPluginType, TIdentityId& aIdentityId, 
						  CAuthExpressionImpl::TType aType, TRequestStatus& aStatus) = 0;
	};

class MEvaluatorClientInterface
/**
	CEvaluator uses this interface to notify the server
	when an evaluation has finished, successfully or otherwise.
	
	The server implements this interface to notify
	the client that the authentication request has
	completed.
 */
	{
public:
	/**
		This function is called by the evaluator when it
		has successfully parsed and evaluated an expression.
		
		In this context, successful means there were no
		errors in running the plugins, or otherwise in
		evaluating the expression, such as OOM.  It does
		not mean that a specific user was identified -
		aIdentity can still be CIdentity::KUnknown.
		
		@param	aIdentityId		Identified user.  This can
								be CIdentity::KUnknown.
		@see EvaluationFailed
	 */
	virtual void EvaluationSucceeded(TIdentityId aIdentityId) = 0;
	/**
		This function is called by the evaluator when
		it has failed to evaluate an expression.  Reasons
		for failure include failing to run a plugin;
		the user cancelling a plugin; OOM; and others.
		
		Failure in this sense does not means the expression
		was evaluated but no specific user was identified.
		That case is handled by EvaluationSucceeded.
		
		@param	aReason		Symbian OS error code.
		
		@see EvaluationSucceeded
	 */
	virtual void EvaluationFailed(TInt aReason) = 0;
	};

class CEvaluator : public CActive
/**
	Expression evaluator.  This asynchronously
	evaluates the supplied expression by calling
	the referenced plugins.
 */
	{
public:
	static CEvaluator* NewL(MEvaluatorPluginInterface* aPluginInterface, MEvaluatorClientInterface* aClientInterface);
	virtual ~CEvaluator();

	void Evaluate(const CAuthExpressionImpl* aExpr);
	
private:
	CEvaluator(MEvaluatorPluginInterface* aPluginInterface, MEvaluatorClientInterface* aClientInterface);
	void ConstructL();
	
	void EvaluatedNode(TIdentityId aIdentity);
	void EvaluateCompound(const CAuthExpressionImpl* aParent);
	void ReplaceComplexIdentity(TIdentityId aIdentity);
	
	// complete client request
	void NotifyClientSucceeded(TIdentityId aIdentity);
	void NotifyClientFailed(TInt aReason);
	
	// manipulate RPN stack
	TBool PushIdentity(TIdentityId aIdentity);
	TIdentityId PopIdentity();
	TIdentityId& LastIdentity();
	void ResetRpnStack();
	// implement CActive
	virtual void RunL();
	virtual void DoCancel();
	
#ifdef _DEBUG
	enum TPanic
		{
		EENRpnStackNonZero = 0x10,
		EECRpnStackTooLow = 0x20, EECBadParentType, EECRpnStackNotOneAtRoot, EECBadRightParent,
		EBusy = 0x30,
		};
	static void Panic(TPanic aPanic);
	TInt RpnDepth() const;
#endif
	
private:
	/** The evaluator uses this to invoke plugins. */
	MEvaluatorPluginInterface*const iPluginInterface;
	/**
		The evaluator uses this to notify the server
		when an evaluation has completed, successfully
		or otherwise.
	 */
	MEvaluatorClientInterface*const iClientInterface;
	
	/** Constant value defined for short name readability only. */
	static const CAuthExpressionImpl::TType KAnd;
	/** Constant value defined for short name readability only. */
	static const CAuthExpressionImpl::TType KOr;

	/** The expression which is currently being evaluated. */
	const CAuthExpressionImpl* iCurrentNode;
	
	/** The plugin sets this value to the returned identity. */
	TIdentityId iIdentity;
	
	/**
		Granularity of RPN stack.  This value should be large
		to parse a reasonable expression without having to
		reallocate, and without wasting too much memory.
	 */
	static const TInt KRPNGranularity;
	/**
		Intermediate results.  (A CArrayFixFlat is used here
		instead of an RArray because, although it uses more
		memory, it can be resized when items are popped.)
	 */
	CArrayFixFlat<TIdentityId>* iRpnStack;
	};

}	// namespace AuthServer

#endif	// #ifndef AUTHSERVER_EVALUATOR_H


