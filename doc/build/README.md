# Build Instructions

After cloning, you must follow some instructions to be able to compile your project with algine.

## Dependencies

Some dependencies are already included into codebase, but some of them you need to download using git submodules:

`git submodule update --init --recursive`

## Options

| Option                      | Description                           | Required  |
|-----------------------------|---------------------------------------|-----------|
| ALGINE_SECURE_OPERATIONS    | enables Algine SOP                    | No        |
| ALGINE_SOP_LEVEL            | sets SOP level                        | No        |
| ALGINE_LUA_PATH             | path to Lua interpreter executable    | No        |
| ALGINE_SOLGEN_PATH          | path to solgen executable             | No        |
| ALGINE_SOLGEN_ARGS          | additional solgen args                | No        |
| ALGINE_SOLGEN_CLANG_ARGS    | additional solgen clang args          | No        |
| ALGINE_SOLGEN_INPUT_CORE    | relative paths to core files          | No        |
| ALGINE_SOLGEN_INPUT_STD     | relative paths to std files           | No        |
| ALGINE_SOLGEN_CORE_CLASSES  | list of core classes                  | No        |
| ALGINE_SOLGEN_STD_CLASSES   | list of std classes                   | No        |
| ALGINE_QT_PLATFORM          | switches to Qt platform               | No        |

## Platform-specific steps

* [Linux](Linux.md)
* [Windows](Windows.md)
