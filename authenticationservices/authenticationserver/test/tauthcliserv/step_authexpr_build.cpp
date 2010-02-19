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


#include <s32mem.h>
#include "tauthcliservstep.h"
#include <scs/scsclient.h>
using namespace AuthServer;

inline void CorruptTypeField(const CAuthExpressionImpl* aExpr)
/**
	Corrupt the type field of the supplied
	expression so the invariant-checking code fails.
	
	@param	aExpr			Expression to corrupt.  This
							is const, even though it is
							modified, because Left and
							Right return pointers to const
							expressions.
 */
	{
	// 4 == _FOFF(CAuthExpression, iType), but iType is private
	// sizeof(CAuthExpression::iType) == 8
	*(((TUint32*)aExpr) + 2) = ~0;
	}


CTStepAuthExprBuild::CTStepAuthExprBuild()
/**
	Record this test step's name.
 */
	{
	SetTestStepName(KTStepAuthExprBuild);
	}


TVerdict CTStepAuthExprBuild::doTestStepL()
/**
	Implement CTestStep by creating simple and
	complex expression types.  In debug mode this
	function also ensures the operations fail
	cleanly in OOM.
 */
	{
	RunTestsL();		// create all expression types
	TestFailedCombsL();
	TestCorruptPersistL();
	
	// create all expression types in OOM
#ifdef _DEBUG
	TInt i = 1;
	TBool failed = EFalse;
	
	do
		{
		__UHEAP_SETFAIL(RAllocator::EDeterministic, i);
		TRAPD(r, RunTestsL());
		TESTL(r == KErrNone || r == KErrNoMemory);
		__UHEAP_RESET;
		
		failed = (r == KErrNoMemory);
		++i;
		} while (failed);
	
	// i is now one greater than the last alloc count.
	// run the tests again, to ensure there really was
	// enough memory, and this wasn't just an error case
	// that wasn't handled properly.
	
	TInt lim = i * 2;
	while (i++ < lim)
		{
		__UHEAP_SETFAIL(RAllocator::EDeterministic, i);
		RunTestsL();
		__UHEAP_RESET;
		}
#endif	
	
	return EPass;
	}


void CTStepAuthExprBuild::RunTestsL()
/**
	Helper function for doTestStepL.  This generates
	authentication expressions.
	
	1. creating simple type and plugin id nodes.
	2. creating simple AND and OR combinations
	3. creating complex (multi-level) AND and OR combinations.
 */
	{
	TestSimpleExprL();
	TestSimpleExprCombsConsL();
	TestComplexExprCombsConsL();
	}


void CTStepAuthExprBuild::TestExprLD(
	void (CTStepAuthExprBuild::*aTestFuncL)(CAuthExpression*),
	CAuthExpression* aExpr)
/**
	Run the supplied test function on the supplied expression.
	This function only exists to place the expression on the
	cleanup stand and delete it afterwards.
	
	This function leaves with KErrNoMemory if the supplied
	expression is NULL.
 */
	{
	User::LeaveIfNull(aExpr);
	
	CleanupStack::PushL(aExpr);
	(this->*aTestFuncL)(aExpr);
	
	// test externalization and internalization
	// (this can fail because of OOM.)
	TSizeStream ss;
	RWriteStream ws(&ss);
	aExpr->ExternalizeL(ws);
	
	HBufC8* buf = HBufC8::NewLC(ss.Size());
	TPtr8 bufPtr(buf->Des());	
	RDesWriteStream dws(bufPtr);
	aExpr->ExternalizeL(dws);
	dws.CommitL();
	
	RDesReadStream drs(*buf);
	CAuthExpression* aeRestore = CAuthExpressionImpl::NewL(drs);
	CleanupStack::PushL(aeRestore);
	
	(this->*aTestFuncL)(aExpr);
	
	CleanupStack::PopAndDestroy(3, aExpr);
	}


void CTStepAuthExprBuild::TestSimpleExprL()
/**
	Test constructing leaf node expressions from
	a plugin ID and a plugin type.
 */
	{
	__UHEAP_MARK;
	
	TestExprLD(&CTStepAuthExprBuild::TestPluginIdL, AuthExpr(KTestPluginId0));
	TestExprLD(&CTStepAuthExprBuild::TestPluginTypeL, AuthExpr(EAuthKnowledge));

	__UHEAP_MARKEND;
	}


