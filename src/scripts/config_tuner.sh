#!/bin/bash

MINARGS=1

DIR=`dirname $0`

INSTALL_PATH=$DIR/..

function usage() {
    echo "Usage: "
    echo "    $0 -c COMPILER <options> -f CONFIG"
    echo " "
    echo "Options: "
    echo "      -d, --dir DIR"
    echo "      -a, --add OPTS ..."
    echo "      -r, --remove OPTS ..."
    echo "      -h, --help"
    echo "      -p, --prog PROG"
    echo "      -s, --search ALGORITHM"
    echo "      -m, --mode MODE"
    echo " "
}

if [ $# -eq $MINARGS ] && [ "$1" = "--help" ]; then
    usage 
    exit 0
fi

if [ $# -lt $MINARGS ]; then
    echo "Usage:"
    echo "     $0 -c COMPILER <options>"
    echo "Try '$0 --help' for information"
    exit 0
fi

if [ $DEBUG ]; then 
    echo "Number of arguments: $#"
    echo "Arguments: $@"
fi


mode=default
dir=.
prog=test


# process args 
while [ $# -gt 0 ]; do
    key="$1"
    case $key in
        -c*|--comp)
            comp="$2"
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
        -p*|--prog)
            prog="$2"  # grab number of dimensions
            shift
            ;;
        -a*|--add) 
            alevel="$2"
            shift
            ;;
        -r*|--remove) 
            rlevel="$2"
            shift
            ;;
        -f*|--conffile) 
            conf="$2"
            shift
            ;;
        *)
           # unknown option
           echo Unknown option: $key
           exit 0 
           ;;
    esac
    shift 
done

if [ "$comp" = "" ]; then
    echo "No compiler specified for tuning"
    usage
    exit 0
fi

if [ "$conf" = "" ]; then
    echo "No config file specified"
    usage
    exit 0
fi

if [ $DEBUG ]; then
    echo "compiler: " $comp
    echo "dir: " $dir
    echo "mode: " $mode
    echo "add: " $alevel
    echo "remove: " $rlevel
    echo "prog: " $prog
    echo "config: " $conf
fi

cd $dir;
PWD=`pwd`

if [ "$comp" == 'jvm' ]; then
	cat "$alevel" > optlist
fi


if [ "$comp" == 'gcc'  ]; then

    if [[ "$alevel" == "O0" || "$alevel" == "O1" || "$alevel" == "O2" ||
          "$alevel" == "O3" || "$alevel" == "Ofast" ]]; then
        gcc -Q --help=optimizers -$alevel | grep enabled | awk '{print $1}' >> optlist
    else
        [ -r $alevel ] || { echo "could not open opt file $alevel. exiting ..."; exit 1; }
    fi

    if [[ "$rlevel" == "O0" || "$rlevel" == "O1" || "$rlevel" == "O2" ||
          "$rlevel" == "O3" || "$rlevel" == "Ofast" ]]; then
        gcc -Q --help=optimizers -$rlevel | grep enabled | awk '{print $1}' >> ex_optlist
        suffix="_excl"
    else
        [ -r $rlevel ] || { echo "could not open opt file $rlevel. exiting ..."; exit 1; }
        suffix="_custom"
    fi
fi

if [ "$rlevel" != "" ]; then 
    # remove optimizations to create final list 
    while IFS= read -r line; do
        sed -i -e s/"$line"$//g optlist 
    done < ex_optlist

    # remove empty lines
    sed -i -e /^$/d optlist
fi

# generate search space configuration file 
opts=`wc -l optlist | awk '{print $1}'` 
echo $opts > tmp
i=0
while [ $i -lt $opts ]; do
    echo "N 0 1 1" >> tmp
    i=$(($i+1))
done

newconf="conf.pseat"
cat $conf tmp > $newconf
rm -rf tmp

# if [ "$comp" == 'gcc' ]; then

inputFile=optlist
 awk 'BEGIN { flag=0 }
{
  if( flag == 1 ){
    if( $0 != old ){
      print "\""old"\","
    }
  }
  old=$0
  flag=1
}
END { print "\""old"\""
      print "};"
 }' $inputFile > tmp 

# fi

mv tmp optlist

# merge optimizations with the template
cat ${INSTALL_PATH}/src/optflags/b2fGen.h optlist > $dir/gen_b2f.h


# add in the number of optimizations for the bit string
sed -e 's/numOPTS =/numOPTS = '"$opts"';/g'  $dir/gen_b2f.h > tmp 
mv tmp $dir/gen_b2f.h 

if [ "$rlevel" == "" ]; then
     suffix=""
fi


# generate bit2flag converter for this search space 
if [ "$comp" == 'gcc' ]; then
	g++ -o $dir/bit2flag -I${PWD} ${INSTALL_PATH}/src/optflags/b2fGen.cpp
else
	g++ -o $dir/bit2flag -I${PWD} ${INSTALL_PATH}/src/optflags/b2fGenJVM.cpp
fi

# clean up
rm -rf optlist ex_optlist

exit 
