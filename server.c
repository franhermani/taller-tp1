#include "server.h"
#include "common_socket.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define FIRST_LEN 16

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

    if (socket_shutdown(&self->socket_client, SHUT_WR) == ERROR)
        return ERROR;

    return OK;
}

int server_receive(server_t *self) {
    char first_req[FIRST_LEN];

    if (socket_receive(&self->socket_client, first_req, FIRST_LEN) == ERROR)
        return ERROR;

    // Parseo req y obtengo la longitud del array y del body
    //int array_length = dbus_get_array_length(&self->dbus, first_req);
    //int body_length = dbus_get_body_length(&self->dbus, first_req);

    int array_length = 19;
    int body_length = 108;

    char array_req[array_length];
    char body_req[body_length];

    if (socket_receive(&self->socket_client, array_req, array_length) == ERROR)
        return ERROR;

    if (socket_receive(&self->socket_client, body_req, body_length) == ERROR)
        return ERROR;

    // TODO: eliminar esto cuando termine de debuggear
    for (int i=0; i < FIRST_LEN; i++)
        printf("%02X ", first_req[i]);
    printf("\n");

    for (int i=0; i < array_length; i++)
        printf("%02X ", array_req[i]);
    printf("\n");

    for (int i=0; i < body_length; i++)
        printf("%02X ", body_req[i]);
    printf("\n");
    //

    //server_print_output(self);

    if (socket_shutdown(&self->socket_client, SHUT_RD) == ERROR)
        return ERROR;

    return OK;
}

void server_print_output(server_t *self) {
    printf("* Id: 0x...\n");
    printf("* Destino: ...\n");
    printf("* Path: ...\n");
    printf("* Interfaz: ...\n");
    printf("* Método: ...\n");

    // TODO: agregar el if y el for
    printf("* Parámetros:\n");
    printf("    * <parámetro1>\n");
    printf("    * <parámetroN>\n");
    printf("\n");
}