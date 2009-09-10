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
copy /s z:\tJarDownloader\ c:\tJarDownloader\
copy z:\certclients.dat c:\system\data\certclients.dat
copy z:\cacerts.dat c:\system\data\cacerts.dat
attrib c:\system\data\cacerts.dat -r
attrib c:\system\data\certclients.dat -r

rem run tJarDownloader tests
tJarDownloader c:\tJarDownloader\Scripts\script1.txt c:\testresults\tjardownloader.log
move c:\testresults\tjardownloader.log e:\testresults\tjardownloader.log
del /s c:\tJarDownloader\
del /s c:\tjartest\
del /s c:\private\200008d1\

rem ITHARNESSMIDP
rem copy test data
copy /s z:\midpfiles\ c:\midpfiles\
copy /s z:\itharnessmidp\ c:\itharnessmidp\
copy z:\javatest.txt c:\javatest.txt
copy z:\javatest2.txt c:\javatest2.txt
copy z:\javatest3.txt c:\javatest3.txt
copy z:\leavetest.txt c:\leavetest.txt
copy z:\otatests.txt c:\otatests.txt
copy z:\javaiaptest.txt c:\javaiaptest.txt
attrib c:\system\data\cacerts.dat -r
attrib c:\system\data\certclients.dat -r
copy z:\cacerts.dat c:\system\data\cacerts.dat
copy z:\certclients.dat c:\system\data\certclients.dat
attrib c:\system\data\cacerts.dat -r
attrib c:\system\data\certclients.dat -r


rem run ITHARNESSMIDP
itharnessmidp c:\javatest.txt c:\testresults\java1.txt

rem copy test results to mmc card
move c:\testresults\java1.txt e:\testresults\java1.txt

del c:\sys\install\integrityservices\E130772A2A09A5.drv

rem run ITHARNESSMIDP
itharnessmidp c:\javatest2.txt c:\testresults\java2.txt

rem copy test results to mmc card
move c:\testresults\java2.txt e:\testresults\java2.txt


rem run ITHARNESSMIDP
itharnessmidp c:\javatest3.txt c:\testresults\java3.txt

rem copy test results to mmc card
move c:\testresults\java3.txt e:\testresults\java3.txt

rem run ITHARNESSMIDP
itharnessmidp c:\leavetest.txt c:\testresults\leavetest.txt

rem copy test results to mmc card
move c:\testresults\leavetest.txt e:\testresults\leavetest.txt

rem run ITHARNESSMIDP
copy z:\certclients.dat c:\system\data\certclients.dat
copy z:\cacerts.dat c:\system\data\cacerts.dat
attrib c:\system\data\cacerts.dat -r
attrib c:\system\data\certclients.dat -r
itharnessmidp c:\otatests.txt c:\testresults\java4.txt

rem copy test results to mmc card
move c:\testresults\java4.txt e:\testresults\java4.txt

rem run ITHARNESSMIDP
rem copy z:\prompt.cfg c:\prompt.cfg
rem ced -i c:\prompt.cfg
itharnessmidp c:\javaiaptest.txt c:\testresults\java5.txt
rem ced -i c:\auto.cfg

rem copy test results to mmc card
move c:\testresults\java5.txt e:\testresults\java5.txt

rem delete midpfiles
del /s c:\midpfiles\
del c:\javatest.txt
del c:\javatest2.txt
del c:\javatest3.txt
del c:\otatests.txt
del c:\javaiaptest.txt
del /s c:\private\200008d1\
del /s c:\system\install\

rem tjavafile
rem copy scripts and data
copy /s z:\tJavaFile\scripts\ c:\tJavaFile\scripts\
copy /s z:\tJavaFile\data\ c:\tJavaFile\data\

rem run tjavafile tests
tJavaFile c:\tJavaFile\scripts\midpjartest.txt c:\testresults\tJavaFilejar.log
tJavaFile c:\tJavaFile\scripts\midpjadtest.txt c:\testresults\tJavaFilejad.log
tJavaFile c:\tJavaFile\scripts\attributestest.txt c:\testresults\tJavaFileattributes.log

