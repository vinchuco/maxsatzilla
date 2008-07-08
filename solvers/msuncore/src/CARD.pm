#------------------------------------------------------------------------------#
# File:        CARD.pm
#
# Description: Generation of cardinality constraints. Currently, implements
#              pairwise encoding and sequential counter encoding for AtMost1
#              constraints. For other constraints, BDD-based encoding is used.
#
# Author:      Joao Marques-Silva.
#
# Created:     28 Sep 2006.
#
# Revision:    $Id$.
#
# Notes:
#    20060928 -> First draft (w/ atmostN and atleastN); Testing.
#    20070207 -> Added special cases and equalsN; Testing.
#    20070412 -> Cleaned up API; Testing.
#    20070915 -> New unified API. Can use BDD-based encoding, and special
#                cases of sequential counter (SC) and pairwise encoding (PW).
#    20070918 -> Implemented BDD encoding with implications. Validating.
#                Working on full encoding with sequential counter.
#    20071202 -> Implemented Sorter encoding. Validating.
#
# TODO:
#    * Implement sorters w/ polarities
#    * Implement bitwise encoding
#    * Implement additional SC cases
#    * Implement network simplifications
#      + E.g. observability don't cares
#------------------------------------------------------------------------------#

package CARD;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

use IDGEN qw( &gen_id );

use UTILS;


BEGIN {
    @CARD::ISA = ('Exporter');
    @CARD::EXPORT_OK = qw( &num_clauses &gen_pw_equals1 &gen_pw_atmost1 &gen_pw_atleast1 &gen_sc_equals1 &gen_sc_atmost1 &gen_sc_atleast1 &bdd_set_mode &gen_bdd_equalsN &gen_bdd_atmostN &gen_bdd_atleastN &srt_set_mode &gen_srt_equalsN &gen_srt_atmostN &gen_srt_atleastN );
}

our $vhash = ( );        # Ref to var hash
our $clcount = 0;        # Clause count
our $nconstr = 0;        # Number of constraints generated
our $clset = '';         # Clause set string
our $bdd_mode = 1;       # Equivalence mode
our $srt_mode = 1;       # Equivalence mode
our $dbgon = 0;          # Activate debug


#------------------------------------------------------------------------------#
# Basic external API
#------------------------------------------------------------------------------#

# Access local stats

sub num_clauses() { return $clcount; }


# Pairwise encoding

sub gen_pw_equals1() { my ($vref) = @_; return &pw_gen_equals1($vref); }

sub gen_pw_atmost1() { my ($vref) = @_; return &pw_gen_atmost1($vref); }

sub gen_pw_atleast1() { my ($vref) = @_; return &pw_gen_atleast1($vref); }


# Sequential counter encoding

sub gen_sc_equals1() { my ($vref) = @_; return &sc_gen_equals1($vref); }

sub gen_sc_atmost1() { my ($vref) = @_; return &sc_gen_atmost1($vref); }

sub gen_sc_atleast1() { my ($vref) = @_; return &sc_gen_atleast1($vref); }

sub gen_sc_atmostN() {
    my ($vref, $tval) = @_;
    my $nvars = $#{$vref}+1;    # number of vars
    if ($tval >= $nvars) {
	if($dbgon) { print "no constr required\n"; } return '';
    }
    if ($tval == $nvars-1) { return &gen_leNm1($vref); }
    if ($tval == 0) { return &gen_le0($vref); }
    if ($tval == 1) { return &sc_gen_atmost1($vref); }
    # The remaining cases....

    &UTILS::vassert(0, "SC encoding not yet implemented for tval>1");

}


# BDD encoding

sub bdd_set_mode() { $bdd_mode = shift; }    # Set CNF generation mode

sub gen_bdd_equalsN() {
    my ($vref, $tval) = @_; return &bdd_gen_equalsN($vref, $tval);
}

sub gen_bdd_atmostN() {
    my ($vref, $tval) = @_;
    if ($bdd_mode == 1) {
	return &bdd_gen_atmostN($vref, $tval);
    } else {
	return &bdd_atmostN_implic($vref, $tval);
    }
}

sub gen_bdd_atleastN() {
    my ($vref, $tval) = @_; return &bdd_gen_atleastN($vref, $tval);
}


