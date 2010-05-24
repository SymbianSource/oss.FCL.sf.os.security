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


echo Copy pkg files to common location and change to that location
call mkdir %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\
copy /y tupsinteg_post1.pkg %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tupsinteg_post1.pkg
copy /y tupsinteg_policies1.pkg %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tupsinteg_policies1.pkg
copy /y tupsinteg_policies2.pkg %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tupsinteg_policies2.pkg
copy /y tupsinteg_evaluator1.pkg %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tupsinteg_evaluator1.pkg
copy /y tupsinteg_eclipse1.pkg %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tupsinteg_eclipse1.pkg
copy /y tupsinteg_dialog1.pkg %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tupsinteg_dialog1.pkg
copy /y tupsinteg_ca_not_instore.pkg %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tupsinteg_ca_not_instore.pkg
copy /y testupsromstub.pkg %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\testupsromstub.pkg
copy /y dummy.txt %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\dummy.txt

copy /y %EPOCROOT%epoc32\release\%1\%2\tupsinteg_post1.exe %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tupsinteg_post1.exe
copy /y %EPOCROOT%epoc32\release\%1\%2\tups_policyevaluator05.dll %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tups_policyevaluator05.dll
copy /y %EPOCROOT%epoc32\data\z\resource\plugins\tups_policyevaluator05.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tups_policyevaluator05.rsc
copy /y %EPOCROOT%epoc32\release\%1\%2\tups_dialogcreator07.dll %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tups_dialogcreator07.dll
copy /y %EPOCROOT%epoc32\data\z\resource\plugins\tups_dialogcreator07.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\tups_dialogcreator07.rsc

if "%1"=="WINSCW" goto WINSCW;
if "%1"=="ARMV5" goto ARMV5;


:WINSCW
copy /y %EPOCROOT%epoc32\release\%1\%2\Z\private\10283558\policies\t_ups_101f7f1f_ff000001_version1.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_ff000001_version1.rsc
copy /y %EPOCROOT%epoc32\release\%1\%2\Z\private\10283558\policies\t_ups_101f7f1f_ff000001_version2.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_ff000001_version2.rsc
copy /y %EPOCROOT%epoc32\release\%1\%2\Z\private\10283558\policies\t_ups_101f7f1f_0000aaa2_version1.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_0000aaa2_version1.rsc
copy /y %EPOCROOT%epoc32\release\%1\%2\Z\private\10283558\policies\eclipse_ups_101f7f1f_0000aaa3.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\eclipse_ups_101f7f1f_0000aaa3.rsc
copy /y %EPOCROOT%epoc32\release\%1\%2\Z\private\10283558\policies\t_ups_101f7f1f_0000aaa4.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_0000aaa4.rsc
copy /y %EPOCROOT%epoc32\release\%1\%2\Z\private\10283558\policies\t_ups_101f7f1f_ff000001_version1.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_ff000001_version1.rsc
copy /y %EPOCROOT%epoc32\release\%1\%2\Z\private\10283558\policies\t_ups_101f7f1f_ff000001_version2.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_ff000001_version2.rsc
goto EXIT;

:ARMV5
copy /y %EPOCROOT%epoc32\data\z\private\10283558\policies\t_ups_101f7f1f_ff000001_version1.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_ff000001_version1.rsc
copy /y %EPOCROOT%epoc32\data\z\private\10283558\policies\t_ups_101f7f1f_ff000001_version2.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_ff000001_version2.rsc
copy /y %EPOCROOT%epoc32\data\z\private\10283558\policies\t_ups_101f7f1f_0000aaa2_version1.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_0000aaa2_version1.rsc
copy /y %EPOCROOT%epoc32\data\z\private\10283558\policies\eclipse_ups_101f7f1f_0000aaa3.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\eclipse_ups_101f7f1f_0000aaa3.rsc
copy /y %EPOCROOT%epoc32\data\z\private\10283558\policies\t_ups_101f7f1f_0000aaa4.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_0000aaa4.rsc
copy /y %EPOCROOT%epoc32\data\z\private\10283558\policies\t_ups_101f7f1f_ff000001_version1.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_ff000001_version1.rsc
copy /y %EPOCROOT%epoc32\data\z\private\10283558\policies\t_ups_101f7f1f_ff000001_version2.rsc %EPOCROOT%epoc32\WINSCW\c\tups\data\%2\t_ups_101f7f1f_ff000001_version2.rsc
goto EXIT;

:EXIT
