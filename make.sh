#!/bin/bash

echo "Compiling";

# configuration
include="include"
source="src"
headers=(math.h io.h)
sources=(main.cpp math.cpp io.cpp)

# compilation
headers_full=""
sources_full=""

for var in ${headers[@]}
do
headers_full="$headers_full $include/algine/$var"
done

for var in ${sources[@]}
do
sources_full="$sources_full $source/$var"
done

time g++ -I $include $headers_full $sources_full -o out/main `pkg-config --libs glfw3 glew` -pthread -lassimp;

echo;
echo "Compilation done";