void CTStepAuthExprBuild::TestPluginIdL(CAuthExpression* aExpr)
/**
	Tests the supplied expression is a simple expression
	for plugin ID KTestPluginId0.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KPluginId);
	TESTL(impl->PluginId() == KTestPluginId0);
	}


void CTStepAuthExprBuild::TestPluginTypeL(CAuthExpression* aExpr)
/**
	Tests the supplied expression is a simple expression
	for plugin type EAuthKnowledge.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KPluginType);
	TESTL(impl->PluginType() == EAuthKnowledge);
	}


void CTStepAuthExprBuild::TestSimpleExprCombsConsL()
/**
	Tests combining two simple types (plugin type or plugin id)
	with the AuthAnd and AuthOr functions, and with the overloaded
	binary AND (&) and OR (|) operators.
 */
	{
	__UHEAP_MARK;
	
	// type AND type
	OpTestExprLD(&CTStepAuthExprBuild::TestTypeAndTypeL, AuthExpr(EAuthKnowledge) & AuthExpr(EAuthBiometric));
	TestExprLD(&CTStepAuthExprBuild::TestTypeAndTypeL, AuthAnd(AuthExpr(EAuthKnowledge), AuthExpr(EAuthBiometric)));
	
	// id OR id
	OpTestExprLD(&CTStepAuthExprBuild::TestTypeOrTypeL, AuthExpr(EAuthKnowledge) | AuthExpr(EAuthBiometric));
	TestExprLD(&CTStepAuthExprBuild::TestTypeOrTypeL, AuthOr(AuthExpr(EAuthKnowledge), AuthExpr(EAuthBiometric)));
	
	// ID AND type
	OpTestExprLD(&CTStepAuthExprBuild::TestIdAndTypeL, AuthExpr(KTestPluginId0) & AuthExpr(EAuthBiometric));
	TestExprLD(&CTStepAuthExprBuild::TestIdAndTypeL, AuthAnd(AuthExpr(KTestPluginId0), AuthExpr(EAuthBiometric)));
	
	// type AND ID
	OpTestExprLD(&CTStepAuthExprBuild::TestTypeAndIdL, AuthExpr(EAuthBiometric) & AuthExpr(KTestPluginId0));
	TestExprLD(&CTStepAuthExprBuild::TestTypeAndIdL, AuthAnd(AuthExpr(EAuthBiometric), AuthExpr(KTestPluginId0)));
	
	// id AND id
	OpTestExprLD(&CTStepAuthExprBuild::TestIdAndIdL, AuthExpr(KTestPluginId0) & AuthExpr(KTestPluginId1));
	TestExprLD(&CTStepAuthExprBuild::TestIdAndIdL, AuthAnd(AuthExpr(KTestPluginId0), AuthExpr(KTestPluginId1)));
	
	__UHEAP_MARKEND;
	}


