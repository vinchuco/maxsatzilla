#!/bin/bash

ulimit -t 1

/usr/bin/time -p stupid.sh 2> /dev/null
exitcode=$?
exit $exitcode
