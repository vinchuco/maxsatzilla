#!/bin/bash

# Check answers from each solver
# To be called only from computeFeatures.sh.
# Authors: Florian Letombe, November, 2007.

if [ $status == $answered ] ; then

    if [ $currentAnswer == $unknownAnswer ] ; then
	currentAnswer=$answer
    elif [ $currentAnswer -ne $answerConflict ] ; then
	if [ $answer -ne $currentAnswer ] ; then
	    currentAnswer=$answerConflict
	    echo "Conflict answer!!!"
	fi
    fi

fi
