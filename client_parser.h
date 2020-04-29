#ifndef CLIENT_PARSER_H
#define CLIENT_PARSER_H

#include "client_dynamic_buffer.h"

#define BUF_FILE_MAX_LEN 32

typedef struct {
    const char *file_path;
    char stat_buf[BUF_FILE_MAX_LEN];
    dynamic_buffer_t *dyn_buf;
} parser_t;

int parser_create(parser_t *self, const char *file_path);

int parser_parse_input_file(parser_t *self);

#endif // CLIENT_PARSER_H
