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

REM Remove the .cre file
del /s /f /q \epoc32\winscw\c\private\10202be9\persists\2001ea7f.cre

REM Restore the Cenrep file with the backup.
copy \epoc32\winscw\c\tauth\tauthsvr\testdata\2001ea7f_backup.txt \epoc32\release\winscw\udeb\z\private\10202be9\2001ea7f.txt 

