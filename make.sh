#!/bin/bash

echo "Compiling";

# configuration
include="include"
contrib="contrib"
source="src"
headers=(math.h io.h)
sources=(main.cpp math.cpp io.cpp)

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

time g++ -I $include -I $contrib $headers_full $sources_full -o out/main `pkg-config --libs glfw3 glew` -pthread -lassimp;

echo;
echo "Compilation done";