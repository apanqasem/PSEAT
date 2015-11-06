#!/bin/bash

function usage() {
    echo "Usage: "
    echo "    $0 <options> -f FILE"
    echo " "
    echo "Options: "
    echo "      -h, --help"
    echo "      -f, --flagsfile FILE"
    echo " "
    echo "FILE:  "
    echo "   file containing compiler optimizationg flags"
}

if [ $# -lt 1 ]; then
    usage
    exit 0
fi	


DIR=`dirname $0`

INSTALL_PATH=$DIR/..
python ${INSTALL_PATH}/src/scripts/gen_makefile_with_opts.py "$@"

