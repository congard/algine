#!/bin/bash

echo "Compiling";
time g++ src/main.cpp -o out/main.o `pkg-config --libs glfw3 glew` -pthread -lassimp;
echo;
echo "Compilation done";