#include "common_socket.h"
#include "client_parser.h"
#include "common_dbus.h"

typedef struct {
    socket_t socket;
    parser_t parser;
    dbus_t dbus;
} client_t;

int client_create(client_t *self, char *argv[]);

int client_destroy(client_t *self);

int client_read_file(client_t *self);

int client_send(client_t *self);

int client_receive(client_t *self);
