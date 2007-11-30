#!/bin/bash

ulimit -St 1

/usr/bin/time -p stupid.sh 2> /dev/null
cpuTimeOutReturnCode=$?
