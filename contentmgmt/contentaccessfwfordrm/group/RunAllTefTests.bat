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
REM Run all TEF (TestExecute Framework) tests.
REM Automatically copies the .htm result files to the memory card (e: drive).
REM 

testexecute.exe z:\tcaf\tcaf.script
copy c:\logs\testexecute\tcaf.htm e:\tcaf.htm
copy c:\logs\testexecute\TestResults.htm e:\tcaf_Summary.htm

testexecute.exe z:\rta\rta.script
copy c:\logs\testexecute\rta.htm e:\rta.htm
copy c:\logs\testexecute\TestResults.htm e:\rta_Summary.htm