#!/usr/bin/env python

import sys

if len( sys.argv ) < 3:
    print "usage " + sys.argv[0] + " <instance list file> <directory>"
    sys.exit(0)

import glob, os

instance_list = sys.argv[1]
target_directory = sys.argv[2]
getfeatures_exec = './getfeatures '

for line in open( instance_list ):
    if line[0] == '#':
        continue
    name, path, number = line.split(' ')
    print "Obtaining features from " + name
    for file in glob.glob( path ):
        os.system( getfeatures_exec + file + ' > ' + target_directory + '/' + name + '.' + os.path.basename(file) + '.features' )
