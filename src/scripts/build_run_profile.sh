#!/bin/bash

MINARGS=1

if [ $# -eq $MINARGS ] && [ "$1" = "--help" ]; then
    echo "Usage: "
    echo "    $0 <options> -- <PROG> [<ARGS>] "
    echo " "
    echo "Options: "
    echo "      -d, --dir DIR"
    echo "      -m, --mode MODE"
    echo "      -p, --point POINT"
    echo " "
    exit 0
fi


if [ $# -lt $MINARGS ]; then
    echo "Usage:"
    echo "     $0 prog dir mode"
    echo "Try '$0 --help' for information"
    exit 0
fi

if [ $DEBUG ]; then 
    echo "Number of arguments: $#"
    echo "Arguments: $@"
fi


# set default values for optional flags
metric=time
mode=default
dir=.


# process args 
while [ $# -gt 0 ]; do
    key="$1"
    case $key in
        -b*|--obj)
            metric="$2"
            shift # option has parameter
            ;;
        -m*|--mode)
            mode="$2"
            shift # option has parameter
            ;;
        -d*|--dir)
            dir="$2"
            shift # option has parameter
            ;;
        -p*|--point)
            dims="$2"  # grab number of dimensions
            shift
            i=0

            # grab values for each dimension 
            while [ $i -lt $dims ]; do
                point[$i]="$2"
                bits=$bits"$2"
                shift
                i=$(($i+1))
            done
            ;;
        --*)
            prog="$2"
            shift
            shift
            args=$@
            break
            ;;
        *)
           # unknown option
           echo Unknown option: $key
           exit 0 
           ;;
    esac
    shift 
done

if [ $DEBUG ]; then
    echo "dir: " $dir
    echo "mode: " $mode
    echo "metric: " $metric
    echo "dims: " $dims
    echo "point: " ${point[@]}
    echo "bitstring: " $bits
    echo "prog: " $prog
    echo "prog args: " $args
fi

[ -r $dir ] && [ -x $dir ] || { echo "could not change directory to $dir. exiting ..."; exit 1; }
# cd $dir

# gather information about environment 
OS_TYPE=`uname -s`
MACHINE=`uname -m`
PLATFORM=${MACHINE}-${OS_TYPE}

if [ $mode = "DEFAULT" ]; then 
  [ -r bit2flag ] && [ -x bit2flag ] || { echo "could not find bit2flag converter"; exit 1;}
  [ `which gen_makefile_with_opts.sh` ] || { echo "could not locate gen_makefile_with_opts.sh"; exit 1;}


  # convert bits to corresponding compiler flags
  ./bit2flag -b $bits > $dir/flags.pseat
  
cd $dir
  #
  # build 
  #
  if [ -r Makefile ] || [ -r makefile ]; then 

      gen_makefile_with_opts.sh -f flags.pseat

      # copy original makefile 
      cp makefile makefile.orig
      cp makefile.gen makefile

			make clean &> /dev/null
      make &> /dev/null

      # restore old makefile 
      mv makefile.orig makefile
      rm -rf makefile.gen
  else
     echo "$0: did not find makefile or Makefile in $dir. build not done. exiting ..."
     exit 1
  fi
fi	


#
# execute and profile
#

[ -r $prog ] && [ -x $prog ] || { echo "$prog not created after attempted build. exiting ..."; exit 1; }

if [ $metric = "time" ]; then 
    exec_time=`time (./$prog $args > $prog.out) 2>&1` 
    exec_time=`echo "${exec_time}" | grep real | awk '{print $2}'\
                                   | sed 's/m/ /' | sed 's/s//' | awk '{printf "%3.2f", ($1 * 60 + $2)}'` 
    echo ${exec_time} > perf.pseat
fi


exit 0
