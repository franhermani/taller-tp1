#define _POSIX_C_SOURCE 200112L

#include "common_dbus.h"
#include <string.h>
#include <stdio.h>

#define OK 0
#define ERROR -1

static int dbus_parse_params(dbus_t *self, char *line);

static int dbus_build_header(dbus_t *self);

static int dbus_build_param_array(dbus_t *self);

static int dbus_build_destiny(dbus_t *self);

static int dbus_build_path(dbus_t *self);

static int dbus_build_interface(dbus_t *self);

static int dbus_build_method(dbus_t *self);

static int dbus_build_firm(dbus_t *self);

static int dbus_build_body(dbus_t *self);


int dbus_create(dbus_t *self) {
    self->last_id = 0;
    return OK;
}

int dbus_parse_line(dbus_t *self, char *line) {
    dbus_parse_params(self, line);
    dbus_build_header(self);
    dbus_build_body(self);
    return OK;
}

static int dbus_parse_params(dbus_t *self, char *line) {
    char *rest = line;

    self->destiny = strtok_r(rest, " ", &rest);
    if (! self->destiny) return ERROR;

    self->path = strtok_r(rest, " ", &rest);
    if (! self->path) return ERROR;

    self->interface = strtok_r(rest, " ", &rest);
    if (! self->interface) return ERROR;

    self->method = strtok_r(rest, " ", &rest);
    if (! self->method) return ERROR;

    rest = self->method;
    self->method = strtok_r(rest, "(", &rest);
    if (! self->method) return ERROR;

    self->method_params = strtok_r(rest, ")", &rest);
    return OK;
}

static int dbus_build_header(dbus_t *self) {
    self->header.endianness = "l";
    self->header.type = 0x01;
    self->header.flags = 0x0;
    self->header.version = 0x01;
    self->header.body_length = 0;   // To be filled later...
    self->header.id = ++self->last_id;

    dbus_build_param_array(self);
    return OK;
}

static int dbus_build_body(dbus_t *self) {
    dbus_build_firm(self);
    return OK;
}

static int dbus_build_param_array(dbus_t *self) {
    self->header.array.length = 0;  // To be filled later...
    dbus_build_destiny(self);
    dbus_build_path(self);
    dbus_build_interface(self);
    dbus_build_method(self);
    return OK;
}

static int dbus_build_destiny(dbus_t *self) {
    self->header.array.destiny.type = 6;
    self->header.array.destiny.data_quant = 1;
    self->header.array.destiny.data_type = "s";
    self->header.array.destiny.end = 0;
    self->header.array.destiny.length = strlen(self->destiny);
    self->header.array.destiny.name = self->destiny;
    self->header.array.destiny.eos = "\0";
    return OK;
}

static int dbus_build_path(dbus_t *self) {
    self->header.array.path.type = 1;
    self->header.array.path.data_quant = 1;
    self->header.array.path.data_type = "o";
    self->header.array.path.end = 0;
    self->header.array.path.length = strlen(self->path);
    self->header.array.path.name = self->path;
    self->header.array.path.eos = "\0";
    return OK;
}

static int dbus_build_interface(dbus_t *self) {
    self->header.array.interface.type = 2;
    self->header.array.interface.data_quant = 1;
    self->header.array.interface.data_type = "s";
    self->header.array.interface.end = 0;
    self->header.array.interface.length = strlen(self->interface);
    self->header.array.interface.name = self->interface;
    self->header.array.interface.eos = "\0";
    return OK;
}

static int dbus_build_method(dbus_t *self) {
    self->header.array.method.type = 3;
    self->header.array.method.data_quant = 1;
    self->header.array.method.data_type = "s";
    self->header.array.method.end = 0;
    self->header.array.method.length = strlen(self->method);
    self->header.array.method.name = self->method;
    self->header.array.method.eos = "\0";
    return OK;
}

static int dbus_build_firm(dbus_t *self) {
    return OK;
}

int dbus_destroy(dbus_t *self) {
    return OK;
}
