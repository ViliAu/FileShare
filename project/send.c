#include <stdio.h>
#include "connector.h"

int main(int argc, char **argv) {
    initialize_winsocks();
    SOCKET client = create_client_socket("127.0.0.1", "6666", 1, 1);

    //send_data(client, "Hello world!", (int)strlen("Hello world!"), 0);
    send(client, "Hello world!", (int)strlen("Hello world!"), 0);
    close_socket(client);
    printf("ASD");
    return 0;
}