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

REM Attempt to stop the execution in case it's accidentally started on a PC
ECHO About to run Security Team Hardware Tests
pause

REM create testresults directory
md c:\testresults
md e:\testresults

REM copy hosts file to redirect cam-ocspserver request to ban-ocspserver
md c:\private
md c:\private\10000882
copy z:\hosts c:\private\10000882\hosts

REM install automated commdb
copy z:\auto.cfg c:\auto.cfg
ced -i c:\auto.cfg

sntpclient -savings cbtime01.symbian.intra bantime01.symbian.intra


REM hash
copy /s z:\thash\ c:\thash\
thash
move c:\hashlog.txt c:\testresults\
move c:\testresults\hashlog.txt e:\testresults\hashlog.txt
del /s c:\thash\

REM random
t_random
move c:\t_random.log c:\testresults\
move c:\testresults\t_random.log e:\testresults\t_random.log

REM padding - uses testexecute framework
copy /s z:\tpadding\ c:\tpadding\
testexecute c:\tpadding\tpadding.script
move c:\logs\testexecute\tpadding.htm e:\testresults\tpadding.htm
del /s c:\tpadding\

REM new cryptospi tests
copy /s z:\testdata\tcryptospi\ c:\testdata\tcryptospi\
testexecute z:\testdata\tcryptospi\scripts\tcryptospi.script
move c:\logs\testexecute\tcryptospi.htm e:\testresults\tcryptospi.htm
del /s c:\testdata\tcryptospi\

REM crypto, asymmetric

REM tasymmetric z:\tasymmetric\asymmetricPerformance.txt c:\testresults\asymmetricPerformance.log

tasymmetric z:\tasymmetric\tasymmetrictests.script c:\logs\testexecute\tasymmetrictests.htm
move c:\logs\testexecute\tasymmetrictests.htm e:\testresults\tasymmetrictests.htm

REM crypto, symmetric

tsymmetric z:\tsymmetric\tsymmetrictests.script c:\logs\testexecute\tsymmetrictests.htm
move c:\logs\testexecute\tsymmetrictests.htm e:\testresults\tsymmetrictests.htm

tsymmetric z:\tsymmetric\AESECBVectorsScript.script c:\logs\testexecute\AESECBVectorsScript.htm
move c:\logs\testexecute\AESECBVectorsScript.htm e:\testresults\AESECBVectorsScript.htm

tsymmetric z:\tsymmetric\AESECB_KAT_VT.script c:\logs\testexecute\AESECB_KAT_VT.htm
move c:\logs\testexecute\AESECB_KAT_VT.htm e:\testresults\AESECB_KAT_VT.htm

tsymmetric z:\tsymmetric\AESECB_KAT_VK.script c:\logs\testexecute\AESECB_KAT_VK.htm
move c:\logs\testexecute\AESECB_KAT_VK.htm e:\testresults\AESECB_KAT_VK.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB128.script c:\logs\testexecute\AESMonteCarloEncryptECB128.htm
move c:\logs\testexecute\AESMonteCarloEncryptECB128.htm e:\testresults\AESMonteCarloEncryptECB128.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB192.script c:\logs\testexecute\AESMonteCarloEncryptECB192.htm
move c:\logs\testexecute\AESMonteCarloEncryptECB192.htm e:\testresults\AESMonteCarloEncryptECB192.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB256.script c:\logs\testexecute\AESMonteCarloEncryptECB256.htm
move c:\logs\testexecute\AESMonteCarloEncryptECB256.htm e:\testresults\AESMonteCarloEncryptECB256.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB128.script c:\logs\testexecute\AESMonteCarloDecryptECB128.htm
move c:\logs\testexecute\AESMonteCarloDecryptECB128.htm e:\testresults\AESMonteCarloDecryptECB128.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB192.script c:\logs\testexecute\AESMonteCarloDecryptECB192.htm
move c:\logs\testexecute\AESMonteCarloDecryptECB192.htm e:\testresults\AESMonteCarloDecryptECB192.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB256.script c:\logs\testexecute\AESMonteCarloDecryptECB256.htm
move c:\logs\testexecute\AESMonteCarloDecryptECB256.htm e:\testresults\AESMonteCarloDecryptECB256.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC128.script c:\logs\testexecute\AESMonteCarloEncryptCBC128.htm
move c:\logs\testexecute\AESMonteCarloEncryptCBC128.htm e:\testresults\AESMonteCarloEncryptCBC128.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC192A.script c:\logs\testexecute\AESMonteCarloEncryptCBC192A.htm
move c:\logs\testexecute\AESMonteCarloEncryptCBC192A.htm e:\testresults\AESMonteCarloEncryptCBC192A.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC192B.script c:\logs\testexecute\AESMonteCarloEncryptCBC192B.htm
move c:\logs\testexecute\AESMonteCarloEncryptCBC192B.htm e:\testresults\AESMonteCarloEncryptCBC192B.htm

tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC256.script c:\logs\testexecute\AESMonteCarloEncryptCBC256.htm
move c:\logs\testexecute\AESMonteCarloEncryptCBC256.htm e:\testresults\AESMonteCarloEncryptCBC256.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC128.script c:\logs\testexecute\AESMonteCarloDecryptCBC128.htm
move c:\logs\testexecute\AESMonteCarloDecryptCBC128.htm e:\testresults\AESMonteCarloDecryptCBC128.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC192.script c:\logs\testexecute\AESMonteCarloDecryptCBC192.htm
move c:\logs\testexecute\AESMonteCarloDecryptCBC192.htm e:\testresults\AESMonteCarloDecryptCBC192.htm

tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC256.script c:\logs\testexecute\AESMonteCarloDecryptCBC256.htm
move c:\logs\testexecute\AESMonteCarloDecryptCBC256.htm e:\testresults\AESMonteCarloDecryptCBC256.htm



REM crypto, bigint

tbigint z:\tbigint\tconstructiontests.script c:\logs\testexecute\tconstruction.htm
move c:\logs\testexecute\tconstruction.htm e:\testresults\tconstruction.htm

tbigint z:\tbigint\tbasicmathstests.script c:\logs\testexecute\tbasicmaths.htm
move c:\logs\testexecute\tbasicmaths.htm e:\testresults\tbasicmaths.htm

tbigint z:\tbigint\tmontgomerytests.script c:\logs\testexecute\tmontgomery.htm
move c:\logs\testexecute\tmontgomery.htm e:\testresults\tmontgomery.htm

REM tbigint z:\tbigint\tperformancetests.script c:\logs\testexecute\tperformance.htm
REM move c:\logs\testexecute\tperformance.htm e:\testresults\tperformance.htm

tbigint z:\tbigint\tprimetests.script c:\logs\testexecute\tprime.htm
move c:\logs\testexecute\tprime.htm e:\testresults\tprime.htm

REM tbigint z:\tbigint\tprimetests2.script c:\logs\testexecute\tprime2.htm
REM move c:\logs\testexecute\tprime2.htm e:\testresults\tprime2.htm

tbigint z:\tbigint\tprimefailuretests.script  c:\logs\testexecute\tprimefailure.htm
move c:\logs\testexecute\tprimefailure.htm e:\testresults\tprimefailure.htm

tbigint z:\tbigint\tprimegen.script c:\logs\testexecute\tprimegen.htm
move c:\logs\testexecute\tprimegen.htm e:\testresults\tprimegen.htm



REM crypto, tpkcs5kdf
md c:\tpkcs5kdf

copy z:\tpkcs12kdftests.script c:\tpkcs5kdf\tpkcs12kdftests.script
tpkcs5kdf c:\tpkcs5kdf\tpkcs12kdftests.script c:\tpkcs5kdf\tpkcs12kdftests.htm
move c:\tpkcs5kdf\tpkcs12kdftests.htm e:\testresults\tpkcs12kdftests.htm
del /s c:\tpkcs5kdf\


REM crypto, pbe

md c:\tpbe
copy z:\tpbe\*.dat c:\tpbe
attrib c:\tpbe\strong.dat -r
attrib c:\tpbe\weak.dat -r
tpbe z:\tpbe\tpbetests_v2.script c:\logs\testexecute\tpbetestsv2.htm
move c:\logs\testexecute\tpbetestsv2.htm e:\testresults\tpbetestsv2.htm

