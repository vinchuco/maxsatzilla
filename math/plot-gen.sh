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
    
    rm -f $FILEPATH/$i.png $TMPDIR/$i.gp
    touch $TMPDIR/$i.gp
    echo "set terminal png medium size 640,480" >> $TMPDIR/$i.gp
    echo "set xlabel \"$XAXIS\"" >> $TMPDIR/$i.gp
    echo "set ylabel \"$YAXIS\"" >> $TMPDIR/$i.gp
    echo "fit m1 * x + b1 \"$i\" via m1, b1" >> $TMPDIR/$i.gp
    echo "fit a2 + b2 * x + c2 * (x ** 2) \"$i\" via a2, b2, c2" >> $TMPDIR/$i.gp
    echo "plot \"$i\", \
          m1 * x + b1, \
          a2 + b2 * x + c2 * (x ** 2)" >> $TMPDIR/$i.gp
    gnuplot $TMPDIR/$i.gp >> $FILEPATH/$i.png
done
    
