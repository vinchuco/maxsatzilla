#------------------------------------------------------------------------------#
# File:        MSUTILS.pm
#
# Description: Utilities used for solving MaxSAT problems.
#
# Author:      Joao Marques-Silva.
#
# Created:     13 Sep 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package MSUTILS;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

require UTILS;  # Must use require, to get INC updated
import UTILS qw( &get_progname &get_progpath &exit_ok &exit_err &exit_quit );

require CLUTILS;
import CLUTILS;

require CLSET;
import CLSET;

require MSUDS;
import MSUDS;

require CARD;
import CARD;

BEGIN {
    @MSUTILS::ISA = ('Exporter');
    @MSUTILS::EXPORT_OK = qw( );
}


#------------------------------------------------------------------------------#
# Local state
#------------------------------------------------------------------------------#

our $DBGF = undef;

sub register_dbghandle() { $DBGF = shift; }


#------------------------------------------------------------------------------#
# Generate cardinality constraint
#------------------------------------------------------------------------------#

sub gen_card_constraint() {    # Generate AtMost cardinality constraint
    my ($opts, $cardset, $bvref, $tval) = @_;
    my @auxbvs = keys %{$bvref};
    if (${$opts}{d}) {
	print $DBGF "Generating card set for @auxbvs w/ rhs $tval\n";
    }
    my $clstr = '';
    if (${$opts}{e} eq 'a') {
	my $msg =
	    'Use of Adders (-e a) for encoding cardinality constraints' .
	    ' not yet implemented';
	&exit_err("$msg\n");
    } elsif (${$opts}{e} eq 'b') {
	&CARD::bdd_set_mode(1);    # set equiv mode
	$clstr = &CARD::gen_bdd_atmostN(\@auxbvs, $tval);
    } elsif (${$opts}{e} eq 'i') {
	&CARD::bdd_set_mode(0);    # set implic mode
	$clstr = &CARD::gen_bdd_atmostN(\@auxbvs, $tval);
    } elsif (${$opts}{e} eq 'c') {
	if ($tval > 1) {
	    $clstr = &CARD::gen_sc_atmostN(\@auxbvs, $tval);
	} else {
	    $clstr = &CARD::gen_sc_atmost1(\@auxbvs);
	}
    } elsif (${$opts}{e} eq 'e') {
	if ($tval > 1) {
	    my $msg =
		'Use of exponential enconding (-e e) for cardinality' .
		' constraints w/ rhs > 1 not to be implemented';
	    &exit_err("$msg\n");
	} else {
	    $clstr = &CARD::gen_pw_atmost1(\@auxbvs);
	}
    } elsif (${$opts}{e} eq 's') {
	&CARD::srt_set_mode(1);    # default equiv mode
	$clstr = &CARD::gen_srt_atmostN(\@auxbvs, $tval);
    }
    my @newcls = split("\n", $clstr);
    for(my $i=0; $i<=$#newcls; $i++) {
	my @clits = split('\s+', $newcls[$i]);
	$newcls[$i] = &CLUTILS::clsig(\@clits);
    }
    if (${$opts}{d}) {
	my $ncls = $#newcls+1;
	print $DBGF "Num of new card cls: $ncls\n";
	$"=', '; print $DBGF "NEW CLS:\n@newcls\n"; $"=' ';
    }
    $cardset->add_cl_set(\@newcls, 'AUX');
    return 0;
}

sub gen_extra_card_constraint() {    # Generate AtMost cardinality constraint
    my ($opts, $cardset, $bvref, $tval) = @_;
    my @auxbvs = keys %{$bvref};
    if (${$opts}{d}) {
	print $DBGF "Generating card set for @auxbvs w/ rhs $tval\n";
    }
    my $clstr = '';
    if (${$opts}{e} eq 'a') {
	my $msg =
	    'Use of Adders (-e a) for encoding cardinality constraints' .
	    ' not yet implemented';
	&exit_err("$msg\n");
    } elsif (${$opts}{e} eq 'b') {
	&CARD::bdd_set_mode(1);    # set equiv mode
	$clstr = &CARD::gen_bdd_atleastN(\@auxbvs, $tval);
    } elsif (${$opts}{e} eq 'i') {
	&CARD::bdd_set_mode(0);    # set implic mode
	$clstr = &CARD::gen_bdd_atleastN(\@auxbvs, $tval);
    } elsif (${$opts}{e} eq 'c') {
	if ($tval > 1) {
	    $clstr = &CARD::gen_sc_atleastN(\@auxbvs, $tval);
	} else {
	    $clstr = &CARD::gen_sc_atleast1(\@auxbvs);
	}
    } elsif (${$opts}{e} eq 'e') {
	if ($tval > 1) {
	    my $msg =
		'Use of exponential enconding (-e e) for cardinality' .
		' constraints w/ rhs > 1 not to be implemented';
	    &exit_err("$msg\n");
	} else {
	    $clstr = &CARD::gen_pw_atleast1(\@auxbvs);
	}
    } elsif (${$opts}{e} eq 's') {
	&CARD::srt_set_mode(1);    # default equiv mode
	$clstr = &CARD::gen_srt_atleastN(\@auxbvs, $tval);
    }
    my @newcls = split("\n", $clstr);
    for(my $i=0; $i<=$#newcls; $i++) {
	my @clits = split('\s+', $newcls[$i]);
	$newcls[$i] = &CLUTILS::clsig(\@clits);
    }
    if (${$opts}{d}) {
	my $ncls = $#newcls+1;
	print $DBGF "Num of new card cls: $ncls\n";
	$"=', '; print $DBGF "NEW CLS:\n@newcls\n"; $"=' ';
    }
    $cardset->add_cl_set(\@newcls, 'AUX');
    return 0;
}


