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


#include <ecom/implementationproxy.h>
#include <e32base.h>

#include "authserver/authplugininterface.h"
#include "authserver/authtypes.h"
#include "authserver/auth_srv_errs.h"



namespace AuthServer
{
class CTestPlugin1 : public CAuthPluginInterface
	{
public:
	static CTestPlugin1* NewL()
	{ return new (ELeave) CTestPlugin1(); }

	CTestPlugin1();
	virtual ~CTestPlugin1() {}
	virtual TPluginId Id() const { return 0x10274104; }
	virtual void Train(TIdentityId aId, HBufC8*& aResult,
  					   TRequestStatus& aRequest);
	virtual void Cancel() {};
 
	virtual  void Identify(TIdentityId& aId, const TDesC& aClientMessage,
						   HBufC8*& aResult, TRequestStatus& aRequest);

	virtual TBool IsActive() const;
	virtual TInt Forget(TIdentityId aId);
	virtual TInt DefaultData(TIdentityId aId, HBufC8*& aOutputBuf);
	virtual TInt Reset(TIdentityId aId, const TDesC& aRegistrationData, HBufC8*& aResult);
	virtual const TPtrC& Name() const;
	virtual const TPtrC& Description() const;
	virtual AuthServer::TEntropy MinEntropy() const;
	virtual AuthServer::TPercentage FalsePositiveRate() const;
	virtual AuthServer::TPercentage FalseNegativeRate() const;
	virtual AuthServer::TAuthPluginType Type() const;

protected:
 	/// The name of the plugin. Derived classes should set this member.
 	TPtrC iName;
 	/// The plugin's description. Derived classes should set this member.
 	TPtrC iDescription;
 	/// The minumum entropy provided by the plugin. Derived classes should set
 	/// this member.
 	AuthServer::TEntropy iMinEntropy;
 	/// The false positive rate of the plugin. Derived classes should set this
 	/// member.
 	AuthServer::TPercentage iFalsePositiveRate;
 	/// The false negative rate of the plugin. Derived classes should set this
 	/// member.
 	AuthServer::TPercentage iFalseNegativeRate;
  	/// The type of the plugin. Derived classes should set this member.
 	AuthServer::TAuthPluginType iType;
 	/// the state of the plugin.
 	TBool iActive;

    };

class CInactivePlugin : public CTestPlugin1
	{
public:
	static CInactivePlugin* NewL()
	{ return new (ELeave) CInactivePlugin(); }

	CInactivePlugin() {}
	virtual ~CInactivePlugin() {}
	virtual TPluginId Id() const { return 0x10274106; }

	virtual TBool IsActive() const { return EFalse; }
	virtual const TPtrC& Name() const;
	virtual const TPtrC& Description() const;
	virtual AuthServer::TEntropy MinEntropy() const;
	virtual AuthServer::TPercentage FalsePositiveRate() const;
	virtual AuthServer::TPercentage FalseNegativeRate() const;
	virtual AuthServer::TAuthPluginType Type() const;
    };


class CUnknownPlugin : public CTestPlugin1
	{
public:
	static CUnknownPlugin* NewL()
	{ return new (ELeave) CUnknownPlugin(); }

	CUnknownPlugin() {}
	virtual ~CUnknownPlugin() {}
	virtual TPluginId Id() const { return 0x10274107; }
	virtual  void Identify(TIdentityId& aId,  const TDesC& aClientMessage,
							HBufC8*& aResult, TRequestStatus& aRequest);

	virtual void Train(TIdentityId aId, HBufC8*& aResult,
  					   TRequestStatus& aRequest);
	virtual TBool IsActive() const { return ETrue; }
	const TPtrC& Name() const;
	const TPtrC& Description() const;
	virtual AuthServer::TEntropy MinEntropy() const;
	virtual AuthServer::TPercentage FalsePositiveRate() const;
	virtual AuthServer::TPercentage FalseNegativeRate() const;
	virtual AuthServer::TAuthPluginType Type() const;
    };
    
class CWaitToComplete : public CTimer
    {
    public: 
    CWaitToComplete() : CTimer(EPriorityNormal)
        { CTimer::ConstructL(); }
        
    void RunL()
        {
        RDebug::Printf("TestPlugin waittocomplete RUNL() : %d", iStatus.Int());
        if (iStatus != KErrNone)
            {
            iReturn = iStatus.Int();
            }
        User::RequestComplete(iCliReq, iReturn); 
        }
        
     
    void SetCliStatus(TRequestStatus& aResult, TInt aReturn)    
        { iCliReq = &aResult; iReturn = aReturn; }
        
    TRequestStatus* iCliReq;    
    TInt iReturn; 
    };
    
class CBlockPlugin : public CTestPlugin1
	{
public:
	static CBlockPlugin* NewL()
	{ return new (ELeave) CBlockPlugin(); }

	CBlockPlugin() 
	    {
	    }
	virtual ~CBlockPlugin() {}
  
	virtual TPluginId Id() const { return 0x10274105; }

	virtual void Train(TIdentityId /*aId*/, HBufC8*& aResult,
  					   TRequestStatus& aRequest)
    	{
		if (!iTimer.IsAdded())
            {
            CActiveScheduler::Add(&iTimer);
            }
        iClientStatus = &aRequest;
        aRequest =  KRequestPending;
        iTimer.SetCliStatus(aRequest, KErrAuthServPluginCancelled);
        iTimer.After(5000000);        
        aResult = 0;
        }

	virtual void Cancel()
	    {
        iTimer.Cancel();
        User::RequestComplete(iClientStatus, KErrCancel);
        }
 
	virtual  void Identify(TIdentityId& aId, const TDesC& /*aClientMessage*/,
							HBufC8*& aResult, TRequestStatus& aRequest)
	    {
		if (!iTimer.IsAdded())
            {
            CActiveScheduler::Add(&iTimer);
            }
        iClientStatus = &aRequest;
        aRequest =  KRequestPending;
        iTimer.SetCliStatus(aRequest, KErrNone);
        iTimer.After(5000000);  
		aId = KUnknownIdentity;
		aResult = 0;
		}
	virtual const TPtrC& Name() const
	  {
	  return iName;
	  }
	virtual const TPtrC& Description() const
	  {
	  return iDescription;
	  }
	virtual TAuthPluginType Type() const 
	  {
	  return iType;
	  }

	virtual TEntropy MinEntropy() const
	  {
	  return iMinEntropy;
	  }

	virtual TPercentage FalsePositiveRate() const
	  {
	  return iFalsePositiveRate;
	  }

	virtual TPercentage FalseNegativeRate() const
	  {
	  return iFalseNegativeRate;
	  }

	private:
	
	TRequestStatus  iStatus;
	TRequestStatus* iClientStatus;
	CWaitToComplete iTimer;
	
    };

} 

IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);
  
