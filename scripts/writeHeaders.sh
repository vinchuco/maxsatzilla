#!/bin/bash

# Write the beginning of the msz file.
# To be called only from computeFeatures.sh.
# Authors: Florian Letombe, November, 2007.

echo "p msz $nbSolvers $nbFeatures $nbInstances $cpuTimeOut"
echo -n "p slv"
i=0
while [ $i -lt $nbSolvers ] ; do
    echo -n " ${solversName[$i]}"
    i=$((i+1))
done
echo
echo -n "p ftr"
i=0
while [ $i -lt $nbFeatures ] ; do
    echo -n " ${features[$i]}"
    i=$((i+1))
done
echo
