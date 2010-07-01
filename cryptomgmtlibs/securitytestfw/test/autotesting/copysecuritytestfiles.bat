@rem
@rem Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

if "%1"=="init" goto init;
if "%1"=="auto" goto auto; 
if "%1"=="cryptohash" goto cryptohash;
if "%1"=="cryptohashdel" goto cryptohashdel;
if "%1"=="tpadding" goto tpadding; 
if "%1"=="tpaddingdel" goto tpaddingdel;
if "%1"=="tcryptospi" goto tcryptospi;
if "%1"=="tcryptospidel" goto tcryptospidel; 
if "%1"=="tasymmetric" goto tasymmetric;
if "%1"=="tsymmetric" goto tsymmetric;
if "%1"=="tbigint" goto tbigint; 
if "%1"=="tasymmetricdel" goto tasymmetricdel;
if "%1"=="tsymmetricdel" goto tsymmetricdel;
if "%1"=="tbigintdel" goto tbigintdel; 
if "%1"=="tpkcs5kdf" goto tpkcs5kdf;
if "%1"=="tpkcs5kdfdel" goto tpkcs5kdfdel;
if "%1"=="tpbe" goto tpbe; 
if "%1"=="tpbedel" goto tpbedel;
if "%1"=="trecog" goto trecog;
if "%1"=="trecogdel" goto trecogdel;
if "%1"=="tkeystore" goto tkeystore; 
if "%1"=="tkeystoredel" goto tkeystoredel;
if "%1"=="tcertapps" goto tcertapps;
if "%1"=="tcertappsdel" goto tcertappsdel; 
if "%1"=="t_keystore" goto t_keystore;
if "%1"=="t_keystoredel" goto t_keystoredel;
if "%1"=="tcaf" goto tcaf; 
if "%1"=="tcafdel" goto tcafdel;
if "%1"=="rta" goto rta;
if "%1"=="rtadel" goto rtadel; 
if "%1"=="tscaf" goto tscaf;
if "%1"=="tscafdel" goto tscafdel;
if "%1"=="mtputils" goto mtputils; 
if "%1"=="mtputilsdel" goto mtputilsdel;
if "%1"=="tpkcs12intg" goto tpkcs12intg;
if "%1"=="tpkcs12intgdel" goto tpkcs12intgdel; 
if "%1"=="tasnpkcs" goto tasnpkcs;
if "%1"=="tasnpkcsdel" goto tasnpkcsdel;
if "%1"=="tpkcs7" goto tpkcs7; 
if "%1"=="tpkcs7del" goto tpkcs7del;
if "%1"=="certstore" goto certstore;
if "%1"=="cacertsdel" goto cacertsdel; 
if "%1"=="certclientdel" goto certclientdel;
if "%1"=="certstoredel" goto certstoredel;
if "%1"=="tocsp" goto tocsp; 
if "%1"=="tocspdel" goto tocspdel; 
if "%1"=="tx509" goto tx509;
if "%1"=="tx509del" goto tx509del;
if "%1"=="tasn1" goto tasn1; 
if "%1"=="tasn1del" goto tasn1del;
if "%1"=="twtlscert" goto twtlscert;
if "%1"=="twtlscertdel" goto twtlscertdel; 
if "%1"=="tpkixcert" goto tpkixcert;
if "%1"=="tpkixcertdel" goto tpkixcertdel;
if "%1"=="tpkcs10" goto tpkcs10; 
if "%1"=="tpkcs10del" goto tpkcs10del;
if "%1"=="tpkixcert_tef" goto tpkixcert_tef;
if "%1"=="tpkixcert_tefdel" goto tpkixcert_tefdel;
goto exit


:init
REM create testresults directory
md c:\testresults

REM copy hosts file to redirect cam-ocspserver request to ban-ocspserver
md c:\private
md c:\private\10000882
copy z:\hosts c:\private\10000882\hosts
goto exit;

:auto
REM install automated commdb
copy z:\auto.cfg c:\auto.cfg
goto exit;

:cryptohash
copy /s z:\thash\ c:\thash\
goto exit;

:cryptohashdel
del /s c:\thash\
goto exit;

:tpadding
copy /s z:\tpadding\ c:\tpadding\
goto exit;

:tpaddingdel
del /s c:\tpadding\
goto exit;

:tcryptospi
copy /s z:\testdata\tcryptospi\ c:\testdata\tcryptospi\
goto exit;

:tcryptospidel
del /s c:\testdata\tcryptospi\
goto exit;

:tasymmetric
md c:\tasymmetric
copy /s z:\tasymmetric\ c:\tasymmetric\
goto exit;

:tsymmetric
md c:\tsymmetric
copy /s z:\tsymmetric\ c:\tsymmetric\
goto exit;

:tbigint
md c:\tbigint
copy /s z:\tbigint\ c:\tbigint\
goto exit;

:tasymmetricdel
del /s c:\tasymmetric\
goto exit;

:tsymmetricdel
del c:\tsymmetric\
goto exit;

:tbigintdel
del c:\tbigint\
goto exit;

