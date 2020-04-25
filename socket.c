#define _POSIX_C_SOURCE 200112L
#define OK 0
#define ERROR -1
#define MAX_LISTEN_QUEUE_LEN 10

#include "socket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

int socket_create(socket_t *self, const char *host, const char *port) {
    self->sd = -1;
    self->is_server = (! host);

    memset(&self->hints, 0, sizeof(struct addrinfo));
    self->hints.ai_family = AF_INET;          // IPv4
    self->hints.ai_socktype = SOCK_STREAM;    // TCP
    self->hints.ai_flags = self->is_server ? AI_PASSIVE : 0;

    if (! socket_resolve_addr(self, host, port)) {
        return ERROR;
    }
    return OK;
}

int socket_resolve_addr(socket_t *self, const char *host, const char *port) {
    struct addrinfo *ai_list, *ptr;

    // Obtains addresses according to the given 'host' and 'port', applying
    // the filters in 'hints' and saves the results in 'ai_list'
    int status = getaddrinfo(host, port, &self->hints, &ai_list);

    // Uses gai_strerror to translate the error code
    if (status != 0) {
        printf("Error in getaddrinfo: %s\n", gai_strerror(status));
        return ERROR;
    }

    // Iterates the results list and selects the first available address
    for (ptr = ai_list; ptr != NULL; ptr = ptr->ai_next) {
        int sd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sd == -1) {
            continue;
        }
        self->sd = sd;
        if (self->is_server) {
            if (socket_bind(self, ptr->ai_addr, ptr->ai_addrlen) == OK)
                break;
        } else {
            if (socket_connect(self, ptr->ai_addr, ptr->ai_addrlen) == OK)
                break;
        }
    }
    freeaddrinfo(ai_list);

    // No available connections
    if (self->sd == -1) {
        return ERROR;
    }
    return OK;
}

int socket_bind(socket_t *self, struct sockaddr *addr, socklen_t len) {
    int status;

    // Configures socket to reuse the address in case the port is in TIME WAIT
    int val = 1;
    status = setsockopt(self->sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    if (status == -1) {
        printf("Error:%s\n", strerror(errno));
        // TODO: close socket
        return ERROR;
    }
    status = bind(self->sd, addr, len);

    if (status == -1) {
        printf("Error:%s\n", strerror(errno));
        // TODO: close socket
        return ERROR;
    }
    return OK;
}

int socket_listen(socket_t *self) {
    int status = listen(self->sd, MAX_LISTEN_QUEUE_LEN);

    if (status == -1) {
        printf("Error:%s\n", strerror(errno));
        // TODO: close socket
        return ERROR;
    }
    return OK;
}

int socket_accept(socket_t *self, socket_t *accepted_socket) {
    return OK;
}

int socket_connect(socket_t *self, struct sockaddr *addr, socklen_t len) {
    int status = connect(self->sd, addr, len);

    if (status == -1) {
        printf("Error:%s\n", strerror(errno));
        // TODO: close socket
        return ERROR;
    }

    return OK;
}