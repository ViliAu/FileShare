all: send recv

recv: connector.o filehandler.o
	gcc recv.c -o ../build/recv connector.o filehandler.o -lWs2_32

send: connector.o filehandler.o
	gcc send.c -o ../build/send connector.o filehandler.o -lWs2_32

connector: connector.c connector.h
	gcc -c ./src/connector.c

filehandler: filehandler.c filehandler.h
	gcc -c ./src/filehandler.c
