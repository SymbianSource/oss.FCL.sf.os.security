/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CERTTOOL_COMMANDS_H_
#define __CERTTOOL_COMMANDS_H_

#include <e32base.h>
#include <ct/rmpointerarray.h>
#include <unifiedcertstore.h>
#include <badesca.h>
//#include <e32cmn.h>

#include <keytool.rsg>

#include "keytool_commands.h"

class CCertToolController;

/**
 * Interface that every keytool command class must implement.
 */
class CCertToolCommand : public CActive
	{
	public:
		virtual void DoCommandL(CUnifiedCertStore& aCertStore, CKeyToolParameters* aParam) = 0;
		
	protected:
		CCertToolCommand(CCertToolController* aController);

	protected:
		CCertToolController* iController;
		CKeyToolParameters* iParams; // we do not own this!		
	};
	
	
/**
 * Implements the certtool display usage action.
 */
class CCertToolUsage : public CCertToolCommand
	{
public:
	static CCertToolUsage* NewLC(CCertToolController* aController);
	static CCertToolUsage* NewL(CCertToolController* aController);
	~CCertToolUsage();
	void ConstructL();

public: // From CCertToolCommand
	void DoCommandL(CUnifiedCertStore& aCertStore, CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();
	
protected:
	CCertToolUsage(CCertToolController* aController);			
	};
	

/**
 * Implements the certtool display usage action.
 */
class CCertToolListStores : public CCertToolCommand
	{
public:
	static CCertToolListStores* NewLC(CCertToolController* aController);
	static CCertToolListStores* NewL(CCertToolController* aController);
	~CCertToolListStores();
	void ConstructL();

public: // From CCertToolCommand
	void DoCommandL(CUnifiedCertStore& aCertStore, CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();
	
protected:
	CCertToolListStores(CCertToolController* aController);			
	
private:
	RArray<MCTCertStore> iCertStoreList; 
	};
	

typedef RArray<TUid> RUidArray;

/**
 * Implements the keytool list action. This is an active object as listing is asynchronous.
 */
class CCertToolList : public CCertToolCommand
	{
public:
	static CCertToolList* NewLC(CCertToolController* aController);
	static CCertToolList* NewL(CCertToolController* aController);
	~CCertToolList();
	void ConstructL();

public: // From CCertToolCommand
	void DoCommandL(CUnifiedCertStore& aCertStore, CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);	
	
protected:
	CCertToolList(CCertToolController* aController);

protected:
	CUnifiedCertStore* iCertStore;
	MCertStore* iCertStoreImp;
	
	TInt iIndex;
	CCertificate* iCertificate;

	RUidArray iApps;
	RArray<RUidArray> iCertApps;
	RPointerArray<CCertificate> iParsedCerts;
	RMPointerArray<CCTCertInfo> iCertInfos;	
	CCertAttributeFilter* iFilter;

	enum TState
		{
		EListCerts,
		ERetrieve,
		EGetApps,
		ESetTrust,
		EFinished,
		EIntermediate
		} iState;			
	};
	



/**
 * Implements the keytool import action.
 */
class CCertToolAdd : public CCertToolCommand
	{
public:
	static CCertToolAdd* NewLC(CCertToolController* aController);
	static CCertToolAdd* NewL(CCertToolController* aController);
	~CCertToolAdd();
	void ConstructL();

public: // From CKeyToolCommand
	void DoCommandL(CUnifiedCertStore& aCertStore, CKeyToolParameters* aParam);
	
public: // From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);
	
private:
	CCertToolAdd(CCertToolController* aController);

	TCertificateFormat DoRecognizeL(const TDesC8& iData);

protected:
	enum TState
		{
		EIntermediate,
		EFinish
		} iState;
	
private:
	HBufC8*	iCertData;
	CUnifiedCertStore* iCertStore;	
	};



/**
 * Implements the certtool remove action. 
 */
class CCertToolRemove : public CCertToolList
	{
public:
	static CCertToolRemove* NewLC(CCertToolController* aController);
	static CCertToolRemove* NewL(CCertToolController* aController);
	~CCertToolRemove();
	
public: // From CActive
	void RunL();

protected:

private:
	CCertToolRemove(CCertToolController* aController);

private:

	};


/**
*
*/
class CCertToolSetApps : public CCertToolList
	{
public:
	static CCertToolSetApps* NewLC(CCertToolController* aController);
	static CCertToolSetApps* NewL(CCertToolController* aController);
	~CCertToolSetApps();

public: // From CActive
	void RunL();
	TInt RunError(TInt aError);	
protected:

private:
	CCertToolSetApps(CCertToolController* aController);
	TInt iCertIndex;
	TBool iCaCert;
	};
	
	
/**
*
*/
class CCertToolAddApps : public CCertToolList
	{
public:
	static CCertToolAddApps* NewLC(CCertToolController* aController);
	static CCertToolAddApps* NewL(CCertToolController* aController);
	~CCertToolAddApps();

public: // From CActive
	void RunL();
	TInt RunError(TInt aError);	

protected:
	enum TState
		{
		EListCerts,
		ERetrieve,
		EGetApps,
		ESetApps,
		EFinished,
		EIntermediate
		} iState;

private:
	CCertToolAddApps(CCertToolController* aController);
	
	};
	
/**
*
*/
class CCertToolRemoveApps : public CCertToolList
	{
public:
	static CCertToolRemoveApps* NewLC(CCertToolController* aController);
	static CCertToolRemoveApps* NewL(CCertToolController* aController);
	~CCertToolRemoveApps();

public: // From CActive
	void RunL();
	TInt RunError(TInt aError);	

protected:
	enum TState
		{
		EListCerts,
		ERetrieve,
		EGetApps,
		ERemoveApps,
		EFinished,
		EIntermediate
		} iState;

private:
	CCertToolRemoveApps(CCertToolController* aController);
	
	};

#endif
