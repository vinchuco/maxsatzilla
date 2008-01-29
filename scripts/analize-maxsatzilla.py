#!/usr/bin/env python

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
    for line in open( 'outputs/maxsatzilla.' + set_name + '.out' ):    
        if line.startswith( instance_path ):
            instance_name = os.path.basename( line[0:-1] )
        #print instance_name
            best_solver = find_best_solver( instance_name )
        if line.startswith( '** Runnning' ):
            run_solver = line.split()[2]
            if run_solver not in best_solver[0].split():
                failures += 1
            #print 'Best solver ' + best_solver[0] + ' but run solver ' + run_solver
    print 'Failures for ' + set_name + ' = ' + str( failures ) + '/' + number
