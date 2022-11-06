#include <stdio.h>
#include "connector.h"
#include "filehandler.h"

SOCKET init_conn();
long wait_for_file_size(SOCKET s);

int main() {
    FILE *output = fopen("output", "wb");
    if (output == NULL) {
        perror("Error opening file");
        return 1;
    }

    initialize_winsocks();
    SOCKET host = create_host_socket(DEFAULT_PORT, 1, 1);
    listen_socket(host, 1);
    printf("Listening to socket\n");

    SOCKET client = accept_connection(host);

    // First wait for file size
    long size = wait_for_file_size(client);
    long progress = 0;
    unsigned char buffer[BUFF_LEN];
    long start = time(NULL);
    while((recv(client, buffer, BUFF_LEN, 0)) > 0) {
        fwrite(buffer, 1, sizeof(buffer), output);
        progress += sizeof(buffer);
        if (time(NULL) > start) {
            printf("\rReceiving files, %.2f%%", (double)((double)progress / (double)size * 100));
            start = time(NULL);
        }
    }
    printf("\rReceiving files, 100%%    \n");

    close_socket(client);
    close_socket(host);
    fclose(output);
    return 0;
}

/* TODO: Error checking 
SOCKET init_conn() {
    initialize_winsocks();
    SOCKET host = create_host_socket(DEFAULT_PORT, 1, 1);
    listen_socket(host, 1);
    printf("Listening to socket\n");

    SOCKET client = accept_connection(host);
    return client;
}*/

long wait_for_file_size(SOCKET s) {
    long l;
    int bytes = recv(s, (char*)&l, BUFF_LEN, 0);
    return l;
}