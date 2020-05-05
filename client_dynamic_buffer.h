#ifndef CLIENT_DYNAMIC_BUFFER_H
#define CLIENT_DYNAMIC_BUFFER_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    char *data;
    size_t length;
    size_t total_size;
} dynamic_buffer_t;

// Creates a dynamic buffer and uses malloc to allocate
// 'size' bytes to store data
// Returns 0 if OK or error code
int dynamic_buffer_create(dynamic_buffer_t *self, size_t size);

// Inserts the first 'len' bytes of 's' in the allocated data last position
// If there isn't enough space, it calls dynamic_buffer_resize()
bool dynamic_buffer_insert_data(dynamic_buffer_t *self, char *s, size_t len);

// Clears the allocated data with memset and sets the length to 0
void dynamic_buffer_clear_data(dynamic_buffer_t *self);

// Destroys the dynamic buffer by freeing the allocated memory for it's data
void dynamic_buffer_destroy(dynamic_buffer_t *self);

#endif // CLIENT_DYNAMIC_BUFFER_H
