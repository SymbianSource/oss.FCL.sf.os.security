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
* This file contains functions which are used to
* test evaluating authentication expressions.
*
*/


#include "tauthcliservstep.h"

using namespace AuthServer;

typedef TTestPluginInterface::TCallEntry TCE;

#define elemCount(___x)		(sizeof(___x) / sizeof(___x[0]))

static TAuthExpressionWrapper BuildLeftAnd(TInt aRemainingLevels);
static TAuthExpressionWrapper BuildRightAnd(TInt aRemainingLevels);
static TAuthExpressionWrapper BuildBalancedAnd(TInt aRemainingLevels);
static TAuthExpressionWrapper BuildFailedAnd(TInt aRemainingLevels);
static TAuthExpressionWrapper BuildSuccessfulOr(TInt aRemainingLevels);


// -------- CTStepActSch --------


// -------- CTStepAuthExprEval --------


void TTestPluginInterface::Evaluate(TPluginId aPluginId, TIdentityId& aIdentity,
		   CAuthExpressionImpl::TType /*aType*/, TRequestStatus& aStatus)
/**
	Implement MEvaluatorPluginInterface by completing
	the request with an identity equal to the plugin id.
 */
	{
	const TCallEntry ce(aPluginId);
	TInt r = iCallLog.Append(ce);
	
	// this can be KErrNoMemory in OOM tests
	if (r == KErrNone)
		{
		if (aPluginId == KTestPluginUnknown)
			aIdentity = KUnknownIdentity;
		else
			aIdentity = static_cast<TIdentityId>(aPluginId);
		}
		
	aStatus = KRequestPending;
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, r);
	}


void TTestPluginInterface::Evaluate(TAuthPluginType aPluginType, TIdentityId& aIdentity, 
									CAuthExpressionImpl::TType /*aType*/, TRequestStatus& aStatus)
/**
	Implement MEvaluatorPluginInterface by completing
	the request with an identity equal to the plugin type.
 */
	{
	const TCallEntry ce(aPluginType);
	TInt r = iCallLog.Append(ce);
	
	// this can be KerrNoMemory in OOM tests
	if (r == KErrNone)
		aIdentity = static_cast<TIdentityId>(aPluginType);
	
	aStatus = KRequestPending;
	TRequestStatus* rs = &aStatus;
	User::RequestComplete(rs, KErrNone);
	}


bool TTestPluginInterface::TCallEntry::operator==(const TTestPluginInterface::TCallEntry& aRhs) const
	{
	if (iCallType != aRhs.iCallType)
		return false;
	
	if (iCallType == CAuthExpressionImpl::EPluginId)
		return iPluginId == aRhs.iPluginId;
	else
		return iPluginType == aRhs.iPluginType;
	}


void TTestClientInterface::EvaluationSucceeded(TIdentityId aIdentityId)
/**
	Implement MEvaluatorClientInterface by recording
	that the evaluation succeeded, and the resulting identity.
 */
	{
	iMode = ESucceeded;
	iIdentityId = aIdentityId;

	CActiveScheduler::Stop();
	}


void TTestClientInterface::EvaluationFailed(TInt aReason)
/**
	Implement MEvaluatorClientInterface by recording
	that the evaluation failed, and the failure reason.
 */
	{
	iMode = EFailed;
	iReason = aReason;
	
	CActiveScheduler::Stop();
	}


