#!/bin/bash

function run() {
    echo;
    LD_LIBRARY_PATH=libPath primusrun ./out/main.o;
    exit 0;
}

echo "Compiling";
time g++ src/main.cpp -o out/main.o `pkg-config --libs glfw3 glew` -pthread -lassimp;
echo;

libPath="/usr/local/lib";

if [ "$1" == "primus" ]
then
    echo "Starting using primusrun";
    run;
fi

if [ "$1" == "optimus" ]
then
    echo "Starting using optirun";
    run;
fi

run;