#!/bin/bash

# Prepare name and directory of an instance.
# To be called only from computeFeatures.sh.
# Authors: Florian Letombe, November, 2007.

instance=$1

# Compute base name of instance file
instanceDirectory=`dirname $instance`
if [ `basename $instance .cnf` != `basename $instance` ] ; then
    instanceName=`basename $instance .cnf`
    extension="cnf"
elif [ `basename $instance .cnf.gz` != `basename $instance` ] ; then
    instanceName=`basename $instance .cnf.gz`
    extension="cnf.gz"
elif [ `basename $instance .dimacs` != `basename $instance` ] ; then
    instanceName=`basename $instance .dimacs`
    extension="dimacs"
elif [ `basename $instance .dimacs.gz` != `basename $instance` ] ; then
    instanceName=`basename $instance .dimacs.gz`
    extension="dimacs.gz"
elif [ `basename $instance .dimacs.seq` != `basename $instance` ] ; then
    instanceName=`basename $instance .dimacs.seq`
    extension="dimacs.seq"
else
    $instanceName="Unrecognised Format"
fi
adimacsInstance="$instanceDirectory/$instanceName.$extension"
if [ ! -e $adimacsInstance ] ; then
    echo "$adimacsInstance not found"
    exit
fi
echo "$instanceName"
echo "$instanceName" >> $logFile