void CTStepAuthExprBuild::TestTypeAndTypeL(CAuthExpression* aExpr)
/**
	Test the supplied expression says know & bio.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KAnd);
	TESTL(impl->Left()->Type() == KPluginType);
	TESTL(impl->Left()->PluginType() == EAuthKnowledge);
	TESTL(impl->Right()->Type() == KPluginType);
	TESTL(impl->Right()->PluginType() == EAuthBiometric);
	}


void CTStepAuthExprBuild::TestTypeOrTypeL(CAuthExpression* aExpr)
/**
	Test the supplied expression says know | bio.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KOr);
	TESTL(impl->Left()->Type() == KPluginType);
	TESTL(impl->Left()->PluginType() == EAuthKnowledge);
	TESTL(impl->Right()->Type() == KPluginType);
	TESTL(impl->Right()->PluginType() == EAuthBiometric);
	}


void CTStepAuthExprBuild::TestIdAndTypeL(CAuthExpression* aExpr)
/**
	Test the supplied expression says id0 & bio.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KAnd);
	TESTL(impl->Left()->Type() == KPluginId);
	TESTL(impl->Left()->PluginId() == KTestPluginId0);
	TESTL(impl->Right()->Type() == KPluginType);
	TESTL(impl->Right()->PluginType() == EAuthBiometric);
	}


void CTStepAuthExprBuild::TestTypeAndIdL(CAuthExpression* aExpr)
/**
	Test the supplied expression says bio & id0.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KAnd);
	TESTL(impl->Left()->Type() == KPluginType);
	TESTL(impl->Left()->PluginType() == EAuthBiometric);
	TESTL(impl->Right()->Type() == KPluginId);
	TESTL(impl->Right()->PluginId() == KTestPluginId0);
	}


void CTStepAuthExprBuild::TestIdAndIdL(CAuthExpression* aExpr)
/**
	Test the supplied expression says id0 & id1.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KAnd);
	TESTL(impl->Left()->Type() == KPluginId);
	TESTL(impl->Left()->PluginId() == KTestPluginId0);
	TESTL(impl->Right()->Type() == KPluginId);
	TESTL(impl->Right()->PluginId() == KTestPluginId1);
	}


void CTStepAuthExprBuild::TestComplexExprCombsConsL()
/**
	Test complex AND and OR combinations.
	For the overloaded operators, this additionally checks
	that precedence works as expected.
 */
	{
	__UHEAP_MARK;
	
	// complex combinations - ensure precedence works
	
	// id AND id AND id == (id AND id) AND id
	OpTestExprLD(&CTStepAuthExprBuild::TestAndAndL, AuthExpr(KTestPluginId0) & AuthExpr(KTestPluginId1) & AuthExpr(KTestPluginId2));
	TestExprLD(&CTStepAuthExprBuild::TestAndAndL, AuthAnd(AuthAnd(AuthExpr(KTestPluginId0), AuthExpr(KTestPluginId1)), AuthExpr(KTestPluginId2)));
	
	// id AND id AND id == (id AND id) OR id
	OpTestExprLD(&CTStepAuthExprBuild::TestAndOrL, AuthExpr(KTestPluginId0) & AuthExpr(KTestPluginId1) | AuthExpr(KTestPluginId2));
	TestExprLD(&CTStepAuthExprBuild::TestAndOrL, AuthOr(AuthAnd(AuthExpr(KTestPluginId0), AuthExpr(KTestPluginId1)), AuthExpr(KTestPluginId2)));
	
	// id OR id AND id == id OR (id AND id)
	OpTestExprLD(&CTStepAuthExprBuild::TestOrAndL, AuthExpr(KTestPluginId0) | AuthExpr(KTestPluginId1) & AuthExpr(KTestPluginId2));
	TestExprLD(&CTStepAuthExprBuild::TestOrAndL, AuthOr(AuthExpr(KTestPluginId0), AuthAnd(AuthExpr(KTestPluginId1), AuthExpr(KTestPluginId2))));
	
	// id OR id OR id == (id OR id) OR id
	OpTestExprLD(&CTStepAuthExprBuild::TestOrOrL, AuthExpr(KTestPluginId0) | AuthExpr(KTestPluginId1) | AuthExpr(KTestPluginId2));
	OpTestExprLD(&CTStepAuthExprBuild::TestOrOrL, AuthOr(AuthOr(AuthExpr(KTestPluginId0), AuthExpr(KTestPluginId1)), AuthExpr(KTestPluginId2)));
	
	__UHEAP_MARKEND;
	}


