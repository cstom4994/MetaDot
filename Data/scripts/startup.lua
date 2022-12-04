-- Copyright(c) 2022, KaoruXun All rights reserved.
-- runs on start of the engine
-- used to load scripts
tl = require("tl")
inspect = require("inspect")
require("lang")

assert(inspect({1, 2, 3, 4}) == "{ 1, 2, 3, 4 }")
assert(inspect(1) == "1")
assert(inspect("Hello") == '"Hello"')

runf("data/scripts/vec.lua")
METADOT_INFO(i18n("loaded_vec"))

content = i18n("welcome")

-- test miniz
-- print("test miniz")
-- local data3 = miniz.compress(content)
-- print(#data3)
-- local out3 = miniz.decompress(data3)
-- assert(out3 == content)

function starts_with(str, start) return str:sub(1, #start) == start end

function ends_with(str, ending)
    return ending == "" or str:sub(-#ending) == ending
end

corouts = {}

-- will call func in coroutine mode
-- call coroutine.yield(numberOfTicksToWait) 0 = means call next tick
-- coroutine.yield(-1) to stop immediately
function registerCoroutine(func, ticksToWait)
    ticksToWait = ticksToWait or 0
    c = {corou = coroutine.create(func), sleepy_ticks = ticksToWait}
    table.insert(corouts, c)
end

-- can be used only in coroutines
-- will wait for specified number of game ticks
--  0 -> returns immediately
function waitFor(ticks) if ticks > 0 then coroutine.yield(ticks - 1) end end

-- can be used only in coroutines
-- stops thread and never returns
function exitThread() coroutine.yield(-1) end

function dump(o)
    if type(o) == 'table' then
        local s = '{ '
        for k, v in pairs(o) do
            if type(k) ~= 'number' then k = '"' .. k .. '"' end
            s = s .. '[' .. k .. '] = ' .. dump(v) .. ','
        end
        return s .. '} '
    else
        return tostring(o)
    end
end
