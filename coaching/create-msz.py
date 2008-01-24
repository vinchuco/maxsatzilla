#!/usr/bin/env python

import sys

if len( sys.argv ) < 4:
    print "usage " + sys.argv[0] + " solver-list instance-set-list msz-file"
    sys.exit(0)

import glob, StringIO

solver_list = sys.argv[1]
feature_list = './maxsat.features'
instance_set_list = sys.argv[2]
msz_file_name = sys.argv[3]
features_directory = './features/'
times_directory = './solver-times/'
timeout = '1000'

def get_instance_time( solver, set_name, instance ):
    for line in open( times_directory + solver + '-' + set_name + '.log' ):
        name, solution, time = line.split()
        if name == instance:
            if not solution.isdigit():
               return '0', time
            return solution, time
    print >> sys.stderr, "Instance " + instance
    raise Exception('Instance not found ' + instance )

def get_feature_value( instance, feature ):
    output = StringIO.StringIO()
    for line in open( instance ):
        if line.startswith( feature ):
            print >> output, "%f" % float( line[ line.rfind(' ') : -1 ] ),
            return output.getvalue()
    print >> sys.stderr, "Feature " + feature
    raise Exception('Feature not found ')

def get_number_of_instances():
    total_number = 0
    print >> msz_file, 'c Instances from sets: ',
    for line in open( instance_set_list ):
        if line[0] == '#':
            continue
        set_name, path, number = line.split()
        print >> msz_file, set_name,
        total_number += int( number )
    print >> msz_file
    return str( total_number )

def check_solution( solution, solver_solution ):
    if solver_solution != '--' and solver_solution != 'xxx':
        if solution == -1:
            solution = int( solver_solution )
        else:
            if solution != int( solver_solution ):
                raise( Exception( 'Solution mistmatch' ) ) 

solvers = []
for solver in open( solver_list ):
    if solver[0] == '#':
        continue
    solvers.append( solver.rstrip() )

features = []
feature_names = []
for feature in open( feature_list ):
    if feature[0] == '#':
        continue
    features.append( feature.rstrip() )
    feature_names.append( feature.rstrip().replace('.','_').replace(' ','_') )

msz_file = open( msz_file_name, 'w' )
number_of_instances = get_number_of_instances()
print >> msz_file, 'p msz ' + str( len( solvers ) ) + ' ' + str( len( features ) ) + ' ' + number_of_instances + ' ' + timeout
print >> msz_file, 'p slv ' + ' '.join( solvers )
print >> msz_file, 'p ftr ' + ' '.join( feature_names )

for line in open( instance_set_list ):
    if line[0] == '#':
        continue        
    instance_counter = 0
    set_name, path, number = line.split()
    print >> msz_file, 'c Set name ' + set_name
    for instance in glob.glob( features_directory + set_name + '.*.features' ):
        instance_counter += 1
        instance_basename = instance[ len( features_directory + set_name + '.' ) : -9 ] 
        print >> msz_file, 'i ' + instance_basename
        solvers_times = []
        feature_values = []
        solution = -1
        try:
            for solver in solvers:
                try:
                    solver_sol, time = get_instance_time( solver, set_name, instance_basename )
                    check_solution( solution, solver_sol )
                    solvers_times.append ( time )
                except Exception:
                    print >> sys.stderr, 'Error with solver ' + solver
                    raise
            for feature in features:
                try:
                    feature_values.append( get_feature_value( instance, feature ) )
                except Exception:
                    print >> sys.stderr, 'Error with feature ' + feature
                    raise
            print >> msz_file, ' '.join( solvers_times )
            print >> msz_file, ' '.join( feature_values )
        except Exception:
            print >> sys.stderr, "Error instance " + instance
    if instance_counter != int( number ):
        print 'Values for ' + set_name + ' : ' + str( instance_counter) + ' ' + number
        raise( Exception( 'Incorrect number of instances' ) )
