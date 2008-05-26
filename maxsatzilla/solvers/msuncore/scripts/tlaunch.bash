#!/bin/bash

# Usage:
# $1 -> tool
# $2 -> timeout
# $3 -> memout
# $4 -> bench
# $5 -> fileout

if [[ $# < 5 ]]; then
    echo "Usage: $0 \"<tool> <opts>\" <timeout> <memout> <bench> <outfile>";
    exit;
fi

#ulimit -a
ulimit -t $2
ulimit -m $3
ulimit -d $3
ulimit -v $3

#echo "Running $1 on $4, writing to $5"
/usr/bin/time -p $1 $4 > $5 2>&1

# jpms
