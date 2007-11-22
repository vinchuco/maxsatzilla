#!/usr/bin/env python

import os, sys, re

RunTimes = 10
numNodes = 0

for i in range( RunTimes ):
   fp = os.popen("./knuth.maxsatz+fl " + sys.argv[1] )
   for line in fp:
      m = re.search( 'NB_MONO= (\d+), NB_UNIT= (\d+), NB_BRANCHE= (\d+), NB_BACK= (\d+)', line)
      if ( m ):
         numNodes += int( m.group(3) )

print 'Average number of nodes per branch ' + str(numNodes/RunTimes)
