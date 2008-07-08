#------------------------------------------------------------------------------#
# File:        CLMSET.pm
#
# Description: Operations on clause multisets.
#
# Author:      Joao Marques-Silva.
#
# Created:     11 Sep 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package CLMSET;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

require UTILS;  # Must use require, to get INC updated
import UTILS qw( &get_progname &get_progpath &exit_ok &exit_err &exit_quit );

use IDGEN qw( &num_id &gen_id &set_id );
use CLUTILS;


#------------------------------------------------------------------------------#
# Object constructor
#------------------------------------------------------------------------------#

sub new {
    shift; # skip module name
    my $self  = {};
    #
    $self->{VAR_NUM}     = undef;
    $self->{CL_NUM}      = undef;
    $self->{CLSETS}      = [];  # array ref
    $self->{ERROR}       = 0;
    $self->{ERROR_MSG}   = "";
    #
    bless($self);           # but see below
    return $self;
}


#------------------------------------------------------------------------------#
# Basic interface, access to -object data
#------------------------------------------------------------------------------#

sub numvars {
    my $self = shift;
    return &IDGEN::num_id();
}

sub numcls {
    my $self = shift;  # CL_NUM must equal num of keys in hash of cls
    return $self->{CL_NUM};
}

sub error {
    my $self = shift;
    return $self->{ERROR};
}

sub errormsg {
    my $self = shift;
    return $self->{ERROR_MSG};
}


#------------------------------------------------------------------------------#
# Manipulation of clause sets
#------------------------------------------------------------------------------#

sub add_clset {
    my $self = shift;
    my $nclset = shift;
    push @{$self->{CLSETS}}, $nclset;
    $self->{CL_NUM} += $nclset->numcls();
}

sub recalc_clnum() {
    my $self = shift;
    my $ncls = 0;
    foreach my $clset (@{$self->{CLSETS}}) {
	#print "Printing another clause set...\n";
	$ncls += $clset->numcls;
    }
    $self->{CL_NUM} = $ncls;
}

#------------------------------------------------------------------------------#
# Clause multiset printing
#------------------------------------------------------------------------------#

sub write_dimacs() {
    my $self = shift;
    my $fname = shift;
    local *CNFF;
    open (CNFF, ">$fname") || &exit_err("Unable to open write file $fname");
    my $vnum = &IDGEN::num_id();
    $self->recalc_clnum();
    my $cnum = $self->{CL_NUM};
    print CNFF "p cnf $vnum $cnum\n";
    close CNFF;
    foreach my $clset (@{$self->{CLSETS}}) {
	#print "Printing another clause set...\n";
	#$clset->print_dimacs(*CNFF);
	$clset->print_dimacs($fname);
    }
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
