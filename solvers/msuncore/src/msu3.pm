#------------------------------------------------------------------------------#
# File:        msu3.pm
#
# Description: Implementation of MSUNCORE (v3.0) algorithm.
#
# Author:      Joao Marques-Silva.
#
# Created:     14 Sep 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package msu3;
 
use Exporter();

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

require UTILS;  # Must use require, to get INC updated
import UTILS qw( &get_progname &get_progpath &exit_ok &exit_err &exit_quit );

require IDGEN;
import IDGEN qw( &num_id &gen_id &set_id );

require CLSET;
import CLSET;

require CLMSET;
import CLMSET;

require MSUDS;
import MSUDS;

require SATUTILS;
import SATUTILS;

require MSUTILS;
import MSUTILS;

BEGIN {
    @msu3::ISA = ('Exporter');
    @msu3::EXPORT_OK = qw( &run_msu );
}

# Shared data structures
our $dbgfile = '';
our $inpfile = '';
our $opts = '';
our $blockvs = undef;
our $coreset = undef;

local *DBGF;


#------------------------------------------------------------------------------#
# Main msuncore script (v3.0)
#------------------------------------------------------------------------------#

sub run_msu() {    # Unique interface with msuncore front-end
    my $ds = shift;
    $dbgfile = $ds->dbgfile;
    $inpfile = $ds->inpfile;
    $blockvs = $ds->blockvs;
    $coreset = $ds->coreset;
    $opts = $ds->opts;
    #
    if (${$opts}{d}) { print DBGF Dumper($ds); }
    if (!defined(${$opts}{e})) {
	${$opts}{e} = 'i';
    } else {
	if (${$opts}{e} ne 'b' && ${$opts}{e} ne 'i' && ${$opts}{e} ne 'c' &&
	    ${$opts}{e} ne 's') {
	    &exit_err("Unavailable cardinality constraint encoding option\n"); }
    }
    &msu3_algorithm($ds);
    return 0;
}

