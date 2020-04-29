#ifndef CLIENT_DYNAMIC_BUFFER_H
#define CLIENT_DYNAMIC_BUFFER_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    char **data;
    size_t length;
    size_t total_size;
} dynamic_buffer_t;

dynamic_buffer_t* dynamic_buffer_create(void);

bool dynamic_buffer_insert_data(dynamic_buffer_t *buf, char *s);

char** dynamic_buffer_get_data(dynamic_buffer_t *buf);

void dynamic_buffer_remove_data(dynamic_buffer_t *buf);

void dynamic_buffer_destroy(dynamic_buffer_t *buf);

#endif // CLIENT_DYNAMIC_BUFFER_H
