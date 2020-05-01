#include "client.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define RESPONSE_MAX_LEN 1024

int main(int argc, char *argv[]) {
    client_t client;

    if (client_create(&client, argv) == ERROR) return ERROR;
    //if (client_read_file(&client) == ERROR) return ERROR;
    if (client_send(&client) == ERROR) return ERROR;
    if (client_receive(&client) == ERROR) return ERROR;

    client_destroy(&client);

    return 0;
}

int client_create(client_t *self, char *argv[]) {
    const char *host = argv[1], *port = argv[2], *file_path = argv[3];
    //if (! file_path) return ERROR;

    if (socket_create(&self->socket, host, port) == ERROR) return ERROR;
    if (parser_create(&self->parser, file_path) == ERROR) return ERROR;
    if (dbus_create(&self->dbus) == ERROR) return ERROR;

    return OK;
}

int client_destroy(client_t *self) {
    dbus_destroy(&self->dbus);
    parser_destroy(&self->parser);
    socket_close(&self->socket);

    return OK;
}

int client_read_file(client_t *self) {
    if (parser_parse_input_file(&self->parser) == ERROR) return ERROR;

    return OK;
}

int client_send(client_t *self) {
    // TODO: eliminar esto
    const char *request = "Hola mundo";

    // TODO: El socket_send va dentro del while que parsea las lineas
    if (socket_send(&self->socket, request, strlen(request)) == ERROR) {
        return ERROR;
    }
    printf("Sent request to server: %s\n", request);
    socket_shutdown(&self->socket, SHUT_WR);

    return OK;
}

int client_receive(client_t *self) {
    char response[RESPONSE_MAX_LEN];

    // TODO: este tmb va en un while, para recibir todos los OK del server
    if (socket_receive(&self->socket, response, RESPONSE_MAX_LEN) == ERROR)
        return ERROR;

    printf("Received response from server: %s\n", response);
    socket_shutdown(&self->socket, SHUT_RD);

    return OK;
}