# Sorter encoding

sub srt_set_mode() { $srt_mode = shift; }    # Set CNF generation mode

sub gen_srt_equalsN() {
    my ($vref, $tval) = @_;
    &UTILS::vassert(0, "equalsN w/ sorters not yet implemented...");
}

sub gen_srt_atmostN() {
    my ($vref, $tval) = @_;
    if ($srt_mode == 1) {
	return &srt_gen_atmostN($vref, $tval);
    } else {
	&UTILS::vassert(0, "atmostN w/ sorters & implic not yet implemented...");
    }
}

sub gen_srt_atleastN() {
    my ($vref, $tval) = @_;
    if ($tval  == 1) { return &gen_ge1($vref); }
    else {
	&UTILS::vassert(0, "atleastN w/ sorters not yet implemented...");
    }
}


#------------------------------------------------------------------------------#
# BDD-based encoding
#------------------------------------------------------------------------------#

sub bdd_gen_equalsN() {
    my ($xvref, $tval) = @_;
    my $cstr = &gen_atleastN($xvref, $tval);
    $cstr .= &gen_atmostN($xvref, $tval);
    return $cstr;
}

sub bdd_gen_atleastN() {    # encode >= card constr in CNF; return as string
    my ($xvref, $tval) = @_;
    my $nvars = $#{$xvref}+1;    # number of vars
    if ($tval <= 0) { if($dbgon) { print "no constr required\n"; } return ''; }
    if ($tval == 1) { return &gen_ge1($xvref); }
    if ($tval == $nvars) { return &gen_geN($xvref); }
    $clset = ''; $nconstr++;
    # traverse levels in reverse order
    for (my $l=$nvars-1; $l>=0; --$l) {
	if($dbgon) { print "current l=$l\n"; }
	my $maxp = ($l<$tval-1) ? $l : $tval-1;
	my $minp = (0>$tval-($nvars-$l)) ? 0 : ($tval-($nvars-$l));
	if($dbgon) { print "maxp=$maxp    &&    minp=$minp\n"; }
	for (my $p=$maxp; $p>=$minp; --$p) {
	    if($dbgon) { print "current p=$p\n"; }
	    my $yv = &mk_yvar("y_$nconstr\_$l\_$p");
	    if ($p<$tval-1 && $p>$tval-($nvars-$l)) {
		if($dbgon) { print "generating ITE\n"; }
		my $lp1 = $l+1; my $pp1 = $p+1;
		my $ynxt1 = &mk_yvar("y_$nconstr\_$lp1\_$pp1");
		my $ynxt2 = &mk_yvar("y_$nconstr\_$lp1\_$p");
		&gen_ite($yv, ${$xvref}[$l], $ynxt1, $ynxt2);
	    }
	    elsif ($p<$tval-1 && $p==$tval-($nvars-$l)) {
		if($dbgon) { print "generating AND\n"; }
		my $lp1 = $l+1; my $pp1 = $p+1;
		my $ynxt = &mk_yvar("y_$nconstr\_$lp1\_$pp1");
		&gen_and($yv, ${$xvref}[$l], $ynxt);
	    }
	    elsif ($p==$tval-1 && $p>$tval-($nvars-$l)) {
		if($dbgon) { print "generating OR\n"; }
		my $lp1 = $l+1;
		my $ynxt = &mk_yvar("y_$nconstr\_$lp1\_$p");
		&gen_or($yv, ${$xvref}[$l], $ynxt);
	    }
	    elsif ($p==$tval-1 && $p==$tval-($nvars-$l)) {
		if($dbgon) { print "generating 2EQUIV\n"; }
		&gen_2equiv($yv, ${$xvref}[$l]);
	    }
	    else { print "Invalid condition in atleast\n"; exit; }
	}
    }
    my $yf = &mk_yvar("y_$nconstr\_0_0");
    $clset .= "$yf 0\n";    # require constraint to be satisfied
    $clcount++;
    return $clset;
}

