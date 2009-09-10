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


#ifndef __REFTESTAGENTDATA_H__
#define __REFTESTAGENTDATA_H__

#include <caf/caf.h>
#include <caf/agentinterface.h>
#include "rtaArchive.h"
#include "drmfilecontent.h"
#include "rtadata.h"

class CRefTestAgentData : public ContentAccess::CAgentData
		{
	public:
		static CRefTestAgentData* NewL(const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::TContentShareMode aShareMode);
		static CRefTestAgentData* NewLC(const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::TContentShareMode aShareMode);

		static CRefTestAgentData* NewL(RFile& aFile, const TDesC& aUniqueId);
		static CRefTestAgentData* NewLC(RFile& aFile, const TDesC& aUniqueId);

		virtual ~CRefTestAgentData();

	public: // From CAgentData
		virtual TInt Read(TDes8& aDes);
		virtual TInt Read(TDes8& aDes,TInt aLength);
		virtual void Read(TDes8& aDes, TRequestStatus& aStatus);
		virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
		virtual void ReadCancel(TRequestStatus& aStatus);
		virtual void DataSizeL(TInt& aSize);
		virtual TInt Seek(TSeek aMode,TInt& aPos);
		virtual TInt SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue);
		virtual TInt EvaluateIntent(ContentAccess::TIntent aIntent);
		virtual TInt ExecuteIntent(ContentAccess::TIntent aIntent);
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue);
		virtual TInt GetAttributeSet(ContentAccess::RAttributeSet& aAttributeSet);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue);
		virtual TInt GetStringAttributeSet(ContentAccess::RStringAttributeSet& aStringAttributeSet);
		virtual TInt Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
		virtual void DataSize64L(TInt64& aSize);
		virtual TInt Seek64(TSeek aMode, TInt64& aPos);
		virtual TInt Read64(TInt64 aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API

	private:
		CRefTestAgentData();
		void ConstructL(const ContentAccess::TVirtualPathPtr& aVirtualPath, ContentAccess::TContentShareMode aShareMode);
		void ConstructL(RFile& aFile, const TDesC& aUniqueId);
		
	private:
		ReferenceTestAgent::RRtaData iServer;
		};

#endif // __REFTESTAGENTCONSUMER_H__
