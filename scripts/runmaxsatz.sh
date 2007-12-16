#!/bin/bash

# Run maxsatz on a MaxSat problem and computes its running time and answer.
# To be called only from computeFeatures.sh.
# Authors: Florian Letombe, November, 2007.

# Resolution
currentName="maxsatz"

# 1. Cheking if result do not already exists
if [ -a "$currentName.slv" ] ; then
    solvingLine=`grep $instanceName $currentName.slv`
else
    solvingLine=""
fi

# 2. If result do not exist for this solver, solve it
#    Otherwise, use existing solution
if [ `echo $solvingLine | wc -c` -eq 1 ] ; then
    echo $currentName >> $logFile
    $time $maxsatz $adimacsInstance > $solversFile 2> $timeFile
    returnValue=$?
    cat $solversFile $timeFile >> $logFile
    # Computing time and checking solution
    runningTime=`grep user $timeFile | tail -c +6`
    answer=`grep "Best Solution=" $solversFile | tail -c +15`
else
    set $solvingLine
    returnValue=$alreadyCode
    runningTime=$2
fi

# 3. Computing status
status=$answered
if [ $returnValue == $cpuTimeOutReturnCode ] ; then
    status=$cpuTimeOutted
elif [ $returnValue == $alreadyCode ] ; then
    status=$alreadyDone
elif [ $returnValue -ne 10 ] ; then
    status=$unknownError
fi

# 4. Computing answer (return value is 10)
if [ $status == $answered ] ; then
    if [ `echo $answer | wc -c` -eq 1 ] ; then
	status=$noAnswerGiven
    else answer=$answer
    fi
fi
