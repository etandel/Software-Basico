#!/usr/bin/env lua

local lfs = require 'lfs'
local ml  = require 'ml'

module('sbtest', package.seeall)

local base = lfs.currentdir()
local exec = base .. '/test '

local find = string.find
local function test_sb(filename)
    local sb_out = io.popen(exec .. filename, 'r')
    local ret = sb_out:read()
    if not ret then
        print("Error on " .. filename .. '\n')
    elseif not filename:find(ret, 1, true) then
        print(filename .. '\n\t' .. ret)
    end
end

local function exec_tests(path)
    for file in lfs.dir(path) do
        if not ('%.$'):match(file) then
            local f = path..'/'..file
            local mode = lfs.attributes(f).mode
            if mode == 'directory' then
                exec_tests(f)
            else
                test_sb(f)
            end
        end
    end
end

exec_tests(base .. '/tests')

--[[
#!/bin/bash

base=.
if [ -x $base/test ]; then
    find $base/tests -type f \
        -exec echo -e "\nTestando {}" \; \
        -exec $base/test {} \;
else
    echo "Erro: Executavel $base/test nao existe."
fi
--]]