sub msu3_algorithm() {    # actual algorithm being run
    my $ds = shift;
    my $clset = $ds->clset;
    my $clmset = $ds->clmset;
    #
    if (${$opts}{d}) {
	open (DBGF, ">$dbgfile") ||
	    &exit_err("Unable to open dbg file $dbgfile\n");
	$ds->set_dbghandle(\*DBGF);
	&MSUTILS::register_dbghandle($ds->dbghandle);
    }
    # 2. Load CNF formula
    $clset->parse($inpfile);
    my $nvars = $clset->numvars;
    my $ncls = $clset->numcls;
    my $nscls = $clset->numcls_soft;

    if (${$opts}{d}) {
	print DBGF "VARS: $nvars    --    CLS: $ncls\n";
	my $nid = &IDGEN::num_id(); print DBGF "IDGEN ids: $nid\n"; }
    &UTILS::report_item("Number of clauses", $ncls);
    &UTILS::report_item("Number of soft clauses", $nscls);

    my $ndcores = 0;
    my $mxub = $nscls;
    my $mxlb = 0;

    # 3. First loop: remove disjoint cores
    while (1) {
	if (${$opts}{d} || ${$opts}{v}) {
	    my $niter = $ndcores+1;
	    print "Running 1st loop iteration $niter...\n";
	    if (${$opts}{d}) { print DBGF "1ST LOOP ITERATION $niter...\n"; }
	}
	# 3.1 Run mscore (either get core or solution)
	my $outcome = &SATUTILS::compute_and_remove_core($ds);
	if ($outcome == 0) { ++$ndcores; }
	elsif ($outcome == 1) {
	    if ($ndcores == 0) {
		my $msg = 'Instance is SAT. Computed maxsat solution';
		&UTILS::report_item($msg, $nscls); return 0;
	    } elsif (${$opts}{d}) {
		print DBGF "Instance is SAT; Moving to next phase...\n";
	    }
	    last;
	} else {
	    &exit_quit("No LB/UB data\nCputime limit exceeded\n"); return 0; }
    }
    my $ncs = $#{$coreset} + 1;
    print "Number of cores: $ncs\n";
    my $tub = $nscls - $ncs;
    print "Current upper bound: $tub\n";

    # Setup additional clause sets
    my $blkclset = CLSET->new();              # where blocked clauses are stored
    $clmset->add_clset($blkclset);            # add to clause multiset
    my $cardset = CLSET->new();               # where card constraint is stored
    $clmset->add_clset($cardset);             # add to clause multiset

    # 4. Compute blocking vars & setup initial clause set of blocked cls
    &MSUTILS::compute_blocking_vars($opts, $coreset, $blockvs);
    &MSUTILS::add_blockvs_coreset($opts, $blkclset, $coreset, $blockvs);
    my $nbvlb = $ndcores;  # Lower bound on blocking vars that must take value 1

    # 5. Second loop: increase card constr
    while (1) {
	# 5.1. Generate clause set with CNF encoding of card constraint
	if (${$opts}{d} || ${$opts}{v}) {
	    my $niter = ($nbvlb - $ndcores)+1;
	    print "Running 2nd loop iteration $niter...\n";
	    if (${$opts}{v} > 2 || ${$opts}{d}) {
		my @bvkeys = keys %{$blockvs};
		my $bvsize = $#bvkeys+1;
		print "Number of blocking vars: $bvsize w/ RHS $nbvlb\n";
		print DBGF "Number of blocking vars: $bvsize w/ RHS $nbvlb\n";
	    }
	    if (${$opts}{d}) { print DBGF "2ND LOOP ITERATION $niter...\n"; }
	}
	&MSUTILS::gen_card_constraint($opts, $cardset, $blockvs, $nbvlb);
	if (${$opts}{q}) {    # Use EQuals instead of AtMost
	    &MSUTILS::gen_extra_card_constraint($opts,$cardset,$blockvs,$nbvlb);
	}
	if (${$opts}{d}) {
	    print DBGF "BLOCK VARS:\n", Dumper($blockvs);
	    print DBGF "BLOCK CLSET:\n", Dumper($blkclset);
	    print DBGF "CARD CLSET:\n", Dumper($cardset);
	}
	my ($outcome, $assign) = &SATUTILS::run_sat_solver($ds);
	if ($outcome == 0) {
	    $nbvlb++;
	    # Find original clauses in core
	    my $coreref = &SATUTILS::compute_unsat_core($ds);
	    my $initref = &MSUTILS::get_initial_clauses($opts,$clset,$coreref);
	    if (${$opts}{v} > 2 || ${$opts}{d}) {
		my $initsz = $#{$initref}+1;
		print "Number of initial cls: $initsz\n";
	    }
	    if (${$opts}{d}) {
		my $initsz = $#{$initref}+1;
		print DBGF "Number of initial cls: $initsz\n";
		$"="\n";print DBGF "INIT SOFT CLS:\n@{$initref}\n";$"=' '; }

	    #$"="\n";print "INIT SOFT CLS:\n@{$initref}\n";$"=' ';

	    &SATUTILS::extract_fromclset($clset, $initref);
	    &MSUTILS::add_blocking_vars($opts, $initref, $blkclset, $blockvs);
	    $cardset->erase;    # erase previous card clset
	} elsif ($outcome == 1) {
	    &UTILS::report_item("Number of block vars", $nbvlb);
	    &UTILS::report_item("Computed maxsat solution", $nscls - $nbvlb);
	    return 0;
	} else {
	    $mxub = $nscls-$nbvlb;
	    &UTILS::report_item("Lower bound for maxsat solution", $mxlb);
	    &UTILS::report_item("Upper bound for maxsat solution", $mxub);
	    &exit_quit("Cputime limit exceeded\n"); last;
	}
    }
    &exit_err("Should *not* get to this part. Terminating...");
    return 0;
}


END {
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
