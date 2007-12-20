#! /bin/bash
for i in `seq 1 34`
do 
    for j in `grep "Squared Sum of Errors for the model on the test data" industrial.results.fsinst-$i | cut -c 54- -`
    do 
	echo -n $j " " >> ../table
    done
    echo >> ../table
done