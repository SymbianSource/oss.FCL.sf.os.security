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
* AuthTool provides a console interface to the AuthServer query
* methods, basically dumping the contents of the AuthServer's database.
*
*/


/**
 @file 
*/
#include <e32cons.h>
#include "authserver_client.h"

using namespace AuthServer;

_LIT(KAnyStr, "Default");
_LIT(KBiometricStr, "Biometric");
_LIT(KKnowledgeStr, "Knowledge");
_LIT(KTokenStr, "Token");

TAuthPluginType types[] = { EAuthDefault, EAuthBiometric, EAuthKnowledge, EAuthToken };
TPtrC typeNames[] = {KAnyStr(), KBiometricStr(), KKnowledgeStr(), KTokenStr() }; 

_LIT(KUnTrainedStr, "Untrained");
_LIT(KTrainedStr , "Trained");
_LIT(KFullyTraininedStr, "Fully Trained");
TPtrC trainNames[] = { KUnTrainedStr(), KTrainedStr(), KFullyTraininedStr() }; 

_LIT(KMsgAuthToolPanic,"AuthTool: ");

TInt ExtractDetailsL();
TInt DisplayPlugins(RPluginDescriptions&);


GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
	
    CTrapCleanup* cleanupstack=CTrapCleanup::New(); 
    TRAPD(error,ExtractDetailsL());
    __ASSERT_ALWAYS(!error,User::Panic(KMsgAuthToolPanic, error)); 
    delete cleanupstack;
	
    __UHEAP_MARKEND;
    return 0;
    }

CConsoleBase* console = 0;

TInt ExtractDetailsL()
  {
  // create the console
  console = Console::NewL(_L("AuthTool"),
										TSize(KDefaultConsWidth,
											  KDefaultConsHeight));
  CleanupStack::PushL(console);
    
  // connect to server
  RAuthClient authClient;
  User::LeaveIfError(authClient.Connect());
  CleanupClosePushL(authClient);

  // get the plugins
  RPluginDescriptions plugins;
  authClient.PluginsL(plugins);
  CleanupClosePushL(plugins);

  DisplayPlugins(plugins);
  
  // get the preferences
  console->Printf(_L("\nPreferences\n-----------\n"));

  TInt count = sizeof(types)/sizeof(TInt);
  for (TInt i = 0  ; i < count ; ++i)
	{
    console->Printf(_L("%10S : 0x%x\n"), &typeNames[i],
					authClient.PreferredTypePluginL(types[i]));
	}
  console->Printf(_L("\nPress any key to continue\n"));
  console->Getch();

  // get the identities
  console->Printf(_L("\nIdentities\n-----------\n"));
  
  RIdentityIdArray ids;
  authClient.IdentitiesL(ids);
  CleanupClosePushL(ids);
  
  for (TInt index = 0  ; index < ids.Count(); ++index)
	{
    HBufC* str = authClient.IdentityStringL(ids[index]);	

	CleanupStack::PushL(str);
	if (str != 0)
	  {
      console->Printf(_L("0x%x : %S\n"), ids[index],
						str);
	  }
	else
	  {
      console->Printf(_L("0x%x : No Name\n"), ids[index]);
	  }
	CleanupStack::PopAndDestroy(str);
	}
  console->Printf(_L("\nPress any key to finish\n"));	
  console->Getch();

  CleanupStack::PopAndDestroy(4,console);
  return 0;
  }

TPtrC* train(TAuthTrainingStatus aT)
  {
  TInt idx =0; 
  switch (aT)
	{
  case EAuthUntrained:
	idx = 0;
	break;
  case EAuthTrained:
	idx = 1;
	break;
  case EAuthFullyTrained:
	idx = 2;
	break;
	}
  return &trainNames[idx];
  } 

TPtrC* type(TAuthPluginType aT)
  {
  TInt idx =0; 
  switch (aT)
	{
  case EAuthDefault:
	idx = 0;
	break;
  case EAuthBiometric:
	idx = 1;
	break;
  case EAuthKnowledge:
	idx = 2;
	break;
  case EAuthToken:
	idx = 3;
	break;
  case EAuthPerformance:
	idx = 4;
	break;
	}
  return &typeNames[idx];
  } 

TInt DisplayPlugins(RPluginDescriptions& aPlugins)
  {
  console->Printf(_L("\nPlugin Descriptions\n-------------------\n"));
  for (TInt i = 0  ; i < aPlugins.Count(); ++i)
	{
	console->Printf(_L("ID            : 0x%x\n"), aPlugins[i]->Id());
	console->Printf(_L("Name          : %S\n"), aPlugins[i]->Name());
	console->Printf(_L("Type          : %S\n"), type(aPlugins[i]->Type()));
	console->Printf(_L("Training      : %S\n"), train(aPlugins[i]->TrainingStatus()));
	console->Printf(_L("#Combinations : %d\n"), aPlugins[i]->MinEntropy());
	console->Printf(_L("FPR           : %d\n"), aPlugins[i]->FalsePositiveRate());
	console->Printf(_L("FNR           : %d\n\n"), aPlugins[i]->FalseNegativeRate());
    console->Printf(_L("\nPress any key to continue\n"));
    console->Getch();
	}
  
  return 0;
  }

