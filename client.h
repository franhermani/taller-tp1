#include "common_socket.h"
//#include "client_parser.h"
#include "common_dbus.h"

typedef struct {
    socket_t socket;
    //parser_t parser;
    dbus_t dbus;
} client_t;

int client_create(client_t *self, const char *host, const char *port);

int client_destroy(client_t *self);

int client_process_input(client_t *self, const char *file_path);

int client_send(client_t *self, byte_msg_t);

int client_receive(client_t *self);
