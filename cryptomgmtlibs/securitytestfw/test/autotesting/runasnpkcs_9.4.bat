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
REM tasnpkcs (relies on tkeystore test data)
md c:\tkeystore
md c:\tkeystore\data
copy /s z:\tkeystore\data\ c:\tkeystore\data\
copy z:\tasnpkcstests.txt c:\tasnpkcstests.txt
tasnpkcs c:\tasnpkcstests.txt c:\testresults\tasnpkcstests.log
move c:\testresults\tasnpkcstests.log e:\testresults\tasnpkcstests.log
del c:\tasnpkcstests.txt
del /s c:\tkeystore\data\
attrib c:\*.* -r
del c:\*.*
