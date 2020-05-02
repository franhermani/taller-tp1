#ifndef SERVER_DBUS_H
#define SERVER_DBUS_H

#include "common_dbus.h"

int dbus_create(dbus_t *self);

int dbus_destroy(dbus_t *self);

int dbus_get_array_length(dbus_t *self, char *first_bytes);

int dbus_get_body_length(dbus_t *self, char *first_bytes);

#endif // SERVER_DBUS_H