void CTStepAuthExprBuild::TestAndAndL(CAuthExpression* aExpr)
/**
	Test the supplied expression says (id0 & id1) & id2.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KAnd);
	TESTL(impl->Left()->Type() == KAnd);
	TESTL(impl->Left()->Left()->Type() == KPluginId);
	TESTL(impl->Left()->Left()->PluginId() == KTestPluginId0);
	TESTL(impl->Left()->Right()->Type() == KPluginId);
	TESTL(impl->Left()->Right()->PluginId() == KTestPluginId1);
	TESTL(impl->Right()->Type() == KPluginId);
	TESTL(impl->Right()->PluginId() == KTestPluginId2);
	}


void CTStepAuthExprBuild::TestAndOrL(CAuthExpression* aExpr)
/**
	Test the supplied expression says (id0 & id 1) | id2.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KOr);
	TESTL(impl->Left()->Type() == KAnd);
	TESTL(impl->Left()->Left()->Type() == KPluginId);
	TESTL(impl->Left()->Left()->PluginId() == KTestPluginId0);
	TESTL(impl->Left()->Right()->Type() == KPluginId);
	TESTL(impl->Left()->Right()->PluginId() == KTestPluginId1);
	TESTL(impl->Right()->Type() == KPluginId);
	TESTL(impl->Right()->PluginId() == KTestPluginId2);
	}


void CTStepAuthExprBuild::TestOrAndL(CAuthExpression* aExpr)
/**
	Test the supplied expression says id0 | (id1 & id2).
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KOr);
	TESTL(impl->Left()->Type() == KPluginId);
	TESTL(impl->Left()->PluginId() == KTestPluginId0);
	TESTL(impl->Right()->Type() == KAnd);
	TESTL(impl->Right()->Left()->Type() == KPluginId);
	TESTL(impl->Right()->Left()->PluginId() == KTestPluginId1);
	TESTL(impl->Right()->Right()->Type() == KPluginId);
	TESTL(impl->Right()->Right()->PluginId() == KTestPluginId2);
	}


void CTStepAuthExprBuild::TestOrOrL(CAuthExpression* aExpr)
/**
	Test the supplied expression says (id0 | id1) | id2.
 */
	{
	CAuthExpressionImpl* impl = static_cast<CAuthExpressionImpl*>(aExpr);
	TESTL(impl->Type() == KOr);
	TESTL(impl->Left()->Type() == KOr);
	TESTL(impl->Left()->Left()->Type() == KPluginId);
	TESTL(impl->Left()->Left()->PluginId() == KTestPluginId0);
	TESTL(impl->Left()->Right()->Type() == KPluginId);
	TESTL(impl->Left()->Right()->PluginId() == KTestPluginId1);
	TESTL(impl->Right()->Type() == KPluginId);
	TESTL(impl->Right()->PluginId() == KTestPluginId2);
	}


void CTStepAuthExprBuild::TestFailedCombsL()
/**
	Test that complex expression builder fails cleanly
	for all permutations of unallocated input.
 */
	{
	__UHEAP_MARK;
	
	// left null only
	CAuthExpression* aeRight = AuthExpr(KTestPluginId0);
	User::LeaveIfNull(aeRight);
	CAuthExpression* aeLeftNull = AuthAnd(0, aeRight);
	TESTL(aeLeftNull == 0);
	
	// right null only
	CAuthExpression* aeLeft = AuthExpr(KTestPluginId1);
	User::LeaveIfNull(aeRight);
	CAuthExpression* aeRightNull = AuthAnd(aeLeft, 0);
	TESTL(aeRightNull == 0);
	
	// both null
	CAuthExpression* aeBothNull = AuthAnd(0, 0);
	TESTL(aeBothNull == 0);
	
	__UHEAP_MARKEND;
	}


void CTStepAuthExprBuild::TestCorruptPersistL()
/**
	Test get expected errors when externalize or
	internalize invalid expressions.
 */
	{
	TInt r;
	
	__UHEAP_MARK;
	CAuthExpression* aeTest = AuthExpr(KTestPluginId0);
	TESTL(aeTest != 0);
	CleanupStack::PushL(aeTest);
	
	TSizeStream ss;
	RWriteStream ws(&ss);
	aeTest->ExternalizeL(ws);
	
	TInt len = ss.Size();
	TAny* buf= User::AllocLC(ss.Size());
	
	// externalize valid expression
	RMemWriteStream mwsValid(buf, len);
	aeTest->ExternalizeL(mwsValid);
	
	// internalize unsupported expression stream
	TInt8& verByteP = ((TInt8*)buf)[0];
	TInt8 origVer = verByteP;
	verByteP = KMaxTInt8;
	RMemReadStream mrsHighVer(buf, len);
	TRAP(r, /* ignore */ CAuthExpressionImpl::NewL(mrsHighVer));
	TESTL(r == KErrAuthServUnsupportedExprVersion);
	verByteP = origVer;
	
	// internalize corrupt expression stream
	TInt8& typeByteP = ((TInt8*)buf)[1];
	TInt8 origType = typeByteP;
	typeByteP = ~0;
	RMemReadStream mrsCorrupt(buf, len);
	TRAP(r, /* ignore */ CAuthExpressionImpl::NewL(mrsCorrupt));
	TESTL(r == KErrAuthServInvalidInternalizeExpr);
	typeByteP = origType;
	
	// internalize valid expression stream
	RMemReadStream mrsValid(buf, len);
	CAuthExpression* aeRestore = CAuthExpressionImpl::NewL(mrsValid);
	TESTL(aeRestore != 0);
	delete aeRestore;
	
	CleanupStack::PopAndDestroy(2, aeTest);
	__UHEAP_MARKEND;
	}


