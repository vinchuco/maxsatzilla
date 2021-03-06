#------------------------------------------------------------------------------#
# File:        msu2.pm
#
# Description: Implementation of MSUNCORE (v2.0) algorithm.
#
# Author:      Joao Marques-Silva.
#
# Created:     15 Sep 2007.
#
# Revision:    $Id$.
#
# Notes:       Optimized version of the algorithm submitted to DATE'08.
#------------------------------------------------------------------------------#

package msu2;
 
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
    @msu2::ISA = ('Exporter');
    @msu2::EXPORT_OK = qw( &run_msu );
}

# Shared data structures
our $dbgfile = '';
our $inpfile = '';
our $opts = '';
our $blockvs = undef;

local *DBGF;


#------------------------------------------------------------------------------#
# Main msuncore script (v2.0)
#------------------------------------------------------------------------------#

sub run_msu() {    # Unique interface with msuncore front-end
    my $ds = shift;
    $dbgfile = $ds->dbgfile;
    $inpfile = $ds->inpfile;
    $blockvs = $ds->blockvs;
    $opts = $ds->opts;
    if (${$opts}{d}) { print Dumper($ds); }
    if (!defined(${$opts}{e})) {
	${$opts}{e} = 'i';
    } else {
	if (${$opts}{e} ne 'b' && ${$opts}{e} ne 'i' && ${$opts}{e} ne 'c' &&
	    ${$opts}{e} ne 's') {
	    &exit_err("Unavailable cardinality constraint encoding option\n"); }
    }
    &msu2_algorithm($ds);
    return 0;
}

sub msu2_algorithm() {    # actual algorithm being run
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

    my $minbs = $nscls;  # assume no clause can be satisfied, i.e. largest UB
    my $iternum = 0;
    my $nunsat = 0;
    my $lbv = 0;
    my $ubv = $nscls;

    # Setup additional clause sets
    my $cardset = CLSET->new();              # where card constraints are stored
    $clmset->add_clset($cardset);            # add to clause multiset
    my $blkclset = CLSET->new();             # store for blocked clauses
    $clmset->add_clset($blkclset);           # add to clause multiset

    # 3. Main loop:
    while (1) {
	++$iternum;
	if (${$opts}{d} || ${$opts}{v}) {
	    print "Running iteration $iternum...\n";
	    if (${$opts}{d}) { print DBGF "ITERATION $iternum...\n"; }
	}
	# 3.1 Run mscore (either get core or solution)
	my ($outcome, $assign) = &SATUTILS::run_sat_solver($ds);
	# 3.2 If unsat
	if ($outcome == 0) {
	    $nunsat++;
	    if (${$opts}{d}) { print DBGF "Instance is UNSAT\n"; }
            if (${$opts}{v} > 2) { print "Instance is UNSAT\n"; }
	    # 3.2.1 Parse computed unsat core
	    my $coreref = &SATUTILS::compute_unsat_core($ds);
	    # 3.2.2 Identify original clauses w/o blocking vars
	    my $origcls = &MSUTILS::get_initial_clauses($opts,$clset,$coreref);
	    my $osize = $#{$origcls}+1;
	    if (${$opts}{v} > 1) { print "Num orig cls in core: $osize\n"; }
	    if (${$opts}{d}) {
		$"="\n"; print DBGF "INIT SOFT CLS:\n@{$origcls}\n"; $"=' ';}
	    # 3.2.3 If there exist clauses w/o blocking vars in core
	    if ($#{$origcls} >= 0) {
		# 3.2.3.1 Remove original cls from clset
		&SATUTILS::extract_fromclset($clset, $origcls);
		# 3.2.3.3 Add blocking variables
		my $bvref =
		    &MSUTILS::add_blocking_vars($opts,$origcls,$blkclset,$blockvs);
		my @bvs = keys %{$bvref};
		if (${$opts}{d}) {
		    $"=' ';print DBGF "New blocking vars: @bvs\n";$"=' ';}
		# 3.2.3.3 Add new card constraint
		# Fixed: jpms:20071128
		#&MSUTILS::gen_card_constraint($opts,$cardset,$bvref,$#bvs);
		&MSUTILS::gen_extra_card_constraint($opts,$cardset,$bvref,1);
	    } else {
		# 3.2.2 Else output max sat solution and terminate
		&UTILS::report_item("Number of block vars", $minbs);
		&UTILS::report_item("Computed maxsat solution", $nscls-$minbs);
		return 0;
	    }
	}
	# 3.3 Else
	elsif ($outcome == 1) {
	    if (${$opts}{d}) { print DBGF "Instance is SAT\n"; }
	    if (${$opts}{v} > 2) { print "Instance is SAT\n"; }
	    if ($iternum == 1) {
		&UTILS::report_item("Computed maxsat solution", $nscls);
		return 0;
	    }
	    # Parse computed solution
	    my @vassigns = split('\s+', $assign);
	    if (${$opts}{d}) { print DBGF "Assigns: @vassigns\n"; }
	    pop @vassigns;  # Remove 0 marker
	    # Access blocking vars assigned value 1
	    my $bvref = &MSUTILS::get_blocking_vars($opts,$blockvs,\@vassigns);
	    if ($#{$bvref}+1 < $minbs) { $minbs = $#{$bvref}+1; }
	    if (${$opts}{d}) {
		print DBGF "Current maxsat solution value: $minbs\n"; }
	    if (${$opts}{v}) {
		&UTILS::report_item("Updated lower bound",$nscls-$minbs);  }
	    #&UTILS::report_item("Number of b vars w/ value 1",$minbs);
	    # 3.3.1 Add new card constraint
	    if (${$opts}{v}) {
		my @bvars = keys %{$blockvs};
		my $nbs = $#bvars+1;
		#my $nbb = $#{$bvref};
		my $nbb = $minbs-1;
		&UTILS::report_item('Number of blocking vars', $nbs);
		&UTILS::report_item('Card constraint rhs', $nbb);
	    }
	    $cardset->erase;    # erase previous card clset
	    #&MSUTILS::gen_card_constraint($opts,$cardset,$blockvs, $#{$bvref}); # DATE'08
	    &MSUTILS::gen_card_constraint($opts,$cardset,$blockvs,$minbs-1);
	    $lbv = $nscls-$minbs;
	    $ubv = $nscls-$nunsat;
	    &UTILS::report_item("Lower bound for maxsat solution", $lbv);
	    &UTILS::report_item("Upper bound for maxsat solution", $ubv);
	    if ($lbv == $ubv) {
		# If bounds are equal, can terminate...
		&UTILS::report_item("Number of block vars", $minbs);
		&UTILS::report_item("Computed maxsat solution", $lbv);
		return 0;
	    }
	}
	else {
	    my $lbv = $nscls-$minbs;
	    &UTILS::report_item("Current lower bound for maxsat solution",$lbv);
	    &UTILS::report_item("Current upper bound for maxsat solution",$ubv);
	    &exit_quit("Cputime limit exceeded\n"); last;
	}
	$ubv = $nscls-$nunsat;
	if ($lbv == $ubv) {
	    # If bounds are equal, can terminate...
	    print "Lower bound equals upper bound. Terminating...\n";
	    &UTILS::report_item("Number of block vars", $minbs);
	    &UTILS::report_item("Computed maxsat solution", $lbv);
	    return 0;
	}
    }
    &exit_err("Should *not* get to this part. Terminating...");
    return 0;
}


END {
}

1;

# jpms
###############################################################################
