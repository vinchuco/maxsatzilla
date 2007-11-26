#!/bin/bash

# Prepare name and directory of an instance.
# To be called only from computeFeatures.sh.
# Authors: Florian Letombe, November, 2007.

instance=$1

# Compute base name of instance file
instanceDirectory=`dirname $instance`
if [ `basename $instance .cnf` == `basename $instance` ] ; then
    instanceName=`basename $instance .cnf.gz`
    extension="cnf.gz"
else
    instanceName=`basename $instance .cnf`
    extension="cnf"
fi
adimacsInstance="$instanceDirectory/$instanceName.$extension"
echo "$instanceName"
echo "$instanceName" >> $logFile
