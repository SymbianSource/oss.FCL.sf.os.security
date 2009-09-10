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

REM run SIF tests
testexecute z:\tusif\tsif\tsif.script
move c:\logs\testexecute\tsif.htm e:\testresults\tsif.htm

testexecute z:\tusif\tsifintegration\tsifintegration.script
move c:\logs\testexecute\tsifintegration.htm e:\testresults\tsifintegration.htm

testexecute z:\tusif\tsifrefinstaller\tsifrefinstaller.script
move c:\logs\testexecute\tsifrefinstaller.htm e:\testresults\tsifrefinstaller.htm

REM run integration tests for Post manufacture installation of a Layered Execution Environment
testexecute z:\tusif\tswtype\tsifswtypeintegration.script
move c:\logs\testexecute\tsifswtypeintegration.htm e:\testresults\tsifswtypeintegration.htm

REM run SCR tests
testexecute z:\tusif\tscr\tscr.script
move c:\logs\testexecute\tscr.htm e:\testresults\tscr.htm

testexecute z:\tusif\tscr\tscr_performance.script
move c:\logs\testexecute\tscr_performance.htm e:\testresults\tscr_performance.htm

REM run STS tests
testexecute z:\tusif\tsts\tsts.script
move c:\logs\testexecute\tsts.htm e:\testresults\tsts.htm

testexecute z:\tusif\tsts\tintegrityservices\scripts\tsts_tintegrityservices.script
move c:\logs\testexecute\tsts_tintegrityservices.htm e:\testresults\tsts_tintegrityservices.htm

testexecute z:\tusif\tsts\tsts_rollbackall.script
move c:\logs\testexecute\tsts_rollbackall.htm e:\testresults\tsts_rollbackall.htm
