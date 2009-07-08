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
REM Warning - The Silent Install test, cleanupinfrastructure test and tpropagation test will format the E: MMC drive
REM Save existing results
md c:\savedresults
copy /s e:\testresults\* c:\savedresults

REM Save the USB Bootloader and BATS hardware board config file
md c:\USBLoader
copy /s e:\usbload.zip c:\USBLoader
copy /s e:\_hw_board_* c:\USBLoader

REM Run test
testexecute z:\tswi\tuiscriptadaptors\scripts\tsilent_h2.script

REM Run test
testexecute z:\tswi\tuiscriptadaptors\scripts\tswidaemonstub_h2.script

REM Run test
testexecute z:\tswi\tuiscriptadaptors\scripts\inc098114_h2.script

md c:\tswi

testexecute z:\tswi\tuiscriptadaptors\scripts\tcleanupinfrastructure.script

testexecute z:\tswi\tuiscriptadaptors\scripts\tpropagation_arm.script

testexecute z:\tswi\tuiscriptadaptors\scripts\test_pa_sapropagated_notdelete.script


REM Restore results
md e:\testresults
copy /s c:\savedresults e:\testresults\*
del /s c:\savedresults\*

REM Restore USB Bootloader and BATS hardware board config file
copy c:\USBLoader\usbload.zip e:\usbload.zip
copy c:\USBLoader\_hw_board_* e:\_hw_board_*
del /s c:\USBLoader\*

# Save Silent Install, cleanupinfrastructure and tpropagation test results
move c:\logs\testexecute\tsilent_h2.htm e:\testresults\tsilent_h2.htm
move c:\logs\testexecute\tswidaemonstub_h2.htm e:\testresults\tswidaemonstub_h2.htm
move c:\logs\testexecute\inc098114_h2.htm e:\testresults\inc098114_h2.htm
move c:\logs\testexecute\tcleanupinfrastructure.htm e:\testresults\tcleanupinfrastructure.htm
copy c:\logs\testexecute\tpropagation_arm.htm e:\testresults\tpropagation_arm.htm
move c:\logs\testexecute\test_pa_sapropagated_notdelete.htm e:\testresults\test_pa_sapropagated_notdelete.htm

REM Run recognizer tests
md c:\trecog
copy /s z:\trecog\* c:\trecog

testexecute z:\trecog\trecogcert.script
move c:\logs\testexecute\trecogcert.htm e:\testresults\trecogcert.htm

testexecute z:\trecog\trecogpkcs12.script
move c:\logs\testexecute\trecogpkcs12.htm e:\testresults\trecogpkcs12.htm

testexecute z:\tswi\swicaptests\scripts\swicaptests.script
move c:\logs\testexecute\swicaptests.htm e:\testresults\swicaptests.htm

testexecute z:\tswi\tsis\scripts\tsis.script
move c:\logs\testexecute\tsis.htm e:\testresults\tsis.htm

testexecute z:\tswi\tsis\scripts\tsis_signed.script
move c:\logs\testexecute\tsis_signed.htm e:\testresults\tsis_signed.htm

testexecute z:\tswi\tintegrityservices\scripts\tintegrityservices.script
move c:\logs\testexecute\tintegrityservices.htm e:\testresults\tintegrityservices.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testuninstallation_arm_only.script
move c:\logs\testexecute\testuninstallation_arm_only.htm e:\testresults\testuninstallation_arm_only.htm

testexecute z:\tswi\tsishelper\scripts\tsishelper.script
move c:\logs\testexecute\tsishelper.htm e:\testresults\tsishelper.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testwritableswicertstore.script
move c:\logs\testexecute\testwritableswicertstore.htm e:\testresults\testwritableswicertstore.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testrucert.script
move c:\logs\testexecute\testrucert.htm e:\testresults\testrucert.htm

REM NQP Tests for DataProvider
testexecute z:\tswi\tdataprovider\scripts\tdataprovider.script
move c:\logs\testexecute\tdataprovider.htm e:\testresults\tdataprovider.htm

md c:\private
md c:\private\101f72a6\
copy z:\tswi\tsis\data\certstore\* c:\private\101f72a6\ 

attrib c:\tswi\*.* -r

copy z:\auto.cfg c:\
copy z:\auto_broken.cfg c:\

testexecute z:\tswi\tuiscriptadaptors\scripts\tuiscriptadaptors.script
move c:\logs\testexecute\tuiscriptadaptors.htm e:\testresults\tuiscriptadaptors.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\tuninstallpkgs_h4.script
move c:\logs\testexecute\tuninstallpkgs_h4.htm e:\testresults\tuninstallpkgs_h4.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testvids.script
move c:\logs\testexecute\testvids.htm e:\testresults\testvids.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testasync.script
move c:\logs\testexecute\testasync.htm e:\testresults\testasync.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testdrmasync.script
move c:\logs\testexecute\testdrmasync.htm e:\testresults\testdrmasync.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\tuiscriptadaptorsFH.script
move c:\logs\testexecute\tuiscriptadaptorsFH.htm e:\testresults\tuiscriptadaptorsFH.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testexpressions.script
move c:\logs\testexecute\testexpressions.htm e:\testresults\testexpressions.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testdrives.script
move c:\logs\testexecute\testdrives.htm e:\testresults\testdrives.htm

