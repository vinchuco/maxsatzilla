#!/bin/bash

# Write the result computed for an instance
# To be called only from computeFeatures.sh.
# Authors: Florian Letombe, November, 2007.

if [ $status == $answered ] ; then 
    echo -n " $runningTime"
    echo "$instanceName $runningTime" >> $currentName.slv
elif [ $status == $alreadyDone ] ; then
    echo -n " $runningTime"
elif [ $status == $cpuTimeOutted ] ; then
    echo -n " $cpuPrint"
    echo "$instanceName $cpuPrint" >> $currentName.slv
elif [ $status == $noAnswerGiven ] ; then
    echo -n " $noAnswerPrint"
    echo "$instanceName $noAnswerPrint" >> $currentName.slv
elif [ $status == $unknownError ] ; then
    echo -n " $errorPrint"
    echo "$instanceName $errorPrint" >> $currentName.slv
fi
