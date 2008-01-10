#!/usr/bin/env python

import sys, os, getopt, glob, os.path, signal, tempfile

opts, args = getopt.getopt( sys.argv[1:], 'p:' )

solver = args[0]
set_list = args[1]
execution_list = args[2:]
if len( opts ) == 1:
    print '# Percentatge = ' + opts[0][1]
    percentatge = int( opts[0][1] )
else:
    percentatge = 100
timeout = '1000'
memout = '2000000'

sys_limits = 'ulimit -t ' + timeout + '; ulimit -m ' + memout + '; ulimit -d ' + memout + '; ulimit -v ' + memout + ';'

total_instances_run = 0

def handler( signal, frame ):
    print "Script interrupted"
    sys.exit(0)
signal.signal( signal.SIGINT, handler )

for set in open( set_list ):
    if set[0] == '#':
        continue
    name, path, number = set.split()
    if name in execution_list:
        counter = 0
        times = int( number ) * percentatge / 100
        total_instances_run += times
        print '# Running ' + solver + ' with ' + name + ' ' + str( times ) + '/' + number
        outfile = os.path.basename( solver.split()[0] ) + '.' + name + '.out'
        os.system( 'cp /dev/null ' + outfile )
        for file in glob.glob( path ):
            if counter < times:
                counter += 1
                timefile = tempfile.NamedTemporaryFile()
                os.popen( sys_limits + '/usr/bin/time -p ' + solver + ' ' + file + ' >> ' + outfile +' 2> ' + timefile.name )
                print 'o ' + file + ' ' + os.popen( 'grep user ' + timefile.name + ' | cut -c5-' ).read() 

print '# Total instances run ' + str( total_instances_run )
