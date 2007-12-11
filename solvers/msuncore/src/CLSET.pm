#------------------------------------------------------------------------------#
# File:        CLSET.pm
#
# Description: Operations on clause sets.
#
# Author:      Joao Marques-Silva.
#
# Created:     08 Sep 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package CLSET;

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
    $self->{FNAME}       = '';
    $self->{REF_VAR_NUM} = undef;
    $self->{REF_CL_NUM}  = undef;
    $self->{SET_TYPE}    = '';  # Type of clause set (std, partial, etc.)
    $self->{CL_NUM}      = undef;
    $self->{CL_NUM_HARD} = 0;
    $self->{CL_NUM_SOFT} = 0;
    $self->{CL_SET}      = {};  # hash of clauses
    $self->{CL_TYPE}     = {};  # hash of cl type
    $self->{CL_STAT}     = {};  # hash of cl status
    $self->{LIT_SET}     = undef; # set of lits
    $self->{LIT_HASH}    = undef; # hash of lits w/ cl refs
    $self->{COMMENTS}    = [];  # array ref
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

sub numcls_hard {
    my $self = shift;  # CL_NUM must equal num of keys in hash of cls
    return $self->{CL_NUM_HARD};
}

sub numcls_soft {
    my $self = shift;  # CL_NUM must equal num of keys in hash of cls
    return $self->{CL_NUM_SOFT};
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
# Manipulation of clauses
#------------------------------------------------------------------------------#

sub clset {
    my $self = shift;
    return $self->clauses;
}

sub clauses {
    my $self = shift;
    my @clkeys = keys %{$self->{CL_SET}};
    return \@clkeys;
}

sub add_cl_lit() {    # Add literal to a clause
    my $self = shift; my $clref = shift; my $lit = shift;
    ##print "Deleting $clref\n";
    undef(${$self->{CL_SET}}{$clref});
    delete(${$self->{CL_SET}}{$clref});
    my $st = ${$self->{CL_STAT}}{$clref};
    undef(${$self->{CL_STAT}}{$clref});
    delete(${$self->{CL_STAT}}{$clref});

    $clref .= " $lit";
    ${$self->{CL_SET}}{$clref} = 1;
    ${$self->{CL_STAT}}{$clref} = $st;
    return $clref;
}

sub lkup_cl() {    # Check cl existence
    my ($self, $ncl) = @_;
    return (defined(${$self->{CL_SET}}{$ncl})) ? 1 : 0;
}

sub add_cl() {    # Add clause
    my ($self, $ncl, $nst) = @_;   # must respect signature!
    &UTILS::vassert(!defined(${$self->{CL_SET}}{$ncl}),
			"Already defined clause: $ncl");
    ${$self->{CL_SET}}{$ncl} = 1;
    ${$self->{CL_STAT}}{$ncl} = $nst;
    $self->{CL_NUM}++;
    # IDGEN should know of the new vars...
}

sub add_cl_set() {    # Add clause subset
    my ($self, $nclset, $nst) = @_;
    foreach my $ncl (@{$nclset}) {
	&UTILS::vassert(!defined(${$self->{CL_SET}}{$ncl}),
			"Already defined clause: $ncl");
	$self->add_cl($ncl, $nst);
	# IDGEN should know of the new vars...
    }
}

sub del_cl() {    # Remove clause
    my ($self, $ncl) = @_;
    &UTILS::vassert(defined(${$self->{CL_SET}}{$ncl}),
		    "Non existing clause: $ncl");
    undef(${$self->{CL_SET}}{$ncl});
    delete(${$self->{CL_SET}}{$ncl});
    undef(${$self->{CL_STAT}}{$ncl});
    delete(${$self->{CL_STAT}}{$ncl});
    $self->{CL_NUM}--;
}

sub del_cl_set() {    # Remove clause subset
    my ($self, $nclset) = @_;
    foreach my $ncl (@{$nclset}) {
	&UTILS::vassert(defined(${$self->{CL_SET}}{$ncl}),
			"Non existing clause: $ncl");
	$self->del_cl($ncl);
	# IDGEN should know of the new vars...
    }
}

sub erase() {    # Remove all clauses
    my $self = shift;
    $self->{REF_VAR_NUM} = undef;
    $self->{REF_CL_NUM}  = undef;
    $self->{CL_NUM}      = undef;
    delete $self->{CL_SET};
    undef $self->{CL_SET};
    delete $self->{CL_STAT};
    undef $self->{CL_STAT};
    $self->{CL_SET}      = {};  # new hash of clauses
    $self->{CL_STAT}     = {};  # new hash of cl status
}


#------------------------------------------------------------------------------#
# Manipulation of literals
#------------------------------------------------------------------------------#

sub litset() {
    my $self = shift;
    if (!defined($self->{LIT_SET})) { $self->compute_litset(); }
    return $self->{LIT_SET};
}

sub compute_litset() {
    my $self = shift;
    foreach my $clkey (keys %{$self->{CL_SET}}) {
	my $clits = &CLUTILS::cllits($clkey);
	foreach my $clit (@{$clits}) { $self->{LIT_SET}->{abs($clit)} = 1; }
    }
}

sub lithash() {
    my $self = shift;
    if (!defined($self->{LIT_HASH})) { $self->compute_lithash(); }
    return $self->{LIT_HASH};
}

sub compute_lithash() {
    my $self = shift;
    foreach my $clkey (keys %{$self->{CL_SET}}) {
	my $clits = &CLUTILS::cllits($clkey);
	foreach my $clit (@{$clits}) {
	    push @{$self->{LIT_HASH}->{$clit}}, $clkey;
	}
    }
}


#------------------------------------------------------------------------------#
# Manipulation of clause status
#------------------------------------------------------------------------------#

sub set_cl_status() {
    my $self = shift; my $clref = shift; my $nst = shift;
    ${$self->{CL_STAT}}{$clref} = $nst;
}

sub get_cl_status() {
    my $self = shift; my $clref = shift;
    return ${$self->{CL_STAT}}{$clref};
}


#------------------------------------------------------------------------------#
# Manipulation of clause type/weight
#------------------------------------------------------------------------------#

sub get_cl_type() {
    my $self = shift; my $clref = shift;
    return ${$self->{CL_TYPE}}{$clref};
}

sub chk_cl_hard() {
    my $self = shift; my $clref = shift;
    return ${$self->{CL_TYPE}}{$clref} == 1;
}

sub chk_cl_soft() {
    my $self = shift; my $clref = shift;
    return ${$self->{CL_TYPE}}{$clref} == 0;
}

sub get_cl_weight() {  # CLSET type expected to be weighted...
    my $self = shift; my $clref = shift;
    return ${$self->{CL_TYPE}}{$clref};
}


#------------------------------------------------------------------------------#
# Parsing of CNF files, dimacs format
#------------------------------------------------------------------------------#

sub parse {
    my $self = shift;
    my $fname = (@_) ? shift : "";  # allow for consistency checking option
    $self->{FNAME} = $fname;

    local *INP;
    my $fn = $self->{FNAME};

    if ($fn ne "") {
	if (open(INP, $fn)) {
	    $self->parse_file(*INP);
	}
	else {
	    $self->{ERROR} = 1;
	    $self->{ERROR_MSG} = "Unable to open $fn";
	}
    }
    else {
	$self->parse_file(*STDIN);
    }
}


#------------------------------------------------------------------------------#
# Private methods
#------------------------------------------------------------------------------#

#------------------------------------------------------------------------------#
# Parse methods
#------------------------------------------------------------------------------#

# read in clause set; clauses treated as strings of lits...
sub parse_file {
    my $self = shift;
    local *INP = shift;
    while(<INP>) {
	chomp;
	if (m/^c/) {
	    push @{$self->{COMMENTS}}, $_;
	} elsif (m/^p cnf\s+(\d+)\s+(\d+)/ || m/^p smcnf\s+(\d+)\s+(\d+)/) {
	    &IDGEN::set_id($1);
	    $self->{REF_VAR_NUM} = $1; $self->{REF_CL_NUM} = $2;
	    $self->{CL_NUM} = $2;
	    $self->{SET_TYPE} = 'default';
	    $self->parse_cnf(*INP);
	} elsif (m/^p pmcnf\s+(\d+)\s+(\d+)/) {
	    &IDGEN::set_id($1);
	    $self->{REF_VAR_NUM} = $1; $self->{REF_CL_NUM} = $2;
	    $self->{CL_NUM} = $2;
	    $self->{SET_TYPE} = 'partial';
	    $self->parse_pmcnf(*INP);
	} elsif (m/^p wmcnf\s+(\d+)\s+(\d+)/) {
	    &IDGEN::set_id($1);
	    $self->{REF_VAR_NUM} = $1; $self->{REF_CL_NUM} = $2;
	    $self->{CL_NUM} = $2;
	    $self->{SET_TYPE} = 'weighted';
	    $self->parse_wmcnf(*INP);
	}
	else { print "ERROR: DIMACS format requires known header!\n"; exit(1); }
    }
}

sub parse_cnf() {  # Format is the default cnf format
    my $self = shift;
    local *INP = shift;
    #
    my @clits = ();
    while(<INP>) {
	chomp;
	next if (m/^c/ || m/^\s*$/);
	#{print "ERROR: Comments in DIMACS format must be in the preamble!\n";
	#exit(1);}
	my @rlits = split(/\s+/,$_);
	if ($rlits[0] eq '') { shift @rlits; }
	if ($rlits[$#rlits] eq '') { pop @rlits; }
	push @clits, @rlits;
	if ($clits[$#clits] == 0) {
	    my @nclits = @clits;    # clean duplicates
	    my $litref = &CLUTILS::clclean(\@nclits);
	    @clits = @{$litref};
	    my $ncl = &CLUTILS::clsig(\@clits);
	    ${$self->{CL_SET}}{$ncl} = 1;
	    ${$self->{CL_STAT}}{$ncl} = 'INIT';  # Options: BLOCK, AUX
	    ${$self->{CL_TYPE}}{$ncl} = 0;  # All cls are soft...
	    $self->{CL_NUM_SOFT}++;
	    @clits = ();
	} # else { print "Non-ended clause?\n"; }
    }
}

sub parse_pmcnf() {  # Format is the partial maxsat cnf format
    my $self = shift;
    local *INP = shift;
    #
    my @clits = ();
    my $hardcl = -1;  # At start, either hard or soft cls need to be specified
    my $flagflips = 0;
    my $nskips = 0;
    while(<INP>) {
	chomp;
	next if (m/^c/ || m/^\s*$/);
	if (m/^p\s+hard.*$/) {
	    &UTILS::vassert($hardcl != 1 && $flagflips < 2);
	    $hardcl = 1; $flagflips++; next;
	} elsif (m/^p\s+soft.*$/) {
	    &UTILS::vassert($hardcl != 0 && $flagflips < 2);
	    $hardcl = 0; $flagflips++; next;
	}
	#{print "ERROR: Comments in DIMACS format must be in the preamble!\n";
	#exit(1);}
	my @rlits = split(/\s+/,$_);
	if ($rlits[0] eq '') { shift @rlits; }
	if ($rlits[$#rlits] eq '') { pop @rlits; }
	push @clits, @rlits;
	if ($clits[$#clits] == 0) {
	    my @nclits = @clits;    # clean duplicates
	    my $litref = &CLUTILS::clclean(\@nclits);
	    @clits = @{$litref};
	    my $keepcl = 1;    # Used to filter trivial soft cls
	    if ($#clits == 1) {
		my $nncl = -1 * $clits[0];
		if (defined(${$self->{CL_SET}}{$nncl})) { $keepcl = 0;}
	    }
	    if ($keepcl) {
		my $ncl = &CLUTILS::clsig(\@clits);
		if (defined(${$self->{CL_SET}}{$ncl})) {
		    my $defhard = ${$self->{CL_TYPE}}{$ncl};
		    my $ishard = ($defhard) ? '' : 'non-';
		    $ishard .= $ishard . 'hard';
		    print "Warn: Repeated clause defined as $ishard: $ncl\n";
		} else {
		    ${$self->{CL_SET}}{$ncl} = 1;
		    ${$self->{CL_STAT}}{$ncl} = 'INIT';  # Options: BLOCK, AUX
		    ${$self->{CL_TYPE}}{$ncl} = $hardcl; # It is 1 if it is hard
		    if ($hardcl) { $self->{CL_NUM_HARD}++; }
		    else         { $self->{CL_NUM_SOFT}++; }
		}
	    } else { $nskips++; }
	    @clits = ();
	} # else { print "Non-ended clause?\n"; }
    }
    print "Number of skipped cls: $nskips\n";
}

sub parse_wmcnf() {  # Format is the weighted maxsat cnf format
    my $self = shift;
    local *INP = shift;
    #
    my @clits = ();
    my $clweight = -1;  # At start, no weight is defined
    while(<INP>) {
	chomp;
	next if (m/^c/ || m/^\s*$/);
	if (m/^p\s+w\s+(\d+)\D*$/) {
	    $clweight = $1; next;
	}
	#{print "ERROR: Comments in DIMACS format must be in the preamble!\n";
	#exit(1);}
	my @rlits = split(/\s+/,$_);
	if ($rlits[0] eq '') { shift @rlits; }
	if ($rlits[$#rlits] eq '') { pop @rlits; }
	push @clits, @rlits;
	if ($clits[$#clits] == 0) {
	    my @nclits = @clits;    # clean duplicates
	    my $litref = &CLUTILS::clclean(\@nclits);
	    @clits = @{$litref};
	    my $ncl = &CLUTILS::clsig(\@clits);
	    ${$self->{CL_SET}}{$ncl} = 1;
	    ${$self->{CL_STAT}}{$ncl} = 'INIT';  # Options: BLOCK, AUX
	    ${$self->{CL_TYPE}}{$ncl} = $clweight; # Current weight being used
	    $self->{CL_NUM_SOFT}++;
	    @clits = ();
	} # else { print "Non-ended clause?\n"; }
    }
}


#------------------------------------------------------------------------------#
# Print methods (only for CNF format)
#------------------------------------------------------------------------------#

sub print() {
    my $self = shift;
    my $fname = shift;
    local *CNFF;
    open (CNFF, ">$fname") || &exit_err("Unable to open write file $fname");
    foreach my $cl (sort keys %{$self->{CL_SET}}) {
	#print "Printing another clause: $cl\n";
	print CNFF "$cl\n";
    }
    close CNFF;
}

sub print_dimacs() {
    my $self = shift;
    #local *FH = @_;
    my $fname = shift;
    local *CNFF;
    open (CNFF, ">>$fname") || &exit_err("Unable to open write file $fname");
    foreach my $cl (sort keys %{$self->{CL_SET}}) {
	#print "Printing another clause: $cl\n";
	print CNFF "$cl 0\n";
    }
    close CNFF;
}

sub write_dimacs() {
    my $self = shift;
    my $fname = shift;
    local *CNFF;
    open (CNFF, ">$fname") || &exit_err("Unable to open write file $fname");
    my $vnum = &IDGEN::num_id();
    my $cnum = $self->{CL_NUM};
    print CNFF "p cnf $vnum $cnum\n";
    foreach my $cl (sort keys %{$self->{CL_SET}}) {
	#print "Printing another clause: $cl\n";
	print CNFF "$cl 0\n";
    }
    close CNFF;
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
