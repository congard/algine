# Linux Build Instructions

The following tools will be needed:

1. C/C++ compiler
2. Git
3. CMake

> [!NOTE]
> The recommended C/C++ compiler is Clang. But the project also should
> compile with GCC.

> [!NOTE]
> The recommended linker is [mold](https://github.com/rui314/mold).
> Mold is a Modern Linker which supports multithreading. It is incredibly fast.

## Fedora

```bash
sudo dnf install mesa-libGL-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel
```

Tested on Fedora 36, Xorg session.

## Debian

```bash
sudo apt build-dep glfw3
sudo apt build-dep libglew2.1 # recommended, but you can also try  sudo apt install libglu1-mesa-dev

sudo apt install xorg-dev
sudo apt install libminizip-dev

sudo ln -s /usr/lib/x86_64-linux-gnu/libGL.so /usr/lib/libGL.so
sudo ln -s /usr/lib/x86_64-linux-gnu/libGLU.so /usr/lib/libGLU.so
sudo ln -s /usr/lib/x86_64-linux-gnu/librt.so /usr/lib/librt.so
sudo ln -s /usr/lib/x86_64-linux-gnu/libz.so /usr/lib/libz.so
sudo ln -s /usr/lib/x86_64-linux-gnu/libm.so /usr/lib/libm.so
sudo ln -s /usr/lib/x86_64-linux-gnu/libX11.so /usr/lib/libX11.so
sudo ln -s /usr/lib/x86_64-linux-gnu/libSM.so /usr/lib/libSM.so
sudo ln -s /usr/lib/x86_64-linux-gnu/libICE.so /usr/lib/libICE.so
sudo ln -s /usr/lib/x86_64-linux-gnu/libXext.so /usr/lib/libXext.so
```

<br>

Note: even if you run into errors like `No rule to make target '/usr/lib/libNAME.so'`, 
you must create symlinks to problem libraries:

```bash
sudo ln -s /usr/lib/x86_64-linux-gnu/libNAME.so /usr/lib/libNAME.so
```

Tested on Debian 10 and 11, Xorg session.