sub bdd_gen_atmostN() {    # encode <= card constr in CNF; return as string
    my ($xvref, $tval) = @_;
    my $nvars = $#{$xvref}+1;    # number of vars
    if ($tval >= $nvars) { if($dbgon) { print "no constr required\n"; } return ''; }
    if ($tval == $nvars-1) { return &gen_leNm1($xvref); }
    if ($tval == 0) { return &gen_le0($xvref); }
    $clset = ''; $nconstr++;
    # traverse levels in reverse order
    for (my $l=$nvars-1; $l>=0; --$l) {
	if($dbgon) { print "current l=$l\n"; }
	my $maxp = ($l<$tval) ? $l : $tval;
	my $minp = (0>$tval-($nvars-$l)+1) ? 0 : ($tval-($nvars-$l)+1);
	if($dbgon) { print "maxp=$maxp    &&    minp=$minp\n"; }
	for (my $p=$maxp; $p>=$minp; --$p) {
	    if($dbgon) { print "current p=$p\n"; }
	    my $yv = &mk_yvar("y_$nconstr\_$l\_$p");
	    if ($p<$tval && $p>$tval-($nvars-$l)+1) {
		if($dbgon) { print "generating ITE\n"; }
		my $lp1 = $l+1; my $pp1 = $p+1;
		my $ynxt1 = &mk_yvar("y_$nconstr\_$lp1\_$pp1");
		my $ynxt2 = &mk_yvar("y_$nconstr\_$lp1\_$p");
		&gen_ite($yv, ${$xvref}[$l], $ynxt1, $ynxt2);
	    }
	    elsif ($p<$tval && $p==$tval-($nvars-$l)+1) {
		if($dbgon) { print "generating OR\n"; }
		my $lp1 = $l+1; my $pp1 = $p+1;
		my $ynxt = &mk_yvar("y_$nconstr\_$lp1\_$pp1");
		&gen_or($yv, -${$xvref}[$l], $ynxt);
	    }
	    elsif ($p==$tval && $p>$tval-($nvars-$l)+1) {
		if($dbgon) { print "generating AND\n"; }
		my $lp1 = $l+1;
		my $ynxt = &mk_yvar("y_$nconstr\_$lp1\_$p");
		&gen_and($yv, -${$xvref}[$l], $ynxt);
	    }
	    elsif ($p==$tval && $p==$tval-($nvars-$l)+1) {
		if($dbgon) { print "generating 2EQUIV\n"; }
		&gen_2equiv($yv, -${$xvref}[$l]);
	    }
	    else { print "Invalid condition in atleast\n"; exit; }
	}
    }
    my $yf = &mk_yvar("y_$nconstr\_0_0");
    $clset .= "$yf 0\n";    # require constraint to be satisfied
    $clcount++;
    return $clset;
}


sub bdd_atmostN_implic() {    # encode <= card constr in CNF; return as string
    my ($xvref, $tval) = @_;
    my $nvars = $#{$xvref}+1;    # number of vars
    if ($tval >= $nvars) { if($dbgon) { print "no constr required\n"; } return ''; }
    if ($tval == $nvars-1) { return &gen_leNm1($xvref); }
    if ($tval == 0) { return &gen_le0($xvref); }
    $clset = ''; $nconstr++;
    # traverse levels in reverse order
    for (my $l=$nvars-1; $l>=0; --$l) {
	if($dbgon) { print "current l=$l\n"; }
	my $maxp = ($l<$tval) ? $l : $tval;
	my $minp = (0>$tval-($nvars-$l)+1) ? 0 : ($tval-($nvars-$l)+1);
	if($dbgon) { print "maxp=$maxp    &&    minp=$minp\n"; }
	for (my $p=$maxp; $p>=$minp; --$p) {
	    if($dbgon) { print "current p=$p\n"; }
	    my $yv = &mk_yvar("y_$nconstr\_$l\_$p");
	    if ($p<$tval && $p>$tval-($nvars-$l)+1) {
		if($dbgon) { print "generating ITE\n"; }
		my $lp1 = $l+1; my $pp1 = $p+1;
		my $ynxt1 = &mk_yvar("y_$nconstr\_$lp1\_$pp1");
		my $ynxt2 = &mk_yvar("y_$nconstr\_$lp1\_$p");
		&gen_implic_pos_ite($yv, ${$xvref}[$l], $ynxt1, $ynxt2);
	    }
	    elsif ($p<$tval && $p==$tval-($nvars-$l)+1) {
		if($dbgon) { print "generating OR\n"; }
		my $lp1 = $l+1; my $pp1 = $p+1;
		my $ynxt = &mk_yvar("y_$nconstr\_$lp1\_$pp1");
		&gen_implic_pos_or($yv, -${$xvref}[$l], $ynxt);
	    }
	    elsif ($p==$tval && $p>$tval-($nvars-$l)+1) {
		if($dbgon) { print "generating AND\n"; }
		my $lp1 = $l+1;
		my $ynxt = &mk_yvar("y_$nconstr\_$lp1\_$p");
		&gen_implic_pos_and($yv, -${$xvref}[$l], $ynxt);
	    }
	    elsif ($p==$tval && $p==$tval-($nvars-$l)+1) {
		if($dbgon) { print "generating 2EQUIV\n"; }
		&gen_2equiv($yv, -${$xvref}[$l]);
	    }
	    else { print "Invalid condition in atleast\n"; exit; }
	}
    }
    my $yf = &mk_yvar("y_$nconstr\_0_0");
    $clset .= "$yf 0\n";    # require constraint to be satisfied
    $clcount++;
    return $clset;
}


