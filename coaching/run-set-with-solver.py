#!/usr/bin/env python

import sys
import os

set_list = sys.argv[1]
solver = sys.argv[2]

command = '../solvers/msuncore/scripts/runonebench ' + solver + {
   'maxsatz'  : " '' '' ''",
   'mspbo'    : " '' '' ''",
   'msuncore' : ' ' + sys.argv[3] + ' ' + sys.argv[4] + ' ' + sys.argv[5]
}[ solver ]

for set in open( set_list ):
    name, path, number = set.split()
    print 'Running ' + solver + ' with ' + name
    os.popen( command + ' ' + path + ' ' + name )

