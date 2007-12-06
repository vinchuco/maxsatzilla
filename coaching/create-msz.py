#!/usr/bin/env python

import sys

if len( sys.argv ) < 4:
    print "usage " + sys.argv[0] + " solver-list feature-list instance-set-list msz-file"
    sys.exit(0)

import glob, StringIO

solver_list = sys.argv[1]
feature_list = sys.argv[2]
instance_set_list = sys.argv[3]
msz_file_name = sys.argv[4]
features_directory = './features/'
times_directory = './solver-times/'
timeout = '1000'

def get_instance_time( solver, name, instance ):
    for line in open( times_directory + solver + '-' + name + '.log' ):
        name, solution, time = line.split()
        if name == instance:
            return solution, time
    print >> sys.stderr, "Instance " + instance
    raise Exception('Instance not found' )

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
    for line in open( instance_set_list ):
        if line[0] == '#':
            continue
        set_name, path, number = line.split()
        total_number += int( number )
    return str( total_number )


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
print >> msz_file, 'p msz ' + str( len( solvers ) ) + ' ' + str( len( features ) ) + ' ' + get_number_of_instances() + ' ' + timeout
print >> msz_file, 'p slv ' + ' '.join( solvers )
print >> msz_file, 'p ftr ' + ' '.join( feature_names )

for line in open( instance_set_list ):
    if line[0] == '#':
        continue        
    set_name, path, number = line.split()
    #print >> msz_file, '# Set name ' + set_name
    for instance in glob.glob( features_directory + set_name + '*.features' ):
        instance_basename = instance[ len( features_directory + set_name + '.' ) : -9 ] 
        print >> msz_file, instance_basename
        solvers_times = []
        feature_values = []
        try:
            for solver in solvers:
                solution, time = get_instance_time( solver, set_name, instance_basename )
                solvers_times.append ( time )
                for feature in features:
                    feature_values.append( get_feature_value( instance, feature ) )
            print >> msz_file, ' '.join( solvers_times )
            print >> msz_file, ' '.join( feature_values )
        except Exception:
            print >> sys.stderr, "Error instance " + instance
