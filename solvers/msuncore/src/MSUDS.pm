#------------------------------------------------------------------------------#
# File:        MSUDS.pm
#
# Description: Data structures for MaxSAT algorithms
#
# Author:      Joao Marques-Silva.
#
# Created:     13 Sep 2007.
#
# Revision:    $Id$.
#------------------------------------------------------------------------------#

package MSUDS;

use strict;
#use warnings;

use Data::Dumper;

use POSIX;

require UTILS;
import UTILS;

require CLSET;
import CLSET;


#------------------------------------------------------------------------------#
# Object constructor
#------------------------------------------------------------------------------#

sub new {
    shift; # skip module name
    my $self  = {};
    #
    my $progname = &UTILS::get_progname();
    $self->{PROGNAME} = $progname;
    $self->{STRPREF} = "___$progname\_";
    $self->{CNFFILE} = $self->{STRPREF} . '__xyzCNFzyx__';
    $self->{OUTFILE} = $self->{STRPREF} . '__xyzOUTzyx__';
    $self->{TMPFILE} = $self->{STRPREF} . '__xyzTMPzyx__';
    $self->{DBGFILE} = $self->{STRPREF} . '__xyzDBGzyx__';
    $self->{COREFILE} = 'mscore.log';
    $self->{LOGFILE} = "$progname\.log";
    $self->{INPFILE} = '';
    $self->{TIMEOUT} = 0;
    $self->{CLSET} = CLSET::new();    # default clause set
    $self->{CLMSET} = CLMSET::new();  # default clause multiset
    $self->{CLMSET}->add_clset($self->{CLSET}); # includes default clset
    $self->{BLOCKVS} = {};            # anonymous hash for blocking vars
    $self->{CORESET} = [];            # anonymous array w/ set of unsat cores
    $self->{CLBVS} = {};              # block vars per original cl
    $self->{MSUVERSION} = undef;      # version of msuncore algorithm being run
    $self->{OPTS} = {};
    $self->{ERROR} = 0;
    $self->{ERROR_MSG}   = "";
    $self->{DBGF} = undef;            # DBG file handle (shared)
    #
    bless($self);
    return $self;
}


#------------------------------------------------------------------------------#
# Access methods
#------------------------------------------------------------------------------#

sub version() { my $self = shift; return $self->{MSUVERSION}; }

sub progname() { my $self = shift; return $self->{PROGNAME}; }

sub cnffile() { my $self = shift; return $self->{CNFFILE}; }

sub outfile() { my $self = shift; return $self->{OUTFILE}; }

sub xtmpfile() { my $self = shift; return $self->{TMPFILE}; }

sub dbgfile() { my $self = shift; return $self->{DBGFILE}; }

sub corefile() { my $self = shift; return $self->{COREFILE}; }

sub logfile() { my $self = shift; return $self->{LOGFILE}; }

sub inpfile() { my $self = shift; return $self->{INPFILE}; }

sub timeout() { my $self = shift; return $self->{TIMEOUT}; }

sub clset() { my $self = shift; return $self->{CLSET}; }

sub clmset() { my $self = shift; return $self->{CLMSET}; }

sub blockvs() { my $self = shift; return $self->{BLOCKVS}; }

sub coreset() { my $self = shift; return $self->{CORESET}; }

sub initclbvs() { my $self = shift; return $self->{CLBVS}; }

sub opts() { my $self = shift; return $self->{OPTS}; }

sub error() { my $self = shift; return $self->{ERROR}; }

sub error_msg() { my $self = shift; return $self->{ERROR_MSG}; }

sub dbghandle() { my $self = shift; return $self->{DBGF}; }


#------------------------------------------------------------------------------#
# Modifier methods
#------------------------------------------------------------------------------#

sub set_version() { my $self = shift; $self->{MSUVERSION} = shift; }

sub set_inpfile() { my $self = shift; $self->{INPFILE} = shift; }

sub set_timeout() { my $self = shift; $self->{TIMEOUT} = shift; }

sub set_dbghandle() { my $self = shift; $self->{DBGF} = shift; }


1;  # so the require or use succeeds

#------------------------------------------------------------------------------#
