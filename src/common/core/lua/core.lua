-- algine:core.lua

-- Returns true if t is array, otherwise false
function isArray(t)
    local i = 0

    for _ in pairs(t) do
        i = i + 1
        if t[i] == nil then return false end
    end

    return true
end

local lua_tostring = tostring

-- If obj is table, creates string representation
-- Otherwise calls default tostring function
tostring = function(obj)
    if type(obj) == 'table' then
        local s = '{'

        if isArray(obj) then
            for k, v in pairs(obj) do
                s = s .. tostring(v) .. ', '
            end
        else
            for k, v in pairs(obj) do
                if type(k) ~= 'number' then k = '"'..k..'"' end
                s = s .. '['.. k ..'] = ' .. tostring(v) .. ', '
            end
        end

        return string.sub(s, 1, #s - 2) .. '}'
    else
        return lua_tostring(obj)
    end
end

-- Calls custom tostring, which allows to print tables
print = function(...)
    local args = {...}
    for i=1,#args - 1 do io.write(tostring(args[i]) .. '\t') end
    io.write(tostring(args[#args]))
    io.write('\n')
end

local loadedScripts = {}

-- Custom require that uses IOSystem to load files
require = function(name)
    local function loadModule(key, loader)
        if loadedScripts[key] == nil then
            -- loader can return nil, so we must be sure that loadedScripts[key] is not nil
            loadedScripts[key] = {loader()}
        end

        return loadedScripts[key][1]
    end

    local algineStrLen = string.len("algine")

    if string.sub(name, 1, algineStrLen) == "algine" then
        return loadModule(name, function() algine_require(string.sub(name, algineStrLen + 2)) end)
    else
        for path in string.gmatch(package.path, "[^;]+") do
            path = path:gsub("?", name)

            if state:exists(path) then
                return loadModule(path, function() load(state:readStr(path))() end)
            end
        end

        state:printErr(("Module %s was not found in %s"):format(name, package.path))
    end

    return nil
end