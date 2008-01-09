#!/usr/bin/env python

import sys, os, getopt, glob, os.path

opts, args = getopt.getopt( sys.argv[1:], 'p:' )

solver = args[0]
set_list = args[1]
solver_time_path = args[2]
if len( opts ) == 1:
    print '# Percentatge = ' + opts[0][1]
    percentatge = int( opts[0][1] )
else:
    percentatge = 40

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