copy z:\auto.cfg c:\auto.cfg
attrib c:\auto.cfg -r
md c:\tswi\tbackuprestore
testexecute z:\tswi\tbackuprestore\scripts\tbackuprestore.script
move c:\logs\testexecute\tbackuprestore.htm e:\testresults\tbackuprestore.htm

testexecute z:\tswi\tbackuprestore\scripts\tbackuprestoredrive_armv5.script
move c:\logs\testexecute\tbackuprestoredrive_armv5.htm e:\testresults\tbackuprestoredrive_armv5.htm

testexecute z:\tswi\tbackuprestore\scripts\tbackuptrestore_exploratory.script
move c:\logs\testexecute\tbackuptrestore_exploratory.htm e:\testresults\tbackuptrestore_exploratory.htm

testexecute z:\tswi\trevocation\scripts\trevocation.script
move c:\logs\testexecute\trevocation.htm e:\testresults\trevocation.htm

testexecute z:\tswi\tdaemon\scripts\tdaemon.script
move c:\logs\testexecute\tdaemon.htm e:\testresults\tdaemon.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testlocations.script
move c:\logs\testexecute\testlocations.htm e:\testresults\testlocations.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testcancel.script
move c:\logs\testexecute\testcancel.htm e:\testresults\testcancel.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testasynccancel.script
move c:\logs\testexecute\testasynccancel.htm e:\testresults\testasynccancel.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testrun.script
move c:\logs\testexecute\testrun.htm e:\testresults\testrun.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testrunft.script
move c:\logs\testexecute\testrunft.htm e:\testresults\testrunft.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testupgrades.script
move c:\logs\testexecute\testupgrades.htm e:\testresults\testupgrades.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testaugmentations.script
move c:\logs\testexecute\testaugmentations.htm e:\testresults\testaugmentations.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testpreinstalled.script
move c:\logs\testexecute\testpreinstalled.htm e:\testresults\testpreinstalled.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testdependencies.script
move c:\logs\testexecute\testdependencies.htm e:\testresults\testdependencies.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testshutdown.script
move c:\logs\testexecute\testshutdown.htm e:\testresults\testshutdown.htm

testexecute z:\tswi\tsisregistrytest\scripts\tsisregistrytest.script
move c:\logs\testexecute\tsisregistrytest.htm e:\testresults\tsisregistrytest.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\tsisstub.script
move c:\logs\testexecute\tsisstub.htm e:\testresults\tsisstub.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testdrm.script
move c:\logs\testexecute\testdrm.htm e:\testresults\testdrm.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testdrmFH.script
move c:\logs\testexecute\testdrmFH.htm e:\testresults\testdrmFH.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testmulti.script
move c:\logs\testexecute\testmulti.htm e:\testresults\testmulti.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testextension.script
move c:\logs\testexecute\testextension.htm e:\testresults\testextension.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testdevicelanguages.script
copy c:\logs\testexecute\testdevicelanguages.htm e:\testresults\testdevicelanguages.htm

REM install automated commdb using sim.tsy for tdev
ced -i z:\autosimtsy.cfg

copy z:\tswi\tdevcerts\config.txt c:\config.txt
testexecute z:\tswi\tdevcerts\scripts\devcerts_match.script
move c:\logs\testexecute\devcerts_match.htm e:\testresults\devcerts_match.htm

testexecute z:\tswi\tdevcerts\scripts\devcerts_mismatch.script
move c:\logs\testexecute\devcerts_mismatch.htm e:\testresults\devcerts_mismatch.htm

testexecute z:\tswi\tdevcerts\scripts\devcerts_aggregate.script
move c:\logs\testexecute\devcerts_aggregate.htm e:\testresults\devcerts_aggregate.htm

testexecute z:\tswi\tdevcerts\scripts\devcerts_corrupt.script
move c:\logs\testexecute\devcerts_corrupt.htm e:\testresults\devcerts_corrupt.htm

testexecute z:\tswi\tdevcerts\scripts\devcerts_backuprestore.script
move c:\logs\testexecute\devcerts_backuprestore.htm e:\testresults\devcerts_backuprestore.htm

testexecute z:\tswi\tdevcerts\scripts\devcerts_backup.script
move c:\logs\testexecute\devcerts_backup.htm e:\testresults\devcerts_backup.htm

attrib c:\config.txt -r
del c:\config.txt
copy z:\tswi\tdevcerts\config2.txt c:\config.txt
testexecute z:\tswi\tdevcerts\scripts\devcerts_restore.script
move c:\logs\testexecute\devcerts_restore.htm e:\testresults\devcerts_restore.htm
attrib c:\config.txt -r
del c:\config.txt

