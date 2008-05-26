##
## Author: jpms
## Version: $Id$
##
package msu4;
 
use Exporter();

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

require UTILS;  # Must use require, to get INC updated
import UTILS qw( &get_progname &get_progpath &exit_ok &exit_err &exit_quit );

require IDGEN;  # Must use require, to get INC updated
import IDGEN qw( &num_id &gen_id &set_id );

require CLUTILS;
import CLUTILS;

require CLSET;
import CLSET;

require CARD;
import CARD;

BEGIN {
    @msu4::ISA = ('Exporter');
    @msu4::EXPORT_OK = qw( &run_msu );
}

our $progname = '';
our $strpref = '';
our $cnffile = '';
our $outfile = '';
our $xxtmpfile = '';
our $dbgfile = '';
our $corefile = '';
our $logfile = '';
our $opts = '';
our $inpfile = '';
our $timeout = '';

# Additional data structures
our %blockvs = ();
our @coreset = ();


################################################################################
# Main msuncore script
################################################################################

sub run_msu() {    # Unique interface with msuncore front-end
    my $ds = shift;
    $progname = $ds->{PROGNAME};
    $strpref = $ds->{STRPREF};
    $cnffile = $ds->{CNFFILE};
    $outfile = $ds->{OUTFILE};
    $xxtmpfile = $ds->{TMPFILE};
    $dbgfile = $ds->{DBGFILE};
    $corefile = $ds->{COREFILE};
    $logfile = $ds->{LOGFILE};
    $inpfile = $ds->{INPFILE};
    $timeout = $ds->{TIMEOUT};
    $opts = $ds->{OPTS};
    if (${$opts}{d}) { print Dumper($ds); }
    if (!defined(${$opts}{e})) {
	${$opts}{e} = 'b';
    } else {
	if (${$opts}{e} ne 'c' && ${$opts}{e} ne 'e') {
	    &exit_err("Unavailable cardinality constraint encoding option\n"); }
    }
    &msu4_algorithm();
}