del /s c:\tpbe\


REM Run recognizer tests
md c:\trecog
copy /s z:\trecog\* c:\trecog

testexecute z:\trecog\trecogcert.script
move c:\logs\testexecute\trecogcert.htm e:\testresults\trecogcert.htm

testexecute z:\trecog\trecogpkcs12.script
move c:\logs\testexecute\trecogpkcs12.htm e:\testresults\trecogpkcs12.htm

REM Enable the following test when DEF076875(or relevant CR) is done.
REM runtlsclientauth.bat


copy /s z:\tkeystore\ c:\tkeystore\
copy /s z:\tcertstore\ c:\tcertstore\
copy /s z:\ttesttools\ c:\ttesttools\
copy /s z:\system\data\test1certstore.dat c:\system\data\test1certstore.dat
copy /s c:\ttesttools\data\certclients.dat c:\private\101f72a6\certclients.dat

TESTEXECUTE c:\ttesttools\scripts\sec-filetokens-tools.script
move c:\logs\testexecute\sec-filetokens-tools.htm e:\testresults\sec-filetokens-tools.htm

del c:\private\101f72a6\certclients.dat
del c:\system\data\test1certstore.dat

del /s c:\ttesttools\
del /s c:\tcertstore\
del /s c:\tkeystore\

rem TCERTAPPS
copy /s z:\tcertapps\ c:\tcertapps\
t_certapps c:\tcertapps\scripts\script1.script c:\logs\testexecute\tcertapps.htm
move c:\logs\testexecute\tcertapps.htm e:\testresults\tcertapps.htm
t_certapps c:\tcertapps\scripts\apipolicing.script c:\logs\testexecute\tcertapps-apipolicing.htm
move c:\logs\testexecute\tcertapps-apipolicing.htm e:\testresults\tcertapps-apipolicing.htm
del /s c:\tcertapps\


REM t_keystore
copy /s z:\tkeystore\ c:\tkeystore\
t_keystore c:\tkeystore\scripts\tkeystore.script c:\logs\testexecute\tkeystore.htm
move c:\logs\testexecute\tkeystore.htm e:\testresults\tkeystore.htm
t_keystore c:\tkeystore\scripts\authobjects.script c:\logs\testexecute\tkeystore_authobjects.htm
move c:\logs\testexecute\tkeystore_authobjects.htm e:\testresults\tkeystore_authobjects.htm
t_keystore c:\tkeystore\scripts\exportscript.script c:\logs\testexecute\tkeystore_export.htm
move c:\logs\testexecute\tkeystore_export.htm e:\testresults\tkeystore_export.htm
t_keystore c:\tkeystore\scripts\reload.script c:\logs\testexecute\tkeystore_reload.htm
move c:\logs\testexecute\tkeystore_reload.htm e:\testresults\tkeystore_reload.htm
t_keystore c:\tkeystore\scripts\oldserver_newfeature.script c:\logs\testexecute\oldserver_newfeature.htm
move c:\logs\testexecute\oldserver_newfeature.htm e:\testresults\oldserver_newfeature.htm
del /s c:\tkeystore\



md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\tcaf.script
move c:\logs\testexecute\tcaf.htm e:\testresults\tcaf.htm
del c:\tcaf\*.*

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\tcafnodrmcap.script
move c:\logs\testexecute\tcafnodrmcap.htm e:\testresults\tcafnodrmcap.htm
del c:\tcaf\*.*

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\drmfileperformance_armv5.script
move c:\logs\testexecute\drmfileperformance_armv5.htm e:\testresults\drmfileperformance_armv5.htm
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

md c:\rta
copy /s z:\rta\* c:\rta
testexecute c:\rta\rta_64bit.script
move c:\logs\testexecute\rta_64bit.htm e:\testresults\rta_64bit.htm
del c:\rta\*.*

md c:\tcaf
copy /s z:\tcaf\* c:\tcaf
testexecute c:\tcaf\consumerstep_64bit.script
move c:\logs\testexecute\consumerstep_64bit.htm e:\testresults\consumerstep_64bit.htm


md e:\testresults\
testexecute z:\tups\tupsdb\scripts\tupsdb.script 
move c:\logs\testexecute\tupsdb.htm e:\testresults\tupsdb.htm

