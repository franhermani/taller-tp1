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
    char buf[BUF_SIZE];
    char *line_break;
    size_t size;
    byte_msg_t byte_msg;
    int s;

    while ((s = fread(&buf, sizeof(char), BUF_SIZE, input)) > 0) {
        line_break = strchr(buf, '\n');
        if (line_break) {
            size = line_break - buf;
            fseek(input, size - BUF_SIZE + 1, SEEK_CUR);
            dynamic_buffer_insert_data(&self->dyn_buf, buf, size);
            dynamic_buffer_insert_data(&self->dyn_buf, "\0", 1);
            byte_msg = dbus_parse_line(&self->dbus, self->dyn_buf.data);
            dynamic_buffer_clear_data(&self->dyn_buf);
            if (client_send(self, byte_msg) == ERROR) return ERROR;
        } else {
            size = BUF_SIZE;
            dynamic_buffer_insert_data(&self->dyn_buf, buf, size);
        }
        memset(&buf, 0, BUF_SIZE);
        if (s < BUF_SIZE) break;
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
    printf("0x%08x: %s", ++self->msg_id, response);
}
