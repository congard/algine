# Build Instructions

After cloning, you must follow some instructions to be able to compile your project with algine.

> [!NOTE]
> It is strongly recommended to use Clang on both Windows and Linux. 

## Dependencies

All needed dependencies will be automatically downloaded using FetchContent. 

## Options

| Option                       | Description                           | Required  |
|------------------------------|---------------------------------------|-----------|
| `ALGINE_SECURE_OPERATIONS`   | enables Algine SOP                    | No        |
| `ALGINE_SOP_LEVEL`           | sets SOP level                        | No        |
| `ALGINE_LUA_PATH`            | path to Lua interpreter executable    | No        |
| `ALGINE_SOLGEN_PATH`         | path to solgen executable             | No        |
| `ALGINE_SOLGEN_ARGS`         | additional solgen args                | No        |
| `ALGINE_SOLGEN_CLANG_ARGS`   | additional solgen clang args          | No        |
| `ALGINE_SOLGEN_INPUT_CORE`   | relative paths to core files          | No        |
| `ALGINE_SOLGEN_INPUT_STD`    | relative paths to std files           | No        |
| `ALGINE_SOLGEN_CORE_CLASSES` | list of core classes                  | No        |
| `ALGINE_SOLGEN_STD_CLASSES`  | list of std classes                   | No        |
| `ALGINE_QT_PLATFORM`         | switches to Qt platform               | No        |
| `ALGINE_DEPS_DIR_POSTFIX`    | sets postfix for dependencies dir     | No        |

> [!NOTE]
> For platform-specific flags see the corresponding files.

## Platform-specific steps

* [Linux](Linux.md)
* [Windows](Windows.md)
