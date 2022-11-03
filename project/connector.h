#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef CONNECTOR_H
#define CONNECTOR_H
	#define _WIN32_WINNT 0x0501

	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <stdio.h>

	#pragma comment(lib,"WS2_32.lib")

	#define DEFAULT_PORT "6666"
	#define MAX_CONNECTIONS 2
	#define BUFF_LEN 512

	initialize_winsocks();
	
	
#endif