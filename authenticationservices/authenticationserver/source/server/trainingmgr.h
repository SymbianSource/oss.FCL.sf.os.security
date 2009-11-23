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
* CTrainingmgr - Auth Server helper class
*
*/


/**
 @file 
 @internalComponent
 @released
*/
#ifndef TRAININGMGR_H
#define TRAININGMGR_H

#include <e32cmn.h>

#include "authrepository.h"
#include "authdb.h"
#include "pluginmgr.h"

namespace AuthServer
{

/** 
 * This AO handles the training of plugins for identities.  
 **/
class CTrainingMgr : public CActive
	{
public:
  
	CTrainingMgr(CPluginMgr&      aPluginMgr,
				 CAuthDb2&        aAuthDb,
				 CAuthRepository& aAuthRepository);
    
    ~CTrainingMgr();
    
	void RegisterIdentityL(const RMessage2& aMessage,
						  TIdentityId aId,
						  CProtectionKey& aProtKey);
	
	TBool RegisterFirstIdentityL(TIdentityId aId,
								CProtectionKey& aProtKey);
	

	void TrainPluginL(const RMessage2& aMessage,
					 CProtectionKey& aProtKey);


	TBool IsBusy() const;
	
	void RunL();  

	void DoCancel(); 
	
private:
	
	enum TState
		{
		EIdle,
		ERegistrationFirstStep,
		ERegistering,
		ERegistrationDone,
		ETrainingDone,
		};

	CTransientKeyInfo* CreateKeyInfoLC();
	
	TBool AddIdentityL();
	TBool AddTrainingResultL();
	void  DoRegistrationStepL();
	void  WriteResultToMsgL();
	void  Complete();
	void  Cleanup();
	TInt RunError(TInt aError);

	TState           iState;
	const RMessage2* iMessage;
	CPluginMgr*      iPluginMgr;
	CAuthDb2*        iAuthDb;
	TInt             iCurrentPluginIdx;
	TPluginId        iCurrentPlugin;
	TIdentityId      iIdentity;
	HBufC8*          iResult;
	HBufC*           iDescription;
	CProtectionKey*  iProtKey;
	CAuthRepository& iAuthRepository;
};
}
#endif //  TRAININGMGR_H
