dofile("dist.info")
cmake_list =''
for i, v in pairs(depends) do
    cmake_list=cmake_list .. string.gmatch(v, "%S+")() .. ";"
end
io.write(cmake_list)