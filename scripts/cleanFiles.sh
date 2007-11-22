#!/bin/bash

# Remove all files generated when an error occurs.
# To be called only from computeFeatures.sh.
# Authors: Florian Letombe, November, 2007.

if [ -a timetable.dat ] ; then rm -f timetable.dat ; fi
if [ -a timetable ] ; then rm -f timetable ; fi
if [ -a result3 ] ; then rm -f result3 ; fi
if [ -a $timeFile ] ; then rm -f $timeFile ; fi
if [ -a $featuresFile ] ; then rm -f $featuresFile ; fi
