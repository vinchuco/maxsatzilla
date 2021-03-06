#!/usr/bin/env python -O

import os, sys, re
import stats

if len( sys.argv ) < 2:
   print 'usage: ' + sys.argv[0] + ' <file-name>'
   sys.exit(-1)

def print_data( list ):
   statistics = stats.Stats( list )
   print 'Average ' + str( statistics.avg() )
   print 'Median ' + str( statistics.median() )
   print 'Deviation ' + str( statistics.stdev() )
   print 'Percentile 10: ' + str( statistics.percentile( 10 ) )
   print 'Percentile 90: ' + str( statistics.percentile( 90 ) )

Iterations = 20
solutions = []
monos = []
units = []
branches = []
backtracks = []

for i in range( Iterations ):
   #print "Iteration " + str( i )
   fp = os.popen("./maxsatz " + sys.argv[1] + " -o")
   for line in fp:
      m = re.search( 'c NB_MONO= (\d+), NB_UNIT= (\d+), NB_BRANCHE= (\d+), NB_BACK= (\d+)', line)
      if ( m ):
         monos.append( int(m.group(1) ) )
         units.append( int( m.group(2) ) )
         branches.append( int( m.group(3) ) )
         backtracks.append( int ( m.group(4) ) )
      m = re.search( 'c Best Solution=(\d+)', line)
      if ( m ):
         solutions.append( int(m.group(1) ) )

print 'number of solutions'
print_data( solutions )
print 'number of monos'
print_data( monos )
print 'number of unit'
print_data( units )
print 'number of branches'
print_data( branches )
print 'number of backtracks'
print_data( backtracks )
