#include "client_dynamic_buffer.h"
#include <stdlib.h>
#include <string.h>

#define OK 0
#define ERROR -1
#define REDIM 2

static bool dynamic_buffer_has_space(dynamic_buffer_t *buf, size_t len);

static bool dynamic_buffer_resize(dynamic_buffer_t *buf, size_t new_size);


int dynamic_buffer_create(dynamic_buffer_t *self, size_t size) {
    self->data = malloc(size * sizeof(char));
    if (! self->data) return ERROR;

    self->length = 0;
    self->total_size = size;

    return OK;
}

bool dynamic_buffer_insert_data(dynamic_buffer_t *self, char *s, size_t len) {
    if (! dynamic_buffer_has_space(self, len)) {
        if (! dynamic_buffer_resize(self, REDIM * self->total_size))
            return false;
    }
    int i;
    for (i = 0; i < len; i ++) self->data[self->length + i] = s[i];
    self->length += len;

    return true;
}

static bool dynamic_buffer_has_space(dynamic_buffer_t *self, size_t len) {
    return (self->length + len <= self->total_size);
}

static bool dynamic_buffer_resize(dynamic_buffer_t *buf, size_t new_size) {
    char *new_data = realloc(buf->data, new_size * sizeof(char));
    if (! new_data) return false;

    buf->data = (char *) new_data;
    buf->total_size = new_size;

    return true;
}

void dynamic_buffer_clear_data(dynamic_buffer_t *self) {
    memset(&self->data, 0, self->length);
    self->length = 0;
}

void dynamic_buffer_destroy(dynamic_buffer_t *self) {
    free(self->data);
}
