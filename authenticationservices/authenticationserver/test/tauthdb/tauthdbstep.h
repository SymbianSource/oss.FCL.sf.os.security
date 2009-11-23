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


#ifndef TAUTHDBSTEP_H
#define TAUTHDBSTEP_H

#include <test/testexecutestepbase.h>
#include <test/testexecutelog.h>
#include "tauthdbserver.h"
#include <f32file.h>

#include "authdb.h"
#include "encryptedprotectionkey.h"
#include "transientkey.h"  
#include "transientkey.h"  
#include "transientkeyinfo.h"

_LIT(KTStepCreateTestDb, "CreateTestDb");
_LIT(KTStepPersist2, "PERSIST2");

#define KNumPlugins 10


class CTAuthDbStepBase : public CTestStep
	{
protected:
	CTAuthDbStepBase() {}
	virtual ~CTAuthDbStepBase();
	
	void CreatePlugins();
	void CreateKeyInfo();
		
	virtual TVerdict doTestStepPreambleL()
	   {
	   iFs.Connect();
	
	   CreatePlugins();
	   CreateKeyInfo();
	   
	   return TestStepResult();
	   }
	virtual TVerdict doTestStepPostambleL()
    	{
		iFs.Close();
		return TestStepResult();
		}
	
	void RemoveExistingDbL()
	/**
		Remove any existing database file.
	 */
		{
		CFileMan* fm = CFileMan::NewL(iFs);
		CleanupStack::PushL(fm);

		TFileName dbName(AuthServer::KDbName);
		dbName[0] = RFs::GetSystemDriveChar();

		TInt r = fm->Delete(dbName);
		if (r != KErrNotFound)
			User::LeaveIfError(r);

		CleanupStack::PopAndDestroy(fm);	
		}

	HBufC* GetNameLC(TInt i);

	void PluginData(AuthServer::TIdentityId aId,
					AuthServer::TPluginId aPlugin,
					TPtr8 aDes) 
	{
	aDes.Fill(aId*aPlugin);
	}
protected:
	RFs		iFs;


	AuthServer::CPluginDesc*       iPlugins[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId1Keys[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId2Keys[KNumPlugins];
	AuthServer::CTransientKeyInfo* iId3Keys[KNumPlugins];
  };

inline CTAuthDbStepBase::~CTAuthDbStepBase() 
    {
    for (TInt i = 0 ; i < KNumPlugins ; ++i)
        {
        delete iPlugins[i];
        delete iId1Keys[i];
        delete iId2Keys[i];
        delete iId3Keys[i];
        }
    }


class CTStepCreateTestDb : public CTAuthDbStepBase
/**
	Creates a test database which tauthcliserv can
	use to test the manager later.
 */
	{
public:
	CTStepCreateTestDb();
	
	// implement CTestStep
	virtual TVerdict doTestStepL();
	};


class CTStepPersist2 : public CTAuthDbStepBase
	{
public:
	CTStepPersist2();
	
private:
	virtual TVerdict doTestStepL();
	
	void TestOpenCloseL();
	
	void TestIdentitiesL();
	void TestDescriptionL(AuthServer::CAuthDb2* aDb, AuthServer::TIdentityId aIdentityId, const TDesC& aExpDesc);
	
	void TestPrefsL();
	
	void TestTrainedPluginsL();
	void TestKeyPresentL(
		AuthServer::CAuthDb2* aAuthDb, AuthServer::TIdentityId aIdentityId, AuthServer::TPluginId aPluginId,
		const AuthServer::CTransientKeyInfo& aTarget);
	
	void TestTrainingStatusL();
	
	void TestRepairL();

	void TestCompressL();
	TInt DbFileSizeL();
	};


inline HBufC* CTAuthDbStepBase::GetNameLC(TInt id)
    {
    _LIT(nameTmpl, "Plugin_%d");
	HBufC* name = HBufC::NewLC(15);
	name->Des().Format(nameTmpl, id);
	return name;
    }
    
inline void CTAuthDbStepBase::CreatePlugins()
	{
	using namespace AuthServer;
	
	const TAuthPluginType   types[KNumPlugins]
		= { EAuthBiometric, EAuthToken, EAuthKnowledge,
			EAuthBiometric, EAuthToken, EAuthKnowledge,
			EAuthToken, EAuthKnowledge
		};
	const TAuthTrainingStatus   training[KNumPlugins]
		= { EAuthUntrained, EAuthTrained, EAuthFullyTrained,
			EAuthUntrained, EAuthTrained, EAuthFullyTrained,
			EAuthTrained, EAuthFullyTrained
		  };


	for (TInt i = 0 ; i < KNumPlugins ; ++i)
		{
			HBufC* name = GetNameLC(i);
			
			iPlugins[i] = CPluginDesc::NewL(i,
											*name,
											types[i],
											training[i],
											i*10000,
											i,
											i*4);
		    CleanupStack::PopAndDestroy(name);									
		}
	}
inline void CTAuthDbStepBase::CreateKeyInfo()
	{
	using namespace AuthServer;

	CProtectionKey* protKey1 = CProtectionKey::NewLC(8);
	CProtectionKey* protKey2 = CProtectionKey::NewLC(8);
	CProtectionKey* protKey3 = CProtectionKey::NewLC(8);

	HBufC8* data = HBufC8::NewLC(4);
	
	for (TInt plugin = 0 ; plugin < KNumPlugins ; ++plugin)
		{
			CTransientKey* transient = 0;
			// id 1
			iId1Keys[plugin] = CTransientKeyInfo::NewL(plugin);
			PluginData(1, plugin, data->Des());
			transient = iId1Keys[plugin]->CreateTransientKeyL(data->Des());
			CEncryptedProtectionKey* epKey1 =
				transient->EncryptL(*protKey1);
			delete transient;
			iId1Keys[plugin]->SetEncryptedProtectionKeyL(epKey1);
			// id 2
			PluginData(2, plugin, data->Des());
			iId2Keys[plugin] = CTransientKeyInfo::NewL(plugin);
			transient = iId2Keys[plugin]->CreateTransientKeyL(data->Des());
			CEncryptedProtectionKey* epKey2 =
				transient->EncryptL(*protKey2);
			delete transient;
			iId2Keys[plugin]->SetEncryptedProtectionKeyL(epKey2);
			// id 3
			PluginData(3, plugin, data->Des());
			iId3Keys[plugin] = CTransientKeyInfo::NewL(plugin);
			transient = iId3Keys[plugin]->CreateTransientKeyL(data->Des());
			CEncryptedProtectionKey* epKey3 =
				transient->EncryptL(*protKey3);
			delete transient;
			iId3Keys[plugin]->SetEncryptedProtectionKeyL(epKey3);
		}
	CleanupStack::Pop(4, protKey1);
	}

#endif	/* TAUTHDBSTEP_H */
