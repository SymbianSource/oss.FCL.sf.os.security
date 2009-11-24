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
* Contains system wide default plugin and authentication strength aliases configuration parameters.
* The strength aliases and it's corresponding authentication expression is specified in the 
* authserver configuration(central repository) file.Also , defined are the enums corresponding 
* to authentication expression creation.
*
*/


/**
 @file 
 @released
 @internalComponent 
*/
#ifndef AUTHREPOSITORY_H
#define AUTHREPOSITORY_H

#include <centralrepository.h>
#include "authserver/authtypes.h"

namespace AuthServer
	{
	
	/**	 The default system wide plugin key*/
	const TUint KAuthDefaultPlugin = {0x00000000}; 
		
	/**
		AuthServer repository configuration parameter Enums.
		These enums are the key Id's for authentication strength aliases and their respective authentication expression defining them.
	 */
	
	enum TAuthStrengthAliasesKeys
		{
		EAuthAliasesCount			= 0x00000100,	// Number of authentication aliases.
		EAuthAliases				= 0x00000200,   // The various authentication aliases like strong, weak, medium, fast etc.
		EAuthAliasesExpr			= 0x00000300,	// The authentication expressions defining each authentication alias.
		};
	
	/** The UID of the AuthServer repository. */
	const TUid KUidAuthServerRepository = {0x2001ea7f};  //536996479

	
/**
 * Provides methods to access information from
 * AuthServer's Central Repository
 */

class CAuthRepository : public CBase
	{
	
public:
	
	static CAuthRepository* NewL();
	
	static CAuthRepository* NewLC();
	
	TPluginId DefaultPluginL() const;
	
	void ListAliasL(RPointerArray<HBufC>& aAuthStrengthAliasList);
	
	void GetAliasDefinitionL(TInt aKey, TDes16& aValue);
	
	~CAuthRepository();
	
private:
	
	void ConstructL();
	
private:
	CRepository* iRepository;
	};


}
#endif //AUTHREPOSITORY_H
