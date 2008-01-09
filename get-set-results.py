#!/usr/bin/env python

import sys, os, getopt, glob, os.path

#opts, args = getopt.getopt( sys.argv, 'p:' )

solver = sys.argv[1]
set_list = sys.argv[2]
solver_time_path = sys.argv[3]
percentatge = 40 #int( opts[0][1] )
timeout = '1000'
memout = '2000000'

sys_limits = 'ulimit -t ' + timeout + '; ulimit -m ' + memout + '; ulimit -d ' + memout + '; ulimit -v ' + memout + ';'

total_instances_run = 0

for set in open( set_list ):
    if set[0] == '#':
        continue
    name, path, number = set.split()
    counter = 0
    times = int( number ) * percentatge / 100
    total_instances_run += times
    print '# Results from ' + solver + ' with ' + name + ' ' + str( times ) + '/' + number
    for line in open( solver_time_path + '/' + solver + '-' + name + '.log' ):
        if counter < times:
            counter += 1
            print line[0:-1]
            
print '# Total instances run ' + str( total_instances_run )
