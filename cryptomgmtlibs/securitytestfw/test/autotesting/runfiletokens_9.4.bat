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
rem ttesttools

copy /s z:\tkeystore\ c:\tkeystore\
copy /s z:\tcertstore\ c:\tcertstore\
copy /s z:\ttesttools\ c:\ttesttools\
copy /s z:\system\data\test1certstore.dat c:\system\data\test1certstore.dat
copy /s c:\ttesttools\data\certclients.dat c:\private\101f72a6\certclients.dat

TESTEXECUTE c:\ttesttools\scripts\sec-filetokens-tools.script
move c:\logs\testexecute\sec-filetokens-tools.htm e:\testresults\sec-filetokens-tools.htm

del c:\private\101f72a6\certclients.dat
del c:\system\data\test1certstore.dat

del /s c:\ttesttools\
del /s c:\tcertstore\
del /s c:\tkeystore\

rem TCERTAPPS
copy /s z:\tcertapps\ c:\tcertapps\
t_certapps c:\tcertapps\scripts\script1.txt c:\testresults\tcertapps.txt
move c:\testresults\tcertapps.txt e:\testresults\tcertapps.txt
t_certapps c:\tcertapps\scripts\apipolicing.txt c:\testresults\tcertapps-apipolicing.txt
move c:\testresults\tcertapps-apipolicing.txt e:\testresults\tcertapps-apipolicing.txt
del /s c:\tcertapps\


REM t_keystore
copy /s z:\tkeystore\ c:\tkeystore\
t_keystore c:\tkeystore\scripts\tkeystore.txt c:\testresults\tkeystore.log
move c:\testresults\tkeystore.log e:\testresults\tkeystore.log
t_keystore c:\tkeystore\scripts\authobjects.txt c:\testresults\tkeystore_authobjects.log
move c:\testresults\tkeystore_authobjects.log e:\testresults\tkeystore_authobjects.log
t_keystore c:\tkeystore\scripts\exportscript.txt c:\testresults\tkeystore_export.log
move c:\testresults\tkeystore_export.log e:\testresults\tkeystore_export.log
t_keystore c:\tkeystore\scripts\reload.txt c:\testresults\tkeystore_reload.log
move c:\testresults\tkeystore_reload.log e:\testresults\tkeystore_reload.log
del /s c:\tkeystore\
