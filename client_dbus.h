#ifndef CLIENT_DBUS_H
#define CLIENT_DBUS_H

#include "common_dbus.h"

// Creates a dbus protocol structure
void dbus_create(dbus_t *self);

// Destroys a dbus protocol structure
void dbus_destroy(dbus_t *self);

// Receives a char* 'line' and converts it to a byte structure
// by aplying dbus protocol to it
byte_msg_t dbus_parse_line(dbus_t *self, char *line);

// Destroys the byte structure by freeing the memory saved for it
void dbus_destroy_byte_msg(dbus_t *self);

#endif // CLIENT_DBUS_H