CLaunchEval* CLaunchEval::NewL()
/**
	Factory function allocates new instance of CLaunchEval.
	
	@return					New instance of CLaunchEval.
 */
	{
	CLaunchEval* self = new(ELeave) CLaunchEval();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


CLaunchEval::CLaunchEval()
/**
	Set timer priority and add self to active scheduler.
 */
:	CActive(CActive::EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}


void CLaunchEval::ConstructL()
/**
	Allocate evaluator and initialize superclass timer.
 */
	{
//	CTimer::ConstructL();
	iEval = CEvaluator::NewL(&iPluginInterface, &iClientInterface);
	}


CLaunchEval::~CLaunchEval()
/**
	Deletes evaluator which was allocated for this object.
 */
	{
	ResetInterfaces();
	delete iEval;
	}


void CLaunchEval::ResetInterfaces()
/**
	Free resources used by plugin and client interfaces.
 */
	{
	iPluginInterface.iCallLog.Reset();
	iClientInterface.iMode = TTestClientInterface::ENone;
	}


void CLaunchEval::Evaluate(const CAuthExpression* aExpr)
/**
	Queue this timer object and start the active
	scheduler.  This function returns when the evaluation
	has completed.
	
	This object's client and plugin interfaces are reset
	before the expression is evaluated, so they can be
	tested by the function which calls this.
	
	@param	aExpr			Expression to evaluate.
 */
	{
	ResetInterfaces();
	iExpr = aExpr;			// store so can see in RunL
	
	// signal this object.  This ensures there
	// is a pending active object before the scheduler
	// is started.
	iStatus = KRequestPending;
	TRequestStatus* rs = &iStatus;
	User::RequestComplete(rs, KErrNone);
	SetActive();
	
	// block until the evaluation has completed.
	CActiveScheduler::Start();
	}


void CLaunchEval::RunL()
/**
	Implement CActive by launching the evaluation.
	At this point the active scheduler should have
	been started.
 */
	{
	iEval->Evaluate(static_cast<const CAuthExpressionImpl*>(iExpr));
	}


void CLaunchEval::DoCancel()
/**
	Implement CActive by cancelling the evaluation
	which is currently in progress.
	
	Not yet implemented.
 */
	{
	// empty.
	}


CTStepAuthExprEval::CTStepAuthExprEval()
/**
	Record this test step's name.
 */
	{
	SetTestStepName(KTStepAuthExprTypePncBadRight);
	}


TVerdict CTStepAuthExprEval::doTestStepL()
	{
 	CActiveScheduler::Install(iActSchd);
 	User::SetJustInTime(ETrue);
 	
 	__UHEAP_MARK;
	TestEvalCreateL();
	TestEvalSimpleL();
	TestEvalAndL();
	TestEvalOrL();
	TestRPNReallocL();
	__UHEAP_MARKEND;
	
	return EPass;
	}


void CTStepAuthExprEval::TestEvalCreateL()
/**
	Test allocating and deleting an evaluator,
	without using it for anything.
 */
	{
	__UHEAP_MARK;
	
	TTestClientInterface tci;
	TTestPluginInterface tpi;
	
	CEvaluator* ev = CEvaluator::NewL(&tpi, &tci);
	delete ev;
	
	__UHEAP_MARKEND;
	}


void CTStepAuthExprEval::TestEvalSimpleL()
/**
	Test evaluating a simple plugin id, and
	evaluating a simple plugin type.
 */
	{
	__UHEAP_MARK;
	
	CLaunchEval* le = CLaunchEval::NewL();
	CleanupStack::PushL(le);

	// simple plugin id	
	CAuthExpression* aeId = AuthExpr(KTestPluginId0);
	User::LeaveIfNull(aeId);
	le->Evaluate(aeId);
	delete aeId;
	
	const TCE aceI0[] = {TCE(KTestPluginId0)};
	TestEvalResultL(le, KTestPluginId0, aceI0, elemCount(aceI0));
	
	// simple plugin type
	CAuthExpression* aeType = AuthExpr(EAuthBiometric);
	User::LeaveIfNull(aeType);
	le->Evaluate(aeType);
	delete aeType;
	
	const TCE aceTB[] = {TCE(EAuthBiometric)};
	TestEvalResultL(le, EAuthBiometric, aceTB, elemCount(aceTB));
	
	CleanupStack::PopAndDestroy(le);
	
	__UHEAP_MARKEND;
	}


void CTStepAuthExprEval::TestEvalAndL()
/**
	Test evaluating simple AND expressions.
 */
	{
	__UHEAP_MARK;
	
	CLaunchEval* le = CLaunchEval::NewL();
	CleanupStack::PushL(le);

	// U & U = U (sc)
	CAuthExpression* aeUU = AuthAnd(AuthExpr(KTestPluginUnknown), AuthExpr(KTestPluginUnknown));
	User::LeaveIfNull(aeUU);
	le->Evaluate(aeUU);
	delete aeUU;

	const TCE aceUU[] = {TCE(KTestPluginUnknown)};
	TestEvalResultL(le, KUnknownIdentity, aceUU, elemCount(aceUU));

	// U & I1 = U (sc)
	CAuthExpression* aeUI1 = AuthAnd(AuthExpr(KTestPluginUnknown), AuthExpr(KTestPluginId1));
	User::LeaveIfNull(aeUI1);
	le->Evaluate(aeUI1);
	delete aeUI1;
	
	const TCE aceUI1[] = {TCE(KTestPluginUnknown)};
	TestEvalResultL(le, KUnknownIdentity, aceUI1, elemCount(aceUI1));
	
	// I1 & U = U
	CAuthExpression* aeI1U = AuthAnd(AuthExpr(KTestPluginId1), AuthExpr(KTestPluginUnknown));
	User::LeaveIfNull(aeI1U);
	le->Evaluate(aeI1U);
	delete aeI1U;

	const TCE aceI1U[] = {TCE(KTestPluginId1), TCE(KTestPluginUnknown)};
	TestEvalResultL(le, KUnknownIdentity, aceI1U, elemCount(aceI1U));
	
	// I1 & I1 = I1
	CAuthExpression* aeI1I1 = AuthAnd(AuthExpr(KTestPluginId1), AuthExpr(KTestPluginId1));
	User::LeaveIfNull(aeI1I1);
	le->Evaluate(aeI1I1);
	delete aeI1I1;

	const TCE aceI1I1[] = {TCE(KTestPluginId1), TCE(KTestPluginId1)};
	TestEvalResultL(le, KTestPluginId1, aceI1I1, elemCount(aceI1I1));
	
	// I1 & I2 = U
	CAuthExpression* aeI1I2 = AuthAnd(AuthExpr(KTestPluginId1), AuthExpr(KTestPluginId2));
	User::LeaveIfNull(aeI1I2);
	le->Evaluate(aeI1I2);
	delete aeI1I2;
	
	const TCE aceI1I2[] = {TCE(KTestPluginId1), TCE(KTestPluginId2)};
	TestEvalResultL(le, KUnknownIdentity, aceI1I2, elemCount(aceI1I2));
	
	CleanupStack::PopAndDestroy(le);
	
	__UHEAP_MARKEND;
	}


void CTStepAuthExprEval::TestEvalOrL()
/**
	Test evaluating simple OR expressions.
 */
	{
	__UHEAP_MARK;
	
	CLaunchEval* le = CLaunchEval::NewL();
	CleanupStack::PushL(le);

	// U | U = U
	CAuthExpression* aeUU = AuthOr(AuthExpr(KTestPluginUnknown), AuthExpr(KTestPluginUnknown));
	User::LeaveIfNull(aeUU);
	le->Evaluate(aeUU);
	delete aeUU;
	
	const TCE aceUU[] = {TCE(KTestPluginUnknown), TCE(KTestPluginUnknown)};
	TestEvalResultL(le, KUnknownIdentity, aceUU, elemCount(aceUU));
	
	// U | I1 = I1
	CAuthExpression* aeUI1 = AuthOr(AuthExpr(KTestPluginUnknown), AuthExpr(KTestPluginId1));
	User::LeaveIfNull(aeUI1);
	le->Evaluate(aeUI1);
	delete aeUI1;
	
	const TCE aceUI1[] = {TCE(KTestPluginUnknown), TCE(KTestPluginId1)};
	TestEvalResultL(le, KTestPluginId1, aceUI1, elemCount(aceUI1));
	
	// I1 | U = I1 (sc)
	CAuthExpression* aeI1U = AuthOr(AuthExpr(KTestPluginId1), AuthExpr(KTestPluginUnknown));
	User::LeaveIfNull(aeI1U);
	le->Evaluate(aeI1U);
	delete aeI1U;
	
	const TCE aceI1U[] = {TCE(KTestPluginId1)};
	TestEvalResultL(le, KTestPluginId1, aceI1U, elemCount(aceI1U));
	
	// I1 | I1 = I1 (sc)
	CAuthExpression* aeI1I1 = AuthOr(AuthExpr(KTestPluginId1), AuthExpr(KTestPluginId1));
	User::LeaveIfNull(aeI1I1);
	le->Evaluate(aeI1I1);
	delete aeI1I1;
	
	const TCE aceI1I1[] = {TCE(KTestPluginId1)};
	TestEvalResultL(le, KTestPluginId1, aceI1I1, elemCount(aceI1I1));
	
	// I1 | I2 = I1 (sc)
	CAuthExpression* aeI1I2 = AuthOr(AuthExpr(KTestPluginId1), AuthExpr(KTestPluginId2));
	User::LeaveIfNull(aeI1I2);
	le->Evaluate(aeI1I2);
	delete aeI1I2;
	
	const TCE aceI1I2[] = {TCE(KTestPluginId1)};
	TestEvalResultL(le, KTestPluginId1, aceI1I2, elemCount(aceI1I2));
	
	CleanupStack::PopAndDestroy(le);
	
	__UHEAP_MARKEND;
	}


void CTStepAuthExprEval::TestEvalResultL(
	CLaunchEval* aLaunchEval, TIdentityId aIdentityId,
	const TTestPluginInterface::TCallEntry* aExpEntries, TInt aEntryCount)
/**
	Test the evaluation produced the expected result, and
	that the expected plugins were called in the right order.
 */
	{
	const TTestClientInterface& cli = aLaunchEval->iClientInterface;
	TESTL(cli.iMode == TTestClientInterface::ESucceeded);
	TESTL(cli.iIdentityId == aIdentityId);
	
	const RArray<TCE>& log = aLaunchEval->iPluginInterface.iCallLog;
	
	TESTL(log.Count() == aEntryCount);
	for (TInt i = 0; i < aEntryCount; ++i)
		{
		TESTL(log[i] == aExpEntries[i]);
		}
	}


static TAuthExpressionWrapper BuildLeftAnd(TInt aRemainingLevels)
/**
	Build an expression where the left side is an
	AND expression and the right side is a plugin ID.
	
	@param	aRemainingLevels The number of layers to build
							below this layer.  If
							aRemainingLevels == 0 this function
							returns a simple plugin ID expression.
 */
	{
	return (aRemainingLevels == 0)
		?	AuthExpr(KTestPluginId1)
		:	AuthAnd(BuildLeftAnd(aRemainingLevels - 1), AuthExpr(KTestPluginId1));
	}


static TAuthExpressionWrapper BuildRightAnd(TInt aRemainingLevels)
/**
	Build an expression where the left side is a
	plugin ID and the right side is an AND expression.

	@param	aRemainingLevels The number of layers to build
							below this layer.  If
							aRemainingLevels == 0 this function
							returns a simple plugin ID expression.
 */
	{
	return (aRemainingLevels == 0)
		?	AuthExpr(KTestPluginId1)
		:	AuthAnd(AuthExpr(KTestPluginId1), BuildRightAnd(aRemainingLevels - 1));
	}


static TAuthExpressionWrapper BuildBalancedAnd(TInt aRemainingLevels)
/**
	Build an expression where both the left and right side
	have the same depth, aRemainingLevels - 1.
	
	@param	aRemainingLevels The number of layers to build
							below this layer.  If
							aRemainingLevels == 0 this function
							returns a simple plugin ID expression.
 */
	{
	return (aRemainingLevels == 0)
		?	AuthExpr(KTestPluginId1)
		:	AuthAnd(
				BuildBalancedAnd(aRemainingLevels - 1),
				BuildBalancedAnd(aRemainingLevels - 1));
	}


static TAuthExpressionWrapper BuildFailedAnd(TInt aRemainingLevels)
/**
	This function creates an expression where the left node
	is a simple plugin ID expression and the right node is
	built recursively with this function.  The final AND node
	has a left unknown plugin ID.
	
	This causes an unknown plugin ID to be automatically pushed
	onto the RPN stack as a right value before the compounder is used.
	
	@param	aRemainingLevels Number of levels to generate after this.
							If aRemainingLevels == 1 this function
							creates an AND node where the left node
							is unknown.  Otherwise it generates an
							AND node where the left node is a known
							plugin ID and the right node is generated
							recursively.
 */
	{
	return (aRemainingLevels == 1)
		?	AuthAnd(AuthExpr(KTestPluginUnknown), AuthExpr(KTestPluginId1))
		:	AuthAnd(AuthExpr(KTestPluginId1), BuildFailedAnd(aRemainingLevels - 1));
	}


static TAuthExpressionWrapper BuildSuccessfulOr(TInt aRemainingLevels)
/**
	This function creates an AND node where the left node
	is a known plugin ID, and the right right node is generated
	recursively.  This creates a right-descent list, but the
	penultimate node is an OR expression whose left node is a
	known plugin ID.
	
	This puts a series of known plugin IDs on the RPN stack from
	the left nodes of the AND nodes.  When the OR node is evaluated
	the left node is known, and so automatically put on the
	RPN stack.
	
	This means that an OR right node is automatically put on the
	RPN stack at a known point, which is used to stress test failing
	to append an OR right expression in OOM.

	@param	aRemainingLevels Number of levels to generate after this.
							If aRemainingLevels == 1 this function
							generates an OR node.  Otherwise it creates
							and AND node as described above.
 */
	{
	return (aRemainingLevels == 1)
		?	AuthOr(AuthExpr(KTestPluginId1), AuthExpr(KTestPluginId1))
		:	AuthAnd(AuthExpr(KTestPluginId1), BuildSuccessfulOr(aRemainingLevels - 1));
	}


void CTStepAuthExprEval::TestRPNReallocL()
/**
	Create a deeply nested expression which is
	deep enough that the evaluator has to reallocate
	its RPN stack, and checks the evaluation fails
	gracefully in OOM.
 */
	{
	__UHEAP_MARK;
	
	RunOomTestsL(BuildLeftAnd, KTestPluginId1, 0);
	RunOomTestsL(BuildRightAnd, KTestPluginId1, 0);
	RunOomTestsL(BuildBalancedAnd, KTestPluginId1, 0);
	RunOomTestsL(BuildFailedAnd, KUnknownIdentity, 1);
	RunOomTestsL(BuildSuccessfulOr, KTestPluginId1, 1);
	
	__UHEAP_MARKEND;
	}

	
void CTStepAuthExprEval::RunOomTestsL(
	TAuthExpressionWrapper (*aAllocator)(TInt),
	TIdentityId aExpectedIdentity, TInt aInitDepth)
/**
	Attempt to evaluate the supplied expresision in OOM.
	
	Running in OOM will both fail the evaluation, when the
	plugin interface attempts to append to the call log, and
	when the evaluator attempts to extend the RPN stack.
	
	OOM can only be tested in debug builds.  In release builds,
	this function evaluates the expression at each depth and
	tests the evaluator produces the correct result.
	
	@param	aAllocator		Function which allocates the expression.
	@param	aExpectedIdentity Identity which should be returned on
							successful evaluation.
	@param	aInitDepth		Initial depth.
 */
	{
	CLaunchEval* le = CLaunchEval::NewL();
	User::LeaveIfNull(le);
	CleanupStack::PushL(le);
	
	const volatile TTestClientInterface& cli = le->iClientInterface;
	
	// depth starts at zero because, even though RPN stack
	// is not used, the evaluator will attempt to grow its
	// call log, and so fail the evaluation.  (This test is
	// therefore also used to test failed plugin evaluations.)
	
	// max depth is 13 because CStepControl::StartL creates
	// a worker thread with a 1MB maximum heap.  The
	// number of allocated node cells for a balanced tree
	// is 2^(depth+1) - 1.  When depth==13, there are
	// 16383 cells using 327,672 bytes excluding cell headers.
	// Allocation fails for depth == 14.
	
	const TInt KMaxDepth = 13;
	for (TInt depth = aInitDepth; depth <= KMaxDepth; ++depth)
		{
		CAuthExpression* ae = aAllocator(depth);
		User::LeaveIfNull(ae);
		CleanupStack::PushL(ae);
		
		// OOM testing only available in debug builds
#ifndef _DEBUG
		le->Evaluate(ae);
		TESTL(cli.iMode == TTestClientInterface::ESucceeded);
		TESTL(cli.iIdentityId == aExpectedIdentity);
#else
		TInt i = 0;
		do
			{
			// Ideally, the heap would be marked before and
			// after the evaluation.  However, CEvaluator uses
			// an CArrayFixFlat<TIdentityId> to store the RPN stack.
			// When the first item is inserted, it allocates a
			// CBufBase object to hold the data.  This object
			// is reset but not deleted when the RPN stack is
			// reset, so there will be a heap imbalance of one
			// if anything was added to the RPN stack, even though
			// the stack is reset.
			
			TInt preSize;
			TInt preCount = User::AllocSize(preSize);
//			__UHEAP_MARK;
			
			__UHEAP_SETFAIL(RAllocator::EDeterministic, i);
			le->Evaluate(ae);
			__UHEAP_RESET;
			
			TESTL(	cli.iMode == TTestClientInterface::EFailed
				||	cli.iMode == TTestClientInterface::ESucceeded);
			
			if (cli.iMode == TTestClientInterface::EFailed)
				{
				TESTL(cli.iReason == KErrNoMemory);
				}
			else
				{
				TESTL(cli.iIdentityId == aExpectedIdentity);
				}
			
			// clear call log so heap checking will work
			le->iPluginInterface.iCallLog.Reset();
			++i;

			TInt postSize;
			TInt postCount = User::AllocSize(postSize);			
			TESTL(postCount == preCount || postCount == preCount + 1);
//			__UHEAP_MARKEND;
			} while (cli.iMode != TTestClientInterface::ESucceeded);
		
		// test evaluation still succeeds and failed allocation
		// was not ignored
		TInt limit = 2 * i;
		while (i++ < limit)
			{
			__UHEAP_SETFAIL(RAllocator::EDeterministic, i++);
			le->Evaluate(ae);
			__UHEAP_RESET;
			
			TESTL(cli.iMode == TTestClientInterface::ESucceeded);
			TESTL(cli.iIdentityId == aExpectedIdentity);
			}

		// clear plugin call log to reset mem usage for next iteration.
		le->iPluginInterface.iCallLog.Reset();
#endif	// #else #ifndef _DEBUG
		CleanupStack::PopAndDestroy(ae);
		}
	
	CleanupStack::PopAndDestroy(le);
	}

