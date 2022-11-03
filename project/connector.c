/* Ignore winsocket1 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

/* Link DLLs */
#pragma comment(lib,"WS2_32.lib")

static WSADATA wsaData = NULL;

void setupWinSocks() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", initResult);
        getchar();
        wsaData = NULL;
    }
}