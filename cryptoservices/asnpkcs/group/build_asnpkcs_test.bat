@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem

rem Compilations needed in order to compile asnpkcs test code
rem ============================================================

rem asnpkcs test code requires testhandler2 in testframework
call cd %SECURITYSOURCEDIR%\testframework\group
call bldmake bldfiles 
call abld test build winscw udeb


call cd %SECURITYSOURCEDIR%\asnpkcs\group
call bldmake bldfiles 
call abld test build winscw udeb



rem Additional compilations needed to execute asnpkcs tests
rem ====================================================

call cd %SECURITYSOURCEDIR%\filetokens\group
call bldmake bldfiles 
call abld test build winscw udeb

  


 


 