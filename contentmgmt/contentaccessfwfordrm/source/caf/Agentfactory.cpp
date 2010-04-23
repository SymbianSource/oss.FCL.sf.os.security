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
#include <caf/agentfactory.h>
#include <caf/agentinterface.h>
#include <caf/caferr.h>

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

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt CAgentData::Read64(TInt64 /* aPos */, TDes8& /* aDes */, TInt /* aLength */, TRequestStatus& /* aStatus */)
	{
	return KErrCANotSupported;
	}

EXPORT_C void CAgentData::DataSize64L(TInt64& /*aSize*/)
	{
	User::Leave(KErrCANotSupported);
	}

EXPORT_C TInt CAgentData::Seek64(TSeek /*aMode*/, TInt64& /*aPos*/)
	{
	return KErrCANotSupported;
	}
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

EXPORT_C void CAgentData::ReadCancel(TRequestStatus&)
	{	
	}

EXPORT_C void CAgentRightsManager::ListRightsL(RStreamablePtrArray<CRightsInfo>& /* aArray */, RFile& /* aFile */, const TDesC& /* aUniqueId */) const
	{
	User::Leave(KErrCANotSupported);
	}

EXPORT_C TInt CAgentRightsManager::DeleteAllRightsObjects (RFile& /* aFile */, const TDesC& /* aUniqueId */) 
	{
	return KErrCANotSupported;
	}
	
EXPORT_C TInt CAgentManager::GetAttribute(TInt /* aAttribute */, TInt& /* aValue */, RFile& /* aFile */, const TDesC& /* aUniqueId */) 	
	{
	return KErrCANotSupported;
	}
	
EXPORT_C TInt CAgentManager::GetAttributeSet(RAttributeSet& /* aAttributeSet */, RFile& /* aFile */, const TDesC& /* aUniqueId */) 
	{
	return KErrCANotSupported;
	}
	
EXPORT_C TInt CAgentManager::GetStringAttribute(TInt /* aAttribute */, TDes& /* aValue */, RFile& /* aFile */, const TDesC& /* aUniqueId */) 
	{
	return KErrCANotSupported;
	}	

EXPORT_C TInt CAgentManager::GetStringAttributeSet(RStringAttributeSet& /* aStringAttributeSet */, RFile& /* aFile */, const TDesC& /* aUniqueId */) 
	{
	return KErrCANotSupported;
	}

EXPORT_C void CAgentManager::DisplayInfoL (TDisplayInfo /* aInfo */, RFile& /* aFile */, const TDesC& /* aUniqueId */)
	{
	User::Leave(KErrCANotSupported);
	}


