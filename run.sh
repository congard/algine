#!/bin/bash

function run() {
    LD_LIBRARY_PATH=$libPath $1 ./out/main;
    exit 0;
}

libPath="/usr/local/lib";

if [ "$1" == "primus" ]
then
    echo "Starting using primusrun";
    run primusrun;
fi

if [ "$1" == "optimus" ]
then
    echo "Starting using optirun";
    run optirun;
fi

run;