// -------- Server fails expressions from client --------


namespace {

class RTestAuthClient : public RAuthClient
/**
	This subclass of RAuthClient is used to
	send a corrupt externalized expression
	to the authentication server.
	
	The test code can't call SendReceive() on
	an instance of RAuthClient because it is
	protected.
 */
	{
public:
	void SendCorruptAuthExprL();
	void SendHighVerAuthExprL();

	static HBufC8* GetAuthExprExternalizationLC();
	};

HBufC8* RTestAuthClient::GetAuthExprExternalizationLC()
/**
	Helper function for SendCorruptAuthExprL and
	SendHighVerAuthExprL.  This allocates a buffer
	containing an externalized auth expr, so the
	calling function can corrupt it.
	
	@return					Buffer containing externalized
							authentication expression.  This
							is also placed on the cleanup stack.
	@see SendCorruptAuthExprL
	@see SendHighVerAuthExprL
 */
	{
	CAuthExpression* ae = AuthAnd(AuthExpr(KTestPluginId0), AuthExpr(KTestPluginId1));
	User::LeaveIfNull(ae);
	CleanupStack::PushL(ae);
	
	TSizeStream ss;
	RWriteStream ws(&ss);
	ae->ExternalizeL(ws);
	
	HBufC8* buf = HBufC8::NewLC(ss.Size());
	TPtr8 bufPtr = buf->Des();
	
	RDesWriteStream dws(bufPtr);
	ae->ExternalizeL(dws);
	dws.CommitL();

	CleanupStack::Pop(2, ae);
	CleanupStack::PushL(buf);
	
	return buf;
	}


void RTestAuthClient::SendCorruptAuthExprL()
/**
	Send an authenticate request to the server,
	with a corrupt authentication expression.
 */
	{
	HBufC8* buf = GetAuthExprExternalizationLC();
	
	// corrupt the type field, one byte after the version field
	buf->Des()[1] = ~0;
	
	// the server should panick this thread with
	// category "AuthServer" and reason EPanicBadExternalizedAuthExpr (3)
	TRequestStatus rs;
	CallSessionFunction(EAuthenticate, TIpcArgs(buf), rs);
	
	// this code will not be reached
	User::WaitForRequest(rs);
	CleanupStack::PopAndDestroy(buf);
	}


void RTestAuthClient::SendHighVerAuthExprL()
/**
	Send an authenticate request to the server,
	with an unsupported version number.
	
	The server should fail the request with
	KErrAuthServUnsupportedExprVersion.
 */
	{
	HBufC8* buf = GetAuthExprExternalizationLC();
	
	// change the version field, which is the first byte
	buf->Des()[0] = KMaxTInt8;
	
	TRequestStatus rs;
	CallSessionFunction(EAuthenticate, TIpcArgs(buf), rs);
	User::WaitForRequest(rs);
	
	// this isn't semantically the right code to
	// fail the test with, because it is expected.
	// However, it is easy to find in the test code
	// if it occurs.
	if (rs != KErrAuthServUnsupportedExprVersion)
		User::Leave(KErrAuthServUnsupportedExprVersion);
	
	CleanupStack::PopAndDestroy(buf);
	}


void RunTestAuthClientFunctionL(void (RTestAuthClient::* aTacFuncL)())
/**
	Connect to the authentication server using
	RTestAuthClient, and call the supplied function.
	
	This is a helper function for CTStepBadExtAuthExpr
	and CTStepAuthExprHighVer which both use
	RTestAuthClient, and only call different functions.
	
	@param	aTacFuncL		Function to call on RTestAuthClient
							once the connection has been made.
							If this function leaves then the
							leave code is propagated through this
							(RunTestAuthClientFunctionL) function.
 */
	{
	RTestAuthClient tac;
	User::LeaveIfError(tac.Connect());
	CleanupClosePushL(tac);
	
	// this function should not return
	(tac.*aTacFuncL)();
	
	CleanupStack::PopAndDestroy(&tac);
	}


}	// namespace


