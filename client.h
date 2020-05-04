#include "common_socket.h"
#include "client_dbus.h"

typedef struct {
    socket_t socket;
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

// Reads the input file for the given 'file_path', applies dbus protocol to
// each line and sends the message to the server
// Returns 0 if OK or error code
int client_process_input(client_t *self, const char *file_path);

// Sends a byte message to the server
// Returns 0 if OK or error code
int client_send(client_t *self, byte_msg_t);

// Receives a message from the server
// Returns 0 if OK or error code
int client_receive(client_t *self);

// Prints the response from the server for each message sent
void client_print_output(client_t *self, const char *response);