REM restore standard automated commdb for the rest of the tests
ced -i z:\auto.cfg

testexecute z:\tswi\tdevcerts\scripts\devcerts_ocsp.script
move c:\logs\testexecute\devcerts_ocsp.htm e:\testresults\devcerts_ocsp.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testeclipse.script
move c:\logs\testexecute\testeclipse.htm e:\testresults\testeclipse.htm

#testexecute z:\tswi\tuiscriptadaptors\scripts\tuileavedialog.script
#move c:\logs\testexecute\tuileavedialog.htm e:\testresults\tuileavedialog.htm

#testexecute z:\tswi\tuiscriptadaptors\scripts\tuileavedialogasync.script
#move c:\logs\testexecute\tuileavedialogasync.htm e:\testresults\tuileavedialogasync.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testappinuse.script
move c:\logs\testexecute\testappinuse.htm e:\testresults\testappinuse.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testVFP.script
move c:\logs\testexecute\testVFP.htm e:\testresults\testVFP.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\switermination.script
move c:\logs\testexecute\switermination.htm e:\testresults\switermination.htm

REM Test non removable flag
testexecute z:\tswi\tuiscriptadaptors\scripts\testnonremovable_armv5.script
move c:\logs\testexecute\testnonremovable_armv5.htm e:\testresults\testnonremovable_armv5.htm

md c:\tswi\tbackuprestore
testexecute z:\tswi\tuiscriptadaptors\scripts\tresetdevice.script
move c:\logs\testexecute\tresetdevice.htm e:\testresults\tresetdevice.htm

REM DevInfoSupport test
tdevinfosupport
copy c:\swi.log e:\testresults\swi.log
del c:\swi.log
testexecute z:\tswi\tuiscriptadaptors\scripts\tuileavedialog.script
move c:\logs\testexecute\tuileavedialog.htm e:\testresults\tuileavedialog.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\tuileavedialogasync.script
move c:\logs\testexecute\tuileavedialogasync.htm e:\testresults\tuileavedialogasync.htm

REM SwiConsole Tests
testexecute z:\tswi\tswiconsole\scripts\tswiconsole.script
move c:\logs\testexecute\tswiconsole.htm e:\testresults\tswiconsole.htm

REM DEF083628: Orphaned files in protected dirs can be overwritten by untrusted packages
testexecute z:\tswi\tuiscriptadaptors\scripts\overwriteprivate_positive.script
move c:\logs\testexecute\overwriteprivate_positive.htm e:\testresults\overwriteprivate_positive.script.htm

testexecute z:\tswi\tautosigning\scripts\tautosigning.script
copy c:\logs\testexeute\tautosigning.htm e:\testresults\tautosigning.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testasynccaps.script
move c:\logs\testexecute\testasynccaps.htm e:\testresults\testasynccaps.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testdiskfullemulator.script
move c:\logs\testexecute\testdiskfullemulator.htm e:\testresults\testdiskfullemulator.htm

del e:\BigTempFile


testexecute z:\tswi\tuiscriptadaptors\scripts\testmandatory_HW.script
move c:\logs\testexecute\testmandatory_hw.htm e:\testresults\testmandatory.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testsystemcaps.script
move c:\logs\testexecute\testsystemcaps.htm e:\testresults\testsystemcaps.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testusercaps.script
move c:\logs\testexecute\testusercaps.htm e:\testresults\testusercaps.htm

testexecute z:\tswi\tuiscriptadaptors\scripts\testusercaps_nousergrantcaps_hw.script
move c:\logs\testexecute\testusercaps_nousergrantcaps_hw.htm e:\testresults\testusercaps_nousergrantcaps_hw.htm

REM Demandpaging  tests
testexecute z:\tswi\tuiscriptadaptors\scripts\tdempaging_armv5.script
move c:\logs\testexecute\tdempaging_armv5.htm e:\testresults\tdempaging_armv5.htm

REM CR0914Tests
testexecute z:\tswi\tsisregistrytest\scripts\integritychecktest.script
move c:\logs\testexecute\integritychecktest.htm e:\testresults\integritychecktest.htm

REM NQP Tests for DataProvider
testexecute z:\tswi\tdataprovider\scripts\tdataprovider.script
move c:\logs\testexecute\tdataprovider.htm e:\testresults\tdataprovider.htm

REM SWI Adorned filename handling tests
testexecute.exe z:\tswi\tuiscriptadaptors\scripts\adornedfilenames.script
copy c:\logs\testexecute\adornedfilenames.htm e:\testresults\adornedfilenames.htm

del c:\temp\*.*
attrib c:\tswi\*.* -r
del c:\tswi\*.*

attrib c:\logs\testexecute\*.* -r
del c:\logs\testexecute\*.*