CTStepBadExtAuthExpr::CTStepBadExtAuthExpr()
/**
	Record this test step's name.
 */
	{
	SetTestStepName(KTStepBadExtAuthExpr);
	}


TVerdict CTStepBadExtAuthExpr::doTestStepL()
/**
	Implement CTestStep by calling RAuthClient::Authenticate
	with a corrupt externalized authentication expression.
	
	The current thread should be panicked with
	category AuthServer and reason EPanicBadExternalizedAuthExpr.
 */
	{
	// this function should not return
	RunTestAuthClientFunctionL(&RTestAuthClient::SendCorruptAuthExprL);
	return EPass;
	}


CTStepAuthExprHighVer::CTStepAuthExprHighVer()
/**
	Record this test step's name.
 */
	{
	SetTestStepName(KTStepAuthExprHighVer);
	}


TVerdict CTStepAuthExprHighVer::doTestStepL()
/**
	Implement CTestStep by calling RAuthClient::SendHighVerAuthExprL()
	and ensuring the server fails the request if
	the externalized auth expr uses an unsupported
	version number.
 */
	{
	RunTestAuthClientFunctionL(&RTestAuthClient::SendHighVerAuthExprL);
	return EPass;
	}


// -------- Debug-mode client-side panicks --------


#ifdef _DEBUG


// -------- CTStepAuthExprTypePnc --------


CTStepAuthExprTypePnc::CTStepAuthExprTypePnc()
/**
	Record this test step's name.
 */
	{
	SetTestStepName(KTStepAuthExprTypePnc);
	}


TVerdict CTStepAuthExprTypePnc::doTestStepL()
/**
	Implement CTestStep by creating a plugin id
	node, corrupting the type field, and testing
	that it is panicked when the type is
	retrieved.  Debug only.
 */
	{
	CAuthExpression* ae = AuthExpr(KTestPluginId0);
	User::LeaveIfNull(ae);
	CAuthExpressionImpl* aeImpl = static_cast<CAuthExpressionImpl*>(ae);
	CorruptTypeField(aeImpl);
	/* ignore */ aeImpl->Type();
	
	// this code should never be reached
	return EPass;
	}


// -------- CTStepAuthExprTypePncBadLeft --------


CTStepAuthExprTypePncBadLeft::CTStepAuthExprTypePncBadLeft()
/**
	Record this test step's name.
 */
	{
	SetTestStepName(KTStepAuthExprTypePncBadLeft);
	}


TVerdict CTStepAuthExprTypePncBadLeft::doTestStepL()
/**
	Implement CTestStep by creating a complex
	expression where the left subexpression is
	corrupt.  This tests that the invariant
	correctly validates the left subtree.
 */
	{
	CAuthExpression* ae = AuthAnd(AuthExpr(KTestPluginId0), AuthExpr(KTestPluginId1));
	User::LeaveIfNull(ae);
	CAuthExpressionImpl* aeImpl = static_cast<CAuthExpressionImpl*>(ae);
	CorruptTypeField(aeImpl->Left());	
	/* ignore */ aeImpl->Type();
	
	// this code should never be reached
	return EPass;
	}


// -------- CTStepAuthExprTypePncBadRight --------


CTStepAuthExprTypePncBadRight::CTStepAuthExprTypePncBadRight()
/**
	Record this test step's name.
 */
	{
	SetTestStepName(KTStepAuthExprTypePncBadRight);
	}


TVerdict CTStepAuthExprTypePncBadRight::doTestStepL()
/**
	Implement CTestStep by creating a complex
	expression where the Right subexpression is
	corrupt.  This tests that the invariant
	correctly validates the Right subtree.
 */
	{
	CAuthExpression* ae = AuthAnd(AuthExpr(KTestPluginId0), AuthExpr(KTestPluginId1));
	User::LeaveIfNull(ae);
	CAuthExpressionImpl* aeImpl = static_cast<CAuthExpressionImpl*>(ae);
	CorruptTypeField(aeImpl->Right());	
	/* ignore */ aeImpl->Type();
	
	// this code should never be reached
	return EPass;
	}


#endif	// #ifdef _DEBUG
