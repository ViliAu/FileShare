#include "connector.h"
#include "filehandler.h"
#include <time.h>

#define CLOCK_INTERVAL 1000
char* get_file_name(char* path);
void send_file_size(long file_size);
void send_filename(char* fname);
int send_chunk(char* buffer, int buffer_size);
void close_application(int errcode);
float calc_remaining_time(int size, int progress, int last_progress, long time);

static SOCKET client;
static FILE* input;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s [path]", argv[0]);
        return 0;
    }
    input = fopen(argv[1], "rb");
    if (input == NULL) {
        perror("Error opening file");
        return 1;
    }

    // IP Query
    char ip[50];
    printf("Enter address: ");
    gets(ip);
    if (strlen(ip) == 0) {
        strcat(ip, DEFAULT_ADDR);
    }

    initialize_winsocks();
    printf("Trying to connect to address %s ...\n", ip);
    client = create_client_socket(ip, DEFAULT_PORT, 1, 1);

    long size = calc_file_size(input);
    char* file_name = get_file_name(argv[1]);

    send_file_size(size);
    send_filename(file_name);
    

    char* file_size;
    trunc_file_size(size, &file_size);
    printf("Sending %s - %s\n", file_name, file_size);
    free(file_size);

    time_t start, stop;
    long progress = 0;
    long last_progress = 0;
    int bytes_read = 0;
    int bytes_sent = 0;
    unsigned char buffer[BUFF_LEN];

    Sleep(1000);
    start = clock();
    stop = clock();
    while (progress < size) {
        bytes_read = fread(buffer, 1, min(BUFF_LEN, size-progress), input);
        bytes_sent = send_chunk(buffer, bytes_read);
        progress += bytes_sent;
        if (clock() > (stop + CLOCK_INTERVAL)) {
            float remaining_time = calc_remaining_time(size, progress, last_progress, clock() - stop);
            printf("\rSending files, %.2f%%, estimated time: %.2fs                      ", ((double)(progress) / (double)size * 100), remaining_time);
            stop = clock();
            last_progress = progress;
        }
    }
    stop = clock();
    printf("\rSending files, 100%%\n                                                     ");
    printf("Elapsed time: %.2fs", (stop-start)*0.001);
    printf("Waiting for server ack...\n");
    char i[1];
    int bytes = recv(client, i, BUFF_LEN, 0);
    printf("Press enter to quit...");
    getchar();

    close_application(0);
}

void send_file_size(long file_size) {
    printf("File size: %d\n", file_size);

    int iresult = send(client, (char *)&file_size, sizeof(long), 0);
    if (iresult < sizeof(long)) {
        printf("Error sending data to server. Terminating.\n");
        close_application(1);
    }
}

char* get_file_name(char* path) {
    // Get filename from path
    char* token = strtok(path, "\\");
    char* last;
    while (token != NULL) {
        last = token;
        token = strtok(NULL, "\\");
    }
    return last;
}

void send_filename(char* fname) {
    int iresult = send(client, fname, strlen(fname)+1, 0);
    if (iresult < 0) {
        printf("Error sending data to server. Terminating.\n");
        close_application(1);
    }
}

void close_application(int errcode) {
    close_socket(client);
    fclose(input);
    exit(errcode);
}

int send_chunk(char* buffer, int buffer_size) {
    int tot = 0;
    while (tot < buffer_size) {
        int bytes = send(client, &buffer[tot], buffer_size-tot, 0);
        if (bytes < 0) {
            printf("Error sending data to server. Terminating.\n");
            close_application(1);
        }
        tot += bytes;
    }
    return tot;
}

float calc_remaining_time(int size, int progress, int last_progress, long time) {
    int remaining = size-progress;
    int prog = progress-last_progress;
    return (float)(remaining / prog) * time * 0.001;
}