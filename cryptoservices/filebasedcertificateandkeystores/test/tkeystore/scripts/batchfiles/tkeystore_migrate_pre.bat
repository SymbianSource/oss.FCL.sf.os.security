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

call \epoc32\winscw\c\tkeystore\scripts\batchfiles\restore_epoc_ini.bat
call del /f /q \epoc32\data\z\private\101f72a6\keys.dat
call del /f /q \epoc32\release\winscw\udeb\z\private\101f72a6\keys.dat
call del /f /q \epoc32\release\winscw\urel\z\private\101f72a6\keys.dat
call del /f /q \epoc32\winscw\c\private\101f72a6\keys.dat
call del /f /q \epoc32\private\101f72a6\keys.dat
