#!/usr/bin/python

import sys, os, glob

list_of_benchs = sys.argv[1]
path = sys.argv[2]

for dir in open( list_of_benchs ):
   if dir.startswith( '#' ):
      continue
   dir = dir.strip()
   for file in glob.glob( dir + '*.cnf' ):
      print os.popen( path + './getfeatures ' + file ).readlines()
      
