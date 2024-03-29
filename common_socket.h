#ifndef SOCKET_H
#define SOCKET_H

#define _POSIX_C_SOURCE 200112L

#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef struct {
    int sd;                 // Socket Descriptor
    struct addrinfo hints;  // Socket Info
    bool is_server;         // Server or Client
} socket_t;

// Creates a socket with the given 'host' and 'port' and binds (if server)
// or connects (if client) to the first available address
int socket_create(socket_t *self, const char *host, const char *port);

// Destroys a socket
void socket_destroy(socket_t *self);

// Shuts down a socket given channel (SHUT_WR, SHUT_RD or SHUT_RDWR)
// Returns 0 if OK or error code
int socket_shutdown(socket_t *self, int channel);

// Closes a socket
// Returns 0 if OK or error code
int socket_close(socket_t *self);

// [Server only] Listens to incoming sockets connections and
// sends them to the queue
// Returns 0 if OK or error code
int socket_listen(socket_t *self);

// [Server only] Accepts incoming sockets connections and
// links them into 'accepted_socket'
// Returns a new socket to manage the new client-server connection
int socket_accept(socket_t *self, socket_t *accepted_socket);

// Tries to send 'length' bytes from 'buffer' to another socket
// Returns number of bytes sent or error code
int socket_send(socket_t *self, const char *buffer, size_t length);

// Tries to receive 'length' bytes from another socket and
// stores them in 'buffer'
// Returns number of bytes received or error code
int socket_receive(socket_t *self, char *buffer, size_t length);

#endif // SOCKET_H
