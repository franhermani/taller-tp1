#ifndef SERVER_DBUS_H
#define SERVER_DBUS_H

#include "common_dbus.h"
#include <stdlib.h>

int dbus_create(dbus_t *self);

int dbus_destroy(dbus_t *self);

int dbus_destroy_array(dbus_t *self);

int dbus_destroy_body(dbus_t *self);

int dbus_get_array_length(dbus_t *self, char *first_req);

int dbus_get_body_length(dbus_t *self, char *first_req);

void dbus_build_array(dbus_t *self, char *array_req, size_t array_size);

void dbus_build_body(dbus_t *self, char *body_req);

#endif // SERVER_DBUS_H
