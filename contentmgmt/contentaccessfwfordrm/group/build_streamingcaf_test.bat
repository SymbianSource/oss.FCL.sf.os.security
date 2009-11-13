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

rem Compilations needed in order to compile streaming caf test code
rem =========================================================

call cd %SECURITYSOURCEDIR%\caf2\streaming\group
call bldmake bldfiles 
call abld test build winscw udeb


rem Additional compilations needed to run streaming caf tests
rem =========================================================

call cd %SECURITYSOURCEDIR%\testframework\group
call bldmake bldfiles 
call abld test build winscw udeb


 
