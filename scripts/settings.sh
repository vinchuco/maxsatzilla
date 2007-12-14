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
# -a 1 -e c
msuncore="$msuncoreDirectory/msuncore"
# -F=1
minimaxsat="$minimaxsatDirectory/minimaxsat"

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

# Solvers/Features commands
solversFile="solversFile$HOSTNAME.txt"
featuresFile="featuresFile$HOSTNAME.txt"
featuresExec="../features.rb ../getfeatures"

# Timeouts/Memory limits
cpuTimeOut=1000
source setCPULimit.sh
#echo "exitcode=$cpuTimeOutReturnCode"
memOut=2000000

# Log file for trace
logFile="logFile$HOSTNAME.log"

# Internal constants for results
exitCode=10
alreadyCode=20

# status values
answered=1
cpuTimeOutted=2
unknownError=3
alreadyDone=4
noAnswerGiven=5

# currentAnswer values
answerConflict=-10
unknownAnswer=-20

#Print values
cpuPrint=-1
errorPrint=-2
noAnswerPrint=-3