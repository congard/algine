@echo off
echo Compiling Algine
rem i - install path (the place where the libraries were installed)
rem I - include path
rem L - library path

SET i=C:/Program Files (x86)/
SET glewI="%i%glew/include"
SET glfwI="%i%GLFW/include"
SET glmI="%i%glm/include"
SET assimpI="%i%Assimp/include"
SET glewL="%i%glew/bin"
SET glfwL="%i%GLFW/lib"
SET glmL="%i%glm/include/glm"
SET assimpL="%i%Assimp/bin"
SET l=-lassimp -lglew32 -lglfw3 -lopengl32

g++ -I %glewI% -I %glfwI% -I %glmI% -I %assimpI% -L %glewL% -L %glfwL% -L %glmL% -L %assimpL% src/main.cpp -o out/main.exe %l%
echo Compilation done