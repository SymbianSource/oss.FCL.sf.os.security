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
# Description: 
#

﻿#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Symbian Foundation License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.symbianfoundation.org/legal/sfl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Perl script that creates PKG files and test MAKESIS tools with different ranges
# 
#

$logFile = "/epoc32/winscw/c/makesis_test.txt";
$makesisExeLocation = "/epoc32/tools/makesis";
$dumpsisExeLocation = "/epoc32/tools/dumpsis";

sub GetTmpFilesCount()
{
	# get a number of temp files
	if ($^O =~ /^MSWIN32$/i)
		{
		my $dirname = "$ENV{'TEMP'}";
		opendir ( DIR, "$dirname" ) or die ( "Can't open dir: $dirname" ); #!
		my @List = readdir DIR;
		closedir DIR;
		
		my @TmpFiles = grep( /^~SI.*.tmp/ , @List );
		}
	else
		{
		my $dirname = "/tmp/";
		opendir ( DIR, "$dirname" ) or die ( "Can't open dir: $dirname" ); #!
		my @List = readdir DIR;
		closedir DIR;

		my @TmpFiles = grep( /tmp*/ , @List );
		}

	my $TmpCountBefore = @TmpFiles;
	
	return $TmpCountBefore;
}

#
#Function to write log into file
#
sub WriteLog {
	my ($log) = @_;
	#public $logfile;

	unless( open($fh, ">> $logFile")) {
		printf STDERR "Can\'t open $logfile:$!\n";
		return;
	}
	printf $fh $log;
	printf $log;
	close $fh;
}


#
# Function to create a file (test.txt or any PKG)
#
sub CreateFile {
	my ($filename, $contents) = @_;

	unless (open($fh, "> $filename")) {
		printf STDERR "Can't open $filename: $!\n";
		return;
	}
	print $fh $contents;
	close $fh;


}

#
# Function to check the log file of the makesis result
#
sub CheckLog {
	my ($pkgfile) = @_[0];
	my ($expectedLog) = @_[1];

	$logMsg = sprintf "Expected Log: %s\n", $expectedLog;
	WriteLog( $logMsg);	

	unless (open($resultlog, "$pkgfile.log")) {
		printf STDERR "Can't open $pkgfile.log: $!\n";
		return 0;
	}

	foreach $line (<$resultlog>) {
		if ( $line =~ m/\Q$expectedLog\E/i) {
			close($resultlog);
			return 1;
		}	
	}	
	close($resultlog);
	return 0;
}

#
# Run MAKESIS with prepared pkg file, log and administrate its result
#
sub MakeSISFile {

	my ($pkgfile) = @_[0];
	my ($expectedResult) = @_[1];
	my ($expectedLog) = @_[2];
	$pkgfile =~ s/\.pkg//;

	if($pkgfile ne "testinterpretflag") {
		@retval = system("$makesisExeLocation -v $pkgfile.pkg $pkgfile-tmp.sis > $pkgfile.log");
	}
	else {
		@retval = system("$makesisExeLocation -v -c $pkgfile.pkg $pkgfile-tmp.sis > $pkgfile.log");
	}

	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;

	if( $? == $expectedResult ) {
		if(CheckLog($pkgfile, $expectedLog)) {
			$NumberOfPassed++;
			WriteLog("Passed\n\n");
		}
		else {
			$NumberOfFailed++;
			WriteLog("Failed : Unexpected Error Log\n\n");
		}
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed : Unexpected Error Code\n\n");
	}

	unlink("$pkgfile-tmp.sis");
	unlink("$pkgfile.log");
	unlink("$pkgfile.pkg");

}

