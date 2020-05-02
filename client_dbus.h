#ifndef CLIENT_DBUS_H
#define CLIENT_DBUS_H

#include "common_dbus.h"

int dbus_create(dbus_t *self);

int dbus_destroy(dbus_t *self);

byte_msg_t dbus_parse_line(dbus_t *self, char *line);

void dbus_destroy_byte_msg(dbus_t *self);

#endif // CLIENT_DBUS_H
