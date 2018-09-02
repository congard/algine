#!/bin/bash

echo "Compiling";
time g++ src/main.cpp -o out/main.o `pkg-config --libs glfw3 glew` -pthread;
echo;

if [ "$1" == "primus" ]
then
    echo "Starting using primusrun";
    echo;
    primusrun ./out/main.o;
    exit 0;
fi

if [ "$1" == "opti" ]
then
    echo "Starting using optirun";
    echo;
    optirun ./out/main.o;
    exit 0;
fi

./out/main.o;
echo;
exit 0;