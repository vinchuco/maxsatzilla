#!/usr/bin/env python

""" Examples:
./scripts/analize-maxsatzilla.py coaching/maxsat.solvers ./coaching/real+structured.instances
./scripts/analize-maxsatzilla.py coaching/partial.solvers ./coaching/cpaior.instances
"""

import sys, os

solvers_file = sys.argv[1]
sets_file = sys.argv[2]

def find_best_solver( instance ):
    best, time = '', 1000.0
    for solver in solvers:
        if instance_time[ solver ][ instance ] < time:
            best, time = solver, instance_time[ solver ][ instance ]
        elif instance_time[ solver ][ instance ] == time:
            best += ' ' + solver
    return best, time

solvers = []
for solver in open( solvers_file ):
    if solver[0] == '#':
        continue
    solvers.append( solver[0:-1] )
    
total_failures = 0
total_number = 0
for line in open( sets_file ):
    if line[0] == '#':
        continue
    set_name, path, number = line.split()
    instance_path = os.path.dirname( path )    

    instance_time = {} 
    for solver in solvers:
        instance_time[ solver ] = {}
        for instance in open( 'coaching/solver-times/' + solver + '-' + set_name + '.log' ):
            name, solution, time = instance.split()
            instance_time[ solver ][ name ] = float( time )

    if not os.path.exists( 'outputs/maxsatzilla.' + set_name + '.out' ):
        print 'No experiments with ' + set_name
        continue

    failures = 0
    checking_state = 0
    for line in open( 'outputs/maxsatzilla.' + set_name + '.out' ):    
        if line.startswith( instance_path ):
            instance_name = os.path.basename( line[0:-1] )
        #print instance_name
            best_solver = find_best_solver( instance_name )
            if checking_state != 0:
                print 'Error : no solver run'
            checking_state = 1
        if line.startswith( '** Runnning' ):
            if checking_state != 1:
                continue
            run_solver = line.split()[2]
            if run_solver not in best_solver[0].split():
                failures += 1
            #print 'Best solver ' + best_solver[0] + ' but run solver ' + run_solver
            checking_state = 0
    print 'Failures for ' + set_name + ' = ' + str( failures ) + '/' + number
    total_failures += failures
    total_number += int( number )

print 'Total Failures : ' + str( total_failures ) + '/' + str( total_number )
