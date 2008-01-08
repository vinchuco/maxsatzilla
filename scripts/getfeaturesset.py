#!/usr/bin/env python

import sys, signal

if len( sys.argv ) < 3:
    print "usage " + sys.argv[0] + " <instance list> <target directory>"
    sys.exit(0)

import glob, os

def handler( signal, frame ):
    print "Script interrupted"
    sys.exit(0)

instance_list = sys.argv[1]
target_directory = sys.argv[2]
getfeatures_exec = './getfeatures '

signal.signal( signal.SIGINT, handler )

for line in open( instance_list ):
    if line[0] == '#':
        continue
    name, path, number = line.split()
    print "Obtaining features from " + name,
    files = glob.glob( path )
    print "with " + str( len( files ) ) + " instances"
    for file in files:
        os.popen( getfeatures_exec + file + ' > ' + target_directory + '/' + name + '.' + os.path.basename(file) + '.features' )
