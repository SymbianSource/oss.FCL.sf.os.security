The metadata and backup file were obtained by using tbackuprestore  test step.


Script file:

LOAD_SUITE tuiscriptadaptors
LOAD_SUITE tbackuprestore

// creates resource file via swi
RUN_TEST_STEP 100 tuiscriptadaptors InstallStep z:\tups\integ\scripts\tups_integ.ini  install_policies1v1

//backup resource file
RUN_TEST_STEP 100 tbackuprestore  BACKUP  z:\tups\integ\scripts\tups_integ.ini	backup_policy_file1

// uninstall SIS files previously installed
RUN_TEST_STEP 100 tuiscriptadaptors UninstallAugStep z:\tups\integ\scripts\tups_integ.ini  un_install_policies1v1


INI file:



 [backup_policy_file1]
FILE1.1 = C:\private\10283558\policies\ups_101f7f1f_ff000001.rsc
SIS1 = z:\tups\integ\packages\tupsinteg_policies1.sis
UID1 = 10285777
INSTALLDRIVE1 = C

[install_policies1v1]
sis=z:\tups\integ\packages\tupsinteg_policies1.sis
script=z:\tups\integ\packages\simple.xml