#------------------------------------------------------------------------------#
# Sequential counter encodings, case k=1, from Sinz CP05 paper
#------------------------------------------------------------------------------#

sub sc_gen_equals1() {    # Based on AtMost1 from Sinz CP05 paper (3n-4 clauses)
  my ($vref) = @_;
  $clset = '';
  $clset .= &sc_gen_atmost1($vref);
  $clset .= &sc_gen_atleast1($vref);
  return $clset;
}

sub sc_gen_atmost1() {    # Version from Sinz CP05 paper (3n-4 clauses)
  my ($vref) = @_;
  $clset = '';
  my $n = $#{$vref}; # note: only $n aux vars (i.e. n-1 aux vars)
  if ($n == 0) { return ''; }  # no need to create card constraints
  #print "N: $n\n";
  my $cid = &IDGEN::gen_id();
  $clset .= "-${$vref}[0] $cid 0\n"; $clcount++;
  for (my $j=1; $j<$n; $j++) {
      $clset .= "-${$vref}[$j] -$cid 0\n"; $clcount++;
      my $nid = &IDGEN::gen_id();
      $clset .= "-${$vref}[$j] $nid 0\n"; $clcount++;
      $clset .= "-$cid $nid 0\n"; $clcount++;
      $cid = $nid;
  }
  $clset .= "-${$vref}[$n] -$cid 0\n"; $clcount++;
  return $clset;
}

sub sc_gen_atleast1() {
    my ($vref) = @_;
    return &gen_ge1($vref);    # No difference in this case
}


#------------------------------------------------------------------------------#
# Pairwise encoding, case k=1
#------------------------------------------------------------------------------#

sub pw_gen_equals1() {
    my ($vref) = @_;
    my $rstr = '';
    $rstr .= &pw_gen_atmost1($vref);
    $rstr .= &pw_gen_atleast1($vref);
    return $rstr;
}

sub pw_gen_atmost1() {
    my ($vref) = @_;
    my $rstr = '';
    for (my $i1=0; $i1<=$#{$vref}; ++$i1) {
	for (my $i2=$i1+1; $i2<=$#{$vref}; ++$i2) {
	    $rstr .= "-${$vref}[$i1] -${$vref}[$i2] 0\n"; ++$clcount;
	}
    }
    return $rstr;
}

sub pw_gen_atleast1() {
    my ($vref) = @_;
    return &gen_ge1($vref);    # No difference in this case
}


#------------------------------------------------------------------------------#
# Sorter-based encoding (odd-even merging)
#------------------------------------------------------------------------------#

