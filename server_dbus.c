#define _POSIX_C_SOURCE 200112L

#include "server_dbus.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1
#define PADDING 8
#define LATER 255

int dbus_create(dbus_t *self) {
    self->last_id = 0;
    return OK;
}

int dbus_destroy(dbus_t *self) {
    return OK;
}

int dbus_get_array_length(dbus_t *self, char *first_bytes) {
    int array_length_pos = sizeof(self->msg.header.endianness) +
                           sizeof(self->msg.header.type) +
                           sizeof(self->msg.header.flags) +
                           sizeof(self->msg.header.version) +
                           sizeof(self->msg.header.body_length) +
                           sizeof(self->msg.header.id);

    // TODO: devolver el uint32_t
    return first_bytes[array_length_pos];
}

int dbus_get_body_length(dbus_t *self, char *first_bytes) {
    int body_length_pos = sizeof(self->msg.header.endianness) +
                          sizeof(self->msg.header.type) +
                          sizeof(self->msg.header.flags) +
                          sizeof(self->msg.header.version);

    // TODO: devolver el uint32_t
    return first_bytes[body_length_pos];
}
