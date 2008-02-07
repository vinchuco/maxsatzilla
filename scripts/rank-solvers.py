#!/usr/bin/env python

import sys, getopt

if len( sys.argv ) == 1:
    print 'usage ' + sys.argv[0] + ' [options] <set-list>'
    print """    options: -v : verbose'
                -d <path to solver times>'
                -s <solver list file>"""
    exit( 0 )

""" Examples
./scripts/rank-solvers.py AUTO IBM DEBUG BF SSA PIMAG SATPLAN UCLID RAMSEY SPINGLASS DIMACS-MOD
./scripts/rank-solvers.py -v -s coaching/partial.solvers FIR MTG SYN
"""

opts, args = getopt.getopt( sys.argv[1:], 'vd:s:' )

solver_times_path = './coaching/solver-times/'
solver_list_file = './coaching/maxsat.solvers'
set_list = args

verbose = False
for option, value in opts:
    if option == '-v':
        verbose = True
    if option == '-d':
        solver_times_path = value
    if option == '-s':
        solver_list_file = value

times = { 'maxsatzilla' : 0.0 } 
failures = { 'maxsatzilla' : 0 }
total_failures = { 'maxsatzilla' : 0 }
TimeOut = 998

for solver in open( solver_list_file ):
    if solver[0] == '#':
        continue
    times[ solver[0:-1] ] = 0.0

def sorted( list, comparison ):
    list2 = list
    list2.sort( comparison )
    return list2

def sort_solvers( times ):    
    return [ solver for solver,value in sorted( times.items(), lambda (x1,x2), (y1,y2) : cmp(x2, y2)) ]

for solver in times.keys():
    total_failures[ solver ] = 0

for set in set_list:
    for solver in times.keys():        
        times[ solver ] = 0.0
        failures[ solver ] = 0
        for line in open( solver_times_path + '/' + solver + '-' + set + '.log' ):
            instance, solution, time = line.split()
            times[ solver ] += float( time )
            if float( time ) > TimeOut:
                failures[ solver ] += 1
        total_failures[ solver ] += failures[ solver ]
        if verbose:
            print 'Total time for solver ' + solver + ' in ' + set + ' ' + str( times[ solver ] )
            print 'Total failures for solver ' + solver + ' in ' + set + ' ' + str( failures[ solver ] )
    print 'Time ranking for ' + set + ' = ' + str( sort_solvers( times ) )
    if verbose:
        print 'Failure ranking for ' + set + ' = ' + str( sort_solvers( failures ) )

if verbose:
    print total_failures 
print 'Total failure ranking : ' + str( sort_solvers( total_failures ) )
