/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Software random implementation
*
*/


/**
 @file
 @internalComponent
 @released
*/

#ifndef __CRYPTOAPI_RANDOMIMPL_H__
#define __CRYPTOAPI_RANDOMIMPL_H__

#include <cryptospi/cryptoplugin.h>
#include <cryptospi/randomplugin.h>

namespace SoftwareCrypto
	{
	using namespace CryptoSpi;
	
	NONSHARABLE_CLASS(CRandomImpl): public CBase, public MRandom
		{
	public:
		// NewL & NewLC
		static CRandomImpl* NewL(TUid aImplementationUid);
		static CRandomImpl* NewLC(TUid aImplementationUid);
		
		// From MPlugin
		void Reset();
		void Close();
		void GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics);
		const CExtendedCharacteristics* GetExtendedCharacteristicsL();
		TAny* GetExtension(TUid aExtensionId);
		TUid ImplementationUid() const;
		
		// From MRandom
		void GenerateRandomBytesL(TDes8& aDest);
		
		static CExtendedCharacteristics* CreateExtendedCharacteristicsL();

	private:
		//Constructor
		CRandomImpl(TUid aImplementationUid);
		
		//Destructor
		~CRandomImpl();
		
	private:
		TUid iImplementationUid;
	
		};

	class TRandomImpl {
	/**
	 * The user interface to the system cryptographically 
	 * secure random number generator.
	 */
	public:
		
		/**
		 * Fills the provided buffer with secure random data up to its
		 * current length, discarding any current content.
		 *
		 * @param aDestination The buffer in which to write the random data.
		 */
		static void Random(TDes8& aDestination);
		};
	
	class RRandomSessionImpl : public RSessionBase
	/**
	 * The client interface to the system random number generator. End
	 * users should use TRandom instead of this interface.
	 *
	 */
		{
	public:
		RRandomSessionImpl();
		
		/**
		 * Fills the provided buffer with secure random data up to its
		 * current length, discarding any current content.
		 *
		 * @param aDestination The buffer in to which to write the random data 
		 */
		TInt GetRandom(TDes8& aDestination);
		
		/**
		 * Opens a new session with the random number server.
		 */
		void ConnectL();
		};
	}

#endif // __CRYPTOAPI_RANDOMIMPL_H__
