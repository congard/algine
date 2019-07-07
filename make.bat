@echo off
echo Compiling Algine
rem i - install path (the place where the libraries were installed)
rem I - include path
rem L - library path

rem configuration
set i=C:/Program Files (x86)/
set glewI="%i%glew/include"
set glfwI="%i%GLFW/include"
set glmI="%i%glm/include"
set assimpI="%i%Assimp/include"
set glewL="%i%glew/bin"
set glfwL="%i%GLFW/lib"
set glmL="%i%glm/include/glm"
set assimpL="%i%Assimp/bin"
set l=-lassimp -lglew32 -lglfw3 -lopengl32

set include=include
set contrib=contrib
set source=src
set headers=math.h io.h node.h framebuffer.h renderbuffer.h core_utils.h shader_compiler.h texture.h shader_program.h
set sources=main.cpp math.cpp io.cpp node.cpp framebuffer.cpp renderbuffer.cpp core_utils.cpp shader_compiler.cpp texture.cpp shader_program.cpp

rem compilation
set headers_full=
set sources_full=

setlocal EnableDelayedExpansion

rem headers paths
for %%v in (%headers%) do (
    set headers_full=!headers_full! %include%/algine/%%v
)

rem sources paths
for %%v in (%sources%) do (
    set sources_full=!sources_full! %source%/%%v
)

g++ -I %glewI% -I %glfwI% -I %glmI% -I %assimpI%^
    -L %glewL% -L %glfwL% -L %glmL% -L %assimpL%^
    -I %include% -I %contrib% %headers_full% %sources_full%^
    -o out/main.exe %l%

echo Compilation done