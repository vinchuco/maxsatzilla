#! /bin/bash
CONFIG_TEMPLATE=$1
CONFIG_PREFIX=`basename $1 .template`
CONFIGS_DIR=configs/
NUMVARS=$2 

rm -Rf $CONFIGS_DIR
mkdir $CONFIGS_DIR

for i in `seq 1 $NUMVARS`
do
    CONFIG=$CONFIG_PREFIX.fsinst-$i
    echo "Generating config file " $CONFIG " for standard options and fsinst " $i
    cp $CONFIG_TEMPLATE $CONFIGS_DIR/$CONFIG
    echo "p fsinst " $i >> $CONFIGS_DIR/$CONFIG
    echo "p model $CONFIG.model" >> $CONFIGS_DIR/$CONFIG
done