sub msu4_algorithm() {    # actual algorithm being run
    if (${$opts}{d}) {
	open (DBGF, ">$dbgfile") ||
	    &exit_err("Unable to open log file $dbgfile\n");
    }
    # 2. Load CNF formula
    my $clset = CLSET->new();
    $clset->parse($inpfile);
    my $nvars = $clset->numvars;
    my $ncls = $clset->numcls;

    if (${$opts}{d}) {
	print DBGF "VARS: $nvars    --    CLS: $ncls\n";
	my $nid = &IDGEN::num_id(); print DBGF "IDGEN ids: $nid\n"; }
    &UTILS::report_item("Number of clauses", $ncls);

    &exit_err("This is a stub for alg 4...\n");

    my $ndcores = 0;
    my $mxub = $ncls;
    my $mxlb = 0;

    # 3. First loop: remove disjoint cores
    while (1) {
	if (${$opts}{d} || ${$opts}{v}) {
	    my $niter = $ndcores+1;
	    print "Running 1st loop iteration $niter...\n";
	    if (${$opts}{d}) { print DBGF "1ST LOOP ITERATION $niter...\n"; }
	}
	# 3.1 Run mscore (either get core or solution)
	my $outcome = &compute_and_remove_core($clset);
	if ($outcome == 0) { ++$ndcores; }
	elsif ($outcome == 1) {
	    if ($ndcores == 0) {
		my $msg = 'Instance is SAT. Computed maxsat solution';
		&UTILS::report_item($msg, $ncls); &finishup(); exit;
	    }
	    last;
	} else { print "No LB/UB data\nCputime limit exceeded\n"; exit(1); }
    }

    my $ncs = $#coreset + 1;
    print "Number of cores: $ncs\n";
    my $tub = $ncls - $ncs;
    print "Current upper bound: $tub\n";

    # 4. Compute blocking vars & setup initial clause set of blocked cls
    &compute_blocking_vars(\@coreset, \%blockvs);
    my $blkclset = CLSET->new();              # where blocked clauses are stored
    &init_block_clause_set($clset, \@coreset, \%blockvs);
    my $nbvlb = $ndcores;  # Lower bound on blocking vars that must take value 1

    # 5. Second loop: increase card constr
    while (1) {
	# 5.1. Generate clause set with CNF encoding of card constraint
	my $cardset = &gen_card_constraint(\%blockvs, $nbvlb);
	if (${$opts}{d} || ${$opts}{v}) {
	    my $niter = ($nbvlb - $ndcores)+1;
	    print "Running 2nd loop iteration $niter...\n";
	    if (${$opts}{d}) { print DBGF "2ND LOOP ITERATION $niter...\n"; }
	}
	my $clmset = CLMSET->new();  # create clause multiset
	$clmset->add_clset($clset);       # cls remaining in clause set
	$clmset->add_clset($blkclset);    # cls w/ blocking vars
	$clmset->add_clset($cardset);     # encoding of card constraint
	my $outcome = &run_sat_solver($clmset);
	if ($outcome == 0) {
	    $nbvlb++;
	    # Find original clauses in core
	    my $coreref = &parse_core_file($corefile);
	    if (${$opts}{d}) {
		$"="\n"; print DBGF "CORE:\n@{$coreref}\n"; $"=' '; }
	    my $initref = &get_initial_clauses($coreref, $clset);
	    if (${$opts}{d}) {
		$"="\n";print DBGF "INIT CLS:\n@{$initref}\n";$"=' '; }
	    &extract_init_clauses($initref, $clset);
	    &add_blocking_vars($initref, $blkclset);
	} elsif ($outcome == 1) {
	    &UTILS::report_item("Number of block vars", $nbvlb);
	    &UTILS::report_item("Computed maxsat solution", $ncls - $nbvlb);
	    &finishup(); exit;
	} else {
	    $mxub = $ncls-$nbvlb;
	    &UTILS::report_item("Lower bound for maxsat solution", $mxlb);
	    &UTILS::report_item("Upper bound for maxsat solution", $mxub);
	    print "Cputime limit exceeded\n";
	    &finishup(); exit;
	}
    }
}


################################################################################
# Auxiliary functions
################################################################################

sub finishup() {
    if (!${$opts}{c}) { system("rm -f $cnffile $outfile $xxtmpfile $corefile"); }
    if (${$opts}{d} && !${$opts}{c}) { close DBGF;system("rm -f $dbgfile $logfile"); }
}

# Run SAT solver on clause multiset
sub run_sat_solver() {
    my ($clmset) = @_;
    my $outcome = -1;
    # 1. Generate CNF file from clset
    $clmset->write_dimacs($cnffile);
    # 2. Invoke mscore 
    my $csh_cmd = "./mscore $cnffile $outfile";
    ##print "CSH_CMD: $csh_cmd\n";
    if (${$opts}{T} ne '') {
	my $time_limit = &available_time(${$opts}{T});
	`csh -f -c "limit cputime $time_limit; $csh_cmd" >& $xxtmpfile`;
    } else { 
	`csh -f -c "$csh_cmd" >& $xxtmpfile`;
    }
    # 3. Parse output file and get outcome
    my ($outcome, $assign) = &parse_sat_outfile($outfile, $xxtmpfile);
    if (${$opts}{d}) { print "Outcome: $outcome\n"; }
    return $outcome;  # 2nd arg is ref to data structure
}

