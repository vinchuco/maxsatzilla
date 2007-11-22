#!/bin/bash

nbInstances=$#
if [ $nbInstances -eq 0 ] ; then
    echo "Usage: $0 <CNF file 1> [<CNF File 2> [... <CNF File i>]]"
    exit 0
fi

arguments="$@"

source settings.sh
ulimit -t $cpuTimeOut

source writeHeaders.sh

for argument in $arguments ; do
    source prepareInstance.sh $argument

    i=0
    while [ $i -lt $nbSolvers ] ; do
	# Try to find if time has already been computed for this solver on this instance
	if [ -a ${solversName[$i]}.slv ] ; then
	    solvingLine=`/usr/bin/grep $instanceName ${solversName[$i]}.slv`
	else
	    solvingLine=""
	fi
        # Resolution (if necessary)
	if [ `echo $solvingLine | wc -c` -eq 1 ] ; then
	    $time ${solvers[$i]} $adimacsInstance > /dev/null 2> $timeFile
	    returnValue=$?
	    runningTime=`grep user $timeFile | tail -c +6`
	    # Write in solver file .slv
	    if [ $returnValue == $cpuTimeOutReturnCode ] ; then
		echo "$instanceName $cpuTimeOut" >> ${solversName[$i]}.slv
	    else echo "$instanceName $runningTime" >> ${solversName[$i]}.slv
	    fi
	else
	    set $solvingLine
	    returnValue=$answered
	    runningTime=$2
	fi
        # Computing output
	if [ $returnValue == $cpuTimeOutReturnCode ] ; then
	    echo -n " $cpuTimeOut"
	else echo -n " $runningTime"
	fi
	i=$(($i+1))
    done

    echo
    $featuresExec $adimacsInstance > $featuresFile
    i=0
    while [ $i -lt $nbFeatures ] ; do
	value=`grep ${features[$i]} $featuresFile | tail -c +4`
	echo -n "$value"
	i=$(($i+1))
    done
    echo
done
source cleanFiles.sh