sub srt_gen_atmostN() {    # encode <= card constr in CNF; return as string
    my ($xvref, $tval) = @_;
    my $nvars = $#{$xvref}+1;    # number of vars

    if ($tval >= $nvars) {
	if($dbgon) { print "no constr required\n"; } return ''; }
    if ($tval == $nvars-1) { return &gen_leNm1($xvref); }
    if ($tval == 0) { return &gen_le0($xvref); }

    my @auxvs = ();
    my $nvars = $#{$xvref} + 1;
    my $radix = int(log($nvars)/log(2)) + 1;
    my $nnvars = int(exp(log(2)*$radix)+0.1);

    #my $xnid = &IDGEN::num_id();
    #print "Number of ids: $xnid w/ nvars:$nvars AND nnvars:$nnvars\n";

    my $zvars = $nnvars - $nvars;
    for(my $i=$nvars+1; $i<=$nnvars; ++$i)  {
	push @auxvs, &IDGEN::gen_id();
    }
    my @newvref = ();
    push @newvref, @{$xvref};
    push @newvref, @auxvs;
    my $tmpclset = '';
    foreach my $vid (@auxvs) { $tmpclset .= "-$vid 0\n"; }

    #my $xnid = &IDGEN::num_id();
    #print "Number of ids: $xnid\n";

    $clset = '';
    # 1. Generate odd-even sorting network
    my $ovref = &recur_srt_gen(\@newvref);
    # 2. Force constraints on outputs
    if ($dbgon) {
	my $ninps = $#{$xvref}+1; my $nzeros = $#{$ovref}-$tval+1;
	print "Number of inputs: $ninps   RHS: $tval\n";
	print "Number of 0-value outputs: $nzeros\n"; }

    for (my $i=0; $i<$#{$ovref}+1-$tval; ++$i) {  # -> #0 = N-RHS
	$clset .= "-${$ovref}[$i] 0\n"; $nconstr++;    # Outputs must be 0
    }
    $clset .= $tmpclset;
    return $clset;
}

sub recur_srt_gen() {
    my $xvref = shift @_;
    my $ovref = [];
    my $nn = $#{$xvref}+1;
    if ($nn == 1) {
	${$ovref}[0] = ${$xvref}[0];
    }
    elsif ($nn == 2) {
	if ($dbgon) {
	    print "Generating SRT comparator w/ inputs ${$xvref}[0] ${$xvref}[1]\n";
	}
	${$ovref}[0] = &IDGEN::gen_id();
	${$ovref}[1] = &IDGEN::gen_id();
	&gen_comparator(${$ovref}[0], ${$ovref}[1], ${$xvref}[0], ${$xvref}[1]);
    }
    else {
	if ($dbgon) { print "Number of elems to sort: $nn\n"; }
	my $nmid = int($nn/2);
	my @nxvecA = @{$xvref}[0 .. $nmid-1];
	my @nxvecB = @{$xvref}[$nmid .. $nn-1];
	if ($dbgon) {
	    my $nl = $nmid; print "Left elems: $nl --> |@nxvecA|\n"; }
	my $ovrefA = &recur_srt_gen(\@nxvecA);
	if ($dbgon) {
	    my $nr = $nn-1-$nmid+1; print "Right elems: $nr --> |@nxvecB|\n"; }
	my $ovrefB = &recur_srt_gen(\@nxvecB);
	$ovref = &srt_recur_odd_even_merge($ovrefA, $ovrefB);
    }
    return $ovref;
}

