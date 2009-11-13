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
REM SWI tests
	
md c:\tswi\

testexecute z:\tswi\tuiscriptadaptors\scripts\testpastub.script
move c:\logs\testexecute\testpastub.htm e:\testresults\testpastub.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testsapropagatedstub.script
move c:\logs\testexecute\testsapropagatedstub.htm e:\testresults\testsapropagatedstub.htm

testexecute z:\tswi\tsis\scripts\tsis_failure.script
move c:\logs\testexecute\tsis_failure.htm e:\testresults\tsis_failure.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testnotargetdevices.script
move c:\logs\testexecute\testnotargetdevices.htm e:\testresults\testnotargetdevices.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testpreinstalled_delete.script
move c:\logs\testexecute\testpreinstalled_delete.htm e:\testresults\testpreinstalled_delete.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\tsisstub_delete.script
move c:\logs\testexecute\tsisstub_delete.htm e:\testresults\tsisstub_delete.htm

REM DEF096906: Regression in SIS file functionality 
testexecute z:\tswi\tuiscriptadaptors\scripts\testuninstallation.script
move c:\logs\testexecute\testuninstallation.htm e:\testresults\testuninstallation.htm

md c:\tswi\tbackuprestore\
md c:\drive_r\
setsubst r: c:\drive_r\
testexecute z:\tswi\tpathsubst\scripts\tpathsubst.script
move c:\logs\testexecute\tpathsubst.htm e:\testresults\tpathsubst.htm
setsubst r:
del c:\drive_r\*.*


REM DEF083628: Orphaned files in protected dirs can be overwritten by untrusted packages
testexecute z:\tswi\tuiscriptadaptors\scripts\overwriteprivate.script
move c:\logs\testexecute\overwriteprivate.htm e:\testresults\overwriteprivate.script.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\cr956_test_armv5.script
move c:\logs\testexecute\cr956_test_armv5.htm e:\testresults\cr956_test_armv5.htm

del c:\temp\*.*
attrib c:\tswi\*.* -r
del c:\tswi\*.*
