#ifndef SERVER_DBUS_H
#define SERVER_DBUS_H

#include "common_dbus.h"
#include <stdlib.h>

// Creates a dbus protocol structure
void dbus_create(dbus_t *self);

// Destroys a dbus protocol structure
void dbus_destroy(dbus_t *self);

// Destroys the parameters array values by freeing the memory saved for them
void dbus_destroy_array(dbus_t *self);

// Destroys the body parameters values and the structure params
// by freeing the memory saved for them
void dbus_destroy_body(dbus_t *self);

// Sets the message id value in the dbus structure by reading
// the first bytes received from the client
void dbus_set_message_id(dbus_t *self, char *first_req);

// Sets the array length in the dbus structure and returns it
// by reading the first bytes received from the client
int dbus_get_array_length(dbus_t *self, char *first_req);

// Sets the body length in the dbus structure and returns it
// by reading the first bytes received from the client
int dbus_get_body_length(dbus_t *self, char *first_req);

// Returns the number of bytes of padding at the end of the array
// according to the 'array_len' received
int dbus_get_array_last_padding(dbus_t *self, int array_len);

// Builds the array structure by reading the bytes received from the client
void dbus_build_array(dbus_t *self, char *array_req, size_t array_size);

// Builds the body structure by reading the bytes received from the client
void dbus_build_body(dbus_t *self, char *body_req);

#endif // SERVER_DBUS_H
