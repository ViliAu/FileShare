#include "connector.h"

int initialize_winsocks() {
    int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (initResult != 0) {
        printf("WSAStartup failed: %d\n", initResult);
        return 1;
    }
    return 0;
}

/* TODO: cleanup */
/* 
    Create a socket for a sending client

    address: Address to connect to
    port: Port to connect to
    tcp: Use TCP or UDP protocol? (0 = udp : 1 = tpc)
    ipv4: Use IPv4 or IPv6 (0 = IPv4 : 1 = IPv6)

    returns: SOCKET if successful, INVALID SOCKET (-1) if unsuccessful
 */
SOCKET create_client_socket(char* address, char* port, int tcp, int ipv4) {
    /* Addrinfo: https://learn.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa */
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    /* Fills a block of memory with zeros. */
    ZeroMemory(&hints, sizeof(hints));

    /* IPv4 or IPv6 */
    hints.ai_family = ipv4 ? AF_INET : AF_INET6;

    /* TCP or UDP */
    hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;
    hints.ai_protocol = tcp ? IPPROTO_TCP : IPPROTO_UDP;

    // Resolve the server address and port
    int initResult = 0;
    initResult = getaddrinfo(address, port, &hints, &result);
    if (initResult != 0) {
        printf("getaddrinfo failed: %d\n", initResult);
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    /* Attempt to connect to the first address returned by the call to getaddrinfo */
    /* TODO try to connect to multiple addresses */
    ptr=result;

    // Create a SOCKET for connecting to server
    SOCKET s;
    s = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (s == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Connect to server
    initResult = connect(s, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (initResult == SOCKET_ERROR) {
        closesocket(s);
        s = INVALID_SOCKET;
        return INVALID_SOCKET;
    }

    /* TODO: try the next address returned by getaddrinfo if the connect call failed */

    /* Free address information (no longer needed) */
    freeaddrinfo(result);

    if (s == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return INVALID_SOCKET;
    }
    return s;
}

/* TODO: cleanup */
/* 
    Create a socket for the receiving host and bind it to a port

    port: Port to bind the socket to
    tcp: Use TCP or UDP protocol? (0 = udp : 1 = tpc)
    ipv4: Use IPv4 or IPv6 (0 = IPv4 : 1 = IPv6)

    returns: SOCKET if successful, INVALID SOCKET (-1) if unsuccessful
 */
SOCKET create_host_socket(char* port, int tcp, int ipv4) {
    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof (hints));
    /* IPv4 or IPv6 */
    hints.ai_family = ipv4 ? AF_INET : AF_INET6;
    /* TCP or UDP */
    hints.ai_socktype = tcp ? SOCK_STREAM : SOCK_DGRAM;
    hints.ai_protocol = tcp ? IPPROTO_TCP : IPPROTO_UDP;

    hints.ai_flags = AI_PASSIVE;

    int initResult = 0;
    // Resolve the local address and port to be used by the server
    initResult = getaddrinfo(NULL, port, &hints, &result);
    if (initResult != 0) {
        printf("getaddrinfo failed: %d\n", initResult);
        WSACleanup();
        return INVALID_SOCKET;
    }

    // Create a socket for listening to connections
    SOCKET s = INVALID_SOCKET;
    s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (s == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return INVALID_SOCKET;
    }

    /* Bind socket to port */
    initResult = bind(s, result->ai_addr, (int)result->ai_addrlen);
    if (initResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        close_socket(s);
        return INVALID_SOCKET;
    }
    /* Free address information (no longer needed) */
    freeaddrinfo(result);

    return s;
}

int listen_socket(SOCKET s, int max_conns) {
    if (listen(s, max_conns) == SOCKET_ERROR) {
        printf( "Listen failed with error: %ld\n", WSAGetLastError());
        close_socket(s);
        return 1;
    }
}

SOCKET accept_connection(SOCKET listen_socket) {
    SOCKET client_socket = INVALID_SOCKET;

    // Accept a client socket
    client_socket = accept(listen_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        close_socket(listen_socket);
        return INVALID_SOCKET;
    }
    return client_socket;
}

int send_data(SOCKET s, unsigned char* buffer, int buff_len, int flags) {
    int iResult = 0;
    iResult = send(s, buffer, (int) strlen(buffer), flags);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        close_socket(s);
        return -1;
    }
    return iResult;
}

int receive_data(SOCKET s, unsigned char* buffer, int buff_len, int flags) {
    int iResult = 0;
    iResult = recv(s, buffer, buff_len, flags);
    if (iResult == -1) {
        printf("recv failed: %d\n", WSAGetLastError());
    }
    return iResult;
}

void close_socket(SOCKET s) {
    closesocket(s);
    WSACleanup();
}