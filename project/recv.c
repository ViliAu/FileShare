#include <stdio.h>
#include "connector.h"

int main() {

    char recv_buffer[BUFF_LEN] = {0};

    initialize_winsocks();
    SOCKET host = create_host_socket("6666", 1, 1);
    listen_socket(host, 1);
    printf("Listening to socket\n");
    SOCKET client = accept_connection(host);

    //int bytes = receive_data(client, recv_buffer, BUFF_LEN, 0);
    int bytes = recv(client, recv_buffer, BUFF_LEN, 0);
    printf("%d %s\n", bytes, recv_buffer);
    close_socket(client);
    close_socket(host);
    return 0;
}