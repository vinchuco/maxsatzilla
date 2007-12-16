#!/bin/bash

# Compute solvers to be called
# To be called only from computeFeatures.sh.
# Authors: Florian Letombe, November, 2007.

# Check input format
if [ $# == 0 ] ; then
    echo "Usage: $0 <maxsatz options | msuncore options | minimaxsat options>"
    exit
fi

if [ $1 == "maxsatz" ] ; then
    solvers[$nbSolvers]="runmaxsatz.sh"
    solverName[$nbSolvers]="maxsatz"
    nbSolvers=$(($nbSolvers+1))
elif [ $1 == "msuncore" ] ; then
    arguments=$@
    solvers[$nbSolvers]="runmsuncore.sh${arguments##msuncore}"
    solverName[$nbSolvers]="msuncore"
    nbSolvers=$(($nbSolvers+1))
elif [ $1 == "minimaxsat" ] ; then
    arguments=$@
    solvers[$nbSolvers]="runminimaxsat.sh${arguments##minimaxsat}"
    solverName[$nbSolvers]="minimaxsat"
    nbSolvers=$(($nbSolvers+1))
else
    echo "Unrecognized solver: $1"
    exit
fi
