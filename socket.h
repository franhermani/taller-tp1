#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>

typedef struct socket_t {
    int socket;
} socket_t;

// Creates a socket
int socket_create(socket_t *self);

// [Server only] Associates a process to a socket
// Returns 0 if OK or error code
int socket_bind(socket_t *self, const char *process);

// [Server only] Listens to incoming sockets connections and
// sends them to the queue
// Returns 0 if OK or error code
int socket_listen(socket_t *self);

// [Server only] Accepts incoming sockets connections and
// links them into 'accepted_socket'
// Returns a new socket to manage the new client-server connection
int socket_accept(socket_t *self, socket_t *accepted_socket);

// [Client only] Connects a local socket to a remote one, given a
// host name and a port number
// Returns 0 if OK or error code
int socket_connect(socket_t *self, const char *host, const char *port);

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
