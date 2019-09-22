print("LUA MODULES:\n",package.path,"\n\nC MODULES:\n",package.cpath,"\n\n")

require "hash.printhash"
http = require "socket.http"

body, status, headers, statusline = http.request("https://www.google.com")
print(statusline)

require("lsqlite3")
print("Loaded SQLite")
local db = sqlite3.open_memory()

db:exec[[
  CREATE TABLE test (id INTEGER PRIMARY KEY, content);
  INSERT INTO test VALUES (NULL, 'Hello World');
]]

for row in db:nrows("SELECT * FROM test") do
    print(row.id, row.content)
end
