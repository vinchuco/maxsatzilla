#!/usr/bin/env python

import sys, glob

solver_list = sys.argv[1]
feature_list = sys.argv[2]
instance_set_list = sys.argv[3]
features_directory = './features/'
times_directory = './solver-times/'
timeout = '1000'

def get_instance_time( solver, name, instance ):
    for line in open( times_directory + solver + '-' + name + '.log' ):
        if line.startswith( instance ):
            return line[ line.rfind(' ') : -1 ]
    raise Exception('Instance not found' + instance )

def get_feature_value( instance, feature ):
    for line in open( instance ):
        if line.startswith( feature ):
            return line[ line.rfind(' ') : -1 ]
    raise Exception('Feature not found ' + feature )

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
    feature_names.append( feature.rstrip().replace( ' ', '_' ) )

print 'p msz ' + str( len( solvers ) ) + ' ' + str( len( features ) ) + ' XXX ' + timeout
print 'p slv ' + ' '.join( solvers )
print 'p ftr ' + ' '.join( feature_names )

for line in open( instance_set_list ):
    if line[0] == '#':
        continue        
    set_name, path = line.split(' ')
    #print '# Set ' + set_name
    for instance in glob.glob( features_directory + set_name + '*.features' ):
        instance_basename = instance[ len( features_directory + set_name + '.' ) : -9 ] 
        print instance_basename
        solvers_times = []
        feature_values = []
        for solver in solvers:
            solvers_times.append ( get_instance_time( solver, set_name, instance_basename ) )
        for feature in features:
            feature_values.append( get_feature_value( instance, feature ) )
        print ' '.join( solvers_times )
        print ' '.join( feature_values )

