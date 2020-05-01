#include "client.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define RESPONSE_MAX_LEN 1024

int main(int argc, char *argv[]) {
    client_t client;
    const char *host = argv[1], *port = argv[2], *file_path = argv[3];

    if (client_create(&client, host, port) == ERROR) return ERROR;
    if (client_process_input(&client, file_path) == ERROR) return ERROR;

    // TODO: esto va en un while
    if (client_receive(&client) == ERROR) return ERROR;

    client_destroy(&client);

    return 0;
}

int client_create(client_t *self, const char *host, const char *port) {
    if (socket_create(&self->socket, host, port) == ERROR) return ERROR;
    if (dbus_create(&self->dbus) == ERROR) return ERROR;

    return OK;
}

int client_destroy(client_t *self) {
    dbus_destroy(&self->dbus);
    socket_close(&self->socket);

    return OK;
}

int client_process_input(client_t *self, const char *file_path) {
    //if (parser_parse_input_file(&self->parser, &self->dbus) == ERROR)
    //    return ERROR;

    // TODO: contemplar entrada estandar
    if (! file_path) return ERROR;

    // TODO: buffer dinamico
    char stat_buf[1024];
    byte_msg_t byte_msg;

    FILE *fd;
    if (! (fd = fopen(file_path, "r"))) {
        printf("Error opening file\n");
        return ERROR;
    }

    int i, s = 1;

    while (! feof(fd)) {
        i = 0;
        while (s != 0) {
            s = fread(&stat_buf[i], sizeof(char), 1, fd);
            if (stat_buf[i] == '\n') {
                stat_buf[i] = '\0';
                byte_msg = dbus_parse_line(&self->dbus, stat_buf);
                if (client_send(self, byte_msg) == ERROR) return ERROR;
                break;
            }
            i ++;
        }
        memset(&stat_buf, 0, sizeof(stat_buf));
    }
    fclose(fd);

    if (socket_shutdown(&self->socket, SHUT_WR) == ERROR)
        return ERROR;

    return OK;
}

int client_send(client_t *self, byte_msg_t byte_msg) {
    int s;
    s = socket_send(&self->socket, (char *) byte_msg.value, byte_msg.length);
    if (s == ERROR) return ERROR;

    printf("Sent request to server:\n");

    // TODO: eliminar esto cuando termine de debuggear
    for (int i=0; i < byte_msg.length; i++)
        printf("%02X ", byte_msg.value[i]);
    printf("\n");
    //

    dbus_destroy_byte_msg(&self->dbus);

    return OK;
}

int client_receive(client_t *self) {
    // TODO: buffer dinamico
    char response[RESPONSE_MAX_LEN];

    if (socket_receive(&self->socket, response, RESPONSE_MAX_LEN) == ERROR)
        return ERROR;

    printf("\nReceived response from server: %s\n", response);

    if (socket_shutdown(&self->socket, SHUT_RD) == ERROR)
        return ERROR;

    return OK;
}