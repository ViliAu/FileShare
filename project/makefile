recv: connector.o filehandler.o
	gcc -o recv connector.o filehandler.o

send: connector.o filehandler.o
	gcc -o send connector.o filehandler.o

connector: connector.c connector.h
	gcc -c connector.c

filehandler: filehandler.c filehandler.h
	gcc -c filehandler.c
