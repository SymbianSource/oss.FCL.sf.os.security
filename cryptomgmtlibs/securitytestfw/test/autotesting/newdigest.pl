#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


use strict;
use Getopt::Long;
use IO::Handle;
use IO::Seekable;
use File::Basename;

main();
exit(0);

sub main {
	if ($#ARGV != 0)
	{
		print "usage: newdigest.pl <logfile>\n";
		exit;
	}
	
	my ($log_file) = $ARGV[0];
	
	if (-f $log_file)
	{
		system ("del $log_file");
	}
	
	opendir( DIR, "." ) or die("Unable to open directory");
	my @dirs = readdir DIR;
	my $entry;
	my $failed = 0;
	my $notests = 0;

	open (SUMMARY, ">$log_file") || die "couldn't open digest file: $!";
	
	foreach $entry ( sort @dirs )
	{
		next if ( $entry eq ".." or $entry eq "." );
		next if ( -d $entry );
		
		if ( $entry =~ /\.log/ || $entry =~ /\.txt/)
		{
			parseLog($entry, \$failed, \$notests);
		}
		elsif ($entry =~ /\.htm/) 
		{
			parseHtm($entry, \$failed, \$notests);
		}
	}
	
	print SUMMARY "\r\nPassed\tFailed\tTotal\r\n";
	printf SUMMARY ("%d\t%d\t%d\n", $notests-$failed, $failed, $notests);
	print SUMMARY "\n";
	
	print SUMMARY "\r\n%Passed\t\t%Failed\r\n";
	printf SUMMARY ("%.2f\t\t%.2f\r\n", 100-$failed/$notests*100, $failed/$notests*100);
}


sub parseLog($$$) {
	my ($entry, $failed, $notests) = @_;

    open (LOG, "$entry" ) or die("open failed");
    my $found = "false";
    while(<LOG>)
    {
	chomp;
	
	my $line  = $_;
	$line =~ s/\x0//g;
	if ($line =~ /tests failed/)
	{
	    $line =~ /(\d+) tests failed out of (\d+)/;
	    printf SUMMARY (" %45s: %s\r\n", $entry, $&);
	    $found = "true";
	    $$failed += $1;
	    $$notests += $2;
	}
    }
    close LOG;

    if ( $found eq "false" && $entry ne "buildid.txt")
    {
	printf SUMMARY ("WARNING: Could not parse file %s\r\n", $entry);
    }
}

sub parseHtm($$$) {
	my ($entry, $failed, $notests) = @_;
    my ($textfile);
    $textfile = $entry;
    $textfile =~ s/\.htm/\.log/;
    next if ( -f $textfile);
	
    my $total = 0;
    my $pass = 0;
    my $unknown = 0;
    my $fail = 0;
    open (LOG, $entry) || die "couldn't open $entry: $!";
    while (<LOG>)
    {
	my ($line) = $_;
	if ($line =~ /PASS = (\d+)/)
	{
	    $pass += $1;
	}
	elsif ($line =~ /FAIL = (\d+)/)
	{
	    $fail += $1;
	}
	elsif ($line =~ /ABORT = (\d+)/)
	{
	    $unknown += $1;
	}
	elsif ($line =~ /PANIC = (\d+)/)
	{
	    $unknown += $1;
	}
	elsif ($line =~ /INCONCLUSIVE = (\d+)/)
	{
	    $unknown += $1;
	}
	elsif ($line =~ /UNKNOWN = (\d+)/)
	{
	    $unknown += $1;
	}
	elsif ($line =~ /UNEXECUTED = (\d+)/)
	{
	    $unknown += $1;
	}
    }
	$total = $pass + $fail + $unknown;
    close LOG;
	if($total != 0)
	{
	  printf SUMMARY (" %45s: %d tests failed out of %d\r\n", $entry, $total-$pass, $total);
	}
	else
	{
	  printf SUMMARY ("WARNING: %45s: %d tests failed out of %d\r\n", $entry, $total-$pass, $total);
	}
    $$notests += $total;
    $$failed += $fail + $unknown;
}
