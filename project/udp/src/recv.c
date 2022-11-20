#include "connector.h"
#include "filehandler.h"
#include <time.h>

#define CLOCK_INTERVAL 1000
SOCKET init_conn();
long wait_for_file_size(SOCKET s);
void wait_for_file_name(SOCKET s, char** dest_ptr);
int recv_chunk(SOCKET s, char* buffer, int buffer_size);
float calc_remaining_time(int size, int progress, int last_progress, long time);

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

    long progress = 0;
    long last_progress = 0;
    int bytes_received = 0;
    unsigned char buffer[BUFF_LEN];
    clock_t start, stop;
    start = clock();
    stop = clock();
    while(progress < size) {
        //bytes_received = recv_chunk(client, buffer, min(BUFF_LEN, size-progress));
        bytes_received = recv(client, buffer, min(BUFF_LEN, size-progress), 0);
        if(bytes_received <= 0) {
            if (bytes_received < 0) {
                printf("Error at socket(): %ld\n", WSAGetLastError());
            }
            break;
        }
        if (fwrite(buffer, 1, bytes_received, output) == -1) {
            perror("Error writing to file");
            break;
        };
        progress += bytes_received;
        if (clock() > (stop + CLOCK_INTERVAL)) {
            float remaining_time = calc_remaining_time(size, progress, last_progress, clock() - stop);
            printf("\rReceiving files, %.2f%%, estimated time: %.2fs                      ", ((double)(progress) / (double)size * 100), remaining_time);
            stop = clock();
            last_progress = progress;
        }
    }
    stop = clock();
    int bytes = send(client, (char*)1, 1, 0);
    printf("\rReceiving files, 100%%                                             \n");
    printf("Elapsed time: %.2fs", (stop-start)*0.001);
    close_socket(client);
    close_socket(host);
    fclose(output);
    printf("Press enter to quit...");
    getchar();
    return 0;
}

long wait_for_file_size(SOCKET s) {
    long l;
    int bytes = recv(s, (char*)&l, BUFF_LEN, 0);
    return l;
}

void wait_for_file_name(SOCKET s, char** dest_ptr) {
    unsigned char buffer[BUFF_LEN] = {0};
    int bytes = recv(s, buffer, BUFF_LEN, 0);
    *dest_ptr = malloc(strlen(buffer)+1);
    strcpy(*dest_ptr, buffer);
}

int recv_chunk(SOCKET s, char* buffer, int buffer_size) {
    int tot = 0;
    while(tot < buffer_size) {
        int recv_bytes = recv(s, &buffer[tot], buffer_size-tot, 0);
        if (recv_bytes <= 0) {
            return tot;
        }
        int sent_bytes = send(s, (char *)&recv_bytes, sizeof(int), 0);
        tot += recv_bytes;
    }
    return tot;
}

float calc_remaining_time(int size, int progress, int last_progress, long time) {
    int remaining = size-progress;
    int prog = progress-last_progress;
    return (float)(remaining / prog) * time * 0.001;
}