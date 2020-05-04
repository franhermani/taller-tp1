#include "common_socket.h"
#include "server_dbus.h"

typedef struct {
    socket_t socket_acceptor;
    socket_t socket_client;
    dbus_t dbus;
    const char *msg;
} server_t;

int server_create(server_t *self, const char *host, const char *port);

int server_destroy(server_t *self);

int server_accept(server_t *self);

int server_receive_and_send(server_t *self);

int server_receive(server_t *self, char *first_req);

int server_receive_array(server_t *self, char *first_req);

int server_receive_body(server_t *self, char *first_req);

int server_send(server_t *self, const char *msg);

void server_print_output(server_t *self);
