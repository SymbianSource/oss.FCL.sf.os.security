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


#ifndef __KEYTOOL_CONTROLLER_H_
#define __KEYTOOL_CONTROLLER_H_

#include <f32file.h>
#include <barsc.h>

#include "controller.h"

#include "keytool_defcontroller.h"
#include "keytool_view.h"
#include "keytool_engine.h"

class CKeyToolParameters;

/**
 * The controller in the MVC pattern for the Symbian OS keytool.
 */
class CKeyToolController : public CController, public KeyToolDefController
	{ 
	public:
		static CKeyToolController* NewLC(MKeyToolView& aView);
	
		static CKeyToolController* NewL(MKeyToolView& aView);
		
		~CKeyToolController();		
		
		/**
		 * Given a command code it delegates to the proper handler.
		 *
		 * @param aCommand The command code to be executed.
		 */
		void HandleCommandL(TInt aCommand, CKeyToolParameters* aParam);

/*	public: 
		void DisplayError(const TDesC& aError, TInt aErrorCode);	
		void DisplayError(const TDesC& aError);
		void DisplayKey(CCTKeyInfo& aKey, TBool aIsDetailed);
		void DisplayKeys(RMPointerArray<CCTKeyInfo> aKeyList, TBool aIsDetailed);
		void DisplayLocalisedMsg(TInt aResourceId);
		void DisplayLocalisedMsg(TInt aResourceId, TInt aErrorCode);
*/
	protected:
		void ConstructL();
		CKeyToolController(MKeyToolView& aView);	

	protected:
		//MKeyToolView& 	iView;
		CKeyToolEngine* iEngine;
		//RFs 			iFsSession;
		//RResourceFile 	iResourceFile;
	};
	
#endif    //__KEYTOOL_CONTROLLER_H_
