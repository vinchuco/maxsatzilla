#!/usr/bin/env python

import sys

if len( sys.argv ) < 3:
     print 'usage : set-list solver [solver-options]'
     sys.exit(0)

set_list = sys.argv[1]
solver = sys.argv[2]

#if len( sys.argv ) > 3:
#     options = ' ' + sys.argv[3] + ' ' + sys.argv[4] + ' ' + sys.argv[5] 

command = './scripts/runonebench ' + solver + { 
    'maxsatz'  : " '' '' ''", 
    'mspbo'    : " '' '' ''" , 
    'msuncore' : " 1 i '' "
}[ solver ]

import os

for set in open( set_list ):
    if set[0] == '#':
        continue
    name, path, number = set.split()
    print 'Running ' + solver + ' with ' + name
    outfile = solver + '.' + name + '.out'
    os.chdir( "../solvers/msuncore" )
    os.system( command + ' ' + path.split('*')[0] + ' ' + name + '> ' + outfile )

