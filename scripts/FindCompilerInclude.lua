if arg[2] ~= "unix" and arg[2] ~= "android" then
    print("You are trying to run this script on a temporary unsupported system. Exiting...")
    os.exit(-1)
end

local handle = assert(io.popen(arg[1] .. " -E -x c++ - -v < /dev/null |& cat", "r"))
local result = handle:read("*a")
handle:close()

local startIndex, endIndex = result:find("#include <...> search starts here:")
local lineStart = endIndex + 2  -- skip : and \n

while true do
    local lineEnd = result:find("\n", lineStart)
    local line = result:sub(lineStart, lineEnd - 1)  -- +1 because of space in the beggining
    lineStart = lineEnd + 1  -- skip \n

    if line == "End of search list." then
        break
    end

    print(line:sub(2))
end
