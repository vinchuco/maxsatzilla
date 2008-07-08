#!/usr/bin/perl

use BSD::Resource;

$SIG{'INT'} = 'INT_handler';
$SIG{'ABRT'} = 'SIG_handler';
$SIG{'SEGV'} = 'SIG_handler';
$SIG{'BUS'} = 'SIG_handler';
$SIG{'QUIT'} = 'SIG_handler';
$SIG{'TERM'} = 'SIG_handler';
$SIG{'XCPU'} = 'SIG_handler';

#my @rutoks = ( RLIMIT_CPU, RLIMIT_DATA, RLIMIT_RSS, RLIMIT_VMEM );
#my @ruvals = ( 1000, 100000000, 100000000, 1000000000 );
my @rutoks = ( RLIMIT_DATA, RLIMIT_RSS, RLIMIT_VMEM );
my @ruvals = ( 100000, 100000, 1000000 );

for(my $i=0; $i<=$#rutoks; ++$i) {
    my $rutok = $rutoks[$i];
    my $ruval = $ruvals[$i];
    my ($rusoft, $ruhard) = &getrlimit($rutok);
    print "Default $rutok:  ACTUAL, HARD = $rusoft, $ruhard\n";
    my $status = &setrlimit($rutok, $ruval, $ruval);
    if ($status == undef) { print "Failed for $rutok....\n"; }
    ($rusoft, $ruhard) = &getrlimit($rutok);
    print "Default $rutok:  ACTUAL, HARD = $rusoft, $ruhard\n";
    print "New $rutok:  ACTUAL, HARD = $rusoft, $ruhard\n";
}

#system("./mscore inst/pimag-instances/cnf/f9.cnf");
system("./src/fm06-ms-p1 -d -v inst/pimag-instances/cnf/g7.cnf");

#for ($x = 0; $x < 1000; $x++) {
#    print("$x\n");
#    sleep 1;
#    &check_limits();
#}

my @rustats = getrusage($ru_who);

print "RU STATS: @rustats\n";

1;

sub check_limits() {
    for(my $i=0; $i<=$#rutoks; ++$i) {
	my $rutok = $rutoks[$i];
	my $ruval = $ruvals[$i];
	my ($rusoft, $ruhard) = &getrlimit($rutok);
	print "Default $rutok:  ACTUAL, HARD = $rusoft, $ruhard\n";
    }
}

sub INT_handler {
    # close all files.
    # send error message to log file.
    print "Process terminating w/ INT signal...\n";
    exit(0);
}

sub SIG_handler {
    # close all files.
    # send error message to log file.
    print "Process terminating w/ valid signal...\n";
    exit(0);
}
