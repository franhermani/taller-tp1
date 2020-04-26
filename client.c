#include "common_socket.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    socket_t socket;
    const char *host = argv[1];
    const char *port = argv[2];

    socket_create(&socket, host, port);

    while (1) {
        // socket_send(&socket, buffer, strlen(buffer));
    }
    return 0;
}

