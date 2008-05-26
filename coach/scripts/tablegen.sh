#! /bin/bash
NUMVARS=$1
PREFIX=$2
LOGFILE_PREFIX="$PREFIX.config.fsinst-"
LOGDIR_SUFFIX=$3
TABLE_NAME=$PREFIX.table

rm -f $TABLE_NAME

for i in `seq 1 $NUMVARS`
do 
    for j in `grep "Squared Sum of Errors for the model on the test data" $LOGFILE_PREFIX$i$LOGDIR_SUFFIX/system.log | cut -c 54- -`
    do 
	echo -n $j " " >> $TABLE_NAME
    done
    echo >> $TABLE_NAME
done
