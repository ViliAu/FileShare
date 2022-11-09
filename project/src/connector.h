#ifndef CONNECTOR_H
	#define CONNECTOR_H

	/* Ignores Winsockets 1 import if importing windows.h */
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif

	/* Define the minimum os version the code supports */
	// Windows XP
	//#define _WIN32_WINNT 0x0501

	/* Cross-platform todo */
	/*
		#ifdef __WIN32__
		# include <winsock2.h>
		#else
		# include <sys/socket.h>
		#endif

		#ifdef __WIN32__
		WORD versionWanted = MAKEWORD(1, 1);
		WSADATA wsaData;
		WSAStartup(versionWanted, &wsaData);
		#endif
	*/


	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <stdio.h>

	/* Compiler directive to add library dependencies */
	//#pragma comment(lib, "WS2_32.lib")

	/* Constants */
	#define DEFAULT_PORT "6666"
	#define DEFAULT_ADDR "127.0.0.1"
	#define MAX_CONNECTIONS 1 /* maximum length of the queue of pending connections to accept */
	#define BUFF_LEN 5120

	/* Contains information about windows sockets implementation */
	static WSADATA wsaData;

	int initialize_winsocks();

	SOCKET create_client_socket(char* address, char* port, int tcp, int ipv4);
	SOCKET create_host_socket(char* port, int tcp, int ipv4);

	int listen_socket(SOCKET s, int max_conns);
	SOCKET accept_connection(SOCKET listen_socket);

	int send_data(SOCKET s, unsigned char* buffer, int buff_len, int flags);
	int receive_data(SOCKET s, unsigned char* buffer, int buff_len, int flags);

	void close_socket(SOCKET s);

#endif