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
REM PKCS12 Integration Tests-USes TEF
copy /s z:\tpkcs12intg\ c:\tpkcs12intg\
testexecute c:\tpkcs12intg\data\tpkcs12lib.script
move c:\logs\testexecute\tpkcs12lib.htm e:\testresults\tpkcs12lib.htm
del /s c:\tpkcs12intg
attrib c:\*.* -r
del c:\*.*