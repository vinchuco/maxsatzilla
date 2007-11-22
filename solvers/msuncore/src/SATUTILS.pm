#------------------------------------------------------------------------------#
# File:        SATUTILS.pm
#
# Description: Utilities for interfacing SAT solvers and unsat core generators
#
# Author:      Joao Marques-Silva.
#
# Created:     13 Sep 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package SATUTILS;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

require MSUDS;  # Must use require, to get INC updated
import MSUDS;

BEGIN {
    @SATUTILS::ISA = ('Exporter');
    @SATUTILS::EXPORT_OK = qw( );
}


#------------------------------------------------------------------------------#
# Execution of (MiniSAT) SAT solver & parsing of result files
#------------------------------------------------------------------------------#

# Run SAT solver on clause set/multiset
sub run_sat_solver() {
    my $ds = shift;
    my $clmset = $ds->clmset;
    my $cnffile = $ds->cnffile;
    my $outfile = $ds->outfile;
    my $xtmpfile = $ds->xtmpfile;
    my $opts = $ds->opts;
    # 1. Generate CNF file from cl multiset
    $clmset->write_dimacs($cnffile);
    # 2. Invoke mscore
    my $progpath = &UTILS::get_progpath();
    my $csh_cmd = "$progpath/../mscore $cnffile $outfile";
    ##print "CSH_CMD: $csh_cmd\n";
    if (${$opts}{T} ne '') {
	my $time_limit = &UTILS::available_time(${$opts}{T});
	`csh -f -c "limit cputime $time_limit; $csh_cmd" >& $xtmpfile`;
    } else { 
	`csh -f -c "$csh_cmd" >& $xtmpfile`;
    }
    # 3. Parse output file and get outcome
    my ($outcome, $assign) = &parse_sat_solver_output($outfile, $xtmpfile);
    if (${$opts}{d}) { print "Outcome: $outcome\n"; }
    return ($outcome, $assign);  # 2nd arg is ref to data structure
}

# Run SAT solver on clause set to compute unsat core, if one exists
sub compute_and_remove_core() {
    my $ds = shift;
    my $clset = $ds->clset;
    my $cnffile = $ds->cnffile;
    my $outfile = $ds->outfile;
    my $xtmpfile = $ds->xtmpfile;
    my $corefile = $ds->corefile;
    my $coreset = $ds->coreset;
    my $DBGF = $ds->dbghandle;

    my $opts = $ds->opts;
    if (${$opts}{d}) { print $DBGF Dumper($ds); }
    # 1. Run SAT solver
    my $outcome = &run_sat_solver($ds);
    # 2. Parse output file and get outcome
    my ($outcome, $assign) = &parse_sat_solver_output($outfile, $xtmpfile);
    # 3.1 If SAT, compute set of blocking vars and return it
    if ($outcome == 0) {
	if (${$opts}{d}) { print $DBGF "Instance is UNSAT\n"; }
	# 3.2.1 Get computed unsat core
	my $coreref = &compute_unsat_core($ds);
	if (${$opts}{d}) {$"="\n"; print $DBGF "CORE:\n@{$coreref}\n"; $"=' '; }
	&extract_fromclset($clset, $coreref);
	push @{$coreset}, $coreref;    # add core to set of cores
    }
    return $outcome;
}

sub parse_sat_solver_output() {
    my ($fname, $tname) = @_;
    my $abort = 0;
    my $outcome = -1;
    my $assign = '';
    open (TMPF, "<$tname") || die "Unable to open TMP output file\n";
    while(<TMPF>) {
	if (m/Cputime limit exceeded/) { $abort = 1; last; }
    }
    close TMPF;
    if (!$abort) {
	open (SATF, "<$fname") || die "Unable to open SAT output file\n";
	while(<SATF>) {
	    chomp;
	    if (m/UNSAT/)  { $outcome = 0; last; }
	    elsif (m/SAT/) { $outcome = 1; }
	    else           { $assign = $_; last; }
	}
	if ($outcome < 0) { die "Invalid SAT solver outcome??\n"; }
	close SATF;
    }
    return ($outcome, $assign);
}

# Compute unsat core; can apply cl filt
sub compute_unsat_core() {
    my $ds = shift;
    my $clset = $ds->clset;
    my $corefile = $ds->corefile;
    my $opts = $ds->opts;
    my $DBGF = $ds->dbghandle;
    #
    my $coreref = &load_unsat_core($clset, $corefile);
    my $csize = $#{$coreref}+1;
    if (${$opts}{v} > 5) { print "Num cls in core: $csize\n"; }
    if (${$opts}{d}) {
	print $DBGF "Num cls in core: $csize\n";
	$"="\n"; print $DBGF "CORE:\n@{$coreref}\n"; $"=' ';
    }
    ## if (${$opts}{p}) {    # Original cl filtering ideas does not work
    ## # Recompute cls in core due cl filtering
    ## $coreref = &MSUTILS::clfilt($opts, $coreref, $clset);
    ## }
    return $coreref;
}

# Compute all clauses in unsat core
sub load_unsat_core() {
    my $clset = shift;
    my $corefile = shift;
    my $corecls = [];
    open (COREF, "<$corefile") || die "Unable to open core file $corefile\n";
    while(<COREF>) {
	chomp;
	next if (m/^c /);
	next if (m/p cnf/);
	my @clits = split('\s+');
	if ($clits[0] eq '') { shift @clits; }
	if ($clits[$#clits] eq '') { pop @clits; }
	my $nclref = &CLUTILS::clsig(\@clits);  # must be terminated w/ 0 marker
	if ($clset->chk_cl_soft($nclref)) {
	    push @{$corecls}, $nclref;  # Only consider soft clauses
	}
    }
    close COREF;
    return $corecls;
}

sub extract_fromclset() {
    my ($clset, $subclset) = @_;
    $clset->del_cl_set($subclset);
}


END {
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
