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

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\tcaf.script
move c:\logs\testexecute\tcaf.htm e:\testresults\tcaf.htm
del c:\tcaf\*.*

md c:\rta
copy /s z:\rta\* c:\rta
testexecute c:\rta\rta.script
move c:\logs\testexecute\rta.htm e:\testresults\rta.htm
del c:\rta\*.*

md c:\tcaf
md c:\tcaf\tscaf
copy /s z:\tcaf\tscaf\* c:\tcaf\tscaf
testexecute c:\tcaf\tscaf\tscaf.script
move c:\logs\testexecute\tscaf.htm e:\testresults\tscaf.htm