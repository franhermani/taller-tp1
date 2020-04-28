#ifndef CLIENT_PARSER_H
#define CLIENT_PARSER_H

#define BUF_FILE_MAX_LEN 32

typedef struct {
    const char *file_path;
    char file_buffer[BUF_FILE_MAX_LEN];
} parser_t;

void parser_create(parser_t *self, const char *file_path);

int parser_parse_input_file(parser_t *self);

#endif // CLIENT_PARSER_H
