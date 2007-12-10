#!/usr/bin/env python

import sys

max_num_lits = 0
max_line_length = 0

for line in open( sys.argv[1] ):
    if line[0] == 'c' or line[0] == 'p':
        continue
    if ( len( line.split() ) > max_num_lits ):
        max_num_lits = len( line.split() )
    if ( len( line ) > max_line_length ):
        max_line_length = len( line )

print 'Max number of literals ' + str( max_num_lits )
print 'Max line ' + str( max_line_length )

