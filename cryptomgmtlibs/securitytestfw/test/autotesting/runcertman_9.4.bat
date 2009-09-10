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

rem run tcertstore tests
t_certstore c:\tcertstore\scripts\unifiedcertstore1-conf1.txt c:\testresults\unifiedcertstore1-conf1.txt
move c:\testresults\unifiedcertstore1-conf1.txt e:\testresults\unifiedcertstore1-conf1.txt

del c:\system\data\cacerts.dat
del c:\system\data\certclients.dat
t_certstore c:\tcertstore\scripts\unifiedcertstore2-conf1.txt c:\testresults\unifiedcertstore2-conf1.txt
move c:\testresults\unifiedcertstore2-conf1.txt e:\testresults\unifiedcertstore2-conf1.txt

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\unifiedcertstore3-conf1.txt c:\testresults\unifiedcertstore3-conf1.txt
move c:\testresults\unifiedcertstore3-conf1.txt e:\testresults\unifiedcertstore3-conf1.txt

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\dsa.txt c:\testresults\tcertstore-dsa.txt
move c:\testresults\tcertstore-dsa.txt e:\testresults\tcertstore-dsa.txt

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\reload.txt c:\testresults\tcertstore-reload.txt
move c:\testresults\tcerstore-reload.txt e:\testresults\tcertstore-reload.txt

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\apipolicing.txt c:\testresults\tcertstore-apipolicing.txt
move c:\testresults\tcertstore-apipolicing.txt e:\testresults\tcertstore-apipolicing.txt

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\filecertstore_errors.txt c:\testresults\tcertstore-errors.txt
move c:\testresults\tcertstore-errors.txt e:\testresults\tcertstore-errors.txt

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\swicertstore1.txt c:\testresults\swicertstore.txt
move c:\testresults\swicertstore.txt e:\testresults\swicertstore.txt

t_certstore c:\tcertstore\scripts\swicertstore4.txt c:\testresults\swicertstore4.txt
move c:\testresults\swicertstore4.txt e:\testresults\swicertstore4.txt

del c:\system\data\cacerts.dat
t_certstore c:\tcertstore\scripts\wri-swicertstore1.txt c:\testresults\wri-swicertstore.txt
move c:\testresults\wri-swicertstore.txt e:\testresults\wri-swicertstore.txt

del c:\system\data\cacerts.dat


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
tocsp c:\system\tocsp\scripts\openssl.txt c:\testresults\tocsp_openssl.log
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\


tocsp c:\system\tocsp\scripts\xcert-local.txt c:\testresults\tocsp_xcert_local.log
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\


tocsp c:\system\tocsp\scripts\openssl-local.txt c:\testresults\tocsp_openssl_local.log
del /s c:\system\tocsp\
copy /s z:\system\tocsp\ c:\system\tocsp\

tocsp c:\system\tocsp\scripts\error.txt c:\testresults\tocsp_error.log

tocsp c:\system\tocsp\scripts\defaultpolicy.txt c:\testresults\tocsp_defaultpolicy.log

tocsp c:\system\tocsp\scripts\httpinterface.txt c:\testresults\tocsp_httpinterface.log

rem copy test results to mmc card
move c:\testresults\tocsp_openssl.log e:\testresults\tocsp_openssl.log
move c:\testresults\tocsp_xcert_local.log e:\testresults\tocsp_xcert_local.log
move c:\testresults\tocsp_openssl_local.log e:\testresults\tocsp_openssl_local.log
move c:\testresults\tocsp_error.log e:\testresults\tocsp_error.log
move c:\testresults\tocsp_defaultpolicy.log e:\testresults\tocsp_defaultpolicy.log
move c:\testresults\tocsp_httpinterface.log e:\testresults\tocsp_httpinterface.log

rem deleting data on c: to save room and leave slots in root folder
del /s c:\system\tocsp\
attrib c:\*.* -r
del c:\*.*

REM Test the JMI CenRep's production version initialization file
tjmicenrep
move c:\jmicenreptest.log e:\testresults\jmicenreptest.log

rem TX509
rem copy tx509 test data
copy /s z:\tx509\ c:\tx509\

rem run tx509 tests
tx509 \TX509\SCRIPTS\TX509.TXT c:\testresults\tx509.log
rem copy results to mmc card 
move c:\testresults\tx509.log e:\testresults\tx509.log
rem run tx509 dev certs tests
tx509 \TX509\SCRIPTS\TX509_DEV_CERTS.TXT c:\testresults\tx509_dev_certs.log
rem copy results to mmc card
move c:\testresults\tx509_dev_certs.log e:\testresults\tx509_dev_certs.log
rem tidy up
del /s c:\tx509\
attrib c:\*.* -r
del c:\*.*

rem TASN1
rem run tasn1 tests
copy /s z:\tasn1\ c:\tasn1\
tasn1 c:\tasn1\scripts\tasn1.txt c:\testresults\tasn1_log.txt
rem copy test results to mmc card
move c:\testresults\tasn1_log.txt e:\testresults\TASN1_log.txt
del /s c:\tasn1\


rem TWTLSCERT
rem copy twtlscert test data
copy /s z:\twtlscert\ c:\twtlscert\

rem run twtlscert tests
twtlscert c:\twtlscert\scripts\twtlscert.txt c:\testresults\twtlscert.log
twtlscert c:\twtlscert\scripts\twtlscert1.txt c:\testresults\twtlscert1.log
twtlscert c:\twtlscert\scripts\twtlscert2.txt c:\testresults\twtlscert2.log
twtlscert c:\twtlscert\scripts\twtlscert3.txt c:\testresults\twtlscert3.log

rem copy test results to mmc card 
move c:\testresults\twtlscert.log e:\testresults\twtlscert.log
move c:\testresults\twtlscert1.log e:\testresults\twtlscert1.log
move c:\testresults\twtlscert2.log e:\testresults\twtlscert2.log
move c:\testresults\twtlscert3.log e:\testresults\twtlscert3.log

del /s c:\twtlscert\
attrib c:\*.* -r
del c:\*.*

rem TPKIXCERT
rem copy tpkixcert test data
copy /s z:\tpkixcert\ c:\tpkixcert\
copy /s z:\pkixtestdata\ c:\pkixtestdata\

rem run tpkixcert tests
tpkixcert c:\tpkixcert\scripts\tpkixtest_part1.txt c:\testresults\tpkixtest1.txt
tpkixcert c:\tpkixcert\scripts\tpkixtest_part2.txt c:\testresults\tpkixtest2.txt
tpkixcert c:\tpkixcert\scripts\cancel1.txt c:\testresults\cancel1.txt

rem copy test results to mmc card 
move c:\testresults\tpkixtest1.txt e:\testresults\tpkixtest1.txt
move c:\testresults\tpkixtest2.txt e:\testresults\tpkixtest2.txt
move c:\testresults\cancel1.txt e:\testresults\tpkixcancel1.txt

attrib c:\*.* -r
del c:\*.*

REM tpkcs10_v2 - Uses TEF
copy /s z:\tpkcs10\ c:\tpkcs10\
testexecute c:\tpkcs10\scripts\tpkcs10_v2.script
move c:\logs\testexecute\tpkcs10_v2.htm e:\testresults\tpkcs10_v2.htm
del /s c:\tpkcs10
attrib c:\*.* -r
del c:\*.*

del c:\system\data\cacerts.dat
del c:\system\data\certclients.dat


 
