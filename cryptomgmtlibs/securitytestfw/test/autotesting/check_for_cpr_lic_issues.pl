#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:       PREQ 2518 - Modification of Symbian OS copy-right notices 
#                   PREQ 2683 - Adding distribution.policy.s60 files for all components in the MCL
#                   Script to check for the conflicting copy-right-licenses and missing policy files.
#                   Just checks the size of the file - verify_issues.txt to decide whether there are 
#                   any issues!
#


#!/usr/bin/perl 
use strict; 
use warnings; 
my $filesize = -s '\epoc32\winscw\c\verify_issues.txt'; 
my $outputfile = '\epoc32\winscw\c\verify_issues.txt';
open(OP,">>$outputfile") || die("Cannot Open File");

if( $filesize == 0 )
{
 	print (OP "\n\n\n\n\n\n\n\n\n No issues found. Test PASSED.\n\n");
 	print (OP "\n Tests completed OK");
 	print (OP "\n Run: 1");
 	print (OP "\n Passed: 1");	
	print (OP "\n 0 tests failed out of 1"); 
}
else
{
		print (OP "\n\n\n\n\n\n\n\n\n\n Tests completed OK");
	        print (OP "\n Issues found.");
 		print (OP "\n Run: 1");
 		print (OP "\n Passed: 0");	
		print (OP "\n 1 tests failed out of 1"); 
}
exit 0; 