# Run SAT solver on clause set to compute unsat core, if one exists
sub compute_and_remove_core() {
    my ($clset) = @_;
    my $outcome = -1;
    # 1. Generate CNF file from clset
    $clset->write_dimacs($cnffile);
    # 2. Invoke mscore 
    my $csh_cmd = "./mscore $cnffile $outfile";
    ##print "CSH_CMD: $csh_cmd\n";
    if (${$opts}{T} ne '') {
	my $time_limit = &available_time();
	`csh -f -c "limit cputime $time_limit; $csh_cmd" >& $xxtmpfile`;
    } else { 
	`csh -f -c "$csh_cmd" >& $xxtmpfile`;
    }
    # 3. Parse output file and get outcome
    my ($outcome, $assign) = &parse_sat_outfile($outfile, $xxtmpfile);
    # 3.1 If SAT, compute set of blocking vars and return it
    if ($outcome == 0) {
	if (${$opts}{d}) { print DBGF "Instance is UNSAT\n"; }
	# 3.2.1 Parse computed unsat core
	my $coreref = &compute_core($corefile, $clset);
	&extract_core_fromclset($coreref, $clset);
	if (${$opts}{d}) { $"="\n"; print DBGF "CORE:\n@{$coreref}\n";$"=' '; }
	push @coreset, $coreref;    # add core to set of cores
    } elsif (${$opts}{d} && $outcome == 1) {
	print DBGF "Instance is SAT; Moving to next phase...\n";
    }
    if (!${$opts}{c}) { system("rm -f $cnffile $outfile $xxtmpfile $corefile"); }
    return $outcome;
}

sub parse_sat_outfile() {
    my ($fname, $tname) = @_;
    my $abort = 0;
    my $outcome = -1;
    my $assign = '';
    open (TMPF, "<$tname") || &exit_err("Unable to open TMP output file\n");
    while(<TMPF>) {
	if (m/Cputime limit exceeded/) { $abort = 1; last; }
    }
    close TMPF;
    if (!$abort) {
	open (SATF, "<$fname") || &exit_err("Unable to open SAT output file\n");
	while(<SATF>) {
	    chomp;
	    if (m/UNSAT/)  { $outcome = 0; last; }
	    elsif (m/SAT/) { $outcome = 1; }
	    else           { $assign = $_; last; }
	}
	if ($outcome < 0) { &exit_err("Invalid SAT solver outcome??\n"); }
	close SATF;
    }
    return ($outcome, $assign);
}

# Compute unsat core; can apply cl filt
sub compute_core() {
    my ($corefile, $clset) = @_;
    my $coreref = &parse_core_file($corefile);
    if (${$opts}{d}) {
	$"="\n"; print DBGF "CORE:\n@{$coreref}\n"; $"=' ';
	"Num cls in core: $#{$coreref}\n";
    }
    return $coreref;
}

