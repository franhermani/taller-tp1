#ifndef CLIENT_DYNAMIC_BUFFER_H
#define CLIENT_DYNAMIC_BUFFER_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    char *data;
    size_t length;
    size_t total_size;
} dynamic_buffer_t;

int dynamic_buffer_create(dynamic_buffer_t *self, size_t size);

bool dynamic_buffer_insert_data(dynamic_buffer_t *self, char *s, size_t len);

int dynamic_buffer_clear_data(dynamic_buffer_t *self);

void dynamic_buffer_destroy(dynamic_buffer_t *self);

#endif // CLIENT_DYNAMIC_BUFFER_H
