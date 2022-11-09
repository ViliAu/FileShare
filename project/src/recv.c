#include <stdio.h>
#include "connector.h"
#include "filehandler.h"



SOCKET init_conn();
long wait_for_file_size(SOCKET s);
void wait_for_file_name(SOCKET s, char** dest_ptr);
int recv_chunk(SOCKET s, char* buffer, int buffer_size);

int main() {
    initialize_winsocks();
    SOCKET host = create_host_socket(DEFAULT_PORT, 1, 1);
    listen_socket(host, 1);
    printf("Listening to socket\n");

    SOCKET client = accept_connection(host);
    // First wait for file size
    long size = wait_for_file_size(client);
    // Then wait for file name
    char* file_name;
    wait_for_file_name(client, &file_name);

    // Get file size string
    char* file_size;
    trunc_file_size(size, &file_size);

    // Print details
    printf("Downloading %s - %s\n", file_name, file_size);
    free(file_size);

    // Check if file exists
    /*char file_copy[256];
    strcat(file_copy, file_name);
    if (check_file_exists(file_name)) {
        strcat(file_copy, " (copy)");
    }*/

    // Open file output
    FILE *output = fopen(file_name, "wb");
    if (output == NULL) {
        perror("Error opening file");
        close_socket(client);
        close_socket(host);
        fclose(output);
        return 1;
    }
    free(file_name);

    long progress = size;
    int bytes_received = 0;
    unsigned char buffer[BUFF_LEN];
    long start = time(NULL);
    while(progress > 0) {
        bytes_received = (recv_chunk(client, buffer, BUFF_LEN));
        if(bytes_received <= 0) {
            break;
        }
        fwrite(buffer, 1, bytes_received, output);
        progress -= bytes_received;
        if (time(NULL) > start) {
            printf("\rReceiving files, %.2f%%", ((double)(size-progress) / (double)size * 100));
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

void wait_for_file_name(SOCKET s, char** dest_ptr) {
    unsigned char buffer[512] = {0};
    int bytes = recv(s, buffer, BUFF_LEN, 0);
    *dest_ptr = malloc(strlen(buffer)+1);
    strcpy(*dest_ptr, buffer);
}

int recv_chunk(SOCKET s, char* buffer, int buffer_size) {
    int i = 0;
    printf("buffer: %d\n", buffer_size);
    int lc = 0; // Loop count
    while(i < buffer_size) {
        lc++;
        int recv_bytes = recv(s, &buffer[i], min(buffer_size, BUFF_LEN), 0);
        if (lc == 1 || lc % 10 == 0) {
            printf("%d\n", recv_bytes);
        }
        if (recv_bytes <= 0 || lc > 100) {
            return i;
        }
        i += recv_bytes;
    }
    return i;
}