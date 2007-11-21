#!/bin/bash

# Settings for running experiments on SAT problems.
# Authors: Florian Letombe, November, 2007.

# Directories
#cnfDirectory="/benchmark"
maxsatzDirectory="../solvers/maxsatz"
minimaxsatDirectory="../solvers/minimaxsat"
msuncoreDirectory="../solvers/msuncore"

# Executables
maxsatz="$maxsatzDirectory/maxsatz"
minimaxsat="$minimaxsatDirectory/minimaxsat"
msuncore="$msuncoreDirectory/msuncore"

# Solvers
solversName[0]="maxsatz"
solversName[1]="minimaxsat"
solversName[2]="msuncore"
solvers[0]="$maxsatz"
solvers[1]="$minimaxsat"
solvers[2]="$msuncore"
nbSolvers=3

# Features
features[0]="Vrs"
features[1]="Cls"
features[2]="CoV"
features[3]="Neg"
features[4]="Pos"
features[5]="Una"
features[6]="Bin"
features[7]="Ter"
nbFeatures=8

# Time commands
timeFile="timeFile$HOSTNAME.txt"
time="/usr/bin/time -p"

# Features commands
featuresFile="featuresFile$HOSTNAME.txt"
featuresExec="../features"

# Timeouts
cpuTimeOut=1000
cpuTimeOutReturnCode=9

# Internal constants for results
answered=1
cpuTimeOutted=2
