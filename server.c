#include "server.h"
#include "common_socket.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define FIRST_SIZE 16

int main(int argc, char *argv[]) {
    server_t server;
    const char *host = 0, *port = argv[1];

    if (server_create(&server, host, port) == ERROR) return ERROR;
    if (server_accept(&server) == ERROR) return ERROR;
    if (server_receive_and_send(&server) == ERROR) return ERROR;
    if (server_destroy(&server) == ERROR) return ERROR;

    return 0;
}

int server_create(server_t *self, const char *host, const char *port) {
    if (socket_create(&self->socket_acceptor, host, port) == ERROR)
        return ERROR;

    if (socket_listen(&self->socket_acceptor) == ERROR)
        return ERROR;

    if (dbus_create(&self->dbus) == ERROR)
        return ERROR;

    self->msg = "OK\n";

    return OK;
}

int server_destroy(server_t *self) {
    if (dbus_destroy(&self->dbus) == ERROR) return ERROR;
    if (socket_close(&self->socket_client)) return ERROR;
    if (socket_close(&self->socket_acceptor)) return ERROR;

    return OK;
}

int server_accept(server_t *self) {
    if (socket_accept(&self->socket_acceptor, &self->socket_client) == ERROR)
        return ERROR;

    return OK;
}

int server_receive_and_send(server_t *self) {
    char first_req[FIRST_SIZE];

    while (socket_receive(&self->socket_client, first_req, FIRST_SIZE) > 0) {
        server_receive(self, first_req);
        memset(&first_req, 0, FIRST_SIZE);
        server_print_output(self);
        server_send(self, self->msg);
    }
    if (socket_shutdown(&self->socket_client, SHUT_RDWR) == ERROR)
        return ERROR;

    return OK;
}

int server_receive(server_t *self, char *first_req) {
    int ARRAY_SIZE = dbus_get_array_length(&self->dbus, first_req);
    int BODY_SIZE = dbus_get_body_length(&self->dbus, first_req);

    // Array length already included in first FIRST_SIZE bytes
    ARRAY_SIZE -= sizeof(int);

    char array_req[ARRAY_SIZE];
    char body_req[BODY_SIZE];

    if (socket_receive(&self->socket_client, array_req, ARRAY_SIZE) == ERROR)
        return ERROR;

    if (socket_receive(&self->socket_client, body_req, BODY_SIZE) == ERROR)
        return ERROR;

    // TODO: eliminar esto cuando termine de debuggear
    /*
    //printf("FIRST BYTES\n");
    for (int i=0; i < FIRST_SIZE; i++)
        printf("%02X ", first_req[i]);
    //printf("\n\n");

    //printf("ARRAY\n");
    for (int i=0; i < ARRAY_SIZE; i++)
        printf("%02X ", array_req[i]);
    //printf("\n\n");

    //printf("BODY\n");
    for (int i=0; i < BODY_SIZE; i++)
        printf("%02X ", body_req[i]);
    //printf("\n\n");
    printf("\n");
    //
    */

    dbus_build_array(&self->dbus, array_req, ARRAY_SIZE);
    dbus_build_body(&self->dbus, body_req);

    memset(&array_req, 0, ARRAY_SIZE);
    memset(&body_req, 0, BODY_SIZE);

    return OK;
}

int server_send(server_t *self, const char *msg) {
    if (socket_send(&self->socket_client, msg, strlen(msg)) == ERROR)
        return ERROR;

    return OK;
}

void server_print_output(server_t *self) {
    printf("* Id: 0x...\n");
    printf("* Destino: %s\n", self->dbus.msg.destiny);
    printf("* Path: %s\n", self->dbus.msg.path);
    printf("* Interfaz: %s\n", self->dbus.msg.interface);
    printf("* Método: %s\n", self->dbus.msg.method);

    int params_quant = self->dbus.msg.header.array.firm.params_quant;
    if (params_quant == 0) return;

    printf("* Parámetros:\n");

    // TODO: tira violacion de segmento
    int i;
    for (i=0; i < params_quant; i++) {
        char *param = self->dbus.msg.body.params[i].value;
        printf("    * %s\n", param);
    }
    printf("\n");
}
