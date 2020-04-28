#include "common_socket.h"
#include <stdio.h>

#define OK 0
#define ERROR -1
#define RESPONSE_MAX_LEN 1024
#define BUF_FILE_MAX_LEN 32

int main(int argc, char *argv[]) {
    socket_t socket_client;
    FILE *fd;
    const char *host = argv[1];
    const char *port = argv[2];

    // TODO: el 3er parametro va a ser input_file
    // TODO: si no se especifica, leer de entrada estandar
    const char *file_path = argv[3];
    char file_buffer[BUF_FILE_MAX_LEN];

    // TODO: eliminar esto
    const char *request = "Hola mundo";

    char response[RESPONSE_MAX_LEN];

    if (! (fd = fopen(file_path, "r"))) {
        printf("Error opening file\n");
        return ERROR;
    }
    int i, s;
    while (! feof(fd)) {
        i = 0;
        while (i < sizeof(file_buffer)) {
            s = fread(&file_buffer[i], sizeof(char), 1, fd);
            if (file_buffer[i] == '\n' || s == 0) break;
            i += 1;
        }
        file_buffer[i] = '\0';
        memset(&file_buffer, 0, sizeof(file_buffer));
    }

    fclose(fd);

    printf("Opening socket...\n");
    if (socket_create(&socket_client, host, port) == ERROR)
        return ERROR;

    if (socket_send(&socket_client, request, strlen(request)) == ERROR)
        return ERROR;

    printf("Sent request to server: %s\n", request);
    socket_shutdown(&socket_client, SHUT_WR);

    if (socket_receive(&socket_client, response, RESPONSE_MAX_LEN) == ERROR)
        return ERROR;

    printf("Received response from server: %s\n", response);
    socket_shutdown(&socket_client, SHUT_RD);
    printf("Closing socket...\n");
    socket_close(&socket_client);
    return 0;
}

