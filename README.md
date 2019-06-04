# Algine
Algine [Al'gine] - OpenGL 3.3+ engine written in C++

Version: 1.5 alpha

Supported systems: Linux, Windows (with MinGW-w64)

Example of using given in `main.cpp` file

Engine tested on Asus x302uv (Intel Core I5-6200U, Nvidia GeForce 920mx, 8GB RAM, Debian GNU/Linux buster/sid, ArchLinux, 1920x1080 screen) and showed good results

If you find any errors or you have recommendations for improving the project - a big one please let me know!

Shaders are written with the expectation that the compiler optimizes their code. In the future, the optimizer may be written

Click on picture to watch video
[![Algine v1.5 alpha](pictures/1.png)](https://youtu.be/HQKTpQhn40c)

# Functionality
 * Textures
 * Mapping (ambient, diffuse, specular, normal)
 * Lighting (with a huge number of properties)
 * Colorful lighting
 * Unlimited count of light sources
 * Shadow mapping
 * Soft shadows
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

# Dependencies
Engine has next dependencies: `glew`, `glfw`, `glm`, `assimp`.

* Installation on Debian-based systems:

  `sudo apt install libglew-dev libglfw3-dev libglm-dev`

  Also you need `pkg-config` to compile using `make.sh` script: `apt install pkg-config`

  `pkg-config --libs glfw3 glew` output is `-lglfw -lGLEW -lGLU -lGL` (to compile project without `pkg-config`)

* Installation on ArchLinux-based systems:

  x11: `pacman -S glew glfw-x11 glm`
  
  wayland: `pacman -S glew-wayland glfw-wayland glm`

  Also you need `pkgconf` to compile using `make.sh` script: `pacman -S pkgconf`

How to install assimp you can read [here](https://github.com/assimp/assimp/blob/master/Build.md)

# Compilation
1. `cd algine`
2. `chmod +x make.sh run.sh`
3. `./make.sh`
4. `./run.sh` - to run on standard videocard,`./run.sh primus` - to run on second videocard using `primusrun`, `./run.sh optimus` - to run on second videocard using `optirun`

If you use Visual Studio Code, you can build/run the sample directly from the editor (Note that Git Bash (MinTTY) is required for proper working on Windows). Just press Ctrl+Shift+B and choose one of these:
1. Build
2. Run
3. Run on Linux with primus
4. Build and run
5. Build and run on Linux with primus

![VSCode](pictures/vscode_project.png)

**About compilation on Windows you can read [here](build.md)**

# Contact
You can contact me by telegram or email:
 * https://t.me/congard
 * dbcongard@gmail.com

# You can support me financially
<b>Payeer:</b> P36281059

Or you can do it even without finance. Just open [this](http://fainbory.com/8aWY) link

Thank you.