testexecute z:\tups\tupsdb\scripts\tupsdbperformance.script 
move c:\logs\testexecute\tupsdbperformance.htm e:\testresults\tupsdbperformance.htm

rem UPS policy library unit tests
testexecute z:\tups\tpolicies\scripts\tpolicycache.script
move c:\logs\testexecute\tpolicycache.htm e:\testresults\tpolicycache.htm
testexecute z:\tups\tpolicies\scripts\tpolicyreader.script
move c:\logs\testexecute\tpolicyreader.htm e:\testresults\tpolicyreader.htm
testexecute z:\tups\tpolicies\scripts\tserviceconfig.script
move c:\logs\testexecute\tserviceconfig.htm e:\testresults\tserviceconfig.htm

rem ups integration tests
rem Creates temporary folder for backup and restore tests.
md c:\tswi
md c:\tswi\tbackuprestore

testexecute z:\tups\integ\scripts\tups_integ.script
move c:\logs\testexecute\tups_integ.htm e:\testresults\tups_integ.htm

tmsgapp.exe
move c:\tmsgapp.log e:\testresults\tmsgapp.log
del c:\tmsgapp.log

attrib c:\logs\testexecute\*.* -r
del c:\logs\testexecute\*.*

rem Deletes temporary folder for backup and restore tests.
attrib c:\tswi\*.* -r
del c:\tswi\*.*

copy /s z:\tpkcs12intg\ c:\tpkcs12intg\
testexecute c:\tpkcs12intg\data\tpkcs12lib.script
move c:\logs\testexecute\tpkcs12lib.htm e:\testresults\tpkcs12lib.htm
del /s c:\tpkcs12intg
attrib c:\*.* -r
del c:\*.*

md c:\tkeystore
md c:\tkeystore\data
copy /s z:\tkeystore\data\ c:\tkeystore\data\
copy z:\tasnpkcstests.script c:\tasnpkcstests.script
tasnpkcs c:\tasnpkcstests.script c:\logs\testexecute\tasnpkcstests.htm
move c:\logs\testexecute\tasnpkcstests.htm e:\testresults\tasnpkcstests.htm
del c:\tasnpkcstests.script
del /s c:\tkeystore\data\
attrib c:\*.* -r
del c:\*.*

t_ctframework
copy c:\T_CTFrameworkLog.txt e:\testresults\T_CTFrameworkLog.txt
move T_CTFrameworkLog.txt c:\testresults\

copy /s z:\tpkcs7\ c:\tpkcs7\
testexecute c:\tpkcs7\tpkcs7_v2.script
move c:\logs\testexecute\tpkcs7_v2.htm e:\testresults\tpkcs7_v2.htm
testexecute c:\tpkcs7\tcms.script
move c:\logs\testexecute\tcms.htm e:\testresults\tcms.htm
testexecute c:\tpkcs7\tinteg_cms.script
move c:\logs\testexecute\tinteg_cms.htm e:\testresults\tinteg_cms.htm
del /s c:\tpkcs7\

rem TCERTSTORE
rem copy tcertstore test data
copy /s z:\tcertstore\ c:\tcertstore\
copy z:\certappmanager.txt c:\certappmanager.txt
copy /s z:\unifiedcertstore2\ c:\unifiedcertstore2\
copy z:\tcertstore\data\TestKeyStore.txt c:\system\data\testkeystore.txt
del c:\system\data\cacerts.dat

del c:\system\data\certclients.dat
t_certstore c:\tcertstore\scripts\t_certstore_sha2.script c:\testresults\t_certstore_sha2.htm
move c:\testresults\t_certstore_sha2.htm e:\testresults\t_certstore_sha2.htm

rem run tcertstore tests
t_certstore c:\tcertstore\scripts\unifiedcertstore1-conf1.script c:\logs\testexecute\unifiedcertstore1-conf1.htm
move c:\logs\testexecute\unifiedcertstore1-conf1.htm e:\testresults\unifiedcertstore1-conf1.htm

