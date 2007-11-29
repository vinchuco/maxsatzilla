#!/bin/bash

ulimit -t 1

/usr/bin/time -p stupid.sh
exitcode=$?
echo $exitcode
