#include "common_socket.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    socket_t socket;
    const char *host = 0;
    const char *port = argv[1];

    int s = socket_create(&socket, host, port);
    printf("Result: %i\n", s);

    return 0;
}

