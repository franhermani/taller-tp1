#include "common_socket.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define REQUEST_MAX_LEN 1024

int main(int argc, char *argv[]) {
    socket_t socket_acceptor, socket_client;
    const char *host = 0;
    const char *port = argv[1];
    char request[REQUEST_MAX_LEN];
    char *response = "OK\n";

    if (socket_create(&socket_acceptor, host, port) == ERROR)
        return ERROR;

    if (socket_listen(&socket_acceptor) == ERROR)
        return ERROR;

    printf("Accepting connections...\n");

    if (socket_accept(&socket_acceptor, &socket_client) == ERROR)
        return ERROR;

    if (socket_receive(&socket_client, request, REQUEST_MAX_LEN) == ERROR)
        return ERROR;

    printf("Received from client: %s\n", request);

    if (socket_send(&socket_client, response, strlen(response)) == ERROR)
        return ERROR;

    printf("Sent to client: %s\n", response);
    printf("Shutting down socket...\n");
    socket_shutdown(&socket_client, SHUT_RDWR);
    socket_close(&socket_client);
    return 0;
}

