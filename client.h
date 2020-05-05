#include "common_socket.h"
#include "client_dynamic_buffer.h"
#include "client_dbus.h"
#include <stdio.h>

typedef struct {
    socket_t socket;
    dynamic_buffer_t dyn_buf;
    dbus_t dbus;
    uint32_t msg_id;
} client_t;

// Creates a client with a socket (associated to the given 'host' and 'port')
// and a dbus protocol
// Returns 0 if OK or error code
int client_create(client_t *self, const char *host, const char *port);

// Destroys the client's socket and dbus protocol
// Returns 0 if OK or error code
int client_destroy(client_t *self);

// Reads the input file by calling client_process_line()
// Returns 0 if OK or error code
int client_process_input(client_t *self, FILE *input);

// Stores each line in a dynamic buffer, calls dbus_parse_line
// to apply the protocol and sends it to the server
// Returns 0 if OK or error code
int client_process_line(client_t *self, FILE *input, char *buf);

// Sends messages to the server
// Returns 0 if OK or error code
int client_send(client_t *self, byte_msg_t);

// Receives messages from the server
// Returns 0 if OK or error code
int client_receive(client_t *self);

// Prints the response from the server for each message sent
void client_print_output(client_t *self, const char *response);
