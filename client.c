#include "client.h"

#define OK 0
#define ERROR -1
#define BUF_SIZE 32
#define RESPONSE_LEN 3

int main(int argc, char *argv[]) {
    client_t client;
    const char *host = argv[1], *port = argv[2], *file_path = argv[3];

    FILE *input = file_path ? fopen(file_path, "r") : stdin;
    if (! input) {
        printf("Error opening file or reading stdin\n");
        return ERROR;
    }
    if (client_create(&client, host, port) == ERROR) return ERROR;
    if (client_process_input(&client, input) == ERROR) return ERROR;
    if (client_receive(&client) == ERROR) return ERROR;
    if (client_destroy(&client) == ERROR) return ERROR;
    if (input != stdin) fclose(input);

    return OK;
}

int client_create(client_t *self, const char *host, const char *port) {
    if (socket_create(&self->socket, host, port) == ERROR) return ERROR;
    if (dynamic_buffer_create(&self->dyn_buf, BUF_SIZE) == ERROR) return ERROR;
    dbus_create(&self->dbus);
    self->msg_id = 0;

    return OK;
}

int client_destroy(client_t *self) {
    dbus_destroy(&self->dbus);
    dynamic_buffer_destroy(&self->dyn_buf);
    if (socket_close(&self->socket) == ERROR) return ERROR;

    return OK;
}

// TODO: modularizar esta funcion
int client_process_input(client_t *self, FILE *input) {
    // TODO: buffer dinamico
    char stat_buf[1024];
    byte_msg_t byte_msg;

    int i, s = 1;

    while (! feof(input)) {
        i = 0;
        while (s != 0) {
            s = fread(&stat_buf[i], sizeof(char), 1, input);
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
    if (socket_shutdown(&self->socket, SHUT_WR) == ERROR)
        return ERROR;

    return OK;
}

int client_send(client_t *self, byte_msg_t byte_msg) {
    int s;
    s = socket_send(&self->socket, (char *) byte_msg.value, byte_msg.length);
    if (s == ERROR) return ERROR;

    dbus_destroy_byte_msg(&self->dbus);

    return OK;
}

int client_receive(client_t *self) {
    char response[RESPONSE_LEN];

    while (socket_receive(&self->socket, response, RESPONSE_LEN) > 0) {
        client_print_output(self, response);
        memset(&response, 0, RESPONSE_LEN);
    }
    return OK;
}

void client_print_output(client_t *self, const char *response) {
    printf("0x%08X: %s", ++self->msg_id, response);
}
