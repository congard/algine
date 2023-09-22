local handle

if isUnix() then
    handle = assert(io.popen(arg[1] .. " -E -x c++ - -v < /dev/null |& cat", "r"))
elseif isWindows() then
    -- On Windows, you must enclose your command line (program + arguments) in additional outer-level quotes:
    -- https://stackoverflow.com/questions/53452818/lua-io-popen-run-program-with-space-in-path

    -- 2>&1 redirects stderr to stdout
    -- findstr is used to filter and display the output
    -- /R processes search strings as regular expressions
    -- /C:"^.*$" used to match any line
    -- https://learn.microsoft.com/en-us/windows-server/administration/windows-commands/findstr
    handle = assert(io.popen("\"\"" .. arg[1] .. "\" -E -x c++ - -v < NUL 2>&1 | findstr /R /C:\"^.*$\"\"", "r"))
else
    error("Unsupported OS")
end

local result = handle:read("*a")
handle:close()

local _, endIndex = result:find("#include <...> search starts here:")
local lineStart = endIndex + 2  -- skip : and \n

while true do
    local lineEnd = result:find("\n", lineStart)
    local line = result:sub(lineStart, lineEnd - 1)  -- +1 because of space in the beginning
    lineStart = lineEnd + 1  -- skip \n

    if line == "End of search list." then
        break
    end

    print(line:sub(2))
end
