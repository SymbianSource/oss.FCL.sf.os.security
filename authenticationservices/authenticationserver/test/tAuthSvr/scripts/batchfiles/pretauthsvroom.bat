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

if exist \epoc32\winscw\c\private\10202be9\persists\2001ea7f.cre del /s /f /q \epoc32\winscw\c\private\10202be9\persists\2001ea7f.cre
if exist \epoc32\data\z\resource\plugins\tauthplugin.rsc.backup del /s /f /q \epoc32\data\z\resource\plugins\tauthplugin.rsc.backup
if exist \epoc32\release\winscw\udeb\z\resource\plugins\tauthplugin.rsc.backup del /s /f /q \epoc32\release\winscw\udeb\z\resource\plugins\tauthplugin.rsc.backup
if exist \epoc32\release\winscw\urel\z\resource\plugins\tauthplugin.rsc.backup del /s /f /q \epoc32\release\winscw\urel\z\resource\plugins\tauthplugin.rsc.backup

REM Rename tauthplugin.rsc to prevent loading of test plugins for OOM tests
rename \epoc32\data\z\resource\plugins\tauthplugin.rsc tauthplugin.rsc.backup
rename \epoc32\release\winscw\udeb\z\resource\plugins\tauthplugin.rsc tauthplugin.rsc.backup
rename \epoc32\release\winscw\urel\z\resource\plugins\tauthplugin.rsc tauthplugin.rsc.backup