#------------------------------------------------------------------------------#
# Manipulation of cores & blocking vars
#------------------------------------------------------------------------------#

# Identify initial set of blocking vars
sub compute_blocking_vars() {    # args: hash ref, array ref, array ref
    my ($opts, $coreref, $blockvs) = @_;
    if (${$opts}{d}) {
	my $ncs = $#{$coreref} + 1;
	print $DBGF "Number of cores: $ncs\n";
    }
    foreach my $core (@{$coreref}) {
	if (${$opts}{d}) { print $DBGF "cls in core: $#{$core}\n"; }
	my $corecls = $#{$core}+1;  # number of cls in core;
	for (my $i=0; $i<$corecls; ++$i) { 
	    my $nid = &IDGEN::gen_id();
	    ${$blockvs}{$nid} = 1;
	}
    }
    if (${$opts}{d}) {
	my @bvs = keys %{$blockvs}; print $DBGF "Blocking vars: @bvs\n"; }
}

# Create initial clauses w/ blocking vars, from set of cores
sub add_blockvs_coreset() {    # args: hash ref, obj, array ref, hash ref
    my ($opts, $bclset, $coreset, $blockvs) = @_;
    my @auxbvs = keys %{$blockvs};  # get array of block vars from keys of hash
    if (${$opts}{d}) { print $DBGF "Aux block vars: @auxbvs\n"; }
    foreach my $core (@{$coreset}) {
	my $corecls = $#{$core}+1;  # number of cls in core;
	foreach my $corecl (@{$core}) {
	    my $clits = &CLUTILS::cllits($corecl);
	    if (${$opts}{d}) { print $DBGF "CL LITS:: @{$clits}\n"; }
	    my $nbv = pop @auxbvs;
	    push @{$clits}, $nbv; push @{$clits}, 0;  # terminate w/ 0
	    if (${$opts}{d}) { print $DBGF "CL LITS:: @{$clits}\n"; }
	    my $clsig = &CLUTILS::clsig($clits);
	    $bclset->add_cl($clsig, 'BLOCK');
	    if (${$opts}{d}) { print $DBGF "Adding new cl: $clsig\n"; }
	}
    }
}

# Get original clauses in core, i.e. w/o blocking vars
sub get_initial_clauses() {
    my ($opts, $clset, $coreref) = @_;  # args: hash ref, object, array ref
    my $initcls = [];
    foreach my $cl (@{$coreref}) {
	# If clause still exists in original clset
	if ($clset->lkup_cl($cl) &&
	    $clset->get_cl_status($cl) eq 'INIT' &&
	    $clset->chk_cl_soft($cl)) {
	    push @{$initcls}, $cl;
	    if (${$opts}{d}) { print $DBGF "$cl status is init\n"; }
	} elsif (${$opts}{d}) { print $DBGF "$cl status is *not* init\n"; }
    }
    return $initcls;
}

# Add blocking vars to set of original clauses
sub add_blocking_vars() {
    my ($opts, $origcls, $bclset, $blockvs) = @_;
    my $newbvs = {};
    foreach my $cl (@{$origcls}) {
	my $clits = &CLUTILS::cllits($cl);
	if (${$opts}{d}) { print $DBGF "CL LITS:: @{$clits}\n"; }
	my $nid = &IDGEN::gen_id();
	${$blockvs}{$nid} = 1;                      # register as blocking var
	push @{$clits}, $nid; push @{$clits}, 0;    # terminate w/ 0
	${$newbvs}{$nid} = 1;
	if (${$opts}{d}) { print $DBGF "CL LITS:: @{$clits}\n"; }
	my $clsig = &CLUTILS::clsig($clits);
	$bclset->add_cl($clsig, 'BLOCK');
	if (${$opts}{d}) { print $DBGF "Adding new cl: $clsig\n"; }
    }
    return $newbvs;
}

# Identify blocking vars assigned value 1
sub get_blocking_vars() {
    my ($opts, $blockvs, $vref) = @_;
    my $bvset = [];
    foreach my $vval (@{$vref}) {
	if ($vval > 0 && defined(${$blockvs}{$vval})) { push @{$bvset}, $vval; }
    }
    if (${$opts}{d}) { print $DBGF "BVSET: @{$bvset}\n"; }
    return $bvset;
}

