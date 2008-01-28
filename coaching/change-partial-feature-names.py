#!/usr/bin/env python

import sys, shutil

solver_results_file = sys.argv[1]

temp_file = open( 'temp.temp', 'w' )
for line in open( solver_results_file ):
    name, solution, time = line.split()
    print >> temp_file, 'normalized-' + name + '.pmcnf' + '           ' + solution + '        ' + time
temp_file.close()
shutil.move( 'temp.temp', solver_results_file )
