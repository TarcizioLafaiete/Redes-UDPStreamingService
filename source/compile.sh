gcc src/server.c src/socketUtils.c src/serverCore.c src/linked_list.c -o server
gcc src/client.c src/socketUtils.c src/clientCore.c src/terminalPrinter.c -o client