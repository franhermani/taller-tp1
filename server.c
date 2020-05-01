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

    printf("Opening socket...\n");
    if (socket_create(&socket_acceptor, host, port) == ERROR)
        return ERROR;

    if (socket_listen(&socket_acceptor) == ERROR)
        return ERROR;

    printf("Accepting connections...\n");
    if (socket_accept(&socket_acceptor, &socket_client) == ERROR)
        return ERROR;

    if (socket_receive(&socket_client, request, REQUEST_MAX_LEN) == ERROR)
        return ERROR;

    printf("Received request from client:\n");

    // TODO: eliminar esto cuando termine de debuggear
    for (int i=0; i < 139; i++)
        printf("%02X ", request[i]);
    printf("\n");
    //

    socket_shutdown(&socket_client, SHUT_RD);

    if (socket_send(&socket_client, response, strlen(response)) == ERROR)
        return ERROR;

    printf("Sent to client: %s\n", response);
    socket_shutdown(&socket_client, SHUT_WR);
    printf("Closing socket...\n");
    socket_close(&socket_client);
    socket_close(&socket_acceptor);
    return 0;
}
