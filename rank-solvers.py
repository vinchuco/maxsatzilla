#!/usr/bin/env python

import sys, getopt

if len( sys.argv ) == 1:
    print 'usage ' + sys.argv[0] + ' [options] <set-list>'
    print """    options: -v : verbose'
                -d <path to solver times>'
                -s <solver list file>"""
    exit( 0 )

opts, args = getopt.getopt( sys.argv[1:], 'vd:s:' )

solver_times_path = './coaching/solver-times/'
solver_list_file = './coaching/maxsat.solvers'
set_list = args

verbose = False
for option in opts:
    if option[0] == '-v':
        verbose = True
    if option[0] == '-d':
        solver_times_path = option[1]
    if option[0] == '-s':
        solver_list_file = option[1]

times = {} 
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

for set in set_list:
    for solver in times.keys():        
        times[ solver ] = 0
        for line in open( solver_times_path + '/' + solver + '.' + set + '.log' ):
            instance, solution, time = line.split()
            times[ solver ] += float( time )
        if verbose:
            print 'Total time for solver ' + solver + ' in ' + set + ' ' + str( times[ solver ] )
    print 'Ranking for ' + set + ' = ' + str( sort_solvers( times ) )
