#!/bin/bash

function compile() {
    echo "Compiling"

    mkdir -p out

    # configuration
    include="include"
    contrib="contrib"
    source="src"
    headers=(math.h io.h node.h framebuffer.h renderbuffer.h core_utils.h shader_compiler.h texture.h shader_program.h bone.h material.h animation.h algine_renderer.h model.h light.h)
    sources=(main.cpp math.cpp io.cpp node.cpp framebuffer.cpp renderbuffer.cpp core_utils.cpp shader_compiler.cpp texture.cpp shader_program.cpp bone.cpp material.cpp animation.cpp algine_renderer.cpp model.cpp light.cpp)

    # compilation
    headers_full=""
    sources_full=""

    # headers paths
    for var in ${headers[@]}
    do
        headers_full="$headers_full $include/algine/$var"
    done

    # sources paths
    for var in ${sources[@]}
    do
        sources_full="$sources_full $source/$var"
    done

    time g++ -I $include -I $contrib $headers_full $sources_full -o out/main `pkg-config --libs glfw3 glew` -pthread -lassimp

    echo
    echo "Compilation done"
}

function run() {
    if [ "$1" == "primus" ]
    then
        echo "Starting using primusrun";
        start primusrun;
    elif [ "$1" == "optimus" ]
    then
        echo "Starting using optirun";
        start optirun;
    else
        echo "Starting"
        start
    fi
}

function start() {
    LD_LIBRARY_PATH=/usr/local/lib $1 ./out/main;
}

function printHelp() {
    echo Algine example bash script
    echo This script must be launched from project root directory! I.e. launch command is \'scripts/algine.sh\'
    echo Available args:
    echo compile - compiles example, binary will be putted on \'out\' dir
    echo run '<optimus/primus/none>' - runs binary
    echo help - print help
}

echo This script is deprecated. Use CMake instead.
echo

if [ "$1" == "compile" ]
then
    compile
elif [ "$1" == "run" ]
then
    run $2
elif [ "$1" == "help" ]
then
    printHelp
else
    echo Unknown arg $1
    echo
    printHelp
fi