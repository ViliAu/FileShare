all: test

test: connector.o
	gcc test.c -o test connector.o -lWs2_32

connector: connector.c connector.h
	gcc -c ./src/connector.c

