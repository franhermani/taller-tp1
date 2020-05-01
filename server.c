#include "server.h"
#include "common_socket.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define REQ_MAX_LEN 1024

int main(int argc, char *argv[]) {
    server_t server;
    const char *host = 0, *port = argv[1];
    const char *response = "OK\n";

    if (server_create(&server, host, port) == ERROR) return ERROR;
    if (server_accept(&server) == ERROR) return ERROR;

    // TODO: esto va en un while
    if (server_receive(&server) == ERROR) return ERROR;
    if (server_send(&server, response) == ERROR) return ERROR;

    server_destroy(&server);

    return 0;
}

int server_create(server_t *self, const char *host, const char *port) {
    if (socket_create(&self->socket_acceptor, host, port) == ERROR)
        return ERROR;

    if (socket_listen(&self->socket_acceptor) == ERROR)
        return ERROR;

    if (dbus_create(&self->dbus) == ERROR) return ERROR;

    return OK;
}

int server_destroy(server_t *self) {
    dbus_destroy(&self->dbus);
    socket_close(&self->socket_client);
    socket_close(&self->socket_acceptor);

    return OK;
}

int server_accept(server_t *self) {
    if (socket_accept(&self->socket_acceptor, &self->socket_client) == ERROR)
        return ERROR;

    return OK;
}

int server_send(server_t *self, const char *msg) {
    if (socket_send(&self->socket_client, msg, strlen(msg)) == ERROR)
        return ERROR;

    printf("\nSent response to client: %s\n", msg);

    if (socket_shutdown(&self->socket_client, SHUT_WR) == ERROR)
        return ERROR;

    return OK;
}

int server_receive(server_t *self) {
    char req[REQ_MAX_LEN];

    if (socket_receive(&self->socket_client, req, REQ_MAX_LEN) == ERROR)
        return ERROR;

    printf("Received request from client:\n");

    // TODO: eliminar esto cuando termine de debuggear
    for (int i=0; i < 139; i++)
        printf("%02X ", req[i]);
    printf("\n");
    //

    if (socket_shutdown(&self->socket_client, SHUT_RD) == ERROR)
        return ERROR;

    return OK;
}