rem copy results to mmc card
move c:\testresults\tJavaFilejar.log e:\testresults\tJavaFilejar.log
move c:\testresults\tJavaFilejad.log e:\testresults\tJavaFilejad.log
move c:\testresults\tJavaFileattributes.log e:\testresults\tJavaFileattributes.log


rem delete tJavaFile stuff
del /s c:\tJavaFile\

REM tmidletuaheaderfilter tests - uses testexecute framework
copy /s z:\tmidletuaheaderfilter\ c:\tmidletuaheaderfilter\
testexecute c:\tmidletuaheaderfilter\tmidletuaheaderfilter.script
move c:\logs\testexecute\tmidletuaheaderfilter.htm e:\testresults\tmidletuaheaderfilter.htm
del /s c:\tmidletuaheaderfilter\

rem tjavahelper
rem copy scripts
copy /s z:\tJavaHelper\ c:\tJavaHelper\
copy /s z:\rta\ c:\rta\
copy /s z:\drmdata\ c:\drmdata\
copy /s z:\midpfiles\ c:\midpfiles\

rem copy rockz.jar which is not duplicated in the ROM but also used for the tJavaInstaller test
copy z:\tJavaFile\Data\Invalid\rockz.jar c:\midpfiles\rockz.jar
rem copy invalid midlet data
rem copy  valid midlet data
md c:\tJavaFile
md c:\tJavaFile\data
md c:\tJavaFile\data\Invalid
md c:\tJavaFile\data\valid
copy z:\tJavaFile\data\Invalid\kfirewithoutmidletname.jad c:\tJavaFile\data\Invalid\kfirewithoutmidletname.jad
copy z:\tJavaFile\data\Invalid\kfirewithoutmidletname.jar c:\tJavaFile\data\Invalid\kfirewithoutmidletname.jar
copy z:\tJavaFile\data\Invalid\kfirewithoutclassname.jad c:\tJavaFile\data\Invalid\kfirewithoutclassname.jad
copy z:\tJavaFile\data\Invalid\kfirewithoutclassname.jar c:\tJavaFile\data\Invalid\kfirewithoutclassname.jar
copy z:\tjavaFile\data\invalid\LongUrlinstallnotify.jad c:\tjavaFile\data\invalid\LongUrlinstallnotify.jad

copy z:\tJavaFile\data\invalid\kfirewithicon2_nospaceaftercolon.jar c:\tJavaFile\data\invalid\kfirewithicon2_nospaceaftercolon.jar
copy z:\tJavaFile\data\invalid\kfirewithicon2_invalidcharaftercolon.jar c:\tJavaFile\data\invalid\kfirewithicon2_invalidcharaftercolon.jar
copy z:\tJavaFile\data\invalid\kfirewithicon2_novendorval.jar c:\tJavaFile\data\invalid\kfirewithicon2_novendorval.jar
copy z:\tJavaFile\data\invalid\kfirewithicon2_noversionval.jar c:\tJavaFile\data\invalid\kfirewithicon2_noversionval.jar
copy z:\tJavaFile\data\invalid\kfirewithicon2_nonameval.jar c:\tJavaFile\data\invalid\kfirewithicon2_nonameval.jar

copy z:\tJavaFile\data\valid\kfirewithicon2_nodescriptionval.jar c:\tJavaFile\data\valid\kfirewithicon2_nodescriptionval.jar
copy z:\tJavaFile\data\valid\kfirewithicon2_noinfourlval.jar c:\tJavaFile\data\valid\kfirewithicon2_noinfourlval.jar
copy z:\tJavaFile\data\valid\nojarvalue.jad c:\tJavaFile\data\valid\nojarvalue.jad
copy z:\tJavaFile\data\valid\nojarvalue.jar c:\tJavaFile\data\valid\nojarvalue.jar

rem copy ocsp certs
copy z:\cacerts.dat c:\system\data\cacerts.dat
copy z:\certclients.dat c:\system\data\certclients.dat
attrib c:\system\data\cacerts.dat -r
attrib c:\system\data\certclients.dat -r

