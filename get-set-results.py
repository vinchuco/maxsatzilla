#!/usr/bin/env python

import sys, os, getopt, glob, os.path

opts, args = getopt.getopt( sys.argv[1:], 'p:' )

file = args[0]
number = int( args[1] )
if len( opts ) == 1:
    print '# Percentatge = ' + opts[0][1]
    percentatge = int( opts[0][1] )
else:
    percentatge = 100

total_instances_run = 0

counter = 0
times = number * percentatge / 100
total_instances_run += times
print '# Results from ' + file + ' ' + str( times ) + '/' + str( number )
for line in open( file ):
    if counter < times:
        counter += 1
        print line[0:-1]
            
print '# Total instances run ' + str( total_instances_run )
