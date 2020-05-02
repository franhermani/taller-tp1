#define _POSIX_C_SOURCE 200112L

#include "server_dbus.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1
#define PADDING 8

static void dbus_build_params(dbus_t *self, char *array_req);

static void dbus_build_param(dbus_t *self, char *array_req, uint8_t type);

static void dbus_build_firm(dbus_t *self, char *array_req);

static void dbus_advance_padding(dbus_t *self);


int dbus_create(dbus_t *self) {
    self->last_id = 0;
    return OK;
}

int dbus_destroy(dbus_t *self) {
    return OK;
}

int dbus_get_array_length(dbus_t *self, char *first_req) {
    int array_length_pos = sizeof(self->msg.header.endianness) +
                           sizeof(self->msg.header.type) +
                           sizeof(self->msg.header.flags) +
                           sizeof(self->msg.header.version) +
                           sizeof(self->msg.header.body_length) +
                           sizeof(self->msg.header.id);

    // TODO: devolver el uint32_t
    return first_req[array_length_pos];
}

int dbus_get_body_length(dbus_t *self, char *first_req) {
    int body_length_pos = sizeof(self->msg.header.endianness) +
                          sizeof(self->msg.header.type) +
                          sizeof(self->msg.header.flags) +
                          sizeof(self->msg.header.version);

    // TODO: devolver el uint32_t
    return first_req[body_length_pos];
}

void dbus_build_array(dbus_t *self, char *array_req, size_t array_size) {
    self->byte_msg.pos = 0;
    self->msg.destiny = NULL;
    self->msg.path = NULL;
    self->msg.interface = NULL;
    self->msg.method = NULL;

    while (self->byte_msg.pos < array_size)
        dbus_build_params(self, array_req);
}

void dbus_build_body(dbus_t *self, char *body_req, size_t body_size) {
    self->byte_msg.pos = 0;
    // TODO: ...
}

static void dbus_build_params(dbus_t *self, char *array_req) {
    uint8_t type = array_req[self->byte_msg.pos];
    
    if (type == 9) {
        dbus_build_firm(self, array_req);
    } else {
        dbus_build_param(self, array_req, type);
    }
}

static void dbus_build_firm(dbus_t *self, char *array_req) {
    firm_t firm;

    self->byte_msg.pos += sizeof(firm.type) +
                          sizeof(firm.data_quant) +
                          sizeof(firm.data_type) +
                          sizeof(firm.end);

    uint8_t params_quant = array_req[self->byte_msg.pos];
    self->msg.header.array.firm.params_quant = params_quant;

    int i;
    for (i=0; i < params_quant; i++) self->byte_msg.pos ++;

    self->byte_msg.pos += sizeof(firm.end2);
    dbus_advance_padding(self);
}

static void dbus_build_param(dbus_t *self, char *array_req, uint8_t type) {
    param_t param;

    self->byte_msg.pos += sizeof(param.type) +
                          sizeof(param.data_quant) +
                          sizeof(param.data_type) +
                          sizeof(param.end);

    // TODO: leer uint32_t
    uint32_t length = array_req[self->byte_msg.pos];

    self->byte_msg.pos += sizeof(param.length);

    char value[length];

    int i;
    for (i=0; i < length; i++) {
        value[i] = array_req[self->byte_msg.pos];
        self->byte_msg.pos ++;
    }
    value[length] = '\0';

    self->byte_msg.pos += sizeof(param.end2);
    dbus_advance_padding(self);

    if (type == 1) self->msg.path = value;
    if (type == 2) self->msg.interface = value;
    if (type == 3) self->msg.method = value;
    if (type == 6) self->msg.destiny = value;
}

static void dbus_advance_padding(dbus_t *self) {
    if (self->byte_msg.pos % PADDING != 0)
        self->byte_msg.pos += PADDING - (self->byte_msg.pos % PADDING);
}
