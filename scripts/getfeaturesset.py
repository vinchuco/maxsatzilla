#!/usr/bin/env python

import sys, glob, os

instance_list = sys.argv[1]
target_directory = sys.argv[2]
getfeatures_exec = './getfeatures '

for line in open( instance_list ):
    if line[0] == '#':
        continue
    name, path = line.split(' ')
    path = path[0:-1] # Removing \n
    print "Obtaining features from " + name
    for file in glob.glob( path ):
        os.system( getfeatures_exec + file + ' > ' + target_directory + '/' + name + '.' + os.path.basename(file) + '.features' )
            
