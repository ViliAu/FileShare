#include "connector.h"
#include "filehandler.h"

char* get_file_name(char* path);
void send_file_size(long file_size);
void send_filename(char* fname);
int send_chunk(char* buffer, int buffer_size);
void close_application(int errcode);

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

    long progress = 0;
    long start = time(NULL);
    int bytes_read = 0;
    unsigned char buffer[BUFF_LEN];
    /* TODO Cleanup */
    /*while (progress > 0) {
        int bytes_read = fread(buffer, 1, min(BUFF_LEN, size), input);
        int i = send_chunk(buffer, bytes_read);

        progress -= i;
        printf("%d\n", progress);
        if (time(NULL) > start) {
            printf("\rSending files, %.2f%%", ((double)(size-progress) / (double)size * 100));
            start = time(NULL);
        }
    }*/
    while (progress < size) {
        int bytes_read = fread(buffer, 1, BUFF_LEN, input);
        int bytes_sent = send(client, buffer, bytes_read, 0);

        progress += bytes_sent;
        if (time(NULL) > start) {
            printf("\rSending files, %.2f%%", ((double)(progress) / (double)size * 100));
            start = time(NULL);
        }
        memset(buffer, 0, sizeof(buffer));
    }
    printf("Done.\n");

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
    printf("bufsize: %d\n", buffer_size);
    int i = 0;
    while (i < buffer_size) {
        int bytes = send(client, &buffer[i], min(BUFF_LEN, buffer_size - i), 0);
        printf("Bytes sent: %d\n", bytes);
        if (bytes < 0) {
            printf("Error sending data to server. Terminating.\n");
            close_application(1);
        }
        /*int bytes_passed;
        bytes = recv(client, (char*)&bytes_passed, BUFF_LEN, 0);
        printf("Bytes actually passed: %d\n", bytes_passed);*/
        i += bytes;
    }
    return i;
} 