sub srt_recur_odd_even_merge() {
    my ($xvrefA, $xvrefB) = @_; # Both A and B *must* already be sorted
    my $ovref = [];
    my $nnA = $#{$xvrefA}+1; my $nnB = $#{$xvrefB}+1;

    if ($nnA == 1 && $nnB < 1)    { ${$ovref}[0] = ${$xvrefA}[0]; }
    elsif ($nnA < 1 && $nnB == 1) { ${$ovref}[0] = ${$xvrefB}[0]; }
    elsif ($nnA == 1 && $nnB == 1) {
	if ($dbgon) {
	    print "Generating MRG comparator w/ inputs ${$xvrefA}[0] ${$xvrefB}[0]\n"; }
	${$ovref}[0] = &IDGEN::gen_id();
	${$ovref}[1] = &IDGEN::gen_id();
	&gen_comparator(${$ovref}[0], ${$ovref}[1],
			${$xvrefA}[0], ${$xvrefB}[0]);
    }
    else {
	if ($dbgon) {
	    print "Merging: |@{$xvrefA}| with |@{$xvrefB}|\n"; }
	# 1. Generate arrays of odd's and even's
	my $minn = ($nnA < $nnB) ? $nnA : $nnB;
	my $odd_xvA = []; my $odd_xvB = [];
	my $even_xvA = []; my $even_xvB = [];
	for (my $ridx=0, my $sidx=0; $ridx<$minn; $ridx+=2, $sidx++) {
	    ${$odd_xvA}[$sidx] = ${$xvrefA}[$ridx];
	    ${$odd_xvB}[$sidx] = ${$xvrefB}[$ridx];
	}
	for (my $ridx=1, my $sidx=0; $ridx<$minn; $ridx+=2, $sidx++) {
	    ${$even_xvA}[$sidx] = ${$xvrefA}[$ridx];
	    ${$even_xvB}[$sidx] = ${$xvrefB}[$ridx];
	}
	if ($nnA > $nnB)   {
	    ${$even_xvA}[$#{$even_xvA}+1] = ${$xvrefA}[$nnA-1];
	} elsif($nnA < $nnB) {
	    ${$even_xvA}[$#{$even_xvA}+1] = ${$xvrefB}[$nnB-1];
	}
	if ($dbgon) {
	    print "Going to ODD merge: |@{$odd_xvA}| and |@{$odd_xvB}|\n"; }
	my $ovrefA = &srt_recur_odd_even_merge($odd_xvA, $odd_xvB);
	if ($dbgon) {
	    print "Going to EVEN merge: |@{$even_xvA}| and |@{$even_xvB}|\n"; }
	my $ovrefB = &srt_recur_odd_even_merge($even_xvA, $even_xvB);
	if ($dbgon) {
	    print "Before final comparison state: |@{$ovrefA}| and |@{$ovrefB}|\n"; }
	# 3. Final comparison stage
	my $oidx = 0;
	if ($#{$ovrefA} >= $#{$ovrefB}) {
	    ${$ovref}[0] = shift @{$ovrefA}; $oidx++;
	}
	my $nncmp = &UTILS::round(($nnA+$nnB)/2) - 1;
	while (@{$ovrefA} && @{$ovrefB}) {
	    #for (my $idx=0; $idx<$nncmp; ++$idx) {
	    my $in1 = shift @{$ovrefA};
	    my $in2 = shift @{$ovrefB};
	    my $o1 = &IDGEN::gen_id();
	    my $o2 = &IDGEN::gen_id();
	    if ($dbgon) {
		print "Generating ALT comparator w/ inputs $in1 $in2 and OUTputs $o1 $o2\n"; }
	    &gen_comparator($o1, $o2, $in1, $in2);
	    ${$ovref}[$oidx++] = $o1;
	    ${$ovref}[$oidx++] = $o2;
	}
	if (@{$ovrefA}) { ${$ovref}[$oidx++] = shift @{$ovrefA}; }
	if (@{$ovrefB}) { ${$ovref}[$oidx++] = shift @{$ovrefB}; }
	if (@{$ovrefA} || @{$ovrefB}) {
	    print "Both lists of sorted items should be empty\n"; exit(10); }
	if ($dbgon) {
	    print "Done merging w/ output |@{$ovref}|...\n"; }
    }
    return $ovref;
}


#------------------------------------------------------------------------------#
# Linear encoding used in SHIPs, case k=1
#------------------------------------------------------------------------------#

sub gen_ild_equals1() {# Updated version from AAAI06&SAT06 papers (4n+2 clauses)
    my ($vref) = @_;     # ild: inverse ladder
    my $clset = '';
    my $n = $#{$vref}+1; # note: n+1 aux vars
    #print "N: $n\n";
    my $cid = &IDGEN::gen_id();
    $clset .= "-$cid 0\n"; $clcount++;
    for (my $j=0; $j<$n; $j++) {
	$clset .= "-$cid -${$vref}[$j] 0\n"; $clcount++;
	my $nid = &IDGEN::gen_id();
	$clset .= "-${$vref}[$j] $nid 0\n"; $clcount++;
	$clset .= "-$cid $nid 0\n"; $clcount++;
	$clset .= "${$vref}[$j] $cid -$nid 0\n"; $clcount++;
	$cid = $nid;
    }
    $clset .= "$cid 0\n"; $clcount++;
    return $clset;
}


#------------------------------------------------------------------------------#
# Standard encoding of special cases
#------------------------------------------------------------------------------#

sub gen_ge1() {
    my ($xvref) = @_;
    my $nvars = $#{$xvref}+1;    # number of vars
    if($dbgon) { print "generating single clause for >= 1 case\n"; }
    my $clset = '';
    for (my $l=0; $l<$nvars; ++$l) { $clset .= "${$xvref}[$l] "; }
    $clset .= "0\n";
    $clcount++;
    return $clset;
}

sub gen_geN() {
    my ($xvref) = @_;
    my $nvars = $#{$xvref}+1;    # number of vars
    if($dbgon) { print "generating multiple unit clauses for >= N case\n"; }
    my $clset = '';
    for (my $l=0; $l<$nvars; ++$l) { $clset .= "${$xvref}[$l] 0\n";
				     $clcount++; }
    return $clset;
}

sub gen_leNm1() {
    my ($xvref) = @_;
    my $nvars = $#{$xvref}+1;    # number of vars
    if($dbgon) { print "generating single clause for <= N-1 case\n"; }
    my $clset = '';
    for (my $l=0; $l<$nvars; ++$l) { $clset .= "-${$xvref}[$l] "; }
    $clset .= "0\n";
    $clcount++;
    return $clset;
}

sub gen_le0() {
    my ($xvref) = @_;
    my $nvars = $#{$xvref}+1;    # number of vars
    if($dbgon) { print "generating multiple unit clauses for <= 0 case\n"; }
    my $clset = '';
    for (my $l=0; $l<$nvars; ++$l) { $clset .= "-${$xvref}[$l] 0\n"; 
				     $clcount++; }
    return $clset;
}


#------------------------------------------------------------------------------#
# Auxiliary functions
#------------------------------------------------------------------------------#

sub mk_yvar() {   # check existence of var; create if not in hash
    my ($vname) = @_;
    #print "VNAME: $vname\n";
    if (!defined(${$vhash}{$vname})) {
	${$vhash}{$vname} = &IDGEN::gen_id();
	if($dbgon) { print "new var id: ${$vhash}{$vname}\n"; }
    }
    return ${$vhash}{$vname};
}


sub gen_ite() {    # generate CNF for ITE component
    my ($ov, $sv, $x1, $x0) = @_;
    my $nov = -1*$ov; my $nsv = -1*$sv; my $nx1 = -1*$x1; my $nx0 = -1*$x0;
    $clset .= "$nsv $x1 $nov 0\n"; $clcount++;
    $clset .= "$nsv $nx1 $ov 0\n"; $clcount++;
    $clset .= "$sv $x0 $nov 0\n"; $clcount++;
    $clset .= "$sv $nx0 $ov 0\n"; $clcount++;
}

sub gen_and() {    # generate CNF for AND component
    my ($ov, $x1, $x0) = @_;
    my $nov = -1*$ov; my $nx1 = -1*$x1; my $nx0 = -1*$x0;
    $clset .= "$x1 $nov 0\n"; $clcount++;
    $clset .= "$x0 $nov 0\n"; $clcount++;
    $clset .= "$nx1 $nx0 $ov 0\n"; $clcount++;
}

sub gen_or() {    # generate CNF for OR component
    my ($ov, $x1, $x0) = @_;
    my $nov = -1*$ov; my $nx1 = -1*$x1; my $nx0 = -1*$x0;
    $clset .= "$nx1 $ov 0\n"; $clcount++;
    $clset .= "$nx0 $ov 0\n"; $clcount++;
    $clset .= "$x1 $x0 $nov 0\n"; $clcount++;
}

sub gen_2equiv() {    # generate CNF for equiv vars
    my ($ov, $iv) = @_;
    my $nov = -1*$ov; my $niv = -1*$iv;
    $clset .= "$niv $ov 0\n"; $clcount++;
    $clset .= "$iv $nov 0\n"; $clcount++;
}

sub gen_comparator() {    # generate CNF for comparator
    my ($omn, $omx, $x1, $x2) = @_;
    &gen_and($omn, $x1, $x2);
    &gen_or($omx, $x1, $x2);
}


# Implicational mode
sub gen_implic_pos_ite() {    # generate CNF for ITE (positive polarity)
    my ($ov, $sv, $x1, $x0) = @_;
    my $nov = -1*$ov; my $nsv = -1*$sv; my $nx1 = -1*$x1; my $nx0 = -1*$x0;
    $clset .= "$nsv $x1 $nov 0\n"; $clcount++;
    $clset .= "$sv $x0 $nov 0\n"; $clcount++;
}

sub gen_implic_pos_and() {    # generate CNF for AND (positive polarity)
    my ($ov, $x1, $x0) = @_;
    my $nov = -1*$ov;
    $clset .= "$x1 $nov 0\n"; $clcount++;
    $clset .= "$x0 $nov 0\n"; $clcount++;
}

sub gen_implic_pos_or() {    # generate CNF for OR (positive polarity)
    my ($ov, $x1, $x0) = @_;
    my $nov = -1*$ov;
    $clset .= "$x1 $x0 $nov 0\n"; $clcount++;
}

sub gen_implic_pos_2equiv() {    # generate CNF for equiv vars
    my ($ov, $iv) = @_;
    my $nov = -1*$ov; my $niv = -1*$iv;
    $clset .= "$niv $ov 0\n"; $clcount++;
    $clset .= "$iv $nov 0\n"; $clcount++;
}


#------------------------------------------------------------------------------#
# Tests and use cases
#------------------------------------------------------------------------------#

sub pckg_tests() {
    print "\n\n";
    &test_gates;
    print "\n\n";
    &test_constr();
    print "\n\n";
}

sub test_constr() {
    my @vars = ();
    my $varref;
    my $varcnt = 10;
    my $clscnt = 100;
    &register_vars(\$varref, \$varcnt, \$clscnt);
    print "x50 + x52 + x54 + x56 >= 2\n";
    my $cstr = &gen_bdd_atleastN([ 50, 52, 54, 56 ], 2 );
    print "CONSTR:\n$cstr";
    print "\n\n";
    print "x80 + x82 + x84 + x86 <= 3\n";
    $cstr = &gen_bdd_atmostN([ 80, 82, 84, 86 ], 3 );
    print "CONSTR:\n$cstr";
    print "\n\n";
    print "x30 + x32 + x34 + x36 + x38 <= 3\n";
    $cstr = &gen_bdd_atmostN([ 30, 32, 34, 36, 38 ], 3 );
    print "CONSTR:\n$cstr";
    print "\n\n";
    print "x40 + x42 + x44 + x46 + x48 >= 3\n";
    $cstr = &gen_bdd_atleastN([ 40, 42, 44, 46, 48 ], 3 );
    print "CONSTR:\n$cstr";
    print "\n\n";
    print "x60 + x62 + x64 + x66 >= 1\n";
    $cstr = &gen_bdd_atleastN([ 60, 62, 64, 66 ], 1 );
    print "CONSTR:\n$cstr";

    print "\n\n";
    print "x60 + x62 + x64 + x66 <= 3\n";
    $cstr = &gen_bdd_atmostN([ 60, 62, 64, 66 ], 3 );
    print "CONSTR:\n$cstr";

    print "\n\n";
    print "x60 + x62 + x64 + x66 >= 4\n";
    $cstr = &gen_bdd_atleastN([ 60, 62, 64, 66 ], 4 );
    print "CONSTR:\n$cstr";

    print "\n\n";
    print "x60 + x62 + x64 + x66 <= 0\n";
    $cstr = &gen_bdd_atmostN([ 60, 62, 64, 66 ], 0 );
    print "CONSTR:\n$cstr";

    print "\n\n";
    print "x60 + x62 + x64 + x66 >= 0\n";
    $cstr = &gen_bdd_atleastN([ 60, 62, 64, 66 ], 0 );
    print "CONSTR:\n$cstr";

    print "\n\n";
    print "x60 + x62 + x64 + x66 <= 4\n";
    $cstr = &gen_bdd_atmostN([ 60, 62, 64, 66 ], 4 );
    print "CONSTR:\n$cstr";

}

sub test_gates() {    # test functions
    $clset = '';   # using package variable
    &gen_ite(100, 50, 21, 22);
    &gen_and(100, 21, 22);
    &gen_or(100, 21, 22);
    &gen_ite(100, 50, -21, 22);
    &gen_and(100, 21, -22);
    &gen_or(100, -21, -22);
    print "$clset";
}


END {
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
