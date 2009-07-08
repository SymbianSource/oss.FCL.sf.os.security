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
REM hash
copy /s z:\thash\ c:\thash\
thash
move c:\hashlog.txt c:\testresults\
move c:\testresults\hashlog.txt e:\testresults\hashlog.txt

REM random
t_random
move c:\t_random.log c:\testresults\
move c:\testresults\t_random.log e:\testresults\t_random.log


REM padding - uses testexecute framework
copy /s z:\tpadding\ c:\tpadding\
testexecute c:\tpadding\tpadding.script
move c:\logs\testexecute\tpadding.htm e:\testresults\tpadding.htm
del /s c:\tpadding\

REM crypto, asymmetric

tasymmetric z:\tasymmetric\tasymmetrictests.txt c:\testresults\tasymmetrictests.log
REM tasymmetric z:\tasymmetric\asymmetricPerformance.txt c:\testresults\asymmetricPerformance.log

move c:\testresults\tasymmetrictests.log e:\testresults\tasymmetrictests.log

REM crypto, symmetric

tsymmetric z:\tsymmetric\tsymmetrictests.txt c:\testresults\tsymmetrictests.log
tsymmetric z:\tsymmetric\AESECBVectorsScript.txt c:\testresults\AESECBVectorsScript.log
tsymmetric z:\tsymmetric\AESECB_KAT_VT.txt c:\testresults\AESECB_KAT_VT.log
tsymmetric z:\tsymmetric\AESECB_KAT_VK.txt c:\testresults\AESECB_KAT_VK.log
tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB128.txt c:\testresults\AESMonteCarloEncryptECB128.log
tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB192.txt c:\testresults\AESMonteCarloEncryptECB192.log
tsymmetric z:\tsymmetric\AESMonteCarloEncryptECB256.txt c:\testresults\AESMonteCarloEncryptECB256.log
tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB128.txt c:\testresults\AESMonteCarloDecryptECB128.log
tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB192.txt c:\testresults\AESMonteCarloDecryptECB192.log
tsymmetric z:\tsymmetric\AESMonteCarloDecryptECB256.txt c:\testresults\AESMonteCarloDecryptECB256.log
tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC128.txt c:\testresults\AESMonteCarloEncryptCBC128.log
tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC192.txt c:\testresults\AESMonteCarloEncryptCBC192.log
tsymmetric z:\tsymmetric\AESMonteCarloEncryptCBC256.txt c:\testresults\AESMonteCarloEncryptCBC256.log
tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC128.txt c:\testresults\AESMonteCarloDecryptCBC128.log
tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC192.txt c:\testresults\AESMonteCarloDecryptCBC192.log
tsymmetric z:\tsymmetric\AESMonteCarloDecryptCBC256.txt c:\testresults\AESMonteCarloDecryptCBC256.log


rem copy logs to mmc card
move c:\testresults\tsymmetrictests.log e:\testresults\tsymmetrictests.log
move c:\testresults\AESECBVectorsScript.log e:\testresults\AESECBVectorsScript.log
move c:\testresults\AESECB_KAT_VT.log e:\testresults\AESECB_KAT_VT.log
move c:\testresults\AESECB_KAT_VK.log e:\testresults\AESECB_KAT_VK.log
move c:\testresults\AESMonteCarloEncryptECB128.log e:\testresults\AESMonteCarloEncryptECB128.log
move c:\testresults\AESMonteCarloEncryptECB192.log e:\testresults\AESMonteCarloEncryptECB192.log
move c:\testresults\AESMonteCarloEncryptECB256.log e:\testresults\AESMonteCarloEncryptECB256.log
move c:\testresults\AESMonteCarloDecryptECB128.log e:\testresults\AESMonteCarloDecryptECB128.log
move c:\testresults\AESMonteCarloDecryptECB192.log e:\testresults\AESMonteCarloDecryptECB192.log
move c:\testresults\AESMonteCarloDecryptECB256.log e:\testresults\AESMonteCarloDecryptECB256.log
move c:\testresults\AESMonteCarloEncryptCBC128.log e:\testresults\AESMonteCarloEncryptCBC128.log
move c:\testresults\AESMonteCarloEncryptCBC192.log e:\testresults\AESMonteCarloEncryptCBC192.log
move c:\testresults\AESMonteCarloEncryptCBC256.log e:\testresults\AESMonteCarloEncryptCBC256.log
move c:\testresults\AESMonteCarloDecryptCBC128.log e:\testresults\AESMonteCarloDecryptCBC128.log
move c:\testresults\AESMonteCarloDecryptCBC192.log e:\testresults\AESMonteCarloDecryptCBC192.log
move c:\testresults\AESMonteCarloDecryptCBC256.log e:\testresults\AESMonteCarloDecryptCBC256.log

REM crypto, bigint

tbigint z:\tbigint\tconstructiontests.txt c:\testresults\tconstruction.log
tbigint z:\tbigint\tbasicmathstests.txt c:\testresults\tbasicmaths.log
tbigint z:\tbigint\tmontgomerytests.txt c:\testresults\tmontgomery.log
REM tbigint z:\tbigint\tperformancetests.txt c:\testresults\tperformance.log
tbigint z:\tbigint\tprimetests.txt c:\testresults\tprime.log
REM tbigint z:\tbigint\tprimetests2.txt c:\testresults\tprime2.log
tbigint z:\tbigint\tprimefailuretests.txt  c:\testresults\tprimefailure.log
tbigint z:\tbigint\tprimegen.txt c:\testresults\tprimegen.log

rem copy logs to mmc card
move c:\testresults\tconstruction.log e:\testresults\tconstruction.log
move c:\testresults\tbasicmaths.log e:\testresults\tbasicmaths.log
move c:\testresults\tmontgomery.log e:\testresults\tmontgomery.log
REM move c:\testresults\tperformance.log e:\testresults\tperformance.log
move c:\testresults\tprime.log e:\testresults\tprime.log
REM move c:\testresults\tprime2.log e:\testresults\tprime2.log
move c:\testresults\tprimefailure.log e:\testresults\tprimefailure.log
move c:\testresults\tprimegen.log e:\testresults\tprimegen.log

REM crypto, tpkcs5kdf
md c:\tpkcs5kdf

copy z:\tpkcs12kdftests.txt c:\tpkcs5kdf\tpkcs12kdftests.txt
tpkcs5kdf c:\tpkcs5kdf\tpkcs12kdftests.txt c:\tpkcs5kdf\tpkcs12kdftests.log
copy c:\tpkcs5kdf\tpkcs12kdftests.log e:\testresults\tpkcs12kdftests.log
del /s c:\tpkcs5kdf\

REM crypto, pbe

md c:\tpbe
copy z:\tpbe\*.dat c:\tpbe
attrib c:\tpbe\strong.dat -r
attrib c:\tpbe\weak.dat -r
tpbe z:\tpbe\tpbetests_v2.txt c:\testresults\tpbetestsv2.log
move c:\testresults\tpbetestsv2.log e:\testresults\tpbetestsv2.log
del /s c:\tpbe\
