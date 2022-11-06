recv: connector.o filehandler.o
	gcc recv.c -o recv connector.o filehandler.o -lWs2_32

send: connector.o filehandler.o
	gcc send.c -o send connector.o filehandler.o -lWs2_32

connector: connector.c connector.h
	gcc -c connector.c

filehandler: filehandler.c filehandler.h
	gcc -c filehandler.c
