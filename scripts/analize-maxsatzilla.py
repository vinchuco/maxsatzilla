#!/usr/bin/env python

import sys, os

solvers_file = sys.argv[1]
sets_file = sys.argv[2]
set_name = sys.argv[3]

solvers = []
for solver in open( solvers_file ):
    if solver[0] == '#':
        continue
    solvers.append( solver[0:-1] )
    
instance_time = {} 
for solver in solvers:
    instance_time[ solver ] = {}
    #print 'Opening ' + 'coaching/solver-times/' + solver + '-' + set_name + '.log'
    for instance in open( 'coaching/solver-times/' + solver + '-' + set_name + '.log' ):
        name, solution, time = instance.split()
        instance_time[ solver ][ name ] = float( time )

def find_best_solver( instance ):
    best, time = '', 1000.0
    for solver in solvers:
        if instance_time[ solver ][ instance ] < time:
            best, time = solver, instance_time[ solver ][ instance ]
        elif instance_time[ solver ][ instance ] == time:
            best += ' ' + solver
    return best, time

def get_path():
    for line in open( sets_file ):
        if line[0] == '#':
            continue
        name, path, number = line.split()
        if name == set_name:
            return os.path.dirname( path )

path = get_path()
for line in open( 'outputs/maxsatzilla.' + set_name + '.out' ):    
    if line.startswith( path ):
        instance_name = os.path.basename( line[0:-1] )
        print instance_name
        best_solver = find_best_solver( instance_name )
    if line.startswith( '** Runnning' ):
        run_solver = line.split()[2]
        if run_solver not in best_solver[0].split():
            print 'Best solver ' + best_solver[0] + ' but run solver ' + run_solver
