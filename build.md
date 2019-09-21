# Building
The engine is currently tested on Linux with gcc 9.1.0-2 and Windows 10 with MinGW (gcc 8.1.0 x86_64-posix-seh-rev0, Built by MinGW-W64 project)
<br>Note: only the GNU compiler is recommended. Using MSVC you will not able to compile a project.

# Building on Linux systems
1. The engine has next dependencies: `glew`, `glfw`, `assimp` which must be installed in the system.
   * Installation on Debian-based systems:
     <br>`sudo apt install libglew-dev libglfw3-dev`
   * Installation on ArchLinux-based systems:
     <br>x11: `pacman -S glew glfw-x11`
     <br>wayland: `pacman -S glew-wayland glfw-wayland`

   How to install assimp you can read [here](https://github.com/assimp/assimp/blob/master/Build.md)
2. You need to install the needed libraries.
   In `contrib` folder there is a Python script called `contrib.py`.
   You must execute it and install `glm`, `nlohmann/json.hpp`, `stb/stb_image.h`, `stb/std_image_write.h`.
   This script WILL NOT install libraries in System directories, ONLY in `contrib` folder!
   <br>This script MUST be launched directly from `contrib` folder!
3. Now you able to build engine via CMake

# Building on Windows
1. First, you must download and install [MinGW-w64](https://sourceforge.net/projects/mingw-w64/). This is required for compilation. Also you need to install [CMake](https://cmake.org/download/);
2. After successful installation, you need to add the `bin` folder to your `PATH`.
   For example, if you installed MinGW in `C:\mingw-w64`, then the path to the `bin` folder will look like this: `C:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin`.
   How to add a folder in the `PATH` you can read for example [here](https://docs.alfresco.com/4.2/tasks/fot-addpath.html);
3. You need to install the needed libraries.
   In `contrib` folder there is a Python script called `contrib.py`, which will install ALL libraries that needed.
   This script WILL NOT install libraries in System directories, ONLY in `contrib` folder!
   <br>This script MUST be launched directly from `contrib` folder!
4. After building will be used command `mklink`, which must be executed via administrator.
   This command is necessary so as not to increase the size of output directories and not to make unnecessary copying of libraries.
   <br>`mklink` will be executed if `-DALGINE_LINK_LIBS_STATICALLY=OFF`
   <br>So you need to run your IDE or CMake via admin or follow [this](https://superuser.com/questions/124679/how-do-i-create-a-link-in-windows-7-home-premium-as-a-regular-user) instruction
5. Building Algine example:
   1. `"C:\Path\To\Cmake\cmake.exe" -DCMAKE_BUILD_TYPE=Release_or_Debug -DALGINE_LINK_LIBS_STATICALLY=OFF -G "CodeBlocks - MinGW Makefiles" D:\Path\To\Algine`
   2. `"C:\Path\To\Cmake\cmake.exe" --build D:\Path\To\Algine\cmake-build-release --target algine -- -j 2`

If you have troubles with assimp, see [this](https://github.com/assimp/assimp/issues/2618)

---

If some of these instructions aren't clear enough, please notify me