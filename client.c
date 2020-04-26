#include "common_socket.h"
#include <stdio.h>

#define RESPONSE_MAX_LEN    1024

int main(int argc, char *argv[]) {
    socket_t socket_client;
    const char *host = argv[1];
    const char *port = argv[2];
    const char *request = argv[3];
    char response[RESPONSE_MAX_LEN];

    socket_create(&socket_client, host, port);

    socket_send(&socket_client, request, strlen(request));
    printf("Sent request to server: %s\n", request);

    socket_receive(&socket_client, response, strlen(response));
    printf("Received response from server: %s\n", response);

    printf("Shutting down socket...\n");
    socket_shutdown(&socket_client, SHUT_RDWR);
    socket_close(&socket_client);
    return 0;
}