del c:\system\data\cacerts.dat
del c:\system\data\certclients.dat
t_certstore c:\tcertstore\scripts\unifiedcertstore2-conf1.script c:\logs\testexecute\unifiedcertstore2-conf1.htm
move c:\logs\testexecute\unifiedcertstore2-conf1.htm e:\testresults\unifiedcertstore2-conf1.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\unifiedcertstore3-conf1.script c:\logs\testexecute\unifiedcertstore3-conf1.htm
move c:\logs\testexecute\unifiedcertstore3-conf1.htm e:\testresults\unifiedcertstore3-conf1.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\dsa.script c:\logs\testexecute\tcertstore-dsa.htm
move c:\logs\testexecute\tcertstore-dsa.htm e:\testresults\tcertstore-dsa.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\reload.script c:\logs\testexecute\tcertstore-reload.htm
move c:\logs\testexecute\tcertstore-reload.htm e:\testresults\tcertstore-reload.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\apipolicing.script c:\logs\testexecute\tcertstore-apipolicing.htm
move c:\logs\testexecute\tcertstore-apipolicing.htm e:\testresults\tcertstore-apipolicing.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\filecertstore_errors.script c:\logs\testexecute\tcertstore-errors.htm
move c:\logs\testexecute\tcertstore-errors.htm e:\testresults\tcertstore-errors.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\swicertstore1.script c:\logs\testexecute\swicertstore1.htm
move c:\logs\testexecute\swicertstore1.htm e:\testresults\swicertstore1.htm

t_certstore c:\tcertstore\scripts\swicertstore4.script c:\logs\testexecute\swicertstore4.htm
move c:\logs\testexecute\swicertstore4.htm e:\testresults\swicertstore4.htm

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\wri-swicertstore1.script c:\logs\testexecute\wri-swicertstore1.htm
move c:\logs\testexecute\wri-swicertstore1.htm e:\testresults\wri-swicertstore1.htm

del c:\system\data\cacerts.dat

REM Composite Certstore Configuration CR1393
del c:\private\101f72a6\cacerts.dat
del c:\private\101f72a6\certclients.dat
t_certstore c:\tcertstore\multiple_certstore\scripts\tfilecertstore14.script c:\logs\testexecute\tfilecertstore14.htm
move c:\logs\testexecute\tfilecertstore14.htm e:\testresults\tfilecertstore14.htm

del c:\private\101f72a6\cacerts.dat
del c:\private\101f72a6\certclients.dat

rem deleting data on c: to save room and leave slots in root folder
del /s c:\unifiedcertstore2\
del /s c:\tcertstore\
del c:\certappmanager.txt
attrib c:\*.* -r
del c:\*.*

rem TOCSP
rem copy tocsp test data
copy /s z:\system\tocsp\ c:\system\tocsp\
del c:\system\data\cacerts.dat

rem run tocsp tests
tocsp c:\system\tocsp\scripts\OPENSSL.script c:\logs\testexecute\TOCSP_openssl.htm
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\


tocsp c:\system\tocsp\scripts\XCERT-LOCAL.script c:\logs\testexecute\xcert_local.htm
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\


tocsp c:\system\tocsp\scripts\OPENSSL-LOCAL.script c:\logs\testexecute\xcert_openssl_local.htm
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\

tocsp c:\system\tocsp\scripts\error.script c:\logs\testexecute\ocsp_error.htm

TOCSP c:\SYSTEM\TOCSP\SCRIPTS\DefaultPolicy.script c:\logs\testexecute\TOCSP_DefaultPolicy.htm

rem copy test results to mmc card
move c:\logs\testexecute\TOCSP_openssl.htm e:\testresults\TOCSP_openssl.htm
move c:\logs\testexecute\xcert_local.htm e:\testresults\xcert_local.htm
move c:\logs\testexecute\xcert_openssl_local.htm e:\testresults\xcert_openssl_local.htm
move c:\logs\testexecute\ocsp_error.htm e:\testresults\ocsp_error.htm
move c:\logs\testexecute\TOCSP_DefaultPolicy.htm e:\testresults\TOCSP_DefaultPolicy.htm

rem deleting data on c: to save room and leave slots in root folder
del /s c:\system\tocsp\
attrib c:\*.* -r
del c:\*.*

rem TX509
rem copy tx509 test data
copy /s z:\tx509\ c:\tx509\

