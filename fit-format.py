#!/usr/bin/env python

import sys, os, tempfile

if len( sys.argv ) < 2:
    print "usage : " + os.path.basename( sys.argv[0] ) + " <file>.log"
    sys.exit(0)

newlogfile = open( sys.argv[1] + '.new', 'w' )
for set in open( sys.argv[1] ):
    name, time = set.split()
    print >> newlogfile, name + ' xxx ' + time
os.rename( newlogfile.name, sys.argv[1] )
