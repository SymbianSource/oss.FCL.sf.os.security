/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "content.h"
#include "resolver.h"
#include "data.h"
#include "agentfactory.h"
#include "agentinfo.h"
#include "agentinterface.h"
#include "attributeset.h"
#include "agent.h"
#include "virtualpath.h"
#include "agentfactory.h"

#ifndef REMOVE_CAF1
#include "attribute.h"
#include "bitset.h"
#endif

using namespace ContentAccess;

EXPORT_C CContent* CContent::NewLC(const TDesC& aURI) 
	{
	return CContent::NewLC(aURI, EContentShareReadOnly);
	}

EXPORT_C CContent* CContent::NewL(const TDesC& aURI)  
	{
	return CContent::NewL(aURI, EContentShareReadOnly);
	}

EXPORT_C CContent* CContent::NewLC(const TDesC& aURI, TContentShareMode aShareMode) 
	{
	CContent* self = new(ELeave) CContent();
	CleanupStack::PushL(self);
	self->ConstructL(aURI, aShareMode);
	return self;
	}

EXPORT_C CContent* CContent::NewL(const TDesC& aURI, TContentShareMode aShareMode)  
	{
	CContent* self=CContent::NewLC(aURI, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CContent* CContent::NewLC(RFile& aFile) 
	{
	CContent* self = new(ELeave) CContent();
	CleanupStack::PushL(self);
	self->ConstructL(aFile);
	return self;
	}

EXPORT_C CContent* CContent::NewL(RFile& aFile) 
	{
	CContent* self=CContent::NewLC(aFile);
	CleanupStack::Pop(self);
	return self;
	}

CContent::CContent() : iDefaultVirtualPath(KNullDesC(), KDefaultContentObject())
	{ 
	}

CContent::~CContent() 
	{ 
	delete iAgentContent;

	if(iVirtualPath)
		{
		delete iVirtualPath;
		}
	else
		{
		iFile.Close();	
		}
	
	// Finished with agent, this closes ECOM handle
	delete iAgentFactory;
	REComSession::FinalClose();
	}


void CContent::ConstructL(RFile &aFile) 
	{
	// Make our own copy of the file handle
	User::LeaveIfError(iFile.Duplicate(aFile));
	
	// Rewind the file pointer
	TInt pos = 0;
	iFile.Seek(ESeekStart, pos);
	
	// For case where CAF is built with an RFile
	CAgentResolver* resolver = CAgentResolver::NewLC(EFalse);
	
	// Find the agent who handles the file
	CAgentInfo& agentInfo = resolver->ResolveFileL(iFile);

	// copy the agent name and Uid
	iAgent = agentInfo.Agent();

	// Construct the agent factory (ECOM handle)
	iAgentFactory = CAgentFactory::NewL(iAgent.ImplementationUid());

	// Construct the CAgentContent object
	iAgentContent = iAgentFactory->CreateContentBrowserL(iFile);

	// Finished with resolver (and the agentInfo object it owns)
	CleanupStack::PopAndDestroy(resolver); 
	}

void CContent::ConstructL(const TDesC& aURI, TContentShareMode aShareMode) 
	{
	iShareMode = aShareMode;

	// Create the agent resolver which will contains a reference to
	// the agent responsible for this piece of content
	CAgentResolver* resolver = CAgentResolver::NewLC(EFalse);
	
	// Create a temporary buffer used to store the translated version of the URI
	HBufC* actualUri = HBufC::NewLC(aURI.Length() + KMaxSIDLength);
	TPtr uri = actualUri->Des();
	
	// Set the TVirtualPathPtr to point to the URI supplied, 
	// this will decode any combined URI into the actual URI and uniqueId
	// for use in subsequent functions
	TVirtualPathPtr contentVirtualPath = aURI;

	// Find the agent who handles the file and translate the URI if it is pointing to a private directory 
	CAgentInfo& agentInfo = resolver->ResolveFileL(contentVirtualPath.URI(), uri, iShareMode);

	// Set the iVirtualPath to point to the translated URI and the uniqueId
	iVirtualPath = CVirtualPath::NewL(*actualUri, contentVirtualPath.UniqueId());
	
	iDefaultVirtualPath = *iVirtualPath;
	
	// Copy the agent name and Uid
	iAgent = agentInfo.Agent();

	// Construct the agent factory (ECOM handle)
	iAgentFactory = CAgentFactory::NewL(iAgent.ImplementationUid());

	// Construct the agent content browser 
	iAgentContent = iAgentFactory->CreateContentBrowserL(uri, iShareMode);	
	
	// Finished with resolver and the CAgentInfo object it owns
	CleanupStack::PopAndDestroy(2, resolver); // actualUri
	}

EXPORT_C TInt CContent::OpenContainer(const TDesC &aUniqueId) 
	{
	return iAgentContent->OpenContainer(aUniqueId);
	}

EXPORT_C TInt CContent::CloseContainer ()
	{
	return iAgentContent->CloseContainer();
	}

EXPORT_C void CContent::GetEmbeddedObjectsL (RStreamablePtrArray<CEmbeddedObject>& aArray) const
	{
	iAgentContent->GetEmbeddedObjectsL(aArray);
	}

EXPORT_C void CContent::GetEmbeddedObjectsL (RStreamablePtrArray<CEmbeddedObject>& aArray, TEmbeddedType aType) const
	{
	iAgentContent->GetEmbeddedObjectsL(aArray, aType);
	}

EXPORT_C TInt  CContent::Search (RStreamablePtrArray<CEmbeddedObject>& aArray, const TDesC8 &aMimeType, TBool aRecursive) 
	{
	return iAgentContent->Search(aArray, aMimeType, aRecursive);
	}

EXPORT_C TInt CContent::GetAttribute (TInt aAttribute, TInt &aValue) const
	{
	return GetAttribute(aAttribute, aValue, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C TInt CContent::GetAttribute (TInt aAttribute, TInt& aValue, const TDesC &aUniqueId) const
	{
	return iAgentContent->GetAttribute(aAttribute, aValue, aUniqueId);
	}

EXPORT_C TInt CContent::GetAttributeSet(RAttributeSet& aAttributeSet) const
	{
	return GetAttributeSet(aAttributeSet, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C TInt CContent::GetAttributeSet(RAttributeSet& aAttributeSet, const TDesC& aUniqueId) const
	{
	return iAgentContent->GetAttributeSet(aAttributeSet, aUniqueId);
	}

EXPORT_C TInt CContent::GetStringAttribute (TInt aAttribute, TDes &aValue) const
	{
	return GetStringAttribute(aAttribute, aValue, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C TInt CContent::GetStringAttribute (TInt aAttribute, TDes &aValue, const TDesC &aUniqueId) const
	{
	return iAgentContent->GetStringAttribute(aAttribute, aValue, aUniqueId);
	}

EXPORT_C TInt CContent::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet) const
	{
	return GetStringAttributeSet(aStringAttributeSet, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C TInt CContent::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet, const TDesC& aUniqueId) const
	{
	return iAgentContent->GetStringAttributeSet(aStringAttributeSet, aUniqueId);
	}

EXPORT_C TInt CContent::AgentSpecificCommand (TInt aCommand, const TDesC8 &aInputBuffer, TDes8 &aOutputBuffer) 
	{
	return iAgentContent->AgentSpecificCommand(aCommand, aInputBuffer, aOutputBuffer);
	}

EXPORT_C void CContent::AgentSpecificCommand (TInt aCommand, const TDesC8 &aInputBuffer, TDes8 &aOutputBuffer, TRequestStatus &aStatus) 
	{
	iAgentContent->AgentSpecificCommand(aCommand, aInputBuffer, aOutputBuffer, aStatus);
	}

EXPORT_C void CContent::NotifyStatusChange(TEventMask aMask, TRequestStatus &aStatus) 
	{
	NotifyStatusChange(aMask, aStatus, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C void CContent::NotifyStatusChange(TEventMask aMask, TRequestStatus &aStatus, const TDesC &aUniqueId) 
	{
	iAgentContent->NotifyStatusChange(aMask, aStatus, aUniqueId);
	}

EXPORT_C TInt CContent::CancelNotifyStatusChange(TRequestStatus &aStatus) 
	{
	return CancelNotifyStatusChange(aStatus, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C TInt CContent::CancelNotifyStatusChange(TRequestStatus &aStatus, const TDesC &aUniqueId) 
	{
	return iAgentContent->CancelNotifyStatusChange(aStatus, aUniqueId);
	}

EXPORT_C void  CContent::RequestRights(TRequestStatus &aStatus) 
	{
	RequestRights(aStatus, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C void  CContent::RequestRights(TRequestStatus &aStatus, const TDesC &aUniqueId) 
	{
	iAgentContent->RequestRights(aStatus, aUniqueId);
	}

EXPORT_C TInt CContent::CancelRequestRights(TRequestStatus &aStatus)
	{
	return CancelRequestRights(aStatus, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C TInt CContent::CancelRequestRights(TRequestStatus &aStatus, const TDesC& aUniqueId)
	{
	return iAgentContent->CancelRequestRights(aStatus, aUniqueId);
	}

EXPORT_C void CContent::DisplayInfoL(TDisplayInfo aInfo) const
	{
	DisplayInfoL(aInfo, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C void CContent::DisplayInfoL(TDisplayInfo aInfo, const TDesC& aUniqueId) const
	{
	iAgentContent->DisplayInfoL(aInfo, aUniqueId);
	}

EXPORT_C TInt CContent::SetProperty(TAgentProperty aProperty, TInt aValue) 
	{
	return iAgentContent->SetProperty(aProperty, aValue);
	}

EXPORT_C CData* CContent::OpenContentL(TIntent aIntent)
	{
	return OpenContentL(aIntent, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C CData* CContent::OpenContentLC(TIntent aIntent)
	{
	return OpenContentLC(aIntent, iDefaultVirtualPath.UniqueId());
	}

EXPORT_C CData* CContent::OpenContentL(TIntent aIntent, const TDesC &aUniqueId)
	{
	CData* data = OpenContentLC(aIntent, aUniqueId);
	CleanupStack::Pop(data);
	return data;
	}

EXPORT_C CData* CContent::OpenContentLC(TIntent aIntent, const TDesC &aUniqueId)
	{
	// Open the content object specified by the Unique Id
	if(iVirtualPath)
		{
		// create a CData based upon the URI supplied when this CContent was created
		return CData::NewLC(iAgent.ImplementationUid(), TVirtualPathPtr(iDefaultVirtualPath.URI(), aUniqueId), aIntent, iShareMode);
		}
	else
		{
		// create a CData based upon the file handle supplied when this CContent was created
		return CData::NewLC(iAgent.ImplementationUid(), iFile, aUniqueId, aIntent);
		}
	}

EXPORT_C const TAgent& CContent::Agent() const
	{
	// The agent handling this content
	return iAgent;
	}

#ifndef REMOVE_CAF1 
// Deprecated functions

EXPORT_C CData* CContent::OpenContentL(TIntent aIntent, TContentShareMode aShareMode)
	{
	CData* data = NULL;
	// Open the content object specified by the Unique Id
	if(iVirtualPath)
		{
		// create a CData based upon the URI supplied when this CContent was created
		data = CData::NewLC(iAgent.ImplementationUid(), iDefaultVirtualPath, aIntent, aShareMode);
		}
	else
		{
		// create a CData based upon the file handle supplied when this CContent was created
		data = CData::NewLC(iAgent.ImplementationUid(), iFile, iDefaultVirtualPath.UniqueId(), aIntent);
		}
	CleanupStack::Pop(data);
	return data;
	}

EXPORT_C CAttribute* CContent::NewAttributeL(TBool aPreloaded)
	{
	return NewAttributeL(aPreloaded, EContentShareReadOnly);
	}

EXPORT_C CAttribute* CContent::NewAttributeL(TBool aPreloaded, TContentShareMode aShareMode)
	{
	CAttribute* attr = NULL;
	
	if(iVirtualPath)
		{
		// if we were opened with a file name
		attr = CAttribute::NewLC(iAgent.ImplementationUid(), iDefaultVirtualPath.URI(), aShareMode);
		}
	else
		{
		// if we were opened with a file handle 
		attr = CAttribute::NewLC(iAgent.ImplementationUid(), iFile);
		}

	// If aPreloaded is set, query the agent immediately for all the attributes
	if (aPreloaded)
		{
		attr->QuerySet().SetAll();
		attr->GetL();
		}

	CleanupStack::Pop(attr);
	return attr;
	}

#endif // REMOVE_CAF1


// DLL entry point - only for EKA1
