#------------------------------------------------------------------------------#
# File:        UTILS.pm
#
# Description: Useful utilies, used in more complex scripts.
#
# Author:      Joao Marques-Silva.
#
# Created:     04 Apr 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package UTILS;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

BEGIN {
    @UTILS::ISA = ('Exporter');
    @UTILS::EXPORT_OK = qw( &get_progname &get_progpath &vassert &exit_ok &exit_err &exit_quit );
}


#------------------------------------------------------------------------------#
# Formatted output
#------------------------------------------------------------------------------#

sub report_item() {
    my ($msg, $num) = @_;
    $msg .= ':';
    my $prtstr = sprintf("%-35s  %d", $msg, $num);
    print "$prtstr\n";
}


#------------------------------------------------------------------------------#
# Signal handling utilities
#------------------------------------------------------------------------------#

sub register_handlers() {
    $SIG{'INT'} = 'UTILS::INT_handler';
    $SIG{'ABRT'} = 'UTILS::SIG_handler';
    $SIG{'SEGV'} = 'UTILS::SIG_handler';
    $SIG{'BUS'} = 'UTILS::SIG_handler';
    $SIG{'QUIT'} = 'UTILS::SIG_handler';
    $SIG{'TERM'} = 'UTILS::SIG_handler';
    $SIG{'XCPU'} = 'UTILS::SIG_handler';
}

my $cleanref = '';
my $msudsref = undef;

sub register_cleanup() {
     $cleanref = shift;
}

sub register_ds() {
     $msudsref = shift;
}

sub SIG_handler {
    &INT_handler();
}

sub INT_handler {
    # close all files.
    print "Cleaning up...\n";
    if ($cleanref ne '') { &{$cleanref}($msudsref); }
    # send error message to log file.
    print "Terminating...\n";
    exit(0);
}

#------------------------------------------------------------------------------#
# Time/Host stamp utilities
#------------------------------------------------------------------------------#

my $host_time_stamp = '';

sub set_host_time_stamp() {
    my $progname = &get_progname();
    #my $hostname = $ENV{HOST};
    #my $hostname = $ENV{HOSTNAME};
    my $hostname = `hostname -s`; chomp($hostname);
    #print "|$hostname|\n"; exit;
    my @ttoks = localtime time;
    my $sec =  sprintf("%02s", $ttoks[0]);
    my $min =  sprintf("%02s", $ttoks[1]);
    my $hour = sprintf("%02s", $ttoks[2]);
    my $day =  sprintf("%02s", $ttoks[3]);
    my $mon =  sprintf("%02s", $ttoks[4] + 1);
    my $year = sprintf("20%02s", $ttoks[5]-100);
    $host_time_stamp = "$progname-$hostname-$year$mon$day-$hour$min$sec";
    return $host_time_stamp;
}

sub get_host_time_stamp() { return $host_time_stamp; }

#------------------------------------------------------------------------------#
# Time utilities
#------------------------------------------------------------------------------#

sub elapsed_time() {
    my ($ut, $st, $ct, $cst) = times();
    return $ut+$st+$ct+$cst;
}

sub available_time() {
    my $timeout = shift;
    return $timeout - &elapsed_time();
}

sub prt_time() {
    my ($ut, $st, $ct, $cst) = times();
    my $tt = $ut+$st+$ct+$cst;
    print "CPU Time: $tt\n";
}


#------------------------------------------------------------------------------#
# Execution path handling
#------------------------------------------------------------------------------#

sub get_progname() {
    my @progname_toks = split(/\//, $0);
    my $progname = $progname_toks[$#progname_toks];
    #print "$progname\n";
    return $progname;
}

sub get_progpath() {
    my @progname_toks = split(/\//, $0);
    pop @progname_toks;
    my $progpath = join('/', @progname_toks);
    #print "$progpath\n";
    return $progpath;
}

sub resolve_inc() {    # Kept here as a template; need a copy in each script
    my ($cref, $pmname) = @_;
    my @progname_toks = split(/\//, $0);
    pop @progname_toks;
    my $progpath = join('/', @progname_toks);
    my $fullname = $progpath . '/' . $pmname;
    my $fh;
    open($fh, "<$fullname") || &exit_err("non-existing $pmname\n");
    return $fh;
}


#------------------------------------------------------------------------------#
# Error/abort conditions
#------------------------------------------------------------------------------#

sub vassert() {
    my $cond = shift;
    &exit_err("ASSERTION FAILED: @_\n") if !$cond;
}

sub exit_ok() { exit 10; }    # 10 denotes ok condition

sub exit_quit() {
    my $msg = shift;
    print "$msg\n";
    &{$cleanref}($msudsref); 
    exit 20;    # 20 denotes resources exceeded condition
}

sub exit_err() {
    my $msg = shift;
    print "ERROR: $msg\n";
    &{$cleanref}($msudsref); 
    exit 30;    # 30 denotes abort/error condition
}

END {
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
