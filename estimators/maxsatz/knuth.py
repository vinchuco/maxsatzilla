#!/usr/bin/env python

import os, sys, re

def deviation( mean, list ):
   dv = 0.0
   for e in list:
      print 'Comparing ' + str(mean) + ' ' + str(e)
      dv += (mean-e)*(mean-e)
   return dv

Iterations = 10
numNodes = 0
times = []

for i in range( Iterations ):
   fp = os.popen("./knuth.maxsatz+fl " + sys.argv[1] )
   for line in fp:
      m = re.search( 'NB_MONO= (\d+), NB_UNIT= (\d+), NB_BRANCHE= (\d+), NB_BACK= (\d+)', line)
      if ( m ):
         times.append( int( m.group(3) ) )
         numNodes += int( m.group(3) )

print 'Average number of nodes per branch ' + str(numNodes/Iterations)
print 'Deviation ' + str( deviation( numNodes/Iterations, times ) )
