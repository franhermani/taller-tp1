#include "server.h"
#include "common_socket.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define FIRST_SIZE 16

int main(int argc, char *argv[]) {
    server_t server;
    const char *host = 0, *port = argv[1];
    bool error = false;

    if (server_create(&server, host, port) == ERROR) return ERROR;
    while (true) {
        if (server_accept(&server) == ERROR) {
            error = true;
            break;
        }
        if (server_receive_and_send(&server) == ERROR) {
            error = true;
            break;
        }
        break;
    }
    if (server_destroy(&server) == ERROR) return ERROR;
    if (error) return ERROR;
    return OK;
}

int server_create(server_t *self, const char *host, const char *port) {
    if (socket_create(&self->socket_acceptor, host, port) == ERROR)
        return ERROR;

    if (socket_listen(&self->socket_acceptor) == ERROR)
        return ERROR;

    dbus_create(&self->dbus);
    self->msg = "OK\n";

    return OK;
}

int server_destroy(server_t *self) {
    dbus_destroy(&self->dbus);
    if (socket_close(&self->socket_client)) return ERROR;
    if (socket_close(&self->socket_acceptor)) return ERROR;
    socket_destroy(&self->socket_client);
    socket_destroy((&self->socket_acceptor));

    return OK;
}

int server_accept(server_t *self) {
    if (socket_accept(&self->socket_acceptor, &self->socket_client) == ERROR)
        return ERROR;

    return OK;
}

int server_receive_and_send(server_t *self) {
    char first_req[FIRST_SIZE];

    int s;
    while ((s = socket_receive(&self->socket_client, first_req,
            FIRST_SIZE)) > 0) {
        server_receive(self, first_req);
        memset(&first_req, 0, FIRST_SIZE);
        server_print_output(self);
        server_send(self, self->msg);
    }
    if (socket_shutdown(&self->socket_client, SHUT_WR) == ERROR) return ERROR;
    if (s == ERROR) return ERROR;

    return OK;
}

int server_receive(server_t *self, char *first_req) {
    if (server_receive_array(self, first_req) == ERROR) return ERROR;
    if (server_receive_body(self, first_req) == ERROR) return ERROR;

    dbus_set_message_id(&self->dbus, first_req);

    return OK;
}

int server_receive_array(server_t *self, char *first_req) {
    int array_len = dbus_get_array_length(&self->dbus, first_req);

    // Array length already included in first FIRST_SIZE bytes
    array_len -= sizeof(int);

    // Also receives last padding bytes
    array_len += dbus_get_array_last_padding(&self->dbus, array_len);

    char *array_req = malloc(array_len * sizeof(char));
    if (! array_req) return ERROR;

    if (socket_receive(&self->socket_client, array_req, array_len) == ERROR)
        return ERROR;

    dbus_build_array(&self->dbus, array_req, array_len);
    free(array_req);

    return OK;
}

int server_receive_body(server_t *self, char *first_req) {
    int body_len = dbus_get_body_length(&self->dbus, first_req);

    if (body_len > 0) {
        char *body_req = malloc(body_len * sizeof(char));
        if (! body_req) return ERROR;

        if (socket_receive(&self->socket_client, body_req, body_len) == ERROR)
            return ERROR;

        dbus_build_body(&self->dbus, body_req);
        free(body_req);
    }
    return OK;
}

int server_send(server_t *self, const char *msg) {
    if (socket_send(&self->socket_client, msg, strlen(msg)) == ERROR)
        return ERROR;

    return OK;
}

void server_print_output(server_t *self) {
    printf("* Id: 0x%08x\n", self->dbus.last_id);
    printf("* Destino: %s\n", self->dbus.msg.destiny);
    printf("* Ruta: %s\n", self->dbus.msg.path);
    printf("* Interfaz: %s\n", self->dbus.msg.interface);
    printf("* Metodo: %s\n", self->dbus.msg.method);

    dbus_destroy_array(&self->dbus);

    if (self->dbus.msg.header.body_length > 0) {
        printf("* Parametros:\n");

        int params_quant = self->dbus.msg.header.array.firm.params_quant;
        int i;
        for (i = 0; i < params_quant; i ++) {
            char *param = self->dbus.msg.body.params[i].value;
            printf("    * %s\n", param);
        }
        dbus_destroy_body(&self->dbus);
    }
    printf("\n");
}
