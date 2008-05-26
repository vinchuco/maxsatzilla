#! /bin/bash
COACH=/home/pmatos/software/zilla-common/trunk/coach/src/coach
CONFIGS_DIR=configs
CURR_DIR=$(pwd)

cd $CONFIGS_DIR

rm -f *.log

CONFIGS=`ls *.config.*`

for i in $CONFIGS
do
    echo "Running coach on " $i
    $COACH $i "$i-logs" >& $i.log
done

cd $CURR_DIR

rm -Rf models/
mkdir models
mv $CONFIGS_DIR/*.model models/    