rem copy files for PDEF099222
md c:\private\102033E6
md c:\private\102033E6\MIDlets
md c:\private\102033E6\MIDlets\[1010fcaf]
copy z:\tJavaFile\data\valid\mazegame.jar c:\private\102033E6\MIDlets\[1010fcaf]\mazegame.jar
copy z:\tJavaFile\data\valid\PDEF099222_uids c:\private\102033E6\MIDlets\[1010fcaf]\uids
md c:\system\install\
md c:\system\install\registry
copy z:\tJavaFile\data\valid\PDEF099222_1010FCAF.reg c:\system\install\registry\1010FCAF.reg

rem run tjavahelper tests
tJavaHelper c:\tJavaHelper\Scripts\MIDPLaunchInfoTest.txt c:\testresults\MIDPLaunchInfoTest.log
tJavaHelper c:\tJavaHelper\Scripts\tJavaInstaller.txt c:\testresults\tJavaInstaller.log
tJavaHelper c:\tJavaHelper\Scripts\tJavaInstallerocsp.txt c:\testresults\tJavaInstallerocsp.log
tJavaHelper c:\tJavaHelper\Scripts\tjmiocsphttpfilter.txt c:\testresults\tjmiocsphttpfilter.log
tJavaHelper c:\tJavaHelper\Scripts\tjavarunoninstall.txt c:\testresults\tjavarunoninstall.log
tJavaHelper c:\tJavaHelper\Scripts\tJavaDRM.txt c:\testresults\tJavaDRM.log

rem NQP tests
tJavaHelper c:\tJavaHelper\Scripts\tjmiotaex.txt c:\testresults\tjmiotaex.log

rem uncomment the below line once the 'DEF100676: OOM tests failing in JMI component' is fixed
rem tJavaHelper c:\tJavaHelper\Scripts\tjmioom.txt c:\testresults\tjmioom.log

rem copy results to mmc card
move c:\testresults\MIDPLaunchInfoTest.log e:\testresults\MIDPLaunchInfoTest.log
move c:\testresults\tJavaInstaller.log e:\testresults\tJavaInstaller.log
move c:\testresults\tJavaInstallerocsp.log e:\testresults\tJavaInstallerocsp.log
move c:\testresults\tjmiocsphttpfilter.log e:\testresults\tjmiocsphttpfilter.log
move c:\testresults\tjavarunoninstall.log e:\testresults\tjavarunoninstall.log
move c:\testresults\tJavaDRM.log e:\testresults\tJavaDRM.log

rem NQP tests
move c:\testresults\tjmiotaex.log e:\testresults\tjmiotaex.log

rem uncomment the below line once the 'DEF100676: OOM tests failing in JMI component' is fixed
rem move c:\testresults\tjmioom.log e:\testresults\tjmioom.log

del /s c:\midpfiles\
del /s c:\itharnessmidp\
del /s c:\tJavaFile\
del /s c:\sys\bin\
del /s c:\tjavahelper\
del /s c:\system\install\
del /s c:\private\00000000\
del /s c:\private\10205ea5\
del /s c:\private\102033E6\MIDlets\[1010fcaf]\
 
rem tjavaregistry
copy /s z:\tJavaRegistry\ c:\tJavaRegistry\
tJavaRegistry c:\tJavaRegistry\scripts\script1.txt c:\testresults\tJavaRegistry.log
move c:\testresults\tJavaRegistry.log e:\testresults\tJavaRegistry.log
del /s c:\tJavaRegistry\

attrib c:\*.* -r
del c:\*.*

REM TJAVAHELPERSERVER tests - uses testexecute framework
copy /s z:\private\10204805\ c:\private\10204805\
testexecute c:\private\10204805\scripts\tjavahelperserver.script
move c:\logs\testexecute\tjavahelperserver.htm e:\testresults\tjavahelperserver.htm
del /s c:\private\10204805\
del /s c:\sys\bin\

REM Run recognizer tests
md c:\trecog
copy /s z:\trecog\* c:\trecog

testexecute z:\trecog\trecogjava.script
move c:\logs\testexecute\trecogjava.htm e:\testresults\trecogjava.htm

del c:\trecog\*.*
