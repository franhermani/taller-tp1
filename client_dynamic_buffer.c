#include "client_dynamic_buffer.h"
#include <stdlib.h>

#define INITIAL_SIZE 64
#define REDIM 2

static bool dynamic_buffer_is_full(dynamic_buffer_t *buf);

static bool dynamic_buffer_resize(dynamic_buffer_t *buf, size_t new_size);


dynamic_buffer_t* dynamic_buffer_create(void) {
    dynamic_buffer_t *buf = malloc(sizeof(dynamic_buffer_t));
    if (! buf) return NULL;

    buf->data = malloc(INITIAL_SIZE * sizeof(char*));
    if (! buf->data) {
        free(buf);
        return NULL;
    }
    buf->length = 0;
    buf->total_size = INITIAL_SIZE;
    return buf;
}

bool dynamic_buffer_insert_data(dynamic_buffer_t *self, char *s) {
    if (dynamic_buffer_is_full(self)) {
        if (! dynamic_buffer_resize(self, REDIM * self->total_size))
            return false;
    }
    self->data[self->length] = s;
    self->length ++;
    return true;
}

static bool dynamic_buffer_is_full(dynamic_buffer_t *self) {
    return (self->length == self->total_size);
}

static bool dynamic_buffer_resize(dynamic_buffer_t *buf, size_t new_size) {
    char *new_data = realloc(buf->data, new_size * sizeof(char*));
    if (! new_data) return false;
    buf->data = (char **) new_data;
    buf->total_size = new_size;
    return true;
}

char** dynamic_buffer_get_data(dynamic_buffer_t *self) {
    return self->data;
}

void dynamic_buffer_remove_data(dynamic_buffer_t *self) {
    self->data = NULL;
}

void dynamic_buffer_destroy(dynamic_buffer_t *self) {
    free(self->data);
    free(self);
}
