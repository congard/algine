# Algine
Algine [Al'gine] - OpenGL 3.3+ engine written in C++

Version: 1.6 alpha
<br>Note: at the moment, new versions are released only for updating changelog

Supported systems: Linux, Windows (with MinGW-w64)

Example of using given in `main.cpp` file

Engine tested on Asus x302uv (Intel Core I5-6200U, Nvidia GeForce 920mx, 8GB RAM, Debian GNU/Linux buster/sid, ArchLinux, 1920x1080 screen) and showed good results

If you find any errors or you have recommendations for improving the project - a big one please let me know!

Click on picture to watch video
[![Algine v1.6 alpha](pictures/1.png)](https://youtu.be/pYUXtRl-LTs)

# Functionality
 * Textures
 * Mapping (ambient, diffuse, specular, normal)
 * Lighting (with a huge number of properties)
 * Colorful lighting
 * Unlimited count of light sources
 * Shadow mapping
 * Soft shadows
 * Directional lighting (shadow mapping)
 * Point lighting
 * Bloom
 * Depth of field (DOF)
 * SSR (Screen Space Reflections)
 * Cinematic Depth of field
 * Support of 40+ 3D model formats (thanks to [Assimp](https://github.com/assimp/assimp))
 * Support of materials
 * Skeletal animations / bone system. Limitations on the *number of bones* / *number of bones per vertex* are **hardware only**.
 * Effect management
 * Etc.

Take into account, this is a test (alpha) version! All major functions will be added in later versions.

# TODO
You can find TODO list [here](http://evassmat.com/ZaDr)

# Dependencies
Engine has next dependencies: `glew`, `glfw`, `glm`, `assimp`.

* Installation on Debian-based systems:
  <br>`sudo apt install libglew-dev libglfw3-dev libglm-dev`
  <br>Also you need `pkg-config` to compile using `algine.sh` script: `apt install pkg-config`
  <br>`pkg-config --libs glfw3 glew` output is `-lglfw -lGLEW -lGLU -lGL` (to compile project without `pkg-config`)

* Installation on ArchLinux-based systems:
  <br>x11: `pacman -S glew glfw-x11 glm`
  <br>wayland: `pacman -S glew-wayland glfw-wayland glm`
  <br>Also you need `pkgconf` to compile using `algine.sh` script: `pacman -S pkgconf`

How to install assimp you can read [here](https://github.com/assimp/assimp/blob/master/Build.md)

# Compilation
1. `cd algine`
2. Several methods:
    1. Via CMake
    2. Using deprecated script
        1. `chmod +x algine.sh`
        2. Execute `./scripts/algine.sh help`
        
**Note:** working directory must be the root of the project! Nor \<project\>/cmake-build-debug or something else

**About compilation on Windows you can read [here](build.md)**

# Contact
You can contact me via telegram or email:
 * [@congard](https://t.me/congard)
 * [dbcongard@gmail.com](mailto:dbcongard@gmail.com)

# You can support me financially
<b>Payeer:</b> P36281059

Or you can do it even without finance. Just open [this](http://fainbory.com/8aWY) link

Thank you.