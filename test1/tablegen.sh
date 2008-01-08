#! /bin/bash
NUMVARS=32
LOGFILE_PREFIX="test1.config.fsinst-"
TABLE_NAME=test1.table

rm -f $TABLE_NAME

for i in `seq 1 $NUMVARS`
do 
    for j in `grep "Squared Sum of Errors for the model on the test data" $LOGFILE_PREFIX$i.log | cut -c 54- -`
    do 
	echo -n $j " " >> $TABLE_NAME
    done
    echo >> $TABLE_NAME
done