# Update list of blocking vars associated with an initial clause
sub update_initcl_blockvs() {
    my ($opts, $blockvs, $blkcls, $initclbvs) = @_;
    foreach my $cl (@{$blkcls}) {
	my $clits = &CLUTILS::cllits($cl);
	my @initlits = ();
	foreach my $lit (@{$clits}) {
	    if (!defined(${$blockvs}{abs($lit)})) {
		push @initlits, $lit;
	    } else { last; }
	}
	my $newblit = pop @{$clits};
	push @initlits, 0;
	my $clstr = &CLUTILS::clsig(\@initlits);
	${${$initclbvs}{$clstr}}{$newblit} = 1;
    }
}


#------------------------------------------------------------------------------#
# Analysis of cores, for filtering hints
#
# Preconditions:
#  Clauses in the core must not also exist in clset
#
# Clauses states: INIT, TAG, BLOCK, AUX
#  INIT:  original cls
#  TAG:   tagged clause; if occurs in another core, then must become BLOCK
#  BLOCK: clause w/ blocked variable
#  AUX:   clause used for encoding card constraint
#
# INIT clauses can be tentatively filtered, and so become TAG. Otherwise,
# INIT clauses become BLOCK. TAG clauses that occur in another core, must
# become BLOCK. Clauses can be tentatively filtered only once.
#------------------------------------------------------------------------------#

sub tag_core_clauses() {
    my ($clset, $coreset) = @_;
    my $corelits = {};
    my $clsetlits = {};
    # Get lits/vars in core clauses
    foreach my $corecl (@{$coreset}) {
	my @clits = &CLUTILS::cllits($corecl);
	foreach my $clit (@clits) { ${$corelits}{abs($clit)} = 1; }
    }
    # Get lits/vars in resulting original formula clauses
    foreach my $stdcl (@{&clset->clauses()}) {
	my @clits = &CLUTILS::cllits($stdcl);
	foreach my $clit (@clits) { ${$clsetlits}{abs($clit)} = 1; }
    }
    #
    my $tag_cls = {};
    foreach my $corecl (@{$coreset}) {
	my @clits = &CLUTILS::cllits($corecl);

    # Remove from working core clauses w/ vars that only occur in core and
    # whose state is INIT; BLOCK or TAG clauses cannot be removed

	my $incore = 1;
	foreach my $clit (@clits) {
	    if (defined(${$clsetlits}{abs($clit)})) {
		$incore = 0; last;
	    }
	}
	if ($incore) {
	    ${$tag_cls}{$corecl} = 1;  # Clause declared TAG

	}
    }

    # State of removed clauses is set to TAG, w/ reference to card constr

    # ...

}


#------------------------------------------------------------------------------#
# Apply clause filtering: NOT WORKING
#------------------------------------------------------------------------------#

sub clfilt() {
    my ($opts, $coreref, $clset) = @_;  # args: hash ref, array ref, object
    if (${$opts}{d}) {
	my $csize = $#{$coreref};
	$" = "\n"; print $DBGF "CORE SIZE: $csize\n";
	print $DBGF "CORE: @{$coreref}\n"; $" = ' ';
    }
    my $coreset = CLSET::new();
    $coreset->add_cl_set($coreref);
    my $deflitset = $clset->lithash();
    my $corelitset = $coreset->lithash();
    if (${$opts}{d}) {
	print $DBGF Dumper($deflitset); print $DBGF Dumper($corelitset);}
    my %coreuniquelits = ();
    # Visit all lits and check which only exist in core
    foreach my $lit (keys %{$deflitset}) {
	my $justcore = 1;
	foreach my $clref (@{$deflitset->{$lit}}) {
	    if (!$coreset->lkup_cl($clref)) { $justcore = 0; last; }
	}
	if ($justcore) { $coreuniquelits{$lit} = 1; }
    }
    if (${$opts}{d}) {
	my @litfilt = keys %coreuniquelits;
	print $DBGF "FILTS THAT OCCUR ONLY IN CORE: @litfilt\n";
    }
    # Now, visit core clauses and check which ones only have core lits
    foreach my $cl (@{$coreref}) {
	if (${$opts}{d}) { print $DBGF "Checking core cl: $cl\n"; }
	my @clits = @{&CLUTILS::cllits($cl)};
	my $corecond = 1;
	foreach my $lit (@clits) {
	    if (!defined($coreuniquelits{$lit})) {
		if (${$opts}{d}) {
		    print $DBGF "For $cl, $lit is not unique...\n"; }
		$corecond = 0; last; }
	}
	if ($corecond) { 
	    if (${$opts}{d}) { print $DBGF "Extracting from core: $cl\n"; }
	    $coreset->del_cl($cl);
	}
    }
    # Now, recreate core clset
    my $ncorecls = $coreset->clauses;
    # Note: for fully disjoint cores, must pick one clause to block...
    if ($#{$ncorecls} < 0) {
	if (${$opts}{d}) {
	    print $DBGF "Empty core. Re-adding 1 cl to core...\n";
	}
	$coreset->add_cl(${$coreref}[0]);
    }
    if (${$opts}{d}) {
	my $csize = $#{$ncorecls};
	$" = "\n"; print $DBGF "NEW CORE SIZE: $csize\n";
	print $DBGF "NEW CORE: @{$ncorecls}\n"; $" = ' ';
    }
    return $ncorecls;
}


END {
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
