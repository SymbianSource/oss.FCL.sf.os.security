/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* authrepository - Central Repository methods implementation
*
*/


/**
 @file 
*/

#include "authrepository.h"
#include "authserver_impl.h"

using namespace AuthServer;

CAuthRepository* CAuthRepository::NewL()
	{
	CAuthRepository* self = CAuthRepository::NewLC();
	CleanupStack::Pop(self);
	return self;
	}


CAuthRepository* CAuthRepository::NewLC()
	{
	CAuthRepository* self = new (ELeave)CAuthRepository();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CAuthRepository::ConstructL()
	{
	iRepository = CRepository::NewL(KUidAuthServerRepository);
	}


CAuthRepository::~CAuthRepository()
	{
	delete iRepository;
	}



/**
	Retrieves the default system wide plugin Id as defined in the 
	central repository file


	@leave	KErrArgument			If the default plugin is not
									defined in the central repository 
									file.
									
	@return  The retrieved Plugin Id from the repository is returned.								
									
 */

TPluginId CAuthRepository::DefaultPluginL() const
	{
	//Read the default plugin value from the configuration file.
	TInt defaultPlugin(0);
	User::LeaveIfError(iRepository->Get(KAuthDefaultPlugin, defaultPlugin));
 	
	//If the default plugin is not set.
	if(defaultPlugin == 0)
		{	
		User::Leave(KErrArgument);
		}
	
	return defaultPlugin;
	
	}


/**
	Retrieves the authentication alias as defined in the authserver's 
	central repository file .
	
	@param	aAuthAliasList			An array to be populated with the
									authentication alias as obtained 
									from the authserver's central repository 
									file.
	@leave	KErrArgument			when the count of authentication aliases
									in authserver's central repository file is
									negative.
	@leave	KErrNotFound			when there is no authentication aliases
									defined in authserver's central repository file.
 */
 
 void CAuthRepository::ListAliasL(RPointerArray<HBufC>& aAuthAliasList)
	{
	TInt authStrengthAliasCount(0);
 	 
	User::LeaveIfError(iRepository->Get(EAuthAliasesCount, authStrengthAliasCount));
	
	// leave if the count is a negative value.
	if(authStrengthAliasCount < 0)
		User::Leave(KErrArgument);
	
	// leave with KErrNotFound when the count is zero.
	if(authStrengthAliasCount == 0)
		User::Leave(KErrNotFound);
	
	for(TInt i = 0; i < authStrengthAliasCount; ++i)
		{
		HBufC* strengthAlias = HBufC::NewLC(KMaxDescLen);
		TPtr value(strengthAlias->Des());
		User::LeaveIfError(iRepository->Get(EAuthAliases+i, value));
		aAuthAliasList.AppendL(strengthAlias);
		CleanupStack::Pop(strengthAlias);
		}
	}
 

 /**
 	Retrieves the authentication definition corresponding to the alias 
 	as defined in the authserver's central repository file .
 	
 	@param	aKey					The index of the authentication alias
 									in the central repository file.
 									zero.
 	
 	@param	aValue					The retrieved definition value for 
 									the alias.		
  */

 void CAuthRepository::GetAliasDefinitionL(TInt aKey, TDes16& aValue)
	 {
	 User::LeaveIfError(iRepository->Get(EAuthAliasesExpr+aKey, aValue));
	 }
