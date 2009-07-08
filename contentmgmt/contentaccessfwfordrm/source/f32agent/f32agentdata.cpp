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


#include <caf/caf.h>
#include "f32agentdata.h"
#include "f32defaultattributes.h"
#include "virtualpath.h"
#include "f32agentui.h"

using namespace ContentAccess;

CF32AgentData* CF32AgentData::NewL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	CF32AgentData* self = new (ELeave) CF32AgentData;
	CleanupStack::PushL(self);
	self->ConstructL(aVirtualPath, aShareMode);
	CleanupStack::Pop(self);
	return self;
	}

CF32AgentData* CF32AgentData::NewL(RFile& aFile, const TDesC& aUniqueId)
	{
	CF32AgentData* self = new (ELeave) CF32AgentData;
	CleanupStack::PushL(self);
	self->ConstructL(aFile, aUniqueId);
	CleanupStack::Pop(self);
	return self;
	}

CF32AgentData::CF32AgentData()
	{
	}

CF32AgentData::~CF32AgentData()
	{
	// Tidy up RFile and RFs
	iFile.Close();
	if(iVirtualPath) // opened by name
		{
		iFs.Close();
		}

	delete iVirtualPath;
	}
  
void CF32AgentData::ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode)
	{
	iVirtualPath = CVirtualPath::NewL(aVirtualPath);
	
	// Check that the client hasn't specified some incorrect UniqueId
	User::LeaveIfError(TF32DefaultAttributes::CheckUniqueId(aVirtualPath.UniqueId()));

	TUint mode = TF32DefaultAttributes::GetFileMode(aShareMode);
	User::LeaveIfError(iFs.Connect());

	// Make the file session shareable
	User::LeaveIfError(iFs.ShareAuto());

	User::LeaveIfError(iFile.Open(iFs, aVirtualPath.URI(), mode));
	}

void CF32AgentData::ConstructL(RFile& aFile, const TDesC& aUniqueId)
	{
	TInt pos = 0;

	// Check that the client hasn't specified some incorrect UniqueId
	User::LeaveIfError(TF32DefaultAttributes::CheckUniqueId(aUniqueId));	
	
	// When creating a CData from a file handle we must duplicate the file handle
	// before doing anything
	User::LeaveIfError(iFile.Duplicate(aFile));
	User::LeaveIfError(iFile.Seek(ESeekStart, pos));  // reset to start of file
	}

void CF32AgentData::DataSizeL(TInt &aSize)
	{
	User::LeaveIfError(iFile.Size(aSize));
	}

TInt CF32AgentData::EvaluateIntent(TIntent /*aIntent*/)
	{
	return KErrNone;
	}

TInt CF32AgentData::ExecuteIntent(TIntent /*aIntent*/)
	{
	return KErrNone;
	}

TInt CF32AgentData::Read(TDes8& aDes) 
	{
	return iFile.Read(aDes);
	}

TInt CF32AgentData::Read(TDes8& aDes,TInt aLength) 
	{
	return iFile.Read(aDes,aLength);
	}

void CF32AgentData::Read(TDes8& aDes,TRequestStatus& aStatus) 
	{
	iFile.Read(aDes, aStatus);
	}

void CF32AgentData::Read(TDes8& aDes,
							 TInt aLength, 
							 TRequestStatus& aStatus) 
	{
	iFile.Read(aDes, aLength, aStatus);
	}
	
TInt CF32AgentData::Read(TInt aPos, TDes8& aDes,
							 TInt aLength, 
							 TRequestStatus& aStatus) 
	{
	iFile.Read(aPos, aDes, aLength, aStatus);
	return KErrNone;
	}
	
void CF32AgentData::ReadCancel(TRequestStatus& aStatus)
{
	iFile.ReadCancel(aStatus);
}

TInt CF32AgentData::Seek(TSeek aMode, TInt& aPos) 
	{
	return iFile.Seek(aMode, aPos);
	}

TInt CF32AgentData::SetProperty(TAgentProperty aProperty, TInt aValue)
	{
	
	if(aProperty==EAgentPropertyAgentUI)
		//	should only pass type EAgentPropertyAgentUI 
		{
		CF32AgentUi* ui = NULL;
	
		// get a pointer to the UI
		TRAPD(err, ui = &AgentUiL());
		if(err)
			{
			return err;
			}
		return ui->SetProperty(aProperty, aValue);
		}
	else
		{
		return KErrCANotSupported;
		}
	}

TInt CF32AgentData::GetAttribute(TInt aAttribute, TInt& aValue)
	{
	if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetAttribute(aAttribute, aValue, iVirtualPath->URI());
		}
	else
		{
		return TF32DefaultAttributes::GetAttribute(aAttribute, aValue, iFile);
		}
	}

TInt CF32AgentData::GetAttributeSet(RAttributeSet& aAttributeSet)
	{
	if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetAttributeSet(aAttributeSet, iVirtualPath->URI());
		}	
	else
		{
		return TF32DefaultAttributes::GetAttributeSet(aAttributeSet, iFile);
		}	
			
	}

TInt CF32AgentData::GetStringAttribute(TInt aAttribute, TDes& aValue)
	{
	if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, iVirtualPath->URI());
		}
	else
		{
		return TF32DefaultAttributes::GetStringAttribute(aAttribute, aValue, iFile);
		}
	}

TInt CF32AgentData::GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet)
	{
	if(iVirtualPath)
		{
		return TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, iVirtualPath->URI());
		}
	else
		{
		return TF32DefaultAttributes::GetStringAttributeSet(aStringAttributeSet, iFile);
		}
	}

CF32AgentUi& CF32AgentData::AgentUiL()
	{
	if(!iAgentUi)
		{
		// load agent UI from f32AgentUi.dll
		iAgentUi = TF32AgentUiFactory::CreateF32AgentUiL();
		}
	return *iAgentUi;
	}
