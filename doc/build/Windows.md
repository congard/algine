# Windows Build Instructions

The following tools will be needed:

1. [MSVC](https://visualstudio.microsoft.com/downloads/)
2. [LLVM](https://releases.llvm.org/download.html)
3. [MSYS2](https://www.msys2.org/)
4. [Git](https://git-scm.com/downloads)
5. [CMake](https://cmake.org/download/)

> [!NOTE]
> It is highly recommended to install LLVM and MSYS2 to its default
> install paths. It will simplify building since you don't need to provide
> additional options.

> [!NOTE]
> MSYS2 is optional, but recommended. If you have MSYS2 installed, Algine
> will be able to perform dependency (GLEW) configuration step automatically.
> However, any other Unix environment can be used (e.g. WSL).
> Read more: https://github.com/nigels-com/glew#code-generation

> [!NOTE]
> In some IDEs (e.g. CLion, Visual Studio with installed CMake Tools)
> CMake is bundled by default.

## LLVM

In order to compile the project, you must use Clang compiler:

| Tool         | Binary      |
|--------------|-------------|
| C compiler   | clang.exe   |
| C++ compiler | clang++.exe |

By default, the aforementioned binaries are located in `$Env:ProgramW6432/LLVM`.

Tested on Windows 11, clang version 16.0.0, target: x86_64-pc-windows-msvc.

## MSYS2 packages

The following packages must be installed in order to use MSYS2 to complete
GLEW configuration:

1. `make`
2. `git`
3. `python` (python 3)

They can be installed in the following way:

```bash
pacman -S make git python
```

## GLEW: make OpenGL extensions

> [!NOTE]
> If you have MSYS2 installed in `$Env:SystemDrive/msys64` with all needed packages,
> you can skip this step. GLEW will be configured automatically. Note, that the
> process can take some time.

> [!NOTE]
> In order to [make GLEW](https://github.com/nigels-com/glew#code-generation)
> OpenGL extensions, you need to have a Unix or Mac environment. For example,
> it can be MSYS2, WSL, Linux distro etc.

```bash
cd $algine_root_folder/deps-win/glew-src
make extensions
```

## Symbolic links

Symlinks support is required in order to use Algine. Luckily, it can be enabled
by enabling Developer mode in the settings.

For Windows 11: Settings -> Privacy & security -> For developers -> Developer Mode (on).

## Options

| Option        | Description   | Required | Default                   |
|---------------|---------------|----------|---------------------------|
| `MSYS64_PATH` | path to MSYS2 | No       | `$Env:SystemDrive/msys64` |

## Notes

> [!NOTE]
> If you are a CLion user, it is recommended to enable terminal emulation in the output 
> console to correctly display colors. 
> See https://www.jetbrains.com/help/clion/terminal-in-the-output-console.html#enable

> [!NOTE]
> Warnings like
> ```
> lld-link: warning: undefined symbol: void __cdecl algine_lua::registerLuaUsertype<class algine::ShapeBuilder>(class sol::basic_table_core<0, class sol::basic_reference<0>> &, void *)
> >> referenced by D:\Development\Projects\cpp\algine\include\common\algine\core\lua\Lua.h:112
> ```
> mean that the specified classes are not listed in your `SolgenLists.lua`.
> Bindings for the classes mentioned in the warnings are not the minimum required.
> However, if you explicitly or implicitly try to use them from the Lua code,
> the engine will crash. This means that these warnings can be safely ignored until
> you use the bindings of the aforementioned classes.
