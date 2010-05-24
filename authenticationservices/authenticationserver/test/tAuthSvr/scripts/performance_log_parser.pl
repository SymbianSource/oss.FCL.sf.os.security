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
# Script: performance_log_parser.pl
#

use strict;
use FileHandle;

my $Test_Case_Start_Tag = "script START_TESTCASE";
my $Test_Case_End_Tag = "END_TESTCASE";
my $Event_print = "PERFORMANCE_LOG_INFORMATION";
my $Test_Case_Actual_Duration = "TEST_CASE_ACTUAL_DURATION";
my $ScrEventCode = 1;

my $defInputFile = "/epoc32/winscw/c/logs/testexecute/tauthsvr_performance.htm";
my $defOutputFile = "/epoc32/winscw/c/logs/authsvr_performance.txt";

sub millisec
	{  
	no warnings 'uninitialized';
	my ($mytime) = @_ ;
	$mytime =~ /([0-9]*):([0-9]*):([0-9]*):([0-9]*)$/;
	my $milisec = $4;
	my $sec = $3;
	my $min = $2;
	my $hour = $1;

	my $msecs = ($milisec / 1000) + ($sec *1000) + ($min * 60000) + ($hour * 3600000);
	return $msecs;
	}

sub process_log_file
	{
	my ($data_file, $output_FH) = @_;
	print $output_FH "#Test Case Name,Actual Duration(ms),Result\n";
	
	# enable reading whole file as one chunk
	my $SAVE_RS = $/;
	undef $/;
	
	# open and read the log file in a single line
	open(DAT, $data_file) || die("Could not open file $data_file!");
	my $raw_data=<DAT>;
	close(DAT);
	$raw_data=~ s/\n/ /g;

	# restore previous record separator
	$/ = $SAVE_RS;	
	
	my $num_passed = 0;
	my $num_failed = 0;
	
	while ($raw_data && $raw_data =~ /$Test_Case_Start_Tag\s+(\S+)/)
		{
		# step through the file in chunks delimited by the start/end test case tags.
		
		my $test_case_name = $1;
		my ($test_case_data) = ($raw_data =~ /$Test_Case_Start_Tag(.*?)$Test_Case_End_Tag/);
		
		if (not $test_case_data)
			{
			print STDERR "Error: Test case: $test_case_name: Suspected missing END_TESTCASE tag.\n";
			}
		
		($raw_data) = ($raw_data =~ /$Test_Case_End_Tag(.*)/);
		($raw_data) = ($raw_data =~ /($Test_Case_Start_Tag.*)/) if $raw_data;
		
		#my ($max_duration) = ($test_case_data =~ /$Test_Case_Max_Duration,([0-9]+)/);
		#if (!defined $max_duration)
			#{
			#$max_duration = 1000;
			#}
			
		my $start_time = "0";
		my $end_time = "0";
		my $first = 1;
		
		while ($test_case_data and ($test_case_data =~ /$Event_print(.*)/) )
			{
			# get the event time. if this is the first time value in the chunk, it is the start time of the event.
			#otherwise, continue until reaching the last time value.
			my $start_log_msg = $1;
		   
			$start_log_msg =~ /,(.+?)\s(.*)/;
			
			if($first == 1)
				{
				$start_time = $1;
				$first = 0;
				}
			else
				{
				$end_time = $1;
				}
	
			$test_case_data = $2;
			}	
		
		if(!($start_time eq "0") && !($end_time eq "0"))
			{
			my ($actual_duration) = ($test_case_data =~ /$Test_Case_Actual_Duration,([0-9]+)/);
			#my $actual_duration = millisec($end_time) - millisec($start_time);
			my $result;
			if( $actual_duration)
				{
				$result = "Passed";
				$num_passed += 1;
				}
						
			print $output_FH "$test_case_name,$actual_duration,$result\n";	
			}
		}
	print $output_FH (sprintf "\n\n%d tests failed out of %d\n", $num_failed, $num_passed+$num_failed);
	}
	
	
sub usage
	{
	print <<USAGE;
usage:
	performance_log_parser.pl <input_file> <output_file>

	<input_file>: A log file to process 
			(default- $defInputFile)

	<out_file>: Final output file name 
			(default- $defOutputFile)
USAGE
	}

	
sub Main
	{
	my @argv = @_;
	# if input and output files are not provided, default ones will be used
	my $input_file = $defInputFile;
	my $output_file = $defOutputFile;
	
	if(@argv > 2)
		{
		usage;
		exit;
		}
	elsif(@argv == 2)
		{# input and output files are provided, don't use default ones
		$input_file = shift @argv;
		$output_file = shift @argv;
		}
	
	my $outputFH = \*STDOUT; # results to STDOUT by default
	
	if (defined $output_file)
		{
		$outputFH = FileHandle->new;
		$outputFH->open("> $output_file") or die "Error: File $output_file: $!";
		}
	process_log_file($input_file,$outputFH);

	$outputFH->close;
}

Main(@ARGV);