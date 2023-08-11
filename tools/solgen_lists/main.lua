--- This script helps you to build only the necessary
--- Lua bindings. It will be automatically launched
--- by the CMake script.
--- It can work as an allow list or deny list.
--- Some notes:
--- * Currently it will not automatically figure out
---   all the needed dependencies for the specified
---   classes, so you must take care of them by yourself;
--- * By default all the bindings are **disabled**;
--- * This script must be launched from its parent
---   directory.

local function printHelp()
    print("Usage:")
    print("help", "", "Print this message")
    print("[file.lua]", "Print lists using the specified builder")
end

if arg[1] == nil or arg[1] == "help" then
    printHelp()
else
    require("builder")
end
