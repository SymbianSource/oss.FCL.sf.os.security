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



echo makesis
cd %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\
makesis %3.pkg %4_unsigned.sis

echo signsis
if "%5"=="ROOT5CA" signsis %4_unsigned.sis %4.sis "\common\generic\security\os\security\cryptomgmtlibs\securitytestfw\testcertificates\ups\test\root5ca\ca.pem"  "\common\generic\security\os\security\cryptomgmtlibs\securitytestfw\testcertificates\ups\test\root5ca\ca.key.pem"
if "%5"=="ALL" signsis %4_unsigned.sis %4.sis "\common\generic\security\os\security\cryptomgmtlibs\securitytestfw\testcertificates\ups\test\symbiantestallcapabilitiesca\symbiantestallcapabilitiesca.pem"  "\common\generic\security\os\security\cryptomgmtlibs\securitytestfw\testcertificates\ups\test\symbiantestallcapabilitiesca\cakey.pem"
if "%5"=="TESTROOT" signsis %4_unsigned.sis %4.sis "\common\generic\security\os\security\cryptomgmtlibs\securitytestfw\testcertificates\ups\test\symbiantestrootcadsa\ca.pem" "\common\generic\security\os\security\cryptomgmtlibs\securitytestfw\testcertificates\ups\test\symbiantestrootcadsa\ca.key.pem"
if "%5"=="SELF" signsis %4_unsigned.sis %4.sis "\common\generic\security\os\security\cryptomgmtlibs\securitytestfw\testcertificates\ups\test\certificatesnotinstore\selfsigned_cert.pem" "\common\generic\security\os\security\cryptomgmtlibs\securitytestfw\testcertificates\ups\test\certificatesnotinstore\selfsigned_key.pem"

if "%1"=="WINSCW" goto COPYWINSCW;
if "%1"=="ARMV5" goto COPYARMV5;

:COPYWINSCW

echo Copy winscw sis to emulator drive or armv5 to data z tree
copy /y %4.sis %EPOCROOT%epoc32\release\%1\%2\z\tups\integ\packages\%4.sis
goto clean;

	
:COPYARMV5
echo Copy winscw sis to emulator drive or armv5 to data z tree
copy /y %4.sis %EPOCROOT%epoc32\data\z\tups\integ\packages\%4.sis


goto clean;

:clean
del /q %4_unsigned.sis 
del /q %4.sis 

goto exit;

:exit

