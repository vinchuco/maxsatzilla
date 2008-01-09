#!/usr/bin/env python

import sys, os, getopt, glob, os.path

#opts, args = getopt.getopt( sys.argv, 'p:' )

solver = sys.argv[1]
set_list = sys.argv[2]
percentatge = 50 #int( opts[0][1] )
timeout = '1000'
memout = '2000000'

sys_limits = 'ulimit -t ' + timeout + '; ulimit -m ' + memout + '; ulimit -d ' + memout + '; ulimit -v ' + memout + ';'

for set in open( set_list ):
    if set[0] == '#':
        continue
    name, path, number = set.split()
    counter = 0
    times = int( number ) * percentatge / 100
    print 'Running ' + solver + ' with ' + name + ' times ' + str( times )
    outfile = os.path.basename( solver ) + '.' + name + '.out'
    os.system( 'touch ' + outfile )
    for file in glob.glob( path ):
        if counter < times:
            counter += 1
            print solver + ' with ' + file 
            os.system( sys_limits + 'time -p ' + solver + ' ' + file + '&> ' + outfile +'2>&1')
