#!/usr/bin/perl -w
##-d:DProf

#------------------------------------------------------------------------------#
# File:        mm.pl
#
# Description: 
#
# Author:      Joao Marques-Silva.
#
# Created:     15 Oct 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package x1;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

sub prt() {
    print "In x1:prt()\n";
}

END {
}


package x2;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

sub prt() {
    print "In x2:prt()\n";
}

END {
}

x1::prt();
x2::prt();

1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
