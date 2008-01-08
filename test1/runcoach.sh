#! /bin/bash
COACH=../coach

rm -f *.log

CONFIGS=`ls *.config.*`

for i in $CONFIGS
do
    echo "Running coach on " $i
    $COACH $i >& $i.log
done

rm -Rf models/
mkdir models
mv *.model.* models/    