#! /bin/bash

CURRENTDIR=$(pwd)
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ -f "$DIR/cosmicGen" ]
then
    echo "${C_GREEN}chips-gen built${C_RESET}"
else
    echo "${C_RED}building chips-gen${C_RESET}"
    NB_CORES=$(grep -c '^processor' /proc/cpuinfo)
    export MAKEFLAGS="-j$((NB_CORES+1)) -l${NB_CORES}"
    cd $DIR
    cmake .
    make
    cd $CURRENTDIR
    echo "${C_GREEN}chips-gen built${C_RESET}"
fi

export CHIPSGEN=$DIR
export PATH=$DIR:$PATH
echo "${C_GREEN}chips-gen setup done${C_RESET}"