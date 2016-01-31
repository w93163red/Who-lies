lsof -i:10000
killall server
cc server.c -lpthread -o server
./server
