md5 = require "md5"

class PrintHash
    new: =>
        @hash = md5.sumhexa("1234")

    printer: =>
        print("The hash is " .. @hash)

printHash = PrintHash()
printHash\printer()
