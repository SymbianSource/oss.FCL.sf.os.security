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
REM AuthServer tests
	
md c:\tauth
copy /s z:\tauth\* c:\tauth

testexecute \tauth\tauthkeys\scripts\tauthkeys.script 
move c:\logs\testexecute\tauthkeys.htm e:\testresults\tauthkeys.htm

testexecute \tauth\tauthdb\scripts\tauthdb.script 
move c:\logs\testexecute\tauthdb.htm e:\testresults\tauthdb.htm

testexecute \tauth\tauthcliserv\scripts\tauthcliserv.script 
move c:\logs\testexecute\tauthcliserv.htm e:\testresults\tauthcliserv.htm

testexecute \tauth\tauthexpr\scripts\tauthexpr.script 
move c:\logs\testexecute\tauthexpr.htm e:\testresults\tauthexpr.htm

testexecute \tauth\tauthplugins\scripts\tauthplugins.script
move c:\logs\testexecute\tauthplugins.htm e:\testresults\tauthplugins.htm

testexecute \tauth\tauthsvr\scripts\tauthsvr2.script
move c:\logs\testexecute\tauthsvr2.htm e:\testresults\tauthsvr2.htm

attrib c:\tauth\*.* -r
del c:\tauth\*.*
rmdir c:\tauth
