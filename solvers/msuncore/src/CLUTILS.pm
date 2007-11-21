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
    @CLUTILS::EXPORT_OK = qw( &cllits &clsig );
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


END {
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
