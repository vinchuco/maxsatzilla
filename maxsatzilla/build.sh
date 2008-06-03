#! /bin/bash -x
#
# This is a script which enables the user to pack the maxsatzilla software 
# with several profiles of choice.
#
# Current Profiles are:
# * release : prepare maxsatzilla for release by disabling debug and 
#             compiling executables with shared libraries (in which case
#             the user should be aware of dependencies.
# * test    : prepare maxsatzilla for testing by enabling debug and 
#             compiling executables with shared libraries (in which case
#             the tester should be aware of dependencies.
# * comp/eval : prepare maxsatzilla for competition/evaluation by 
#               disabling debug and compiling executables with static
#               librares, including libc. Resulting executable will
#               be set up into a directory with all required solvers
#               and files to have it ready to roll.
#
# Ver. 1.0 (03/06/2008) - Initial script

