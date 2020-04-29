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

bool dynamic_buffer_insert_data(dynamic_buffer_t *buf, char *s) {
    if (dynamic_buffer_is_full(buf)) {
        if (! dynamic_buffer_resize(buf, REDIM * buf->total_size))
            return false;
    }
    buf->data[buf->length] = s;
    buf->length ++;
    return true;
}

static bool dynamic_buffer_is_full(dynamic_buffer_t *buf) {
    return (buf->length == buf->total_size);
}

static bool dynamic_buffer_resize(dynamic_buffer_t *buf, size_t new_size) {
    char *new_data = realloc(buf->data, new_size * sizeof(char*));
    if (! new_data) return false;
    buf->data = (char **) new_data;
    buf->total_size = new_size;
    return true;
}

char** dynamic_buffer_get_data(dynamic_buffer_t *buf) {
    return buf->data;
}

void dynamic_buffer_remove_data(dynamic_buffer_t *buf) {
    buf->data = NULL;
}

void dynamic_buffer_destroy(dynamic_buffer_t *buf) {
    free(buf->data);
    free(buf);
}
