#ifndef SOCKET_H
#define SOCKET_H

#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct socket_t {
    int sd;                 // Socket Descriptor
    struct addrinfo hints;  // Socket Info
    bool is_server;         // Server or Client
} socket_t;

// Creates a socket
int socket_create(socket_t *self, const char *host, const char *port);

// Obtains addresses according to the given 'host' and 'port' and selects
// the first available address
// Returns 0 if OK or error code
int socket_resolve_addr(socket_t *self, const char *host, const char *port);

// [Server only] Associates a socket to a given process
// Returns 0 if OK or error code
int socket_bind(socket_t *self, struct sockaddr *addr, socklen_t len);

// [Server only] Listens to incoming sockets connections and
// sends them to the queue
// Returns 0 if OK or error code
int socket_listen(socket_t *self);

// [Server only] Accepts incoming sockets connections and
// links them into 'accepted_socket'
// Returns a new socket to manage the new client-server connection
int socket_accept(socket_t *self, socket_t *accepted_socket);

// [Client only] Tries to connect a client socket to a server one
// Returns 0 if OK or error code
int socket_connect(socket_t *self, struct sockaddr *addr, socklen_t len);

// Tries to send 'length' bytes from 'buffer' to another socket
// Returns number of bytes sent or error code
int socket_send(socket_t *self, const char *buffer, size_t length);

// Tries to receive 'length' bytes from another socket and
// stores them in 'buffer'
// Returns number of bytes received or error code
int socket_receive(socket_t *self, char *buffer, size_t length);

// Shuts down a socket given channel
// Returns 0 if OK or error code
void socket_shutdown(socket_t *self, int channel);

// Destroys a socket
void socket_destroy(socket_t *self);

#endif //SOCKET_H
