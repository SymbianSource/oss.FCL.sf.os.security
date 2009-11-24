/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tauthpluginsstep.h"
#include "consts.h"

#include <test/testexecutelog.h>
#include "authserver/authplugininterface.h"
#include <e32std.h>
#include <s32file.h>
#include <ecom/implementationinformation.h>
#include <ecom/ecom.h>
#include <authserver/authpatchdata.h>
#include <authserver/auth_srv_errs.h>
#include <u32hal.h> 

using namespace AuthServer;

CTStepInterface::CTStepInterface()
	{
	SetTestStepName(KTStepInterface);
	}

CTStepInterface::~CTStepInterface()
	{}

TInt HexString2Num(const TDesC8& aStr)
  {
  	TInt data = 0;
	TInt char2hex = 0;
	TInt c = 0;
	for (TInt i = 0 ; i < aStr.Length() ; ++i)
	  {
	  c = aStr[i];
	  
	  if (c < 'A')
	    {
	    char2hex='0';
	    }
	  else if (c < 'a')
	    {
	    char2hex=('A'-10);
	    }
	  else
	    {
	    char2hex=('a'-10);
	    }
	  data = data << 4;      
	  data += aStr[i]-char2hex; 
	  }
	return data;
  }

TVerdict CTStepInterface::doTestStepPreambleL()
	{
	TVerdict verdict = CTAuthPluginsStepBase::doTestStepPreambleL();
	iActiveScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);
	return verdict;
	}

TVerdict CTStepInterface::doTestStepPostambleL()
	{
	TVerdict verdict = CTAuthPluginsStepBase::doTestStepPostambleL();
	delete iActiveScheduler;
	return verdict;
	}
	
TVerdict CTStepInterface::doTestStepL()
	{
	if (TestStepResult() != EPass)
		{
		return TestStepResult();
		}
__UHEAP_MARK;		// Check for memory leaks
	
    TBool res = ETrue; 
    TBool tmp = ETrue; 
	TInt  err = KErrNone;

	RImplInfoPtrArray impl;
	CleanupClosePushL(impl);
	REComSession::ListImplementationsL(KCAuthPluginInterfaceUid, impl);
	
	TEST(tmp = impl.Count() > 0);
	res = tmp && res;

	
	TInt data = HexString2Num(impl[0]->DataType());

    INFO_PRINTF2(_L("Implementation Id: 0x%x"),impl[0]->ImplementationUid().iUid);
    INFO_PRINTF2(_L("Display Name: %S"),&impl[0]->DisplayName());
    INFO_PRINTF2(_L("Default Data: %x"),data);
    INFO_PRINTF2(_L("Rom Only: %d"),impl[0]->RomOnly());
    INFO_PRINTF2(_L("Rom Based: %d"),impl[0]->RomBased());

	CAuthPluginInterface* plugin = 0;

	TEComResolverParams resolverParams;
	TBufC8<16> pluginIdTxt;
			  
	pluginIdTxt.Des().Format(_L8("%x"), impl[0]->ImplementationUid().iUid);
				  
	resolverParams.SetDataType(pluginIdTxt);
				  
	TAny* tempPlugin = 0; 
	TUid Dtor_ID_Key = TUid::Null();
  	TRAP(err, tempPlugin = 
	REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
										Dtor_ID_Key,
										resolverParams));
  	
  	plugin = reinterpret_cast<CAuthPluginInterface*>(tempPlugin);	
	if (plugin != 0)
	    {
	    
	    	

	    TRequestStatus t;
	    TIdentityId id;

	    HBufC8* data = 0; 
	    plugin->Identify(id, KNullDesC(), data,t);
        CleanupStack::PushL(data);
        
	    TEST(tmp = id == 22);
    	res = tmp && res;
	    
	    TEST(tmp = *data == KIdentifyData);
    	res = tmp && res;

	    HBufC8* data2 = 0; 
        plugin->Train(id,data2,t);
        CleanupStack::PushL(data2);
        
        TEST(tmp = *data2 == KTrainData);
    	res = tmp && res;
    	
	    HBufC8* data4 = 0; 
        TEST(tmp = plugin->DefaultData(0, data4) == KErrNotSupported);
    	res = tmp && res;

        TEST(tmp = plugin->Name() == KPluginName);
    	res = tmp && res;		

        TEST(tmp = plugin->Description() == KPluginDescription);
    	res = tmp && res;

        TEST(tmp = plugin->MinEntropy() == KEntropy);
    	res = tmp && res;

        TEST(tmp = plugin->FalsePositiveRate() == KFalsePos);
    	res = tmp && res;

        TEST(tmp = plugin->FalseNegativeRate() == KFalseNeg);
    	res = tmp && res;
		
	    CleanupStack::PopAndDestroy(2);
	    delete plugin;
	    REComSession::DestroyedImplementation(Dtor_ID_Key);
	    }
	else
	    {
	    ERR_PRINTF1(_L("Failed to load plugin"));
	        
	    res = false;
	    }
	    
	 
	TInt count = impl.Count();		
	for (TInt i = 0; i < count; i++)
		{

		TEComResolverParams resolverParams;
		TBufC8<16> pluginIdTxt;
			  
		pluginIdTxt.Des().Format(_L8("%x"), impl[i]->ImplementationUid().iUid);
		//paste begin

    	pluginIdTxt.Des().UpperCase();
		resolverParams.SetDataType(pluginIdTxt);
			  
		//To load plugins from sources other than ROM the patch 
		// data KEnablePostMarketAuthenticationPlugins must be set to True.
		TUint32 enablePostMarketPlugin = KEnablePostMarketAuthenticationPlugins;

#ifdef __WINS__
    
		// Default SymbianOS behavior is to only load auth plugins from ROM.
		enablePostMarketPlugin = 0;

		// For the emulator allow the constant to be patched via epoc.ini
		UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalIntProperty,
       (TAny*)"KEnablePostMarketAuthenticationPlugins", &enablePostMarketPlugin); // read emulator property (if present)

