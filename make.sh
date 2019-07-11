#!/bin/bash

echo "Compiling";

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

time g++ -I $include -I $contrib $headers_full $sources_full -o out/main `pkg-config --libs glfw3 glew` -pthread -lassimp;

echo;
echo "Compilation done";