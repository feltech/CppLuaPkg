print("LUA MODULES:\n",package.path,"\n\nC MODULES:\n",package.cpath,"\n\n")

require "hash.printhash"
print("Printed hash")
http = require "socket.http"
print("Loaded socket")

body, status, headers, statusline = http.request("https://www.google.com")
print(statusline)

require("lsqlite3")
print("Loaded sqlite3")

local db = sqlite3.open_memory()

db:exec[[
  CREATE TABLE test (id INTEGER PRIMARY KEY, content);
  INSERT INTO test VALUES (NULL, 'Hello World');
  INSERT INTO test VALUES (NULL, 'Hello Lua');
  INSERT INTO test VALUES (NULL, 'Hello Sqlite3')
]]

for row in db:nrows("SELECT * FROM test") do
    print(row.id, row.content)
end

print("Finished!")