#endif

    	  TAny* tempPlugin = 0;
		  TInt err = 0;
		  TUid Dtor_ID_Key = TUid::Null();
			  
		 if(enablePostMarketPlugin == 0) 
		  	{
		  	TRAP(err, tempPlugin = 
			REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
												Dtor_ID_Key,
												resolverParams,
												KRomOnlyResolverUid));
		  	}
		  
		  else
		  	{
		  	TRAP(err, tempPlugin = 
			REComSession::CreateImplementationL(KCAuthPluginInterfaceUid,
												Dtor_ID_Key,
												resolverParams));
		  	}
		  
		 if (err == KErrNotFound)
		    {
		    err = KErrAuthServNoSuchPlugin;  
		    }
		  User::LeaveIfError(err);
		    						  	  	
		plugin = reinterpret_cast<CAuthPluginInterface*>(tempPlugin);	

	    INFO_PRINTF2(_L("ID:           0x%x"),plugin->Id());
	    INFO_PRINTF2(_L("Name:         %S"),&plugin->Name());
	    INFO_PRINTF2(_L("Description:  %S"),&plugin->Description());
	    INFO_PRINTF2(_L("Type          %x"),plugin->Type());
	    INFO_PRINTF2(_L("MinEntropy:   %d"),plugin->MinEntropy());
	    INFO_PRINTF2(_L("FPR           %d"),plugin->FalsePositiveRate());
	    INFO_PRINTF2(_L("FNR           %d"),plugin->FalseNegativeRate());
	    INFO_PRINTF2(_L("IsActive      %d"),plugin->IsActive());
		HBufC8* buf = 0;
		INFO_PRINTF2(_L("DefaultData   %d"),plugin->DefaultData(0,buf));
		if (buf) { delete buf; }
		
	    delete plugin;
		delete impl[i];
		REComSession::DestroyedImplementation(Dtor_ID_Key);
		}
	
	CleanupStack::PopAndDestroy(&impl);
	
	REComSession::FinalClose();

	SetTestStepResult(res ? EPass : EFail);
	
__UHEAP_MARKEND;	
	return TestStepResult();
	}

