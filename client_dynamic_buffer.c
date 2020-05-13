#include "client_dynamic_buffer.h"
#include <string.h>

#define OK 0
#define ERROR -1
#define REDIM 2

// Returns true if the buffer has space to store 'len' bytes
// or false if not
static bool dynamic_buffer_has_space(dynamic_buffer_t *self, size_t len);

// Changes the size of the memory allocated to 'new_size'
// Returns true if realloc worked or false if not
static bool dynamic_buffer_resize(dynamic_buffer_t *self, size_t new_size);


int dynamic_buffer_create(dynamic_buffer_t *self, size_t size) {
    self->data = malloc(size * sizeof(char));
    if (! self->data) return ERROR;

    self->length = 0;
    self->total_size = size;

    return OK;
}

void dynamic_buffer_destroy(dynamic_buffer_t *self) {
    free(self->data);
}

bool dynamic_buffer_insert_data(dynamic_buffer_t *self, char *s, size_t len) {
    if (! dynamic_buffer_has_space(self, len)) {
        size_t new_size = self->total_size * REDIM;
        if (new_size - self->length < len) new_size = len;
        if (! dynamic_buffer_resize(self, new_size)) return false;
    }
    int i;
    for (i = 0; i < len; i ++) self->data[self->length + i] = s[i];
    self->length += len;

    return true;
}

void dynamic_buffer_clear_data(dynamic_buffer_t *self) {
    memset(self->data, 0, self->length);
    self->length = 0;
}

static bool dynamic_buffer_has_space(dynamic_buffer_t *self, size_t len) {
    return (self->length + len <= self->total_size);
}

static bool dynamic_buffer_resize(dynamic_buffer_t *self, size_t new_size) {
    self->data = realloc(self->data, new_size * sizeof(char));
    if (! self->data) return false;

    self->total_size = new_size;

    return true;
}
