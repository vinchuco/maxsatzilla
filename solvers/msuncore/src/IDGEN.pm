#------------------------------------------------------------------------------#
# File:        IDGEN.pm
#
# Description: Generation and management of integer IDs (for vars and lits)
#
# Author:      Joao Marques-Silva.
#
# Created:     26 Sep 2006.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package IDGEN;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

BEGIN {
    @IDGEN::ISA = ('Exporter');
    @IDGEN::EXPORT_OK = qw( &num_id &gen_id &set_id );
    $IDGEN::idref = 0;
}


#------------------------------------------------------------------------------#
# ID generation and manipulation
#------------------------------------------------------------------------------#

sub num_id() {
    return $IDGEN::idref;
}

sub gen_id() {
    return ++$IDGEN::idref;
}

sub set_id() {
    $IDGEN::idref = shift;
}


END {
}

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
