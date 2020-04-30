#include "common_socket.h"
#include "client_parser.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define RESPONSE_MAX_LEN 1024

int main(int argc, char *argv[]) {
    socket_t socket_client;
    parser_t parser;

    const char *host = argv[1];
    const char *port = argv[2];
    const char *file_path = argv[3];
    if (! file_path) return ERROR;

    parser_create(&parser, file_path);
    parser_parse_input_file(&parser);
    // TODO: ver en que momento destruir el parser
    parser_destroy(&parser);

    // TODO: eliminar esto
    const char *request = "Hola mundo";
    char response[RESPONSE_MAX_LEN];

    printf("Opening socket...\n");
    if (socket_create(&socket_client, host, port) == ERROR) {
        return ERROR;
    }

    // TODO: El socket_send va dentro del while que parsea las lineas
    if (socket_send(&socket_client, request, strlen(request)) == ERROR) {
        return ERROR;
    }
    printf("Sent request to server: %s\n", request);
    socket_shutdown(&socket_client, SHUT_WR);

    // TODO: este tmb va en un while, para recibir todos los OK del server
    if (socket_receive(&socket_client, response, RESPONSE_MAX_LEN) == ERROR)
        return ERROR;

    printf("Received response from server: %s\n", response);
    socket_shutdown(&socket_client, SHUT_RD);
    printf("Closing socket...\n");
    socket_close(&socket_client);
    return 0;
}
