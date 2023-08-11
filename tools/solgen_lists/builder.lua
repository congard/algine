--- This script is responsible for building lists

local typesInfo = require("types")
local types = typesInfo.types
local minimumRequiredTypes = typesInfo.minimumRequiredTypes

ListMode = {
    Deny = 0,
    Allow = 1
}

local listMode = ListMode.Allow
local list = {}

local function inverseList()
    local invList = {}

    for key, _ in pairs(types) do
        if list[key] == nil then
            invList[key] = true
        end
    end

    list = invList
end

--- Sets the current list mode.
--- Allowed `mode` values:
--- * `ListMode.Deny`
--- * `ListMode.Allow`
function setListMode(mode)
    listMode = mode
end

--- Adds the specified elements representing
--- class names to the list.
function add(...)
    local elements = {...}
    for _, value in ipairs(elements) do
        list[value] = true
    end
end

--- Clears the list.
function clear()
    list = {}
end

--- Prints *allow list* constructed from the `list`
--- Format:
--- * 1st line: class names
--- * 2nd line: file paths
local function printList()
    if listMode == ListMode.Deny then
        inverseList()
    end

    -- minimum required types must be included anyway
    for _, v in ipairs(minimumRequiredTypes) do
        list[v] = true
    end

    -- Worth reading: http://lua-users.org/wiki/TernaryOperator

    local listSize = 0

    for _, _ in pairs(list) do
        listSize = listSize + 1
    end

    local counter = 0

    -- print names first
    for key, _ in pairs(list) do
        counter = counter + 1
        io.write(key .. (counter ~= listSize and "," or ""))
    end

    io.write(";")

    counter = 0

    for key, _ in pairs(list) do
        counter = counter + 1
        io.write(types[key].file .. (counter ~= listSize and "," or ""))
    end
end

-- execute user lists script
dofile(arg[1])

printList()