# Compute all clauses in unsat core
sub parse_core_file() {
    my $corefile = shift;
    my @corecls = ();
    open (COREF, "<$corefile") ||
	&exit_err("Unable to open core file $corefile\n");
    while(<COREF>) {
	chomp;
	next if (m/^c /);
	next if (m/p cnf/);
	my @clits = split('\s+');
	if ($clits[0] eq '') { shift @clits; }
	if ($clits[$#clits] eq '') { pop @clits; }
	my $nclref = &CLUTILS::clsig(\@clits);
	push @corecls, $nclref;
    }
    if (${$opts}{d}) { $" = "\n"; print DBGF "CORECLS: @corecls\n"; $" = ' '; }
    close COREF;
    return \@corecls;
}

sub extract_core_fromclset() {
    my ($coreref, $clset) = @_;  # ref to array; object
    $clset->del_cl_set($coreref);
}

sub compute_blocking_vars() {   # Identify set of bocking vars
    my ($coreref, $blockvs) = @_;  # ref to array, ref to array
    if (${$opts}{d}) {
	my $ncs = $#{$coreref} + 1;
	print DBGF "Number of cores: $ncs\n";
    }
    foreach my $core (@{$coreref}) {
	if (${$opts}{d}) { print DBGF "cls in core: $#{$core}\n"; }
	my $corecls = $#{$core}+1;  # number of cls in core;
	for (my $i=0; $i<$corecls; ++$i) { 
	    my $nid = &IDGEN::gen_id();
	    ${$blockvs}{$nid} = 1;
	}
    }
    if (${$opts}{d}) {
	my @bvs = keys %{$blockvs}; print DBGF "Blocking vars: @bvs\n"; }
}

sub init_block_clause_set() {
    my ($bclset, $coreref, $blockvs) = @_;  # object, ref to array, ref to hash
    my @auxbvs = keys %{$blockvs};  # get array of block vars from keys of hash
    if (${$opts}{d}) { print DBGF "Aux block vars: @auxbvs\n"; }

    foreach my $core (@{$coreref}) {
	my $corecls = $#{$core}+1;  # number of cls in core;
	foreach my $corecl (@{$core}) {
	    my $clits = &CLUTILS::cllits($corecl);
	    if (${$opts}{d}) { print DBGF "CL LITS:: @{$clits}\n"; }
	    my $nbv = pop @auxbvs;
	    push @{$clits}, $nbv; push @{$clits}, 0;  # terminate w/ 0
	    if (${$opts}{d}) { print DBGF "CL LITS:: @{$clits}\n"; }
	    my $clsig = &CLUTILS::clsig($clits);
	    $bclset->add_cl($clsig, 'BLOCK');
	    if (${$opts}{d}) { print DBGF "Adding new cl: $clsig\n"; }
	}
    }
}

# Get original clauses in core, i.e. w/o blocking vars
sub get_initial_clauses() {
    my ($coreref, $clset) = @_;
    my @initcls = ();
    foreach my $cl (@{$coreref}) {
	if ($clset->lkup_cl($cl)) {  # If clause still exists in original clset
	    push @initcls, $cl;
	    if (${$opts}{d}) { print DBGF "$cl status is init\n"; }
	} elsif (${$opts}{d}) { print DBGF "$cl status is *not* init\n"; }
    }
    return \@initcls;
}

# Extract init cls form clause set
sub extract_init_clauses() {
    my ($initref, $clset) = @_;  # ref to array; object
    foreach my $cl (@{$initref}) {
	$clset->del_cl($cl);
    }
}

# Add blocking vars to set of original clauses
sub add_blocking_vars() {
    my ($origcls, $bclset) = @_;
    foreach my $cl (@{$origcls}) {
	my $clits = &CLUTILS::cllits($cl);
	if (${$opts}{d}) { print DBGF "CL LITS:: @{$clits}\n"; }
	my $nid = &IDGEN::gen_id();
	$blockvs{$nid} = 1;                         # register as blocking var
	push @{$clits}, $nid; push @{$clits}, 0;    # terminate w/ 0
	if (${$opts}{d}) { print DBGF "CL LITS:: @{$clits}\n"; }
	my $clsig = &CLUTILS::clsig($clits);
	$bclset->add_cl($clsig, 'BLOCK');
	if (${$opts}{d}) { print DBGF "Adding new cl: $clsig\n"; }
    }
}

# Generate cardinality constraint
sub gen_card_constraint() {
    my ($bvref, $tval) = @_;
    my @auxbvs = keys %{$bvref};
    my $cardset = CLSET->new();
    if (${$opts}{d}) {
	print DBGF "Generating card set for @auxbvs w/ rhs $tval\n";
    }
    my $clstr = &CARD::gen_atmostN(\@auxbvs, $tval);
    my @newcls = split("\n", $clstr);
    for(my $i=0; $i<=$#newcls; $i++) {
	my @clits = split('\s+', $newcls[$i]);
	$newcls[$i] = &CLUTILS::clsig(\@clits);
    }
    if (${$opts}{d}) { $"=', '; print DBGF "NEW CLS:\n@newcls\n"; $"=' '; }
    $cardset->add_cl_set(\@newcls, 'AUX');
    return $cardset;
}


END {
}

1;

# jpms
###############################################################################
