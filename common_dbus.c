#define _POSIX_C_SOURCE 200112L

#include "common_dbus.h"
#include <string.h>
#include <stdio.h>

#define OK 0
#define ERROR -1

int dbus_create(dbus_t *dbus) {
    return OK;
}

int dbus_parse_line(dbus_t *self, char *line, const char *delim) {
    char *destiny, *path, *interface, *method;
    char *rest = line;

    destiny = strtok_r(rest, delim, &rest);
    path = strtok_r(rest, delim, &rest);
    interface = strtok_r(rest, delim, &rest);
    method = strtok_r(rest, delim, &rest);

    dbus_parse_destiny(self, destiny);
    dbus_parse_path(self, path);
    dbus_parse_interface(self, interface);
    dbus_parse_method(self, method);

    return OK;
}

int dbus_parse_destiny(dbus_t *self, char *destiny) {
    return OK;
}

int dbus_parse_path(dbus_t *self, char *path) {
    return OK;
}

int dbus_parse_interface(dbus_t *self, char *interface) {
    return OK;
}

int dbus_parse_method(dbus_t *self, char *method) {
    return OK;
}

int dbus_write_header(dbus_t *self) {
    return OK;
}

int dbus_write_body(dbus_t *self) {
    return OK;
}

int dbus_destroy(dbus_t *self) {
    return OK;
}