#
# Additional test to check that stub files don't change every time 
# they are built.
#
sub TestSISStubFile {

	$teststubpkg = "teststub.pkg";
	$teststubsis = "teststub.sis";
	$comparisonsis = "comparison.sis";
	$teststublog = "teststub.log";

	WriteLog("Test invariant stub file\n");

	# Generate test PKG file contents
	$PkgFile = sprintf( $PkgFileTempl, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($teststubpkg, $PkgFile);	

	# Create a stub sis file
	my $result = system("$makesisExeLocation -s $teststubpkg $teststubsis > $teststublog");
	
	# Wait at least a second and generate it again
	sleep 2;
	rename($teststubsis, $comparisonsis) or  
        warn "Couldn't rename $teststubsis to $comparisonsis: $!\n";

	my $result2 = system("$makesisExeLocation -s $teststubpkg $teststubsis > $teststublog");

	# Check that the files are the same and log the result
	use File::Compare;
	$NumberOfTests++;
	if (compare($teststubsis, $comparisonsis) == 0 
	    && $result == 0
		&& $result2 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $teststublog;
	unlink $teststubpkg;
	unlink $teststubsis ;
	unlink $comparisonsis;
}

#
# New test code for DEF083525 - makesis -vs does not create a stub sis file 
# This is based on TestSISStubFile, it builds the stub file with -s and -vs options 
# and checks the resulting files are the same.  The defects was that the "s" option
# was ignored so a stub file was not built.
#
sub TestDEF083525 {

	$teststubpkg = "teststub.pkg";
	$teststubsis = "teststub.sis";
	$comparisonsis = "comparison.sis";
	$teststublog = "teststub.log";

	WriteLog("Test for DEF083525 - makesis -vs does not create a stub sis file\n");

	# Generate test PKG file contents
	$PkgFile = sprintf( $PkgFileTempl, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($teststubpkg, $PkgFile);	

	# Create a stub sis file
	my $result = system("$makesisExeLocation -s $teststubpkg $teststubsis > $teststublog");
	
	# Wait at least a second and generate it again this time using the -vs options
	# This defect was that the -vs options was ignored
	sleep 2;
	rename $teststubsis, $comparisonsis;
	my $result2 = system("$makesisExeLocation -vs $teststubpkg $teststubsis > $teststublog");

	# Check that the files are the same and log the result
	use File::Compare;
	$NumberOfTests++;
	if (compare($teststubsis, $comparisonsis) == 0 
	    && $result == 0
		&& $result2 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $teststublog;
	unlink $teststubpkg;
	unlink $teststubsis ;
	unlink $comparisonsis;
}

#
# New test code for PDEF081989 - makesis parameter -d does not work any more. 
# This creates a testembedded.sis file embedding testembedding.sis. This test checks the 
# working of -d parameter where the search directory containing the embedding sis file is  
# specified with -d option.
#

sub TestPDEF081989 {

	$testembeddingpkg = "testembedding.pkg";
	$testembeddingsis = "testembedding.sis";
	$testembeddedpkg = "testembedded.pkg";
	$testembeddedsis = "testembedded.sis";
	$sisFileToEmbed = "/epoc32/winscw/c/";
	$outputFile = $sisFileToEmbed.$testembeddingsis;
	$testEmbedLog = "testembedded.log";

	WriteLog("Test for PDEF081989 - Test for successful creation of a sis file with -d option.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileTempl, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($testembeddingpkg, $PkgFile);	
	
	# Generate test PKG file contents for embedded pkg file.
	$PkgFile = sprintf( $embedContents, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($testembeddedpkg , $PkgFile);

	
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $testembeddingpkg $outputFile > $testEmbedLog");
	
	# Create an embedded sis file.Here the sis file embedded is situated elsewhere(in "\\epoc32\\winscw\\c\\").
	# Makesis -d option is used to specify the directory to search for sis file embedded. 
	my $result1 = system("$makesisExeLocation -d$sisFileToEmbed $testembeddedpkg $testembeddedsis > $testEmbedLog");
	
	$NumberOfTests++;
	if ($result == 0 && $result1 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $testembeddingpkg;
	unlink $outputFile;
	unlink $testembeddedpkg;
	unlink $testembeddedsis;
	unlink $testEmbedLog;
	
}

#
# Test code for DEF104895 - makesis with -d option
# This creates a testembedded.sis file embedding testembedding.sis. This test checks the 
# working of -d parameter where the search directory(bacward slashes) containing the embedding sis file is  
# specified with -d option.
#

sub TestDEF104895 {

	$testembeddingpkg = "testembedding.pkg";
	$testembeddingsis = "testembedding.sis";
	$testembeddedpkg = "testembedded.pkg";
	$testembeddedsis = "testembedded.sis";
	$sisFileToEmbed = "\\epoc32\\winscw\\c\\";
	$outputFile = $sisFileToEmbed.$testembeddingsis;
	$testEmbedLog = "testembedded.log";

	WriteLog("Test for DEF104895 - Test for successful creation of a sis file with -d option.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileTempl, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($testembeddingpkg, $PkgFile);	
	
	# Generate test PKG file contents for embedded pkg file.
	$PkgFile = sprintf( $embedContents, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($testembeddedpkg , $PkgFile);

	
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $testembeddingpkg $outputFile > $testEmbedLog");
	
	# Create an embedded sis file.Here the sis file embedded is situated elsewhere(in "\\epoc32\\winscw\\c\\").
	# Makesis -d option is used to specify the directory to search for sis file embedded. 
	my $result1 = system("$makesisExeLocation -d$sisFileToEmbed $testembeddedpkg $testembeddedsis > $testEmbedLog");
	
	$NumberOfTests++;
	if ($result == 0 && $result1 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $testembeddingpkg;
	unlink $outputFile;
	unlink $testembeddedpkg;
	unlink $testembeddedsis;
	unlink $testEmbedLog;
	
}

#
# New test code for DEF107033 - makesis parameter -d does not work with language dependent files
# 
#

sub TestDEF107033() {

	$testlanguagepkg = "testlanguage.pkg";
	$testlanguagesis = "testlanguagesis.sis";
	$testlanguagelog = "testlanguage.log";

	WriteLog("Test makesis parameter -d does not work with language dependent files\n");

	# Generate test PKG file contents
	$PkgFile = sprintf( $PkgLanguageFileTemp); 
	# Create PKG file
	CreateFile($testlanguagepkg, $PkgFile);	

	# Create a stub sis file
	my $result = system("$makesisExeLocation -d/epoc32/winscw/c/tswi/ $testlanguagepkg $testlanguagesis > $testlanguagelog");
	

	$NumberOfTests++;

	if ($result == 0) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}

	unlink $testlanguagepkg;
	unlink $testlanguagesis;
	unlink $testlanguagelog ;

}

#
# Test code for DEF104895 - makesis with -d option
# This creates a testembedded.sis file embedding testembedding.sis. This test checks the 
# working of -d parameter where the search directory(bacward slashes) containing the embedding sis file is  
# specified with -d option.
#

sub TestLDEF104895 {

	$testembeddingpkg = "testembedding.pkg";
	$testembeddingsis = "testembedding.sis";
	$testembeddedpkg = "testembedded.pkg";
	$testembeddedsis = "testembedded.sis";
	$sisFileToEmbed = "\\\\epoc32\\\\winscw\\\\c\\\\";
	$outputFile = $sisFileToEmbed.$testembeddingsis;
	$testEmbedLog = "testembedded.log";

	WriteLog("Test for DEF104895 - Test for successful creation of a sis file with -d option.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileTempl, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($testembeddingpkg, $PkgFile);	
	
	# Generate test PKG file contents for embedded pkg file.
	$PkgFile = sprintf( $embedContents, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($testembeddedpkg , $PkgFile);

	
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $testembeddingpkg $outputFile > $testEmbedLog");
	
	# Create an embedded sis file.Here the sis file embedded is situated elsewhere(in "\\epoc32\\winscw\\c\\").
	# Makesis -d option is used to specify the directory to search for sis file embedded. 
	my $result1 = system("$makesisExeLocation -d$sisFileToEmbed $testembeddedpkg $testembeddedsis > $testEmbedLog");
	
	$NumberOfTests++;
	if ($result == 0 && $result1 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $testembeddingpkg;
	unlink $outputFile;
	unlink $testembeddedpkg;
	unlink $testembeddedsis;
	unlink $testEmbedLog;
	
}


#
# New test code for DEF090878 - unexpected error by makesis when processing pkg file saved in UTF8 format
# This test uses existing utf8.pkg which contains UTF-8 encoded characters and in turn refers to utf8.txt
#
sub TestDEF090878 {

	my $path = "/epoc32/winscw/c/tswi";
	my $pkgfile = "$path/utf8";
	my $expectedResult = 0;

	WriteLog("Test for DEF090878 - unexpected error by makesis when processing pkg file saved in UTF8 format\n");
	WriteLog("UTF-8 encoded file: $pkgfile.pkg\n");
	
	# Do MAKESIS test
	@retval = system("$makesisExeLocation $pkgfile.pkg $pkgfile-tmp.sis > $pkgfile.log");
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;
	
	if( $? == $expectedResult ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}

	unlink("$pkgfile-tmp.sis");
	unlink("$pkgfile.sis");
	unlink("$pkgfile.log");
}

#
# Test code for DEF112831 - makesis crashes on .pkg containing non-existing embedded sis
#

sub TestDEF112831() {

	my $expectedResult = 256;
	$testmissingembeddedpkg = "missingembedded.pkg";
	$testmissingembeddedsis = "missingembedded.sis";
	$testmissingembeddedlog = "missingembedded.log";

	WriteLog("Test for DEF112831 - makesis crashes on .pkg containing non-existing embedded sis\n");

	# Generate test PKG file contents
	$PkgFile = sprintf( $missingEmbed); 
	# Create PKG file
	CreateFile($testmissingembeddedpkg, $PkgFile);	

	# Create a stub sis file
	my $result = system("$makesisExeLocation -d/epoc32/winscw/c/tswi/ $testmissingembeddedpkg $testmissingembeddedsis > $testmissingembeddedlog");

	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;

	if ($result == 256) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}

	unlink $testmissingembeddedpkg;
	unlink $testmissingembeddedsis;
	unlink $testmissingembeddedlog ;

}

# Windows Test
# Test code for  DEF091942
# Test case generates a SIS from a Japanese named pkg file. Checks to see if the SIS fle generated has the same name.
# NOTE: Does not validate the console output.
 
sub TestDEF091942 {

	my $path = "\\epoc32\\winscw\\c\\tswi";
	
	$NumberOfTests++;
	WriteLog("Test for DEF091942 - makesis can not handle package files names written with japanese symbols\n");
	
	# Generate Japanese pkg 
	system("WScript.exe //B //Nologo $path\\displayjpn_1.vbs");

	# Check to see if the sis file has been generated
	system("WScript.exe //B //Nologo $path\\displayjpn_2.vbs");

	if ( -f "$path\\passed.txt") {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	} else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}

	# Tidy up
	system("WScript.exe //B //Nologo $path\\displayjpn_3.vbs");
}

# Linux Test
# Test code for  DEF091942 
# Test case generates a SIS from a Japanese named pkg file. Checks to see if the SIS fle generated has the same name.
# NOTE: Does not validate the console output.
 
sub TestLDEF091942{

	$pkgfile = "ゎわこんァア龐龑.pkg";
	$sisfile = "ゎわこんァア龐龑.sis";
	$logfile = "ゎわこんァア龐龑.log";

	$NumberOfTests++;

	my $file = "/epoc32/winscw/c/tswi/passed.txt";
	
	WriteLog("Test for DEF091942 - makesis can not handle package files names written with japanese symbols\n");
	
	# Generate test PKG file contents for japanese pkg file.
	$PkgFile = sprintf( $JAPPkgFileTempl); 
	# Create PKG file
	CreateFile($pkgfile, $PkgFile);

	# Do MAKESIS test
	@retval = system("$makesisExeLocation -v $pkgfile > logfile");

	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	if(-f $sisfile){
		CreateFile($file,$TempData);
	}

	if( -f $file ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}

	unlink("$pkgfile.sis");
	unlink("$pkgfile.log");
	unlink("$pkgfile.pkg");
}


 
#
# New test code for DEF091780 - Makesis have problems parsing IF-ENDIF block
# test files are generated according to defect description, except that ALL files are located in current folder
#
sub TestDEF091780 {

	my $ifendif = "ifendif.pkg";
	my $expectedResult = 0;

	WriteLog("Test for DEF091780 - Makesis have problems parsing IF-ENDIF block\n");

	# Create PKG file
	CreateFile($ifendif , $ifendifContent);
	$ifendif =~ s/\.pkg//;
	
	# Create options-related files
	CreateFile('osver1J.txt', "1J");
	CreateFile('osver2J.txt', "2J");
	CreateFile('osver1F.txt', "1F");
	CreateFile('osver2F.txt', "2F");

	# Do MAKESIS test
	@retval = system("$makesisExeLocation $ifendif.pkg $ifendif-tmp.sis > $ifendif.log");
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;
	
	if( $? == $expectedResult ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}	
	
	# tidy up
	unlink("$ifendif.pkg");
	unlink("$ifendif-tmp.sis");
	unlink("$ifendif.sis");
	unlink("$ifendif.log");
	unlink("osver1J.txt");
	unlink("osver2J.txt");
	unlink("osver1F.txt");
	unlink("osver2F.txt");
}

#
# Test code for SIS files with comma at the end of the file - DEF108815
# 
#
sub TestEndFileComma {

	my $pkgName = "endFileComma.pkg";
	my $logName = "endFileComma.log";
	my $sisName = "endFileComma.sis";
	my $expectedResult = 256;
	
	WriteLog("Test for DEF108815 - makesis crashes if a trailing comma is present on an install-file \n");

	# Create PKG file
	CreateFile($pkgName, $endFileCommaContent);
	$ifendif =~ s/\.pkg//;
	
	# Do MAKESIS test
	@retval = system("$makesisExeLocation $pkgName $sisName > $logName");
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;
	
	if( $? == $expectedResult ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}	
	
	# tidy up
	#unlink("$pkgName");
	unlink("$sisName");
	unlink("$logName");
}

#
# Test code for package file containing very long destination folder name - DEF115795
# 
#
sub TestLongFolderName {

	my $pkgName = "longfolder.pkg";
	my $logName = "longfolder.log";
	my $sisName = "longfolder.sis";
	my $expectedResult = 256;
	
	WriteLog("Test for DEF115795 - SWI completes the installation despite the files not being installed \n");

	# Create PKG file
	CreateFile($pkgName, $longfolder);
	$ifendif =~ s/\.pkg//;
	
	# Do MAKESIS test
	@retval = system("$makesisExeLocation $pkgName $sisName > $logName");
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;
	
	if( $? == $expectedResult ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}	
	
	# tidy up
	unlink("$pkgName");
	unlink("$sisName");
	unlink("$logName");
}

#
#  Test code for invalid version number in pkg file - DEF112718
# 
#
sub TestInvalidVersion1 {

	my $pkgName = "invalidVersion.pkg";
	my $LogFile = "invalidVersion.log";
	my $sisName = "invalidVersion.sis";
	my $ExpectedLogFile = "InvalidVersionExpected.log";
	
	WriteLog("Test for DEF112718 - Invalid version number in pkg file \n");

	# Create PKG file
	CreateFile($pkgName, $invalidVersion1);
	
	my $trailingData = "Created  $sisName.";
	my $OutputData = "Processing $pkgName...\n$DEF112718ExpectedOutput$trailingData";
	
	# Create expected log file
	CreateFile($ExpectedLogFile ,$OutputData);

	# Create a sis file
	my $result = system("$makesisExeLocation $pkgName $sisName > $LogFile");
	
	use File::Compare;
	my $result1;

	if(compare($LogFile ,$ExpectedLogFile)== 0)
 		{ 
 		$result1 = 0;			
 		}
 	else
 		{
 		$result1 = 1;
 		}
	$NumberOfTests++;

 	if ($result == 0 && $result1 == 0) 
 		{
 		$NumberOfPassed++;
 		WriteLog("Passed\n\n");
 		}
 	else 
 		{
 		$NumberOfFailed++;
 		WriteLog("Failed\n\n");
 		}

	
	# tidy up
	unlink("$pkgName");
	unlink("$sisName");
	unlink("$logName");
	unlink ("$LogFile");
}

#
#  Test code for invalid version number in pkg file - DEF112718
# 
#

sub TestInvalidVersion2 {

	my $pkgName = "invalidVersion.pkg";
	my $logName = "invalidVersion.log";
	my $sisName = "invalidVersion.sis";
	my $expectedResult = 256;
	
	WriteLog("Test for DEF112718 - Invalid version number in pkg file with negative values \n");

	# Create PKG file
	CreateFile($pkgName, $invalidVersion2);
	
	
	# Do MAKESIS test
	@retval = system("$makesisExeLocation $pkgName $sisName > $logName");
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;
	
	if( $? == $expectedResult ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}	
	
	# tidy up
	unlink("$pkgName");
	unlink("$sisName");
	unlink("$logName");
}


#
#  Test code for Preinstalled pkg files without having sourcefiles - DEF113569
# 
#
sub TestPreInstalledWithoutSourceFiles {

	my $pkgName = "preinstalledwithoutsourcefile.pkg";
	my $logName = "preinstalledwithoutsourcefile.log";
	my $sisName = "preinstalledwithoutsourcefile.sis";
	my $ExpectedLogFile = "preinstalledwithoutsourcefileExpected.log";

	WriteLog("Test for DEF113569 - Preinstalled pkg files without having sourcefiles \n");

	# Create PKG file
	CreateFile($pkgName, $PreinstalledPkgWithoutSourcefiles);

	my $OutputData = "Processing $pkgName...\n$DEF113569ExpectedOutput";
	
	# Create expected log file
	CreateFile($ExpectedLogFile ,$OutputData);

	# Do MAKESIS test
	@retval = system("$makesisExeLocation $pkgName $sisName > $logName");

	use File::Compare;
	my $result1;

	if(compare($logName ,$ExpectedLogFile)== 0)
 		{ 
 		$result1 = 0;			
 		}
 	else
 		{
 		$result1 = 1;
 		}

	$NumberOfTests++;

	
	if ($result == 0 && $result1 == 0) 
 		{
 		$NumberOfPassed++;
 		WriteLog("Passed\n\n");
 		}
 	else 
 		{
 		$NumberOfFailed++;
 		WriteLog("Failed\n\n");
 		}

	# tidy up
	unlink("$pkgName");
	unlink("$sisName");
	unlink("$logName");
	unlink ("$ExpectedLogFile");
}


# New test code for DEF091860 - Quotes missing in reversed package produced by dumpsis for IF EXIST statement. 
# This creates a DEF091860.sis file . This test checks whether the pkg file generated by dumpsis when reprocessed 
# by makesis generates a sis file successfully.
#
sub TestDEF091860() {

	$DEF091860pkg = "DEF091860.pkg";
	$DEF091860sis = "DEF091860.sis";
	$DumpsisGenPkgPath = "\/DEF091860";
	$DEF091860Log = "DEF091860.log";

				
	WriteLog("Test for DEF091860 - Test for successful creation of a sis file when pkg file generated from dumpsis is used.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $DEF091860PkgContents, "-1,-1,-1"); 
	# Create PKG file
	CreateFile($DEF091860pkg, $PkgFile);	
	
	# Create DEF091860.sis file
	my $result = system("$makesisExeLocation $DEF091860pkg $DEF091860sis > $DEF091860Log");
	
	WriteLog("result: $result\n");

	# Execute DumpSIS on the created DEF091860.sis.
	my $result1 = system("/epoc32/tools/DUMPSIS -x $DEF091860sis > $DEF091860Log");
	
	WriteLog("result1: $result1[0]\n");

	use Cwd;
    $dir = cwd;
	chdir $dir.$DumpsisGenPkgPath;
	
	#Regenerate DEF091860.sis from the dumpsis generated pkg file.
	$result2  = system("$makesisExeLocation $DEF091860pkg $DEF091860sis > $DEF091860Log");
	chdir $dir;
	
	WriteLog("result2: $result2\n");

	$NumberOfTests++;
	if ($result == 0 && $result1 == 0 && $result2 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $DEF091860pkg;
	unlink $DEF091860sis;
	unlink $DEF091860Log;
	use File::Path;
	rmtree "$dir$DumpsisGenPkgPath";
}

#
# New test code for DEF090912  makesis shows error: file I/O fault. for PC folder with unicode (katakana) UCS2 
# NB: japanese source file is located in current folder instead of on folder tree for simplicity
#
sub TestDEF090912 {

	my $path = "\\epoc32\\winscw\\c\\tswi";
	my $ucs2 = "$path\\ucs2jpn";
	my $expectedResult = 0;

	WriteLog("Test for DEF090912: makesis shows error \"file I/O fault\" for PC folder with unicode (katakana) UCS2\n");
	
	# tricky thing to create file with Japanese  name on any locale
	# use //B to launch script engine in batch mode
	system("WScript.exe //B //Nologo $path\\ucs2jpn.vbs");

	# Do MAKESIS test
	@retval = system("$makesisExeLocation $ucs2.pkg $ucs2-tmp.sis > $ucs2.log");
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;
	
	if( $? == $expectedResult ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}	
	
	# tidy up
	unlink("$ucs2-tmp.sis");
	unlink("$ucs2.log");
	system("WScript.exe //B //Nologo $path\\rmucs2jpn.vbs");
}


# Linux Test
# New test code for DEF090912  makesis shows error: file I/O fault. for PC folder with unicode (katakana) UCS2 
# NB: japanese source file is located in current folder instead of on folder tree for simplicity
#
sub TestLDEF090912 {

	my $path = "/epoc32/winscw/c/tswi";
	my $ucs2 = "$path/ﾅｿﾄ.txt";
	my $pkgfile = "$path/ucs2jpn";
	my $expectedResult = 0;

	WriteLog("Test for DEF090912: makesis shows error \"file I/O fault\" for PC folder with unicode (katakana) UCS2\n");
	
		
	# Create PKG file
	
	CreateFile($ucs2, $TempData);

	# Do MAKESIS test
	@retval = system("$makesisExeLocation -v $pkgfile.pkg  > $pkgfile.log");
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);

	$NumberOfTests++;
	
	if( $? == $expectedResult ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}	
	
	# tidy up
	unlink("$pkgfile.log");
	unlink("$pkgfile.sis");
}




#
# New test code for DEF093400:  Temporary files are left undeleted after MakeSIS/SignSIS call 
# NB: japanese source file is located in current folder instead of on folder tree for simplicity
#

sub TestDEF093400 
{
	WriteLog("Test for DEF093400: Temporary files are left undeleted after MakeSIS/SignSIS call\n");
	
	my $pkg = "temp.pkg";
	my $expectedResult = 0;

	# Create PKG file
	chmod S_IWUSR | S_IWGRP | S_IWOTH, '$pkg';
	my $temp = sprintf $PkgFileTempl, "-1,-1,-1"; 
	CreateFile($pkg, $temp);
	$pkg =~ s/\.pkg//;
	
	# Create options-related files
	chmod S_IWUSR | S_IWGRP | S_IWOTH, '$pkg.txt';
	CreateFile("$pkg.txt", "temp file");
	chmod S_IRUSR | S_IRGRP | S_IROTH, '$pkg.txt';

	my $TmpCountBefore = GetTmpFilesCount();

	# Do MAKESIS test
	@retval = system("$makesisExeLocation $pkg.pkg $pkg-tmp.sis > $pkg.log");

	my $TmpCountAfter = GetTmpFilesCount();
	
	$logMsg = sprintf "Expected code:%5d   result Code:%5d\n", $expectedResult, $?;
	WriteLog( $logMsg);
	$logMsg = sprintf "Temp Files detected - Before:%5d   After:%5d\n", $TmpCountBefore, $TmpCountAfter;
	WriteLog( $logMsg);

	$NumberOfTests++;
	
	if(  $? == $expectedResult && $TmpCountBefore == $TmpCountAfter ) {
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else {
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}	
	
	# tidy up
	unlink("$pkg.pkg");
	unlink("$pkg-tmp.sis");
	unlink("$pkg.txt");
	unlink("$pkg.log");
}


#
# New test code for DEF093156 - MAKEKEYS does not process correctly non European symbols in cmd line 
# This test checks whether the certificate and key files which have names with japanese symbols are created successfully .
#
sub TestDEF093156 {

	my $path = "\\epoc32\\winscw\\c\\tswi";
 	$DEF093156Log = "DEF093156.log";
	$ResFile1="ex.txt";
	$ResFile2="ResultMakeKeys.txt";
 
	WriteLog("Test for DEF093156 - Makekeys test for successful creation of a certificate and key file with japanese symbols.\n");
  
 	# create a key file and a certificate file with japanese symbols
	system("WScript.exe //B //Nologo $path\\testmakekeys.vbs > $DEF093156Log");

 	$NumberOfTests++;
	if ( !-f $ResFile1 )
	{
 		$NumberOfFailed++;
 		WriteLog("Failed In Step 1\n\n");
		unlink $DEF093156Log;
		exit;
	}
	
	#Make sure that the DN in the created certificate with japanese symbols is encoded properly
	system("WScript.exe //B //Nologo $path\\testmakekeys2.vbs $path 1 > $DEF093156Log");
	
	if ( -f $ResFile2 )
	{
		$NumberOfPassed++;
 		WriteLog("Passed\n\n");
 	}
	else
	{
 		$NumberOfFailed++;
 		WriteLog("Failed In Step 2\n\n");
	}

 	unlink $DEF093156Log;
	unlink $ResFile1;
	unlink $ResFile2;
}

#
# New test code for INC092755: Makesis includes the same file to the generated SIS file if the pkg file has IF-ELSE condition block where only the destination path changes
#
sub TestINC092755 {

	WriteLog("Test for INC092755: Makesis includes the same file for IF-ELSE where only the dest path changes\n");
	
	my $path = "\\epoc32\\winscw\\c\\tswi";
	my $INC092755 = "INC092755";
	
	# Call DumpSIS to compare SIS file against expected result and number of files extracted
	WriteLog("Calling DumpSIS...\n");
	@retval = system("/epoc32/tools/DUMPSIS -x $path\\$INC092755.sis > $INC092755.log");
	
	use Cwd;
    my $dir = cwd."/$INC092755";
	opendir ( DIR, "$dir" ) or die ( "Can't open dir: $dir" ); #!
	my @List = readdir DIR;
	closedir DIR;
	
	my @files = grep( /^file[0-9]*/ , @List );
	my $fileCount = @files;
	print "Extracted: $fileCount file(s)	Expected: 1 file(s)\n";
	
	$NumberOfTests++;
	if ( $fileCount == 1 )
	{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
	}
	else
	{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
	}

	unlink("$INC092755.log");
	rmdir("$INC092755");
	unlink("$INC092755.sis");
}

#
# New test code for CR904  
# This test checks for the successful creation of sis file when pkg file contains the parameter $ (which represents 
# system drive) in the destination file.
# 


sub TestsysDriveparameter {

	$pkgFilewithsysDriveparameter = "testsysDrive.pkg";
	$sisFile = "testsysDrive.sis";
	$LogFile = "testsysDrive.log";
	

	WriteLog("Test for successful creation of a sis file with $ parameter specified in pkg file to represent system drive.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileTemp2, "-1,-1,-1"); 

	# Create PKG file
	CreateFile($pkgFilewithsysDriveparameter, $PkgFile);	
	
		
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $pkgFilewithsysDriveparameter $sisFile > $LogFile");
	
	
	$NumberOfTests++;
	if ($result == 0 ) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $pkgFilewithsysDriveparameter;
	unlink $sisFile;
	unlink $LogFile
	
}


#
# New test code for CR1027 - SA ROM Upgrade  
# This test checks for the successful creation of sis file when PKG file contains the newly added ( as part of CR1027 ) 
# ROM upgrade capability install flag RU with one of the right install types SA ( only SA, PU & SP are valid with RU).
# 

sub TestSisRUWithSA {

	$pkgFilewithRUAndSA = "testsysRUWithSA.pkg";
	$sisFile = "testsysRUWithSA.sis";
	$LogFile = "testsysRUWithSA.log";
	

	WriteLog("CR1027 - Test for successful creation of a sis file with the ROM Upgrade install flag RU with the right install type SA.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileRUWithSA, "-1,-1,-1"); 

	# Create PKG file
	CreateFile($pkgFilewithRUAndSA, $PkgFile);	
	
		
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $pkgFilewithRUAndSA  $sisFile > $LogFile");
	
	$NumberOfTests++;
	if ($result == 0 ) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $pkgFilewithRUAndSA;
	unlink $sisFile;
	unlink $LogFile	
}



#
# New test code for CR1027 - SA ROM Upgrade  
# This test checks for the un-successful creation of sis file when PKG file contains the newly added ( as part of CR1027 ) 
# ROM upgrade capability install flag RU with the wrong install type other than SA, PU & SP (like PA & PP.).
# 

sub TestSisRUWithNonSA {

	$pkgFilewithRUAndNonSA = "testsysRUWithNonSA.pkg";
	$sisFile = "testsysRUWithNonSA.sis";
	$LogFile = "testsysRUWithNonSA.log";
	

	WriteLog("CR1027 - Test for Un-successful creation of a sis file with the ROM Upgrade install flag RU with the wrong install type, anything other than SA.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileRUWithNonSA, "-1,-1,-1"); 

	# Create PKG file
	CreateFile($pkgFilewithRUAndNonSA, $PkgFile);	
	
		
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $pkgFilewithRUAndNonSA  $sisFile > $LogFile");
	
	
	$NumberOfTests++;
	if ($result != 0 ) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $pkgFilewithRUAndNonSA;
	unlink $sisFile;
	unlink $LogFile	
}

#
# New test code for CR1122 - WildCard Support for ROM Stubs.
# This test checks for the successful creation of sis file when PKG file contains the wildcard charetors(? and *) in it. 
# These packages can be upgraded (eclipsed) in much wider way as per the wildcard charector's behaviour.
# 

sub TestSISWithWildCards {

	$pkgFilewithWildCards = "testSysWithWildCards.pkg";
	$sisFile = "testSysWithWildCards.sis";
	$LogFile = "testSysWithWildCards.log";
	

	WriteLog("CR1122 - Test for successful creation of a sis file for wildcarded ROM stub package file.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $WildCardedpkgFile, "-1,-1,-1"); 

	# Create PKG file
	CreateFile($pkgFilewithWildCards, $PkgFile);	
	
		
	# Create an embedding sis file
	my $result = system("$makesisExeLocation -s $pkgFilewithWildCards  $sisFile > $LogFile");
	
	
	$NumberOfTests++;
	if ($result == 0 ) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $pkgFilewithWildCards;
	unlink $sisFile;
	unlink $LogFile;
}


#
# New test code for CR1122 - WildCard Support for ROM Stubs.
# This test checks for the Un-successful creation of sis file when PKG file contains the wildcard charetors(? and *) and PA install type in it.
# 

sub TestSISWithWildCardsPA {

	$pkgFilewithWildCards = "testSysWithWildCards.pkg";
	$sisFile = "testSysWithWildCards.sis";
	$LogFile = "testSysWithWildCards.log";
	

	WriteLog("CR1122 - Test for un-successful creation of a SIS file for wildcarded PA package file.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $WildCardedPApkgFile, "-1,-1,-1"); 

	# Create PKG file
	CreateFile($pkgFilewithWildCards, $PkgFile);	
	
		
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $pkgFilewithWildCards  $sisFile > $LogFile");
	
	
	$NumberOfTests++;
	if ($result != 0 ) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $pkgFilewithWildCards;
	unlink $sisFile;
	unlink $LogFile;
}


#
# New test code for CR1122 - WildCard Support for ROM Stubs.
# This test checks for the Un-successful creation of sis file when PKG file contains the wildcard charetors(? and *) and PP install type in it.
#
sub TestSISWithWildCardsPP {

	$pkgFilewithWildCards = "testSysWithWildCards.pkg";
	$sisFile = "testSysWithWildCards.sis";
	$LogFile = "testSysWithWildCards.log";
	

	WriteLog("CR1122 - Test for un-successful creation of a SIS file for wildcarded PA package file.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $WildCardedPPpkgFile, "-1,-1,-1"); 

	# Create PKG file
	CreateFile($pkgFilewithWildCards, $PkgFile);	
	
		
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $pkgFilewithWildCards  $sisFile > $LogFile");
	
	
	$NumberOfTests++;
	if ($result != 0 ) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $pkgFilewithWildCards;
	unlink $sisFile;
	unlink $LogFile;
}


sub TestSingleCharFilename {

	$pkgFileName = "temp.pkg";
	$sisFile = "t.sis";
	$LogFile = "singleCharFilename.log";
	
	WriteLog("Test for successful creation of a sis file with a single char in its filename.\n"); #DEF108728

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileTemp2, "-1,-1,-1"); 

	# Create PKG file
	CreateFile($pkgFileName, $PkgFile);	
			
	# Create an embedding sis file
	my $result = system("$makesisExeLocation $pkgFileName $sisFile > $LogFile");
	
	
	$NumberOfTests++;
	if ($result == 0 ) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed with result $result\n\n");
		}

	unlink $pkgFileName;
	unlink $sisFile;
	unlink $LogFile
}


#
# Test code for DEF111264 - Makesis should warn if wildcards are used in the \sys\bin\ directory in ROM stubs .
# This test checks verifies that a warning is generated when a ROM Stub file contains an exe with wildcards(* or ?) specified . 
# 

sub TestDEF111264 {

	$pkgFile = "ROMStubWildCardWarning.pkg";
	$sisFile = "ROMStubWildCardWarning.sis";
	$LogFile = "ROMStubWildCardWarning.log";
	$ExpectedLogFile = "ROMStubWildCardExpected.log";

	WriteLog("DEF111264 - Makesis should warn if wildcards are used in the \sys\bin\ directory in ROM stubs.\n");

	# Generate test PKG file contents for pkg file.
	$PkgFile = sprintf( $ExeWithWildCardpkgFile, "-1,-1,-1"); 
	
	# Create PKG file
	CreateFile($pkgFile, $PkgFile);	

	# Create expected log file
	my $trailingData = "Created  $sisFile.";
	my $OutputData = "Processing $pkgFile...\n$DEF111264ExpectedOutput$trailingData";
	
	CreateFile($ExpectedLogFile ,$OutputData);

	# Create a sis file
	my $result = system("/epoc32/tools/makesis -s $pkgFile  $sisFile > $LogFile");

	use File::Compare;
	my $result1;
	
	if(compare($LogFile ,$ExpectedLogFile)== 0)
		{ 
		$result1 = 0;			
		}
	else
		{
		$result1 = 1;
		}

	$NumberOfTests++;
	
	if ($result == 0 && $result1 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	unlink $pkgFile; 
	unlink $sisFile;
	unlink $LogFile;
	unlink $ExpectedLogFile;
}


#
# Test code for DEF113349 - Attempting to embed a PP SIS file in an SA SIS file causes makesis to crash.
# This test checks verifies that a warning is generated when attempted to embed a PP/PA SIS file in an SA SIS. 
# 

sub TestDEF113349 {

	$pkgEmbeddedFile = "EmdeddedPA.pkg";
	$sisEmbeddedFile = "EmbeddedPA.sis";
	$pkgFile = "EmdeddingPA.pkg";
	$sisFile = "EmbeddingPA.sis";
	$LogFile = "DEF113349.log";
	$ExpectedLogFile = "DEF113349Expected.log";

	WriteLog("DEF113349 - Attempting to embed a PP SIS file in an SA SIS file causes makesis to crash.\n");

	# Generate test PKG file contents for the embedded pkg file.
	$PkgFile = sprintf( $EmbeddedPApkgFile, "-1,-1,-1"); 
	
	# Create PKG file
	CreateFile($pkgEmbeddedFile, $PkgFile);	

	# Create SIS file for the embedded package of type = PA.
	my $result = system("/epoc32/tools/makesis $pkgEmbeddedFile $sisEmbeddedFile > $LogFile");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $EmbeddingPApkgFile, "-1,-1,-1"); 
	
	# Create PKG file
	CreateFile($pkgFile , $PkgFile);	
	
	# Create SIS file for the embedding package of type = SA.
	my $result1 = system("/epoc32/tools/makesis $pkgFile $sisFile > $LogFile");
	
	my $OutputData = "Processing $pkgFile...\n$DEF113349ExpectedOutput";
	
	# Create expected log file
	CreateFile($ExpectedLogFile ,$OutputData);

	use File::Compare;
	my $result2;
	
	if(compare($LogFile ,$ExpectedLogFile)== 0)
		{ 
		$result2 = 0;			
		}
	else
		{
		$result2 = 1;
		}

	$NumberOfTests++;
	
	if ($result == 0 && $result1 == 256 && $result2 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
	
	unlink $pkgEmbeddedFile;
	unlink $sisEmbeddedFile;
	unlink $pkgFile; 
	unlink $sisFile;
	unlink $LogFile;
	unlink $ExpectedLogFile;
}

#
# Test code for DEF113116 : It is not possible to abort an installation without causing an error. 
# This test checks for the successful creation of sis file when pkg file contains a new display text option
# ForceAbort (FA).
# 


sub TestDEF113116 {

	$pkgFile = "textoption_FA.pkg";
	$sisFile = "textoption_FA.sis";
	$LogFile = "textoption_FA.log";
	$DumpsisGenPkgPath = "\/textoption_FA";

	WriteLog("DEF113116 : Test MakeSIS and DumpSIS support for ForceAbort (FA) text option.\n");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileWithFAOption, "-1,-1,-1"); 

	# Create PKG file
	CreateFile($pkgFile , $PkgFile);	

	# Create sis file using MakeSIS
	my $result = system("$makesisExeLocation $pkgFile $sisFile > $LogFile");

	# Recreate pkg file using DumpSIS.
	my $result1 = system("$dumpsisExeLocation $sisFile > $LogFile");
	
	$NumberOfTests++;
	if ($result == 0 && $result1 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}

	use cwd;
	$dir = cwd;

	unlink $pkgFile;
	unlink $sisFile;
	unlink $LogFile;

	use File::Path;
	rmtree "$dir$DumpsisGenPkgPath";
	
}

#
# Test code for Makesis -c option.
# This test verifies that errors are reported where the SIS file being generated will not be installable by InterpretSIS.
# 

sub TestInterpretsisReport {

	$pkgEmbeddedFile = "Emdedded.pkg";
	$sisEmbeddedFile = "Embedded.sis";
	$pkgFile = "Interpretsis.pkg";
	$sisFile = "Interpretsis.sis";
	$LogFile = "Interpretsis.log";
	$ExpectedLogFile = "InterpretsisExpected.log";

	WriteLog("Makesis -c\n");

	# Generate test PKG file contents for the embedded pkg file.
	$PkgFile = sprintf( $PkgFileTempl, "-1,-1,-1"); 
	
	# Create PKG file
	CreateFile($pkgEmbeddedFile, $PkgFile);	

	# Create SIS file for the embedded package.
	my $result = system("/epoc32/tools/makesis $pkgEmbeddedFile $sisEmbeddedFile > $LogFile");

	# Generate test PKG file contents for embedding pkg file.
	$PkgFile = sprintf( $PkgFileInterpretsisVersionTemplate, "-1,-1,-1"); 
	
	# Create PKG file
	CreateFile($pkgFile , $PkgFile);	
	
	# Create SIS file for the embedding package of type = SA.
	my $result1 = system("/epoc32/tools/makesis -c $pkgFile $sisFile > $LogFile");
	
	# Create expected log file
	my $trailingData = "Created  $sisFile.";
	my $OutputData = "Processing $pkgFile...\n$InterpretsisExpectedOutput$trailingData";
	
	CreateFile($ExpectedLogFile ,$OutputData);
	
	use File::Compare;
	my $result2;
	
	if(compare($LogFile ,$ExpectedLogFile)== 0)
		{ 
		$result2 = 0;			
		}
	else
		{
		$result2 = 1;
		}

	$NumberOfTests++;
	
	if ($result == 0 && $result1 == 256 && $result2 == 0) 
		{
		$NumberOfPassed++;
		WriteLog("Passed\n\n");
		}
	else 
		{
		$NumberOfFailed++;
		WriteLog("Failed\n\n");
		}
	
	unlink $pkgEmbeddedFile;
	unlink $sisEmbeddedFile;
	unlink $pkgFile; 
	unlink $sisFile;
	unlink $LogFile;
	unlink $ExpectedLogFile;
}

#
# Main
#
# Create environment and control test flow to testing MAKESIS.EXE 
# (generate text.txt, pkg file, test makesis and check the result)
#

unlink($logFile);
WriteLog("makesis test.\n\n");

#
# Generate test.txt this is the only one element of every PKGs.
#
$contents = "This is a test text.";
CreateFile('TEST.txt', $contents);

#
# Generate HelloApp.exe this is the only one element of every PKGs.
#
$contents = "This is a test exe.";
CreateFile('HelloApp.exe', $contents);

#
# Counters for results
#
$NumberOfTests  = 0;
$NUmberOfPassed = 0;
$NumberOfFailed = 0;

#
# Array of contents of test pkgs and expected results
#
#                file name,            	range,      		expected, expected log,            		Title
#                                	(from)      (to)   	result    message
#               ----------------------------------------------------------------------------------------------------------------------------------------- 
@TestItems = (	["test01.pkg", 		"-1,-1,-1",		0,	"Generating SIS installation file...",	"Range not specified 1."],
		["test02.pkg", 		"-1,-1,-1 ~ -1,-1,-1",	0,	"Generating SIS installation file...",	"Range not specified 2."],
		["test03.pkg", 		"1, 0, 0",		0,	"Generating SIS installation file...",	"Only from specified 1."],
		["test04.pkg", 		"1,-1,-1 ~ -1,-1,-1",	0,	"Generating SIS installation file...",	"Only from specified 2."],
		["test05.pkg", 		"1,-1,-1 ~  2,-1,-1",	0,	"Generating SIS installation file...",	"Correct from and to specified."],
		["test06.pkg", 		"-1,-1,-1 ~  2,-1,-1",	0,	"Generating SIS installation file...",	"Only to specified."],
		["test07.pkg", 		"1,-1,-1 ~  1,-1,-1",	0,	"Generating SIS installation file...", 	"Correct. Same Major, not minor specified."],
		["test08.pkg", 		"1, 1,-1 ~  1, 1,-1",	0,	"Generating SIS installation file...", 	"Correct. Same Major, minor and not build specified."],		
		["test09.pkg", 		"1, 1,-1 ~  1, 2,-1",	0,	"Generating SIS installation file...",	"Correct. Same Major and differ minor specified."],
		["test10.pkg", 		"1, 1, 1 ~  1, 1, 2",	0,	"Generating SIS installation file...",	"Correct. Same Major, Minor and differ build specified."],
		["test11.pkg", 		"1, 1,-1 ~  1, 1, 2",	0,	"Generating SIS installation file...",	"Correct. Same Major, Minor and differ build specified."],
		["test12.pkg", 		"1, 1, 1 ~  1, 1, 1",	0,	"Generating SIS installation file...",	"Correct. Same Major, minor and build specified."],
		["test13.pkg", 		"4,-1,-1 ~  2,-1,-1",	256,	"verification failure",			"Wrong range of major of from and to."],
		["test14.pkg", 		"1, 2,-1 ~  1, 1,-1",	256,	"verification failure",			"Wrong range of minor of from and to."],
		["test15.pkg", 		"1, 2, 2 ~  1, 2, 1", 	256,	"verification failure",			"Wrong range of build of from and to."],
		["test16.pkg", 		"1,-1,-1 ~  2,1,1 ",	256,	"invalid destination path or syntax",	"exe name contains non ascii characters."],
		["test17.pkg", 		"1,-1,-1 ~  2,1,1 ",	256,	"invalid destination path or syntax",	"package file encoding is not supported."],
		["testForward.pkg", 	"1,-1,-1 ~  2,1,1 ",	0,	"Generating SIS installation file...",	"package file can have forward slash."],
		["testFN_RI.pkg", 	"1,0,0 ~ 1,0,0", 0, "Generating SIS installation file...", "DEF126367: Check interaction between FN and RI."]
	     );


# For DEF92320
# Test for filename & Dir name, should not contain symbols like |,/,<,>,:,?,*
# Array of contents of test pkgs 
#
#              	file name,     		symbols ,  	expected , 	expected log,            		Title
#                                			result     	message
#               -------------------------------------------------------------------------------------------------------------------------- 
@TestItems1 = (	["test01.pkg", 		"*", 		256,		"invalid destination path or syntax", 	"File paths should not contain *,:,<,>"],
		["test02.pkg", 		"\"<42>\"", 	256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test03.pkg", 		"?", 		256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test05.pkg", 		" \"", 		256,		"error: unknown line",			"File paths should not contain *,:,<,>"],
		["test06.pkg", 		"<", 		256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test07.pkg", 		">",  		256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test08.pkg", 		"|",  		256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test11.pkg",  	"\"<58>\"",	256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test12.pkg", 		":",		256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test13.pkg",   	"\"<63>\"",	256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test14.pkg", 		"\"<34>\"", 	256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test15.pkg", 		"\"<62>\"", 	256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test16.pkg", 		"\"<124>\"", 	256,	 	"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test17.pkg", 		"\"<60>\"", 	256,		"invalid destination path or syntax",	"File paths should not contain *,:,<,>"],
		["test18.pkg", 		"\"<92>\"", 	0,		"Generating SIS installation file...",	"contain \\"],
		["test19.pkg", 		"*", 		256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test20.pkg", 		"\"<42>\"", 	256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test21.pkg", 		"?", 		256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test23.pkg", 		" \"", 		256,		"error: unknown line",			"DIR paths should not contain *,:,<,>"],
		["test24.pkg", 		"<", 		256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test25.pkg", 		">",  		256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test26.pkg", 		"|",  		256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test28.pkg", 		"\"<58>\"",	256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test29.pkg", 		":",		256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test30.pkg", 		"\"<63>\"",	256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test31.pkg", 		"\"<34>\"", 	256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test32.pkg", 		"\"<62>\"", 	256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test33.pkg", 		"\"<124>\"", 	256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test34.pkg", 		"\"<60>\"", 	256,		"invalid destination path or syntax",	"DIR paths should not contain *,:,<,>"],
		["test35.pkg", 		"\"<92>\"", 	0,		"Generating SIS installation file...",	"DIR pcontain\\"],
	    );



# CR1125 - Add Package Versions to SIS File Conditionals Test Cases
# Array of test PKG data and expected results for each test case and associated itterations
#
#              	file name,	conditional statement,			Expected,	Expected					Test Case ID
#									result		log
#               ------------------------------------------------------------------------------------------------------------------------------------ 
@TestItems2 = (	["test01.pkg", "VERSION(0xE1000001,=,1,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A001 : A"],
		["test02.pkg", "VERSION(0xE1000001,<>,1,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A001 : B"],
		["test03.pkg", "VERSION(0xE1000001,>,1,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A001 : C"],
		["test04.pkg", "VERSION(0xE1000001,>=,1,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A001 : D"],
		["test05.pkg", "VERSION(0xE1000001,<,1,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A001 : E"],
		["test06.pkg", "VERSION(0xE1000001,<=,1,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A001 : F"],
		["test07.pkg", "VERSION(test,=,1,2,3)",			256,		"Expected numeric value read alphanumeric value",	"CR1125 SEC-SWI-PKGVER-A002 : A"],
		["test08.pkg", "VERSION(0xE1000001,test,1,2,3)",	256,		"Invalid Relational Operator",			"CR1125 SEC-SWI-PKGVER-A002 : B"],
		["test09.pkg", "VERSION(0xE1000001,=,test,2,3)",	256,		"Expected numeric value read alphanumeric value",	"CR1125 SEC-SWI-PKGVER-A002 : C"],
		["test10.pkg", "VERSION(0xE1000001,=,1,test,3)",	256,		"Expected numeric value read alphanumeric value",	"CR1125 SEC-SWI-PKGVER-A002 : D"],
		["test11.pkg", "VERSION(0xE1000001,=,1,2,test)",	256,		"Expected numeric value read alphanumeric value",	"CR1125 SEC-SWI-PKGVER-A002 : E"],
		["test12.pkg", "VERSION(0xE1000001,=,-2,2,3)",		256,		"Negative version components are not supported",	"CR1125 SEC-SWI-PKGVER-A003 : A"],
		["test13.pkg", "VERSION(0xE1000001,=,-1,2,3)",		256,		"Wildcards are not supported",			"CR1125 SEC-SWI-PKGVER-A003 : B"],
		["test14.pkg", "VERSION(0xE1000001,=,0,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : C"],
		["test15.pkg", "VERSION(0xE1000001,=,1,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : D"],
		["test16.pkg", "VERSION(0xE1000001,=,126,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : E"],
		["test17.pkg", "VERSION(0xE1000001,=,127,2,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : F"],
		["test18.pkg", "VERSION(0xE1000001,=,128,2,3)",		0,		"Warning : The valid version number ranges are :",		"CR1125 SEC-SWI-PKGVER-A003 : G"],
		["test19.pkg", "VERSION(0xE1000001,=,1,-2,3)",		256,		"Negative version components are not supported",	"CR1125 SEC-SWI-PKGVER-A003 : H"],
		["test20.pkg", "VERSION(0xE1000001,=,1,-1,3)",		256,		"Wildcards are not supported",			"CR1125 SEC-SWI-PKGVER-A003 : I"],
		["test21.pkg", "VERSION(0xE1000001,=,1,0,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : J"],
		["test22.pkg", "VERSION(0xE1000001,=,1,1,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : K"],
		["test23.pkg", "VERSION(0xE1000001,=,1,98,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : L"],
		["test24.pkg", "VERSION(0xE1000001,=,1,99,3)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : M"],
		["test25.pkg", "VERSION(0xE1000001,=,1,100,3)",		0,		"Warning : The valid version number ranges are :",		"CR1125 SEC-SWI-PKGVER-A003 : N"],
		["test26.pkg", "VERSION(0xE1000001,=,1,2,-2)",		256,		"Negative version components are not supported",	"CR1125 SEC-SWI-PKGVER-A003 : O"],
		["test27.pkg", "VERSION(0xE1000001,=,1,2,-1)",		256,		"Wildcards are not supported",			"CR1125 SEC-SWI-PKGVER-A003 : P"],
		["test28.pkg", "VERSION(0xE1000001,=,1,2,0)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : Q"],
		["test29.pkg", "VERSION(0xE1000001,=,1,2,1)",		0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : R"],
		["test30.pkg", "VERSION(0xE1000001,=,1,2,32766)",	0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : S"],
		["test31.pkg", "VERSION(0xE1000001,=,1,2,32767)",	0,		"Generating SIS installation file...",		"CR1125 SEC-SWI-PKGVER-A003 : T"],
		["test32.pkg", "VERSION(0xE1000001,=,1,2,32768)",	0,		"Warning : The valid version number ranges are :",		"CR1125 SEC-SWI-PKGVER-A003 : U"],
		["test33.pkg", "VERSION(0xE1000001,=,*,2,3)",		256,		"Wildcards are not supported",	"CR1125 SEC-SWI-PKGVER-A004 : A"],
		["test34.pkg", "VERSION(0xE1000001,=,1,*,3)",		256,		"Wildcards are not supported",	"CR1125 SEC-SWI-PKGVER-A004 : B"],
		["test35.pkg", "VERSION(0xE1000001,=,1,2,*)",		256,		"Wildcards are not supported",	"CR1125 SEC-SWI-PKGVER-A004 : C"],
		["test36.pkg", "VERSION(0xE1000001,=,1,2,)",		256,		"Expected numeric value read ",	"CR1125 SEC-SWI-PKGVER-A005 : A"],
		["test37.pkg", "VERSION(0xE1000001,=,1,)",		256,		"Expected numeric value read ",	"CR1125 SEC-SWI-PKGVER-A005 : B"],
		["test38.pkg", "VERSION(0xE1000001,=,)",		256,		"Expected numeric value read ",	"CR1125 SEC-SWI-PKGVER-A005 : C"],
		["test39.pkg", "VERSION(0xE1000001,)",			256,		"Invalid Relational Operator",	"CR1125 SEC-SWI-PKGVER-A005 : D"],
		["test40.pkg", "VERSION()",				256,		"Expected numeric value read ",	"CR1125 SEC-SWI-PKGVER-A005 : E"],
		["testinterpretflag.pkg", "VERSION(0x11113011,=,1,2,3)",		0,		"SIS installation file VALID for InterpretSis",	"CR1125 SEC-SWI-PKGVER-A006"],
	     );


#
# Template string to generate PKG file
#
$AnotherPkgFileTempl = "
;
; Autogenerated test install file
;
&EN
;
#{\"my test\"}, (0x80002233), 1, 2, 3, TYPE=SA
;
%%{\"Vendor\"}
;
;Installation name and header data
(0x101f74aa),%s,{\"test\"}
;
;Made up files to install
\"HelloApp.exe\"-\"c:\\sys\\bin\\multiÅžÑEEžÑEEÃŠ.exe\"
;
";


#
# Template string to generate PKG file
#
$PkgFileTempl = "
;
; Autogenerated test install file
;
&EN
;
#{\"test\"}, (0x101f74aa), 1, 0, 0, TYPE=SA
;
%%{\"Ati\"}
;
;Installation name and header data
(0x101f74aa),%s,{\"test\"}
;
;Made up files to install
\"TEST.txt\"-\"!:\\System\\Apps\\TEST.txt\"
;

";

#
#
# Pkg file can have forward slashes for source and destination file
$ForwardSlash = "
;
; Autogenerated test install file
;
&EN
;
#{\"test\"}, (0x101f74aa), 1, 0, 0, TYPE=SA
;
%{\"Ati\"}
;

;Made up files to install

\"TEST.txt\"-\"!:/Documents/InstTest/file1.txt\" 
;
";

#
#
# Package file contains a FileNull followed by a RunInstall. The wildcard in the FN should be allowed and 
# makesis should not be confused by the flags in the RI
$FN_RI =" 
&EN
#{\"Symbian Email\"}, (0x800126e5), 1, 0, 0, TYPE=SA
%{\"Symbian\"}
:\"Symbian\"

\"\"- \"C:/private/80009b91/temp/*.*\", FN
\"HelloApp.exe\"-\"C:/sys/bin/postinstall.exe\", FR, RI
";

#
#
# Pkg file can have forward slashes for source and destination file
$ForwardSlash = "
;
; Autogenerated test install file
;
&EN
;
#{\"test\"}, (0x101f74aa), 1, 0, 0, TYPE=SA
;
%{\"Ati\"}
;

;Made up files to install

\"TEST.txt\"-\"!:/Documents/InstTest/file1.txt\" 
;
";


#
# Template string to generate PKG file
#
$PkgFileTemp2 = "
;
; Autogenerated test install file
;
&EN
;
#{\"test\"}, (0x101f74aa), 1, 0, 0, TYPE=SA
;
%%{\"Ati\"}
;
;Installation name and header data
(0x101f74aa),%s,{\"test\"}
;
;Made up files to install
\"TEST.txt\"-\"\$\:\\System\\Apps\\TEST.txt\"
;
";

#
# Template string to generate embedded package (testembedded.pkg).This is used for testing -d option of makesis .
#
$embedContents = "
;
; Autogenerated test install file for testing -d option.
;
&EN
;
#{\"embed\"}, (0x101f74a0), 1, 0, 0, TYPE=SA
;
%{\"Ati\"}
;
;Installation name and header data
(0x101f74a0),%s,{\"embed\"}
;
;embedded sis file
\@\"testembedding.sis\",(0x101f74aa)

;Made up files to install
\"TEST.txt\"-\"!:\\System\\Apps\\TEST.txt\"
;
";

#
# Template string to generate a package with 0xA0 chars (No-Break Space) inside
#
$ifendifContent = "
;
; Autogenerated test install file for testing DEF091780: Makesis have problems parsing IF-ENDIF block
;

;Languages
&JA,FR

;Header
#{\"JA hira UCS2\",\"FR hira UCS2\" }, (0x80000003), 1, 2, 3, TYPE=SA
%{\"AA-JA\",\"CC-FR\"}
:\"venDor\"

!({\"JAOpt1\",\"FROpt1\"},{\"JAOpt3\",\"FROpt3\"})

if option1 
\xA0\xA0\xA0{\"osver1J.txt\" \"osver1F.txt\"}-\"!:\\documents\\hello.txt\"; 
endif

if option2 
\xA0\xA0\xA0{\"osver2J.txt\" \"osver2F.txt\"}-\"!:\\documents\\hello.txt\"; 
endif
";

#
#
# A string to generate a package with Japanese JIS 0208-1990 encoding
$unsupportedEncContent = "
;Languages
&JA

;Header
#{\"PC Japanese JIS 0208-1990 encoding\"}, (0xA2000222), 1, 2, 3, TYPE=SA

%{\"Vendor\"}
:\"Vendor\"

\"TEST.txt\"-\"!:\Documents\InstTest\Ã±ÃÃ±âÃ±âEtxt\" 
";


#
# Template string to generate PKG file with If Exist statement for DEF091860.
#
$DEF091860PkgContents = "
;
; PKG file designed to test conditional installation.
; Autogenerated test install file
;
&EN
;
#{\"test\"}, (0x101f74aa), 1, 0, 0, TYPE=SA
;
%{\"Ati\"}
;
;Installation name and header data
(0x101f74aa),%s,{\"test\"}
;
;Made up files to install
\"TEST.txt\"-\"!:\\System\\Apps\\TEST.txt\"
;
; The file âtest1.txtâEis expected to exist.
if exists(\"test1.txt\")
\"TEST.txt\"-\"!:\\System\\Apps\\TEST1.txt\"
else
\"TEST.txt\"-\"!:\\System\\Apps\\TEST2.txt\"
endif

; The file âtest2.txtâEis not expected to exist.
if exists(\"test2.txt\")
\"TEST.txt\"-\"!:\\System\\Apps\\TEST3.txt\"
else
\"TEST.txt\"-\"!:\\System\\Apps\\TEST4.txt\"
endif
;
";

#
# Template string to generate PKG file for language dependent files
#
$PkgLanguageFileTemp = "
;
;
&EN,FR,98
;
#{\"Language-EN\",\"Langauge-FR\",\"Language-Zulu\"}, (0x101f74aa), 1, 0, 0, TYPE=SA
;
%%{\"Vendor\",\"Vendour\",\"Verkoper\"}
;
;
;Made up files to install
{\"englishfile.txt\" \"frenchfile.txt\" \"germanfile.txt\"}  -\"!:\\System\\Apps\\TEST.txt\"
;
";



# For DEF92320
# Template string to generate PKG file,File name contain symbols like |,/,<,>,:,?,*
#
$PkgFileWithSymbol = "
;
; Autogenerated test install file
;
&EN
;
#{\"test\"}, (0x101f74aa), 1, 2, 3, TYPE=SA
;
%%{\"Ati\"}
;
;Made up files to install
\"TEST.txt\"-\"!:\\System\\Apps\\TEST%s.txt\"
;
";

# For DEF92320
# Template string to generate PKG file, DIR path contain symbols like |,/,<,>,:,?,*
#
$PkgFileWithSymbol1 = "
;
; Autogenerated test install file
;
&EN
;
#{\"test\"}, (0x101f74aa), 1, 2, 3, TYPE=SA
;
%%{\"Ati\"}
;
;Made up files to install
\"TEST.txt\"-\"!:\\System\\Ap%sps\\TEST.txt\"
;
";

# For DEF115795
# Template string to generate a package with very long destination folder
#
$longfolder = "
;
&EN
;
#{\"STSisapt444\"},(0x88900000),1,1,1,NOCOMPRESS,TYPE= SA
;
%{\"Test\"}
;
;
\"TEST.txt\"-\"!:\siv1aaa\\10003a3f\\import\\apps\\siv2aa\\10003a3f\\import\\apps\\siv3aa\\10003a3f\\import\\apps\\siv4aa\\10003a3f\\import\\apps\\siv5aa\\10003a3f\\import\\apps\\siv6aa\\10003a3f\\import\\apps\\siv7aa\\10003a3f\\import\\apps\\siv8aa\\10003a3f\\import\\apps\\siv9aa\\10003a3f\\import\\apps\\siv10a\\10003a3f\\import\\apps\\siv11a\\10003a3f\\import\\appspp\\file1.txt\"
;
";	

#
# Template string to generate a package with comma at the end
#
$endFileCommaContent = "
; A simple SIS file

;Languages
&EN,FR

;Header
#{\"Simple\",\"Simple-FR\"}, (0x80000001), 4, 5, 6

%{\"Simple-Vendor\",\"Simple-Vendor-FR\"}
:\"Unique Vendor Name\"
	
;Ordinary file to selected drive
\"TEST.txt\"-\"!:/Documents/InstTest/file1.txt\",
";
	

# Data to write in the created file for Linux test 
$TempData = "Osver";

#
# Template string to generate PKG file
#
$JAPPkgFileTempl = "
;
; Autogenerated test install file
;
&JA
;
#{\"names large sample UTF8\"}, (0xA2000222), 1, 2, 3, TYPE=SA
;
%%{\"Vendor\"}
;

;Made up files to install
\"\\epoc32\\winscw\\c\\tswi\\utf8.txt\"-\"!:\\utf8.txt\"
;

";


#
# Package template string to generate PKG file with RU and SA
#
$PkgFileRUWithSA = "
;
; Autogenerated test install file
;
&EN
;
#{\"SA ROM Upgrade\"}, (0x802730A2), 1, 0, 0, TYPE=SA, RU
;
%%{\"Security Services\"}
;
;Installation name and header data
(0x802730A2),%s,{\"SA ROM Upgrade\"}
;
;Made up files to install
\"TEST.txt\"-\"\$\:\\System\\Apps\\TEST.txt\"
;
";

#
# Package template string to generate PKG file with RU and non SA or PU or SP
#
$PkgFileRUWithNonSA = "
;
; Autogenerated test install file
;
&EN
;
#{\"SA ROM Upgrade\"}, (0x802730A2), 1, 0, 0, TYPE=PP, RU
;
%%{\"Security Services\"}
;
;Installation name and header data
(0x802730A2),%s,{\"SA ROM Upgrade\"}
;
;Made up files to install
\"TEST.txt\"-\"\$\:\\System\\Apps\\TEST.txt\"
;
";

#
# Package template string to generate PKG file with wildcards for ROM Stub.
#
$WildCardedpkgFile = "
;
; Autogenerated test install file
;
&EN
;
#{\"Wildcard Suported ROM App\"}, (0x802730B1), 1, 2, 3
;
%%{\"Security Services\"}
;
;Installation name and header data
(0x802730B1),%s,{\"Wildcard Suported ROM App\"}
;
;Made up files to install
\"\"-\"z:\\sys\\bin\\wildcard_rom_stub_lib?.dll\"
;
\"\"-\"z:\\sys\\bin\\*.exe\"
;

; Misc data files for baseline eclipsing test
\"\"-\"z:\\cr1122test\\*.txt\"
;
\"\"-\"z:\\cr1122etst\\wildcard_config?.cnf\"
;
";


#
# Package template string to generate PKG file with wildcards for PA package
#
$WildCardedPApkgFile = "
;
; Autogenerated test install file
;
&EN
;
#{\"Wildcarded PA\"}, (0x802730B1), 1, 2, 3, TYPE = PA
;
%%{\"Security Services\"}
;
;Installation name and header data
(0x802730B1),%s,{\"wildcarded PA\"}
;
;Made up files to install
\"\\epoc32\\release\\<PLATFORM>\\<CONFIGURATION>\\tsaromupgradeexe.exe\"-\"c:\\sys\\bin\\*exe_PA?.exe\"
;
";

#
# Package template string to generate PKG file with wildcards for PP package
#
$WildCardedPApkgFile = "
;
; Autogenerated test install file
;
&EN
;
#{\"Wildcarded PP\"}, (0x802730B1), 1, 2, 3, TYPE = PP
;
%%{\"Security Services\"}
;
;Installation name and header data
(0x802730B1),%s,{\"Wildcarded PP\"}
;
;Made up files to install
\"\\epoc32\\release\\<PLATFORM>\\<CONFIGURATION>\\tsaromupgradeexe.exe\"-\"c:\\sys\\bin\\*exe_PA?.exe\"
;
";

#
# Package template string to generate PKG file having wildcards specified in the executable filename.
#
$ExeWithWildCardpkgFile = "
;
; Autogenerated test install file
;
&EN
;
#{\"Wildcard in Exe Name\"}, (0x801130c1), 1, 2, 3
;
%%{\"Security Services\"}
;
;Installation name and header data
(0x801130c1),%s,{\"Wildcard in Exe Name\"}
;
;Made up files to install
\"\"-\"z:\\sys\\bin\\wildcard_rom_stub.*\"
;
\"\"-\"z:\\sys\\bin\\wildcard_rom_stub?.dll\"
;
";

#
# Template string to generate a package with invalid version
#
$invalidVersion1 = "
; A simple SIS file

;Languages
&EN,FR

;Header
#{\"Simple\",\"Simple-FR\"}, (0x80000001), 32767, 32767, 32767

%{\"Simple-Vendor\",\"Simple-Vendor-FR\"}
:\"Unique Vendor Name\"
	
;Ordinary file to selected drive
\"TEST.txt\"-\"!:/Documents/InstTest/file1.txt\"
";


#
# Template string to generate a package with invalid version number for negative values
#
$invalidVersion2 = "
; A simple SIS file

;Languages
&EN,FR

;Header
#{\"Simple\",\"Simple-FR\"}, (0x80000001), -26, -1, -3245

%{\"Simple-Vendor\",\"Simple-Vendor-FR\"}
:\"Unique Vendor Name\"
	
;Ordinary file to selected drive
\"TEST.txt\"-\"!:/Documents/InstTest/file1.txt\"
";

#
# Template string to generate a pkg with an embedded sis file. The embedded sis file is missing on the computer.
#
$missingEmbed = "
;
; Pkg referencing missing embedded package.
;
&EN
;
#{\"missing embedded\"}, (0x80000001), 1, 0, 0, TYPE=SA
;
;missing embedded sis file
\@\"missingembedded.sis\",(0x80000002)
;
";

# Package template string to generate PKG file with ForceAbort (FA) text option.
#
$PkgFileWithFAOption = "
;
; Autogenerated test install file
;
&EN
;
#{\"Force Abort Text Option\"}, (0x8072302A), 1, 0, 0, TYPE=SA
;
%%{\"Security Services\"}
;
;Installation name and header data
(0x802730A2),%s,{\"SA ROM Upgrade\"}
;
;Made up files to install
\"TEST.txt\"-\"\", FT , FA
;
";

#
#Test code for Preinstalled pkg files without having sourcefiles
#
$PreinstalledPkgWithoutSourcefiles = "
;A preinstalled SIS file

;Languages
&EN,FR

;Header
#{\"Simple\",\"Simple-FR\"}, (0x80000001), 1, 2, 3, TYPE = PA

%{\"Simple-Vendor\",\"Simple-Vendor-FR\"}
:\"Unique Vendor Name\"
	
;Ordinary file to selected drive
\"\"-\"!:/Documents/InstTest/file1.txt\"
";



#
# Template string to generate Expected output file for ROM stub file (containing exe with wildcards) created by makesis for DEF111264.
#
$DEF111264ExpectedOutput= "Unique vendor name not found.

Warning: Executables should be included explicitly (without wildcards),to enable SID checks to work as expected.
";

#
# Template string to generate Expected output file for Preinstalled APP package having no source files specified.
#
$DEF113569ExpectedOutput= " Error : Source file is missing for PreInstalled APP : 
(14) : error: file I/O fault, Source file is missing for PreInstalled APP 
";


#
# Template string to generate Expected output file when the version range is invalid.

$DEF112718ExpectedOutput= "Warning : The valid version number ranges are : (Major: 0..127) (Minor: 0..99 ) (Build: 0..32,767).\n";
 
#
# Package template string to generate an embedded PKG file of type PA.
#
$EmbeddedPApkgFile = "
;
; Autogenerated test install file
;
&EN
;
#{\"Test Embedded PA\"}, (0x01101335), 1, 1, 1, TYPE=PA
;
%{\"Symbian Software Ltd\"}
:\"Symbian Software Ltd\"
;
;Installation name and header data
(0x01101335),%s,{\"Test Embedded PA\"}
;
";

#
# Package template string to generate an embedding PKG file whose embedded pkg is of type=PA.
#
$EmbeddingPApkgFile = "
;
; Autogenerated test install file
;
&EN
;
#{\"Test Embedding PA\"}, (0x01011243), 1, 0, 1, TYPE=SA
;
%{\"Symbian Software Ltd\"}
:\"Symbian Software Ltd\"
;
;Installation name and header data
(0x01011243),%s,{\"Test Embedding PA\"}
;
\@\"embeddedPA.sis\" , (0x01101335)
;
";

#
# CR1125 VERSION Condition Package template string to generate PKG files
#
$PkgFileCR1125VersionTemplate = "
;
; Auto Generated Template PKG File
; VERSION Conditional Testing
;
&EN
;
#{\"CR1125 SEC-SWI-PKGVER\"}, (0xEA001000), 1, 0, 2, TYPE=SA
%{\"Symbian Software Ltd.\"}
: \"Symbian Software Ltd.\"
;
; Version Conditional Block
IF %s
    {
    \"TEST.txt\"
    }-\"C:\\tswi\\tpkgver\\test_result_etrue.txt\"
ENDIF
;
";

#
# Package template string to generate a PKG with features not installable by InterpretSIS
#
$PkgFileInterpretsisVersionTemplate = "
;
; Autogenerated test install file
;
&EN
;
#{\"Test Makesis -c\"}, (0x01011243), 1, 0, 1, TYPE=PP
;
%{\"Symbian Software Ltd\"}
:\"Symbian Software Ltd\"
;
!({\"Add-on 1 (20KB)\"},{\"Add-on 2 (75KB)\"},{\"Add-on 3 (80KB)\"}) 
;
\@\"Embedded.sis\" , (0x101f74aa)
;
";

#
# Template string to generate Expected output file for DEF113349.
#
$DEF113349ExpectedOutput= "embeddedPA.sis is a stub.	 
WARNING : Embedded Preinstalled Stub (PA/PP type) SIS file is not supported.
(16) : error: SISfile error, Stub File
";

#
# 
#
$InterpretsisExpectedOutput= "*** SIS installation file INVALID for InterpretSis ***
(8) : Invalid Application Type. Package type PP not supported
(12) : User options are not supported
(14) : Embedded SIS file will not be installed by InterpretSis
";

#
# Do test for each elements of TestItems array
#
for my $Test ( @TestItems )  {

	# Generating PKG file contents
	if( $Test->[0] eq  "test16.pkg" )
		{
		$PkgFile = sprintf( $AnotherPkgFileTempl, $Test->[1]);
		}
	elsif ( $Test->[0] eq  "test17.pkg" )
		{
		$PkgFile = $unsupportedEncContent;
		}
	elsif ( $Test->[0] eq  "testForward.pkg" )
		{
		$PkgFile = $ForwardSlash;
		}
	elsif ( $Test->[0] eq  "testFN_RI.pkg" )
		{
		$PkgFile = $FN_RI;
		}
	else
		{
 		$PkgFile = sprintf( $PkgFileTempl, $Test->[1]); 
		}
	
	# Create PKG file
	CreateFile($Test->[0], $PkgFile);	

	# Do MAKESIS test

	$logMsg = sprintf "%s\n (symbol:%s)\n", $Test->[4], $Test->[1];
	WriteLog($logMsg);

	MakeSISFile($Test->[0], $Test->[2], $Test->[3]);

}

#
# Do test for each elements of TestItems1 array
#
 $Count = 1;
for my $Test1 ( @TestItems1 )  {

	# Generating PKG file contents
	if( $Count >= 19 )
		{
		$PkgFile = sprintf( $PkgFileWithSymbol1 , $Test1->[1]);
		$Count ++  ;
		}
	else
		{
		$PkgFile = sprintf( $PkgFileWithSymbol , $Test1->[1]);
		$Count++ ;
		}

	# Create PKG file
	CreateFile($Test1->[0], $PkgFile);	

	# Do MAKESIS test

	$logMsg1 = sprintf "%s\n (Symbol:%s)\n", $Test1->[4], $Test1->[1];
	WriteLog($logMsg1);

	MakeSISFile($Test1->[0], $Test1->[2], $Test1->[3]);

}


#
# Run CR1125 MakeSIS Tests (TestItems2 array)
#
for my $Test2 ( @TestItems2 )  {

	# Generating PKG file contents
	$PkgFile = sprintf( $PkgFileCR1125VersionTemplate , $Test2->[1]);

	# Create PKG file
	CreateFile($Test2->[0], $PkgFile);	

	# Do MAKESIS test
	$logMsg1 = sprintf "%s\n (Condition: %s)\n", $Test2->[4], $Test2->[1];
	WriteLog($logMsg1);

	MakeSISFile($Test2->[0], $Test2->[2], $Test2->[3]);
}


#
# Additional test to check that stub files don't change every time 
# they are built.
#
TestSISStubFile();

#
# Test for Test$parameter()
#
TestsysDriveparameter();

# Call the tests for CR1027 - ROM Upgrade with SA SIS package.
TestSisRUWithSA();
TestSisRUWithNonSA();

# Call the test for CR1122 - Wildcard support for ROM stub.
TestSISWithWildCards();
TestSISWithWildCardsPA();
TestSISWithWildCardsPP();

#
# Test for DEF111264.Verifying that warnings are generated by Makesis 
# when wildcards are used in \sys\bin directory in ROM stubs. 
#

TestDEF111264();

#
# Test for DEF113116.
#  
TestDEF113116();

#
# Test for DEF083525
#
TestDEF083525();

#
# Test for PDEF081989.Testing the working of Makesis -d option .
#

TestPDEF081989();


#
# Test for DEF093400
#
TestDEF093400();

#
# Test for DEF090878
#
TestDEF090878();

#
# Test for DEF107033.Testing the working of Makesis -d option with language dependent files.
#

TestDEF107033();

# Test for an output filename with a single char - DEF108728
TestSingleCharFilename();

# Test for DEF108815
TestEndFileComma();


# Test for DEF115795
TestLongFolderName();

# Test for DEF112718-1
TestInvalidVersion1();
 
# Test for DEF112718-2
TestInvalidVersion2();

# Test for DEF112831
TestDEF112831();

#
# Test for DEF113349.
#

TestDEF113349();

# Test for DEF113569
TestPreInstalledWithoutSourceFiles();

#
# Test for Makesis -c option. Added as part of the fix for DEF126467.
#
TestInterpretsisReport();

#
# These tests are very specific to windows OS only
#
#

if ($^O =~ /^MSWIN32$/i)
{
#
# Test for DEF091860
#
TestDEF091860();

#
# Test for DEF091942.
#
TestDEF091942();

#
# Test for INC092755
#
TestINC092755();

#
# Test for DEF090912
#
TestDEF090912();

#
# Test for DEF093156
#
TestDEF093156();

#
# Test for TestDEF091780
#
TestDEF091780();

#
# Test for DEF104895.Testing the working of Makesis -d option with 
# embedding sis file in path containing backward slasesh.
#

TestDEF104895();
}

#
# These tests are very specific to Linux OS only
#
#
if ($^O =~ /^LINUX$/i)
{

#
# Test for DEF091942.
#
TestLDEF091942();


#
# Test for DEF090912
#
TestLDEF090912();

#
# Test for DEF104895.Testing the working of Makesis -d option with 
# embedding sis file in path containing backward slasesh.
#

TestLDEF104895();

}


unlink("Test.txt");
unlink("HelloApp.exe");
unlink("test1.txt");
unlink("test2.txt");

#
# Display the result
#
WriteLog("\n\nTests completed OK\n");
WriteLog(sprintf "Run: %d\n", $NumberOfTests );
WriteLog(sprintf "Passed: %d\n", $NumberOfPassed );
WriteLog(sprintf "%d tests failed out of %d\n", $NumberOfFailed, $NumberOfTests ); 

