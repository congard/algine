# Algine
Algine [Al'gine] - OpenGL 3.3+ engine written in C++

Version: 1.1 alpha

Supported systems: Linux (Windows in the future)

Example of using given in `main.cpp` file

Engine tested on Asus x302uv (Intel Core I5-6200U, Nvidia GeForce 920mx, 8GB RAM, Debian GNU/Linux 9.4.0, 1920x1080 screen) and showed good results

If you find any errors or you have recommendations for improving the project - a big one please let me know!

**Note:** strong optimization will be carried out in version 1.2 alpha

![Algine v1.1 alpha](pictures/6.png)

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
 * CMF models support
 * Depth of field (DOF)
 * Etc.

Take into account, this is a test (alpha) version! All major functions will be added in later versions.

# Dependencies
Engine has next dependencies: `glew`, `glfw`, `glm`.

Installation on Debian-based systems:

`sudo apt install libglew-dev libglfw3-dev libglm-dev`

Also you need `pkg-config` to compile using `build/make.sh` script

`sudo apt install pkg-config`

`pkg-config --libs glfw3 glew` output is `-lglfw -lGLEW -lGLU -lGL` (to compile project without `pkg-config`)

# Compilation
1. `cd algine`
2. `chmod +x build/make.sh`
3. `./build/make.sh` - to run on standard videocard,`./build/make.sh primus` - to run on second videocard using `primusrun`, `./build/make.sh opti` - to run on second videocard using `optirun`

# Contact
You can contact me by telegram or email:
 * https://t.me/congard
 * dbcongard@gmail.com

# You can support me financially
<b>Payeer:</b> P36281059