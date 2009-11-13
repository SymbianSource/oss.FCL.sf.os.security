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




/**
 @file
 @internalComponent
 @released
*/

#ifndef __F32AGENTDATA_H__
#define __F32AGENTDATA_H__

#include <f32file.h>
#include <caf/agentinterface.h>

namespace ContentAccess
	{
	class TVirtualPathPtr;
	class CF32AgentUi;

	/**
	 F32 agent implementation of the CAgentData class used to read
	 plaintext files
	 
	 @internalComponent
	 @released
	 */
	class CF32AgentData : public CAgentData
		{
	public:
		static CF32AgentData* NewL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
		static CF32AgentData* NewL(RFile& aFile, const TDesC& aUniqueId);
		

		~CF32AgentData();

	public: // From CAgentData
		virtual TInt Read(TDes8& aDes);
		virtual TInt Read(TDes8& aDes,TInt aLength);
		virtual void Read(TDes8& aDes, TRequestStatus& aStatus);
		virtual void Read(TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
		virtual void ReadCancel(TRequestStatus& aStatus);
		virtual void DataSizeL(TInt& aSize);
		virtual TInt Seek(TSeek aMode,TInt& aPos);
		virtual TInt SetProperty(TAgentProperty aProperty, TInt aValue);
		virtual TInt EvaluateIntent(TIntent aIntent);
		virtual TInt ExecuteIntent(TIntent aIntent);
		virtual TInt GetAttribute(TInt aAttribute, TInt& aValue);
		virtual TInt GetAttributeSet(RAttributeSet& aAttributeSet);
		virtual TInt GetStringAttribute(TInt aAttribute, TDes& aValue);
		virtual TInt GetStringAttributeSet(RStringAttributeSet& aStringAttributeSet);
		virtual TInt Read(TInt aPos, TDes8& aDes, TInt aLength, TRequestStatus& aStatus);
	private:
		CF32AgentData();
		void ConstructL(const TVirtualPathPtr& aVirtualPath, TContentShareMode aShareMode);
		void ConstructL(RFile& aFile, const TDesC& aUniqueId);
		CF32AgentUi& AgentUiL();
		
	private:
		/** Handle to the filesystem */
		RFs iFs;
		
		/** RFile object */
		RFile iFile;

		CVirtualPath* iVirtualPath;
		CF32AgentUi* iAgentUi;
		};
} // namespace ContentAccess
#endif // __F32AGENTDATA_H__
