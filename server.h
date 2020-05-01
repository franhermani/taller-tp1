#include "common_socket.h"
#include "common_dbus.h"

typedef struct {
    socket_t socket_acceptor;
    socket_t socket_client;
    dbus_t dbus;
} server_t;

int server_create(server_t *self, const char *host, const char *port);

int server_destroy(server_t *self);

int server_accept(server_t *self);

int server_send(server_t *self, const char *msg);

int server_receive(server_t *self);

void server_print_output(server_t *self);