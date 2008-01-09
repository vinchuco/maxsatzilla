#/usr/bin/env python

import sys, getopt, os

opts, args = getopt.getopt( sys.argv, 'p:' )

solver = args[0]
set_list = args[1]
percentatge = int( opts[0][1] )

print solver
print set_list

exit

sys_limits = 'ulimit -t ' + timeout + '; ulimit -m ' + memout + '; ulimit -d ' + memout + + '; ulimit -v ' + memout + ';'

for set in open( set_list ):
    if set[0] == '#':
        continue
    name, path, number = set.split()
    print 'Running ' + solver + ' with ' + name
    counter = 0
    times = number * percentatge / 100
    outfile = solver + '.' + name + '.out'
    os.system( 'touch ' + outfile )
    for file in glob.glob( path ):
        if counter < times:
            counter += 1
            print solver + ' with ' + file 
            os.system( solver + ' ' + file + '>> ' + outfile )
