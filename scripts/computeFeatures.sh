#!/bin/bash

source settings.sh
ulimit -t $cpuTimeOut

readingInstances=0

nbInstances=$#
if [ $nbInstances -eq 0 ] ; then
    echo "Usage: $0 <CNF file 1> [<CNF File 2> [... <CNF File i>]]"
    exit 0
fi

source writeHeaders.sh

for argument in "$@" ; do
    source prepareInstance.sh $argument

    i=0
    while [ $i -lt $nbSolvers ] ; do
        # Resolution
	$time ${solversName[$i]} $adimacsInstance > /dev/null 2> $timeFile
	returnValue=$?
        # Computing time
	runningTime=`grep user $timeFile | tail -c +6`
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
	echo -n " $value"
	i=$(($i+1))
    done
    echo
done
source cleanFiles.sh