rem run tx509 tests
tx509 \TX509\SCRIPTS\TX509.script c:\logs\testexecute\tx509.htm
rem copy results to mmc card 
move c:\logs\testexecute\tx509.htm e:\testresults\tx509.htm
rem run tx509 distinguished name tests
tx509 \tx509\scripts\tx509-dnames.script c:\logs\testexecute\tx509_dnames.htm
rem copy results to mmc card
move c:\logs\testexecute\tx509_dnames.htm e:\testresults\tx509_dnames.htm
rem run tx509 dev certs tests
tx509 \TX509\SCRIPTS\TX509_DEV_CERTS.script c:\logs\testexecute\tx509_dev_certs.htm
rem copy results to mmc card
move c:\logs\testexecute\tx509_dev_certs.htm e:\testresults\tx509_dev_certs.htm
rem tidy up
del /s c:\tx509\
attrib c:\*.* -r
del c:\*.*

rem TASN1
rem run tasn1 tests
copy /s z:\tasn1\ c:\tasn1\
tasn1 c:\tasn1\scripts\tasn1.script c:\logs\testexecute\tasn1_log.htm
rem copy test results to mmc card
move c:\logs\testexecute\tasn1_log.htm e:\testresults\TASN1_log.htm
del /s c:\tasn1\


rem TWTLSCERT
rem copy twtlscert test data
copy /s z:\twtlscert\ c:\twtlscert\

rem run twtlscert tests
twtlscert c:\twtlscert\scripts\twtlscert.script c:\logs\testexecute\twtlscert.htm
twtlscert c:\twtlscert\scripts\twtlscert1.script c:\logs\testexecute\twtlscert1.htm
twtlscert c:\twtlscert\scripts\twtlscert2.script c:\logs\testexecute\twtlscert2.htm
twtlscert c:\twtlscert\scripts\twtlscert3.script c:\logs\testexecute\twtlscert3.htm

rem copy test results to mmc card 
move c:\logs\testexecute\twtlscert.htm e:\testresults\twtlscert.htm
move c:\logs\testexecute\twtlscert1.htm e:\testresults\twtlscert1.htm
move c:\logs\testexecute\twtlscert2.htm e:\testresults\twtlscert2.htm
move c:\logs\testexecute\twtlscert3.htm e:\testresults\twtlscert3.htm

del /s c:\twtlscert\
attrib c:\*.* -r
del c:\*.*

rem TPKIXCERT
rem copy tpkixcert test data
copy /s z:\tpkixcert\ c:\tpkixcert\
copy /s z:\pkixtestdata\ c:\pkixtestdata\

rem run tpkixcert tests
tpkixcert c:\tpkixcert\scripts\tpkixtest_part1.script c:\logs\testexecute\tpkixtest1.htm
tpkixcert c:\tpkixcert\scripts\tpkixtest_part2.script c:\logs\testexecute\tpkixtest2.htm
tpkixcert c:\tpkixcert\scripts\cancel1.script c:\logs\testexecute\cancel1.htm

rem copy test results to mmc card 
move c:\logs\testexecute\tpkixtest1.htm e:\testresults\tpkixtest1.htm
move c:\logs\testexecute\tpkixtest2.htm e:\testresults\tpkixtest2.htm
move c:\logs\testexecute\cancel1.htm e:\testresults\tpkixcancel1.htm

attrib c:\*.* -r
del c:\*.*

REM tpkcs10_v2 - Uses TEF
copy /s z:\tpkcs10\ c:\tpkcs10\
testexecute c:\tpkcs10\scripts\tpkcs10_v2.script
move c:\logs\testexecute\tpkcs10_v2.htm e:\testresults\tpkcs10_v2.htm
del /s c:\tpkcs10

REM tpkixcert_tef
copy /s z:\tpkixcert_tef\ c:\tpkixcert_tef\
testexecute c:\tpkixcert_tef\scripts\tpkixcert_tef.script
move c:\logs\testexecute\tpkixcert_tef.htm e:\testresults\tpkixcert_tef.htm
del /s c:\tpkixcert_tef

attrib c:\*.* -r
del c:\*.*

del c:\system\data\cacerts.dat
del c:\system\data\certclients.dat


 