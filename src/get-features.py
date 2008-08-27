#!/usr/bin/python

import sys, os, glob

if len( sys.argv ) < 2:
   print "usage: " + sys.argv[0] + " <bench_list_file>"
   sys.exit(0)
list_of_benchs = sys.argv[1]
if len( sys.argv ) > 2:
   path = sys.argv[2]
else:
   path = os.getcwd()

features_directory = path + '/features/'
benchpath = None

for dir in open( list_of_benchs ):
   if dir.startswith( '#localpath' ):
      benchpath = dir.split()[1]
      print 'Benchmarks path is : ' + benchpath
      continue
   if dir.startswith( '#' ):
      continue
   dir = dir.strip()
   for file in glob.glob( dir + '*.cnf' ):
      if benchpath:
         out_file_name = file.split( benchpath )[1].replace('/','.')
      else:
         out_file_name = os.path.basename( file )
      out_file = open( features_directory + out_file_name + '.features', 'w' )
      for line in os.popen( path + '/getfeatures ' + file ).readlines():
         print >>out_file, line,
      out_file.close()
      
