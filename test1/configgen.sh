#! /bin/bash
NUMVARS=32

for i in `seq 1 $NUMVARS`
do
    CONFIG=test1.config.fsinst-$i
    echo "Generating config file " $CONFIG " for standard options and fsinst " $i
    cp config.template $CONFIG
    echo "p fsinst " $i >> $CONFIG
    echo "p model test1.model.fsinst-$i" >> $CONFIG
done