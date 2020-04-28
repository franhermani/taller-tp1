#include "client_parser.h"
#include <stdio.h>
#include <string.h>

#define OK 0
#define ERROR -1

void parser_create(parser_t *self, const char *file_path) {
    self->file_path = file_path;
}

int parser_parse_input_file(parser_t *self) {
    FILE *fd;
    if (! (fd = fopen(self->file_path, "r"))) {
        printf("Error opening file\n");
        return ERROR;
    }
    int i, s;
    while (! feof(fd)) {
        i = 0;
        while (i < sizeof(self->file_buffer)) {
            s = fread(&self->file_buffer[i], sizeof(char), 1, fd);
            if (self->file_buffer[i] == '\n' || s == 0) break;
            i += 1;
        }
        self->file_buffer[i] = '\0';
        memset(&self->file_buffer, 0, sizeof(self->file_buffer));
    }
    fclose(fd);
    return OK;
}