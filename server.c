#include "common_socket.h"
#include <stdio.h>

#define REQUEST_MAX_LEN    1024
#define RESPONSE_MAX_LEN    1024

int main(int argc, char *argv[]) {
    socket_t socket_acceptor, socket_client;
    const char *host = 0;
    const char *port = argv[1];
    char request[REQUEST_MAX_LEN];
    char response[RESPONSE_MAX_LEN];

    socket_create(&socket_acceptor, host, port);
    socket_listen(&socket_acceptor);

    while (1) {
        printf("Accepting connections...\n");
        socket_accept(&socket_acceptor, &socket_client);

        socket_receive(&socket_client, request, sizeof(request));
        printf("Received from client: %s\n", request);

        socket_send(&socket_client, response, sizeof(response));
        printf("Sent to client: %s\n", response);

        printf("Shutting down socket...\n");
        socket_shutdown(&socket_client, SHUT_RDWR);
        socket_close(&socket_client);
    }
}

