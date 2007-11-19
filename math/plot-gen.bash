#! /bin/bash 
#
# This script receives one argument: the path of the files and the prefix
# and generates the plots for all the files with that prefix in the same path
FILEPATH=$1
PREFIX="$2_"
TMPDIR=/usr/tmp/

for i in $FILEPATH/$PREFIX*.dat
do
    echo "Processing $i..."
    NOEXT=$(basename $i .dat)
    STRIPPEDFILE=${NOEXT#$PREFIX}
    YAXIS=${STRIPPEDFILE%_*}
    XAXIS=${STRIPPEDFILE#*_}
    
    rm -f $i.png $i.gp
    touch $i.png
    echo "set terminal png truecolor medium size 640,480" >> $i.gp
    echo "set xlabel \"$XAXIS\"" >> $i.gp
    echo "set ylabel \"$YAXIS\"" >> $i.gp
    echo "plot \"$i\"" >> $i.gp
    gnuplot $i.gp >> $i.png
done
    