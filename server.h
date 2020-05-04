#include "common_socket.h"
#include "server_dbus.h"

typedef struct {
    socket_t socket_acceptor;
    socket_t socket_client;
    dbus_t dbus;
    const char *msg;
} server_t;

// Creates a server with a socket (associated to the given 'host' and 'port')
// and a dbus protocol
// Returns 0 if OK or error code
int server_create(server_t *self, const char *host, const char *port);

// Destroys the server's socket (client and acceptor) and dbus protocol
// Returns 0 if OK or error code
int server_destroy(server_t *self);

// Waits for incoming client connections and creates a channel
// when a client is accepted
// Returns 0 if OK or error code
int server_accept(server_t *self);

// Receives messages from the client, prints the output
// and sends the response back
// Returns 0 if OK or error code
int server_receive_and_send(server_t *self);

// Receives messages (array and body) from the client
// Returns 0 if OK or error code
int server_receive(server_t *self, char *first_req);

// Receives array from the client and applies dbus protocol to it
// Returns 0 if OK or error code
int server_receive_array(server_t *self, char *first_req);

// Receives body from the client and applies dbus protocol to it
// Returns 0 if OK or error code
int server_receive_body(server_t *self, char *first_req);

// Sends response to the client
// Returns 0 if OK or error code
int server_send(server_t *self, const char *msg);

// Prints the decoded (with dbus protocol) message from the client
void server_print_output(server_t *self);
