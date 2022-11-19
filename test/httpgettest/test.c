#include "connector.h"

int main() {
    initialize_winsocks();
    SOCKET s = create_client_socket("3.232.242.170", "80", 1, 1);
    char buffer[512] = {0};
    char* req = "GET /?format=json HTTP/1.1\r\nHost: api.ipify.org\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36\r\nAccept-Encoding: gzip,deflate,sdch\r\nAccept-Language: en-US,en;q=0.8\r\n\r\n";
    int bytes = send(s, req, strlen(req)+1, 0);
    int bytesr = recv(s, buffer, 512, 0);
    printf("%d, %d, %s", bytes, bytesr, buffer);
    close_socket(s);
    WSACleanup();
    getchar();
    return 0;
}