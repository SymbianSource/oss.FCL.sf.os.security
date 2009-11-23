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
copy /s z:\tInstalledAppsRegistry\ c:\tInstalledAppsRegistry\
md c:\System
md c:\System\Install
md c:\System\Install\Registry
md e:\System
md e:\System\Install
md e:\System\Install\Registry
attrib c:\tInstalledAppsRegistry\Data\0000000C.REG -r
attrib c:\tInstalledAppsRegistry\Data\0000000D.REG -r
copy c:\tInstalledAppsRegistry\Data\0000000C.REG c:\System\Install\Registry\
copy c:\tInstalledAppsRegistry\Data\0000000D.REG e:\System\Install\Registry\
del c:\tInstalledAppsRegistry\Data\0000000C.REG
del c:\tInstalledAppsRegistry\Data\0000000D.REG

rem run tinstalledappsregistry tests    		
tInstalledAppsRegistry c:\tInstalledAppsRegistry\scripts\script1.txt c:\testresults\tInstalledAppsRegistry.log

rem copy logs to mmc card
move c:\testresults\tInstalledAppsRegistry.log e:\testresults\tInstalledAppsRegistry.log

attrib c:\*.* -r
del c:\*.*

REM tintegritysupport 					

REM copy /s z:\tIntegritySupport\data\ c:\tIntegritySupport\data\
REM tIntegritySupport z:\tIntegritySupport\scripts\tisrecovery.txt c:\testresults\tisrecovery.log
REM tIntegritySupport z:\tIntegritySupport\scripts\tisrecoveryapp.txt c:\testresults\tisrecoveryapp.log
REM del /s c:\tIntegritySupport\data\


