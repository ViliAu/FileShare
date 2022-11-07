#include "connector.h"
#include "filehandler.h"

char* get_file_name(char* path);
void send_file_size(long file_size);
void send_filename(char* fname);
void send_file();
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
    unsigned char buffer[BUFF_LEN];
    while (fread(buffer, 1, sizeof(buffer), input) > 0) {
        int i = send(client, buffer, sizeof(buffer), 0);
        if (i == -1) {
            printf("Sending failed!");
            close_application(1);
        }

        progress += sizeof(buffer);
        if (time(NULL) > start) {
            printf("\rSending files, %.2f%%", (double)((double)progress / (double)size * 100));
            start = time(NULL);
        }
    }
    // Send terminating packet
    send(client, 0, 0, 0);
    printf("\rSending files, 100%%    \n");

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