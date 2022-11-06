#include <stdio.h>
#include "connector.h"

int main(int argc, char **argv) {
    initialize_winsocks();
    SOCKET client = create_client_socket("37.136.115.77", "6666", 1, 1);

    //send_data(client, "Hello world!", (int)strlen("Hello world!"), 0);
    int iresult = send(client, "Hello world!", (int)strlen("Hello world!"), 0);
    printf("%d", iresult);
    close_socket(client);
    return 0;
}