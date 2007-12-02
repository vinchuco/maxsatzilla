#!/bin/bash

nbParameters=$#
if [ $nbParameters -eq 0 ] ; then
    echo "Usage: $0
       \"<Solver 1 [arguments 1]>\" [\"<Solver 2 [arguments 2]>\" [... \"<Solver n [arguments n]>\"]]
       -instances <CNF file 1> [<CNF File 2> [... <CNF File i>]]"
    exit
fi

# echo "Settings"
source settings.sh
ulimit -t $cpuTimeOut
ulimit -v $cpuTimeOut

# echo "Inits"
readingInstances=0
nbSolvers=0
nbInstances=0

# echo "Arguments"
for argument in "$@" ; do
    if [ "$argument" == "-instances" ] ; then
	# Start reading instances names after this point
	readingInstances=1
	nbInstances=$(($nbParameters-$nbSolvers-1))
	#echo $nbInstances
	source writeHeaders.sh
    elif [ $readingInstances == 0 ] ; then
	# Reading solvers names
	source solverScriptFactory.sh $argument
    else
	# Preparing instance
	source prepareInstance.sh $argument

	# Computing CPU times for each solver
	currentAnswer=$unknownAnswer
	i=0
	while [ $i -lt $nbSolvers ] ; do
	    source ${solvers[$i]}
	    source mergeAnswers.sh
	    source writeSolverResult.sh
	    i=$(($i+1))
	done
	echo

	# Computing features
	featuringLineFirst=""
	featuringLineLast=""
	if [ -a "${features[0]}.ftr" ] ; then
	    featuringLineFirst=`grep $instanceName ${features[0]}.ftr`
	    if [ -a "${features[$((nbFeatures-1))]}.ftr" ] ; then
		featuringLineLast=`grep $instanceName ${features[$((nbFeatures-1))]}.ftr`
	    fi
	fi
	if [ `echo $featuringLineFirst | wc -c` -eq 1 ] || [ `echo $featuringLineLast | wc -c` -eq 1 ] ; then
	    ../getfeatures $adimacsInstance | sed -f ../filter.sed > $featuresFile
	    i=0
	    while [ $i -lt $nbFeatures ] ; do
		value=`grep ${features[$i]} $featuresFile | cut -d':' -f2`
		echo -n "$value"
		echo "$instanceName $value" >> ${features[$i]}.ftr
		i=$(($i+1))
	    done
	else
	    i=0
	    while [ $i -lt $nbFeatures ] ; do
		set `grep $instanceName ${features[$i]}.ftr`
		value=$2
		echo -n " $value"
		i=$(($i+1))
	    done
	fi
	echo
    fi
done
source cleanFiles.sh
