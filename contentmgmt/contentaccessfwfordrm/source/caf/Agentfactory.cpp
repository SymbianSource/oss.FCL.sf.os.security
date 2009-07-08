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


#include <ecom/ecom.h>
#include "agentfactory.h"
#include "agentinterface.h"
#include "caferr.h"

using namespace ContentAccess;


CAgentFactory* CAgentFactory::NewL(TUid aUid)
	{
	return static_cast<CAgentFactory*> (REComSession::CreateImplementationL(aUid, _FOFF(CAgentFactory,iEcomKey)));
	}


EXPORT_C CAgentFactory::~CAgentFactory()
	{
	// Finished with ECOM session so destroy implementation pointer
	// iEcomKey was set by ECOM during construction
	// This may cause the agent DLL to unload
	REComSession::DestroyedImplementation(iEcomKey);
	}


// default implementations

EXPORT_C TInt CAgentManager::RenameDir(const TDesC& /*aOldName*/, const TDesC& /* aNewName */)
	{
	return KErrCANotSupported;
	}

EXPORT_C void CAgentImportFile::Cancel()
	{
	// default implementation does nothing	
	}
EXPORT_C TBool CAgentImportFile::ContentMimeTypeL(TDes8& /*aContentMime*/)
	{
	User::Leave(KErrCANotSupported);
	return EFalse;
	}
EXPORT_C TInt CAgentManager::CopyFile(RFile& /* aSource */, const TDesC& /* aDestination */)
	{
	return KErrCANotSupported;
	}

EXPORT_C TInt CAgentData::Read(TInt /* aPos */, TDes8& /* aDes */, TInt /* aLength */, TRequestStatus& /* aStatus */)
	{
	return KErrCANotSupported;
	}
	
EXPORT_C void CAgentData::ReadCancel(TRequestStatus&)
	{	
	}
