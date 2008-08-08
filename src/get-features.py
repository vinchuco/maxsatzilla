#!/bin/env python

import sys, os, glob

list_of_benchs = sys.argv[1]

for dir in open( list_of_benchs ):
   if dir.startswith( '#' ):
      continue
   dir = dir.strip()
   for file in glob.glob( dir + '*.cnf' ):
      print os.popen( './getfeatures ' + file ).readlines()
      
