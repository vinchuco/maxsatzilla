#!/usr/bin/env python

import sys, glob, re, shutil

prefix_in = sys.argv[1]
prefix_out = sys.argv[2]
directory = sys.argv[3]

for file in glob.glob( directory + prefix_in + '*' ):
    name = file[ len( directory + prefix_in ) : ]
    shutil.move( file, directory + prefix_out + name )
    
