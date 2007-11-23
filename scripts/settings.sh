#!/bin/bash

# Settings for running experiments on SAT problems.
# Authors: Florian Letombe, November, 2007.

# Directories
#cnfDirectory="/benchmark"
maxsatzDirectory="../solvers/maxsatz"
msuncoreDirectory="../solvers/msuncore/src"
minimaxsatDirectory="../solvers/minimaxsat"

# Executables
maxsatz="$maxsatzDirectory/maxsatz"
msuncore="$msuncoreDirectory/msuncore -a 1 -e c"
minimaxsat="$minimaxsatDirectory/minimaxsat -F=1"

# Solvers
solversName[0]="maxsatz"
solversName[2]="msuncore"
solversName[1]="minimaxsat"

solvers[0]="$maxsatz"
solvers[2]="$msuncore"
solvers[1]="$minimaxsat"

nbSolvers=2

# Features
features[0]="SBC"
features[1]="SBM" 
features[2]="SBV" 
features[3]="SMM" 
features[4]="SB1" 
features[5]="SB9" 
features[6]="SAM" 
features[7]="SAV" 
features[8]="SFR" 
features[9]="SFV" 
features[10]="SCV" 
features[11]="STT" 
features[12]="GBC" 
features[13]="GBV" 
features[14]="GBM" 
features[15]="GVV" 
features[16]="GMM" 
features[17]="GB1" 
features[18]="GB9" 
features[19]="GAM" 
features[20]="GAV" 
features[21]="GFR" 
features[22]="GFV" 
features[23]="GSV" 
features[24]="GTT" 
features[25]="Vrs" 
features[26]="Cls" 
features[27]="Neg" 
features[28]="Pos" 
features[29]="Uni" 
features[30]="Bin"
features[31]="Ter" 
nbFeatures=32

# Time commands
timeFile="timeFile$HOSTNAME.txt"
time="/usr/bin/time -p"

# Features commands
featuresFile="featuresFile$HOSTNAME.txt"
featuresExec="../features.rb ../getfeatures"

# Timeouts
cpuTimeOut=1000
cpuTimeOutReturnCode=9

# Internal constants for results
answered=1
cpuTimeOutted=2
