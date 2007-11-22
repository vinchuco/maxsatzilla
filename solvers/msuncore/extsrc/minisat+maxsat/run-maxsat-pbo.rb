#!/bin/env ruby

require 'cnf'

Benchmark = ARGV[0]
MaxTimeout = ARGV[1].to_i
RemoveFiles = true
PBO_FileName = File.basename( Benchmark, '.cnf' ) + '.pbo'

formula = Formula.new
formula.load_cnf( Benchmark )
formula.to_pbo( PBO_FileName )

#trap "SIGINT", proc{ File.delete( PBO_FileName ) if RemoveFiles; exit }

system('./run-minisat+.csh ' + PBO_FileName )

File.delete( PBO_FileName ) if RemoveFiles
