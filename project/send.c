#include "connector.h"
#include "filehandler.h"

int send_file_size(SOCKET s, FILE* f);
int send_filename(SOCKET s, char* path);
void send_file();

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s [path]", argv[0]);
        return 0;
    }
    FILE *input = fopen(argv[1], "rb");
    if (input == NULL) {
        perror("Error opening file");
        return 1;
    }

    initialize_winsocks();
    SOCKET client = create_client_socket("127.0.0.1", DEFAULT_PORT, 1, 1);

    int iresult = send_file_size(client, input);
    if (iresult < sizeof(long)) {
        printf("Error sending data to server. Terminating.\n");
        close_socket(client);
        fclose(input);
        return 1;
    }

    long size = calc_file_size(input);
    char* file_size;
    trunc_file_size(size, &file_size);
    printf("Sending file ... - %s\n", file_size);
    free(file_size);

    long progress = 0;
    long start = time(NULL);
    unsigned char buffer[BUFF_LEN];
    while (fread(buffer, 1, sizeof(buffer), input) > 0) {
        int i = send(client, buffer, sizeof(buffer), 0);
        if (i == -1) {
            printf("Sending failed!");
            close_socket(client);
            fclose(input);
            return 1;
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

    close_socket(client);
    fclose(input);
    return 0;
}

int send_file_size(SOCKET s, FILE* f) {
    long file_size = calc_file_size(f);
    printf("File size: %d\n", file_size);
    int iresult = send(s, (char *)&file_size, sizeof(long), 0);
    return iresult;
}

int send_filename(SOCKET s, char* path) {
    // Get filename from path
    char* token = strtok(path, "\\");
    char* last;
    while (token != NULL) {
        last = token;
        token = strtok(NULL, "\\");
    }

    int iresult = send(s, last, sizeof(last), 0);
    return iresult;
}