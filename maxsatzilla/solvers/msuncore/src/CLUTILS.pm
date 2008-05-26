#------------------------------------------------------------------------------#
# File:        CLUTILS.pm
#
# Description: Clause handling utilities
#
# Author:      Joao Marques-Silva.
#
# Created:     09 Sep 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package CLUTILS;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

BEGIN {
    @CLUTILS::ISA = ('Exporter');
    @CLUTILS::EXPORT_OK = qw( &cllits &clsig &clclean );
}


#------------------------------------------------------------------------------#
# Manipulation of clauses' literals
#------------------------------------------------------------------------------#

sub cllits {  # Input must be string of lits
    my @lits = split (/\s+/, shift);
    return \@lits;
}

sub clsig() {  # Input must be array terminated with 0 marker
    my $clref = shift;
    my $llit = pop @{$clref};  # rm end of cl marker
    my @nclits = sort { abs($a) <=> abs($b) } @{$clref};
    return "@nclits";
}

sub clclean() {  # Remove duplicate literals
    my $clref = shift;
    my $llit = pop @{$clref};  # rm end of cl marker
    my @nclits = sort { abs($a) <=> abs($b) } @{$clref};
    # Now, remove duplicate literals...
    my @rclits = ();
    my $olit = 0;
    for my $lit (@nclits) {    # Remove duplicates...
	if ($lit != $olit) { push @rclits, $lit; $olit = $lit; }
    }
    push @rclits, 0;  # add end of cl marker
    return \@rclits
}


END {
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
