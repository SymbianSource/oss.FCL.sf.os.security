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
if exist \epoc32\data\epoc_temp.ini copy \epoc32\data\epoc_temp.ini \epoc32\data\epoc.ini
if exist \epoc32\data\epoc_temp.ini del \epoc32\data\epoc_temp.ini

rmdir /s /q \epoc32\winscw\c\sys
rmdir /s /q \epoc32\winscw\c\resource\plugins

REM deletes cloned client tauthsvr_1.exe (cloned using SETCAP)
del \epoc32\winscw\c\sys\bin\tauthsvr_new.exe