#------------------------------------------------------------------------------#
# File:        msu1.pm
#
# Description: Implementation of MSUNCORE (v1.0) algorithm.
#
# Author:      Joao Marques-Silva.
#
# Created:     14 Sep 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package msu1;
 
use Exporter();

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

require UTILS;  # Must use require, to get INC updated
import UTILS;

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
    @UTILS::ISA = ('Exporter');
    @UTILS::EXPORT_OK = qw( &run_msu );
}

# Shared data structures
our $dbgfile = '';
our $inpfile = '';
our $opts = '';
our $blockvs = undef;

local *DBGF;


#------------------------------------------------------------------------------#
# Main msuncore script (v1.0)
#------------------------------------------------------------------------------#

sub run_msu() {    # Unique interface with msuncore front-end
    my $ds = shift;
    $dbgfile = $ds->dbgfile;
    $inpfile = $ds->inpfile;
    $blockvs = $ds->blockvs;
    $opts = $ds->opts;
    if (${$opts}{d}) { print Dumper($ds); }
    if (!defined(${$opts}{e})) {
	${$opts}{e} = 'e';
    } else {
	if (${$opts}{e} ne 'c' && ${$opts}{e} ne 'e' &&
	    ${$opts}{e} ne 'b' && ${$opts}{e} ne 'i') {
	    die "Unavailable cardinality constraint encoding option\n"; }
    }
    &msu1_algorithm($ds);
    return 0;
}

sub msu1_algorithm() {    # actual algorithm being run
    my $ds = shift;
    my $clset = $ds->clset;
    my $clmset = $ds->clmset;
    #
    if (${$opts}{d}) {
	open (DBGF, ">$dbgfile") || die "Unable to open dbg file $dbgfile\n";
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
    if (${$opts}{v} > 1) { &UTILS::report_item("Number of variables", $nvars); }
    &UTILS::report_item("Number of clauses", $ncls);
    &UTILS::report_item("Number of soft clauses", $nscls);

    my $minbs = $nscls;  # assume no clause can be satisfied, i.e. largest UB
    my $frstsat = 1;
    my $iternum = 0;
    my $nunsat = 0;
    my $lbv = 0;
    my $ubv = $nscls;

    # Setup additional clause sets
    my $cardset = CLSET->new();               # where card constraint is stored
    $clmset->add_clset($cardset);             # add to clause multiset
    my $blkclset = CLSET->new();              # tmp store for blocked clauses
    my $clbvcset = CLSET->new();              # clset w/ card constr for each cl
    $clmset->add_clset($clbvcset);            # add to clause multiset

    # 3. Main loop:
    while (1) {
	++$iternum;
	if (${$opts}{d} || ${$opts}{v}) {
	    print "Running iteration $iternum...\n";
	    if (${$opts}{d}) { print DBGF "ITERATION $iternum...\n"; }
	}
	if (${$opts}{m}) {
	    &add_percl_card_constraints($opts, $ds->initclbvs, $clbvcset);
	}
	# 3.1 Run mscore (either get core or solution)
	my ($outcome, $assign) = &SATUTILS::run_sat_solver($ds);
	# 3.2 If unsat
	if ($outcome == 0) {
	    if ($frstsat) { $nunsat++; }
	    if (${$opts}{d}) { print DBGF "Instance is UNSAT\n"; }
	    # 3.2.1 Parse computed unsat core
	    my $coreref = &SATUTILS::compute_unsat_core($ds);
	    # 3.2.2 Identify original clauses w/o blocking vars
	    my $origcls = &MSUTILS::get_initial_clauses($opts,$clset,$coreref);
	    my $osize = $#{$origcls}+1;
	    if (${$opts}{v} > 1) { print "Num orig cls in core: $osize\n"; }
	    if (${$opts}{d}) {
		$"="\n"; print DBGF "INIT SOFT CLS:\n@{$origcls}\n"; $"=' '; }
	    # 3.2.3 If there exist clauses w/o blocking vars in core
	    if ($#{$origcls} >= 0) {
		# 3.2.3.1 Remove original cls from clset
		&SATUTILS::extract_fromclset($clset, $origcls);
		# 3.2.3.2 Add blocking variables
		my $bvref =
		    &MSUTILS::add_blocking_vars($opts,$origcls,
						$blkclset,$blockvs);
		if (${$opts}{m}) {
		    $clbvcset->erase;
		    &MSUTILS::update_initcl_blockvs($opts, $blockvs,
						    $blkclset->clauses,
						    $ds->initclbvs);
		    if (${$opts}{d}) {
			print DBGF Dumper($ds->initclbvs); }
		}
		my $bclref = $blkclset->clauses;
		# Put blocked in init clset
		##$clset->add_cl_set($bclref,'BLOCK');
		$clset->add_cl_set($bclref,'INIT');
		$blkclset->erase;
		if (${$opts}{d}) {
		    my @bvs = keys %{$bvref};
		    $"=' ';print DBGF "New blocking vars: @bvs\n";$"=' ';}
		# 3.2.3.3 Add new card constraint
		&MSUTILS::gen_card_constraint($opts, $cardset, $bvref, 1);
		if (${$opts}{q}) {    # Use EQuals instead of AtMost
		    &MSUTILS::gen_extra_card_constraint($opts,
							$cardset,$bvref,1);
		}
	    } else {
		print "Must always have init clauses for max sat. ";
		print "There is a bug. Terminating...\n";
		last;
	    }
	}
	# 3.3 Else
	elsif ($outcome == 1) {
	    if ($iternum == 1) {
		&UTILS::report_item("Computed maxsat solution", $nscls); last;
	    }
	    # Parse computed solution
	    my @vassigns = split('\s+', $assign);
	    if (${$opts}{d}) { print DBGF "Assigns: @vassigns\n"; }
	    pop @vassigns;  # Remove 0 marker
	    # Access blocking vars assigned value 1
	    my $bvref = &MSUTILS::get_blocking_vars($opts,$blockvs,\@vassigns);
	    if ($#{$bvref}+1 >= $minbs) { die "Too many blocking vars??\n"; }
	    $minbs = $#{$bvref}+1;
	    # 3.3.1 Output max sat solution and terminate
	    if (${$opts}{v} > 1) {
		&UTILS::report_item("Number of block vars", $minbs);
	    }
	    &UTILS::report_item("Computed maxsat solution", $nscls - $minbs);
	    return 0;
	}
	else {
	    my $lbv = $nscls-$iternum;    # LB: num cls - num iters
	    &UTILS::report_item("Current lower bound for maxsat solution",$lbv);
	    # Can update UB: use num of iters when extracting cores (as in MSU3)
	    &UTILS::report_item("Current upper bound for maxsat solution",$ubv);
	    print "Cputime limit exceeded\n";
	    return 0;
	}
    }
    return 0;
}

# Add constraint relating blocking vars for each cl
sub add_percl_card_constraints() {
    my ($opts, $initclbvs, $clbvcset) = @_;
    my $nclconstr = 0;
    foreach my $clkey (keys %{$initclbvs}) {
	my $bvref = ${$initclbvs}{$clkey};
	my @bvsize = keys %{$bvref};
	if ($#bvsize >= ${$opts}{m}-1) {
	    if (${$opts}{d}) {
		my @bvars = keys %{$bvref};
		print DBGF "New card constr on cl $clkey bvars: @bvars\n";
	    }
	    ++$nclconstr;
	    &MSUTILS::gen_card_constraint($opts, $clbvcset, $bvref, 1);
	}
    }
    if (${$opts}{v} > 3) { print "Number of percl card constr: $nclconstr\n"; }
}


END {
}

1;

# jpms
###############################################################################
