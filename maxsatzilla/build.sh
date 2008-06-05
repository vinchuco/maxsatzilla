#! /bin/bash 
#
# This is a script which enables the user to pack the maxsatzilla software 
# with several profiles of choice.
#
# Current Profiles are:
# * release : prepare maxsatzilla for release by disabling debug and 
#             compiling executables with shared libraries (in which case
#             the user should be aware of dependencies).
# * test    : prepare maxsatzilla for testing by enabling debug and 
#             compiling executables with shared libraries (in which case
#             the tester should be aware of dependencies).
# * comp/eval : prepare maxsatzilla for competition/evaluation by 
#               disabling debug and compiling executables with static
#               librares, including libc. Resulting executable will
#               be set up into a directory with all required solvers
#               and files to have it ready to roll.
#
# Ver. 1.0 (03/06/2008) - Initial script

CFLAGS=
CXXFLAGS=
CPPFLAGS=
LDFLAGS=

print_usage() {
    echo "./build.sh <profile>"
    echo "Available Profiles:"
    echo "* release : prepare maxsatzilla for release by disabling debug and compiling executables with shared libraries (in which case the user should be aware of dependencies.)"
    echo "* test    : prepare maxsatzilla for testing by enabling debug and compiling executables with shared libraries (in which case the tester should be aware of dependencies.)"
    echo "* comp    : prepare maxsatzilla for competition/evaluation by disabling debug and compiling executables with static librares, including libc. Resulting executable will be set up into a directory with all required solvers and files to have it ready to roll."
    echo "* eval    : same as comp."
}

build_eval() {
    local outputdir="maxsatzilla-eval_TIME`date +%H%M_%d-%m-%y`"    
    local solversoutdir="$outputdir/solvers"
    echo "Output of build is in $outputdir"

    rm -Rf $outputdir
    mkdir -p $solversoutdir

    
    
}

build_maxsatz() {
    local solverdir="solvers/maxsatz"
    cd $solverdir
    make clean
    make
}

build_minimaxsat() {
    local solverdir="solvers/minimaxsat"
    cd $solverdir
    make clean
    make 
}

build_minisatp {
    local solverdir="solvers/minisat+"
    cd $solverdir
    make clean
    make
}

build_minisat2 {
    local solverdir="solvers/minisat2/core"
    cd $solverdir
    make clean
    make
}

build_

if [ $# != 1 ]
then
    echo "Missing profile argument: release, test, comp, eval."
    print_usage
    exit 1
fi

if [ $1 = "release" ]
then
    echo "Building Profile: release"
    build_release
    exit 0
elif [ $1 = "test" ]
then
    echo "Building Profile: test"
    build_test
    exit 0
elif [[ $1 == "comp" || $1 == "eval" ]]
then
    echo "Building Profile: comp/eval"
    build_eval
    exit 0
fi