#!/usr/bin/env python

import sys, getopt

opts, args = getopt.getopt( sys.argv[1:], 'v' )

solver_times_path = args[0]
set_list = args[1:]

verbose = False
for option in opts:
    if option[0] == '-v':
        verbose = True

solvers = [ 'maxsatz-a-e', 'mspbo-a-e', 'msuncore-a1-ei' ]
times = { 'maxsatz-a-e' : 0.0, 'mspbo-a-e' : 0.0, 'msuncore-a1-ei' : 0.0 }

def sort_solvers( times ):
    from operator import itemgetter
    return [ solver for solver,value in sorted(times.items(), key=itemgetter(1)) ]

for set in set_list:
    for solver in solvers:        
        times[ solver ] = 0
        for line in open( solver_times_path + '/' + solver + '-' + set + '.log' ):
            instance, solution, time = line.split()
            times[ solver ] += float( time )
        if verbose:
            print 'Total time for solver ' + solver + ' in ' + set + ' ' + str( times[ solver ] )
    print 'Ranking for ' + set + ' = ' + str( sort_solvers( times ) )