:tpkcs5kdf
md c:\tpkcs5kdf
copy z:\tpkcs12kdftests.script c:\tpkcs5kdf\tpkcs12kdftests.script
goto exit;

:tpkcs5kdfdel
del /s c:\tpkcs5kdf\
goto exit;

:tpbe
md c:\tpbe
copy z:\tpbe\*.dat c:\tpbe
attrib c:\tpbe\strong.dat -r
attrib c:\tpbe\weak.dat -r
goto exit;

:tpbedel
del /s c:\tpbe\
goto exit;

:trecog
md c:\trecog
copy /s z:\trecog\* c:\trecog
goto exit;

:trecogdel
del c:\trecog\
goto exit;

:tkeystore
copy /s z:\tkeystore\ c:\tkeystore\
copy /s z:\tcertstore\ c:\tcertstore\
copy /s z:\ttesttools\ c:\ttesttools\
copy /s z:\system\data\test1certstore.dat c:\system\data\test1certstore.dat
copy /s c:\ttesttools\data\certclients.dat c:\private\101f72a6\certclients.dat
goto exit;

:tkeystoredel
del c:\private\101f72a6\certclients.dat
del c:\system\data\test1certstore.dat
del /s c:\ttesttools\
del /s c:\tcertstore\
del /s c:\tkeystore\
goto exit;

:tcertapps
copy /s z:\tcertapps\ c:\tcertapps\
goto exit;

:tcertappsdel
del /s c:\tcertapps\
goto exit;

:t_keystore
copy /s z:\tkeystore\ c:\tkeystore\
goto exit;

:t_keystoredel
del /s c:\tkeystore\
goto exit;

:tcaf
md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
goto exit;

:tcafdel
del c:\tcaf\*.*
goto exit;

:rta
md c:\rta
copy /s z:\rta\* c:\rta
goto exit;

:rtadel
del c:\rta\*.*
goto exit;

:tscaf
md c:\tcaf
md c:\tcaf\tscaf
copy /s z:\tcaf\tscaf\* c:\tcaf\tscaf
goto exit;

:tscafdel
del c:\tcaf\tscaf\*.*
goto exit;

:mtputils
md c:\tcaf
md c:\tcaf\tmtputils
copy /s z:\tcaf\tmtputils\* c:\tcaf\tmtputils
goto exit;

:mtputilsdel
del c:\tcaf\tmtputils\*.*
goto exit;

:tpkcs12intg
copy /s z:\tpkcs12intg\ c:\tpkcs12intg\
goto exit;

:tpkcs12intgdel
del /s c:\tpkcs12intg
goto exit;

:tasnpkcs
md c:\tkeystore
md c:\tkeystore\data
copy /s z:\tkeystore\data\ c:\tkeystore\data\
copy z:\tasnpkcstests.script c:\tasnpkcstests.script
goto exit;

:tasnpkcsdel
del c:\tasnpkcstests.script
del /s c:\tkeystore\data\
goto exit;

:tpkcs7
md c:\tpkcs7
copy /s z:\tpkcs7\ c:\tpkcs7\
goto exit;

:tpkcs7del
del /s c:\tpkcs7\
goto exit;

:certstore
copy /s z:\tcertstore\ c:\tcertstore\
copy z:\certappmanager.txt c:\certappmanager.txt
copy /s z:\unifiedcertstore2\ c:\unifiedcertstore2\
copy z:\tcertstore\data\TestKeyStore.txt c:\system\data\testkeystore.txt
goto exit;

:cacertsdel
del c:\system\data\cacerts.dat
goto exit;

:certclientdel
del c:\system\data\certclients.dat
goto exit;

:certstoredel
rem deleting data on c: to save room and leave slots in root folder
del /s c:\unifiedcertstore2\
del /s c:\tcertstore\
del c:\certappmanager.txt
goto exit;


:tocsp
rem copy tocsp test data
md c:\system
md c:\system\tocsp
copy /s z:\system\tocsp\ c:\system\tocsp\
goto exit;

:tocspdel
del c:\system\tocsp\
goto exit;


:tx509
copy /s z:\tx509\ c:\tx509\
goto exit;

:tx509del
del /s c:\tx509\
goto exit;

:tasn1
copy /s z:\tasn1\ c:\tasn1\
goto exit;

:tasn1del
del /s c:\tasn1\
goto exit;

:twtlscert
copy /s z:\twtlscert\ c:\twtlscert\
goto exit;

:twtlscertdel
del /s c:\twtlscert\
goto exit;

:tpkixcert
copy /s z:\tpkixcert\ c:\tpkixcert\
copy /s z:\pkixtestdata\ c:\pkixtestdata\
goto exit;

:tpkixcertdel
del c:\pkixtestdata\*.*
goto exit;

:tpkcs10
copy /s z:\tpkcs10\ c:\tpkcs10\
goto exit;

:tpkcs10del
del /s c:\tpkcs10
goto exit;

:tpkixcert_tef
copy /s z:\tpkixcert_tef\ c:\tpkixcert_tef\
goto exit;

:tpkixcert_tefdel
del /s c:\tpkixcert_tef
goto exit;


:exit


 
