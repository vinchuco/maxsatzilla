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

for i in range( Iterations ):
   #print "Iteration " + str( i )
   fp = os.popen("./minimaxsat " + sys.argv[1] + " -one-branch -H=4" )
   for line in fp:
      m = re.search( 'o (\d+)', line)
      if ( m ):
         solutions.append( int(m.group(1) ) )

print 'number of solutions'
print_data( solutions )

