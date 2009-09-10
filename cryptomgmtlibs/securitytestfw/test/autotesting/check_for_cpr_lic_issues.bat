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
@rem Description:  Check for the issues not considering the
@rem  --- Top level bld.inf ( Not in MCL )
@rem  --- openssllib ( Maintained by OE )
@rem  --- opencryptolibs ( Maintained by OE )
@rem  --- testframework ( Not in MCL )
@rem  --- openssl ( Maintained by OE )
@rem  --- SFUpdateLicenceHeader.pl ( Don't complain about the script itself!! )
@rem  --- nokia_security_metadata/distribution.policy.s60 ( Not a part of Symbian Foundation )
@rem

@rem   Remove the issues( lines ) related to the above from the generated verif_issues.txt LOG file
perl -ni.bak -e "print unless /\/common\/generic\/security\/distribution.policy.s60/;"  \epoc32\winscw\c\verify_issues.txt
perl -ni.bak -e "print unless /openssllib/;"  \epoc32\winscw\c\verify_issues.txt
perl -ni.bak -e "print unless /opencryptolibs/;"  \epoc32\winscw\c\verify_issues.txt
perl -ni.bak -e "print unless /testframework/;"  \epoc32\winscw\c\verify_issues.txt
perl -ni.bak -e "print unless /openssl/;"  \epoc32\winscw\c\verify_issues.txt
perl -ni.bak -e "print unless /SFUpdateLicenceHeader.pl/;"  \epoc32\winscw\c\verify_issues.txt
perl -ni.bak -e "print unless /\/common\/generic\/security\/os\/security\/security_info\/nokia_security_metadata\/distribution.policy.s60/;"  \epoc32\winscw\c\verify_issues.txt

@rem   Append the result - Pass or fail to the same verify_issues.txt file.
perl \epoc32\winscw\c\check_for_cpr_lic_issues.pl
