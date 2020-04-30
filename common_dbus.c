#define _POSIX_C_SOURCE 200112L

#include "common_dbus.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1

static int dbus_parse_params(dbus_t *self, char *line);

static int dbus_build_header(dbus_t *self);

static int dbus_build_body(dbus_t *self);

static int dbus_build_param_array(dbus_t *self);

static int dbus_build_destiny(dbus_t *self);

static int dbus_build_path(dbus_t *self);

static int dbus_build_interface(dbus_t *self);

static int dbus_build_method(dbus_t *self);

static int dbus_build_firm(dbus_t *self);

static int dbus_build_params_quant(dbus_t *self);

static char *dbus_build_params_types(dbus_t *self);

static int dbus_write_message(dbus_t *self);

static void dbus_write_header(dbus_t *self);

static void dbus_write_body(dbus_t *self);

static void dbus_destroy_firm_types(dbus_t *self);

static void dbus_destroy_byte_msg(dbus_t *self);


int dbus_create(dbus_t *self) {
    self->last_id = 0;
    return OK;
}

int dbus_parse_line(dbus_t *self, char *line) {
    dbus_parse_params(self, line);
    dbus_build_header(self);
    dbus_build_body(self);
    dbus_write_message(self);
    return OK;
}

static int dbus_parse_params(dbus_t *self, char *line) {
    char *rest = line;

    self->msg.destiny = strtok_r(rest, " ", &rest);
    if (! self->msg.destiny) return ERROR;

    self->msg.path = strtok_r(rest, " ", &rest);
    if (! self->msg.path) return ERROR;

    self->msg.interface = strtok_r(rest, " ", &rest);
    if (! self->msg.interface) return ERROR;

    self->msg.method = strtok_r(rest, " ", &rest);
    if (! self->msg.method) return ERROR;

    rest = self->msg.method;
    self->msg.method = strtok_r(rest, "(", &rest);
    if (! self->msg.method) return ERROR;

    self->msg.firm = strtok_r(rest, ")", &rest);
    return OK;
}

static int dbus_build_header(dbus_t *self) {
    self->msg.header.endianness = 'l';
    self->msg.header.type = 1;
    self->msg.header.flags = 0;
    self->msg.header.version = 1;
    self->msg.header.body_length = 0;   // To be filled later...
    self->msg.header.id = ++self->last_id;

    dbus_build_param_array(self);
    return OK;
}

static int dbus_build_body(dbus_t *self) {
    return OK;
}

static int dbus_build_param_array(dbus_t *self) {
    self->msg.header.array.length = 0;  // To be filled later...
    dbus_build_destiny(self);
    dbus_build_path(self);
    dbus_build_interface(self);
    dbus_build_method(self);
    if (self->msg.firm) dbus_build_firm(self);
    return OK;
}

static int dbus_build_destiny(dbus_t *self) {
    self->msg.header.array.destiny.type = 6;
    self->msg.header.array.destiny.data_quant = 1;
    self->msg.header.array.destiny.data_type = 's';
    self->msg.header.array.destiny.end = 0;
    self->msg.header.array.destiny.length = strlen(self->msg.destiny);
    self->msg.header.array.destiny.name = self->msg.destiny;
    self->msg.header.array.destiny.eos = "\0";
    return OK;
}

static int dbus_build_path(dbus_t *self) {
    self->msg.header.array.path.type = 1;
    self->msg.header.array.path.data_quant = 1;
    self->msg.header.array.path.data_type = 'o';
    self->msg.header.array.path.end = 0;
    self->msg.header.array.path.length = strlen(self->msg.path);
    self->msg.header.array.path.name = self->msg.path;
    self->msg.header.array.path.eos = "\0";
    return OK;
}

static int dbus_build_interface(dbus_t *self) {
    self->msg.header.array.interface.type = 2;
    self->msg.header.array.interface.data_quant = 1;
    self->msg.header.array.interface.data_type = 's';
    self->msg.header.array.interface.end = 0;
    self->msg.header.array.interface.length = strlen(self->msg.interface);
    self->msg.header.array.interface.name = self->msg.interface;
    self->msg.header.array.interface.eos = "\0";
    return OK;
}

static int dbus_build_method(dbus_t *self) {
    self->msg.header.array.method.type = 3;
    self->msg.header.array.method.data_quant = 1;
    self->msg.header.array.method.data_type = 's';
    self->msg.header.array.method.end = 0;
    self->msg.header.array.method.length = strlen(self->msg.method);
    self->msg.header.array.method.name = self->msg.method;
    self->msg.header.array.method.eos = "\0";
    return OK;
}

static int dbus_build_firm(dbus_t *self) {
    self->msg.header.array.firm.type = 9;
    self->msg.header.array.firm.data_quant = 1;
    self->msg.header.array.firm.data_type = 'g';
    self->msg.header.array.firm.end = 0;
    self->msg.header.array.firm.params_quant = dbus_build_params_quant(self);
    self->msg.header.array.firm.params_types = dbus_build_params_types(self);
    self->msg.header.array.firm.end2 = 0;
    return OK;
}

static int dbus_build_params_quant(dbus_t *self) {
    int i, n = 1;
    for (i = 0; i < strlen(self->msg.firm); i ++) {
        if (self->msg.firm[i] == ',') n ++;
    }
    return n;
}

static char *dbus_build_params_types(dbus_t *self) {
    int params_quant = self->msg.header.array.firm.params_quant;
    char *params_types = malloc(params_quant * sizeof(char*));

    int i;
    for (i = 0; i < self->msg.header.array.firm.params_quant; i ++) {
        params_types[i] = 's';
    }
    params_types[i] = '\0';
    return params_types;
}

static int dbus_write_message(dbus_t *self) {
    // TODO: usar el TDA dynamic_buffer
    self->pos = 0;
    self->byte_msg = malloc(1024 * sizeof(uint8_t));
    if (! self->byte_msg) return ERROR;

    dbus_write_header(self);

    if (self->msg.firm) {
        dbus_write_body(self);
        dbus_destroy_firm_types(self);
    }
    dbus_destroy_byte_msg(self);
    return OK;
}

static void dbus_write_header(dbus_t *self) {
    self->byte_msg[self->pos] = self->msg.header.endianness;
    self->byte_msg[++self->pos] = self->msg.header.type;
    self->byte_msg[++self->pos] = self->msg.header.flags;
    self->byte_msg[++self->pos] = self->msg.header.version;

    int i;
    for (i=0; i < 4; i ++)
        self->byte_msg[++self->pos] = 0;    // To be filled later...

    // TODO: aca meto el entero de 4 bytes en little endian
    // TODO: armarme una funcion que lo pase a hexa y soporte
    // TODO: enteros de mas de un byte
    self->byte_msg[++self->pos] = self->last_id;
    for (i=0; i < 3; i ++)
        self->byte_msg[++self->pos] = 0;

    
}

static void dbus_write_body(dbus_t *self) {

}

static void dbus_destroy_firm_types(dbus_t *self) {
    free(self->msg.header.array.firm.params_types);
}

static void dbus_destroy_byte_msg(dbus_t *self) {
    // TODO: eliminar esto cuando termine de debuggear
    self->byte_msg[++self->pos] = '\0';
    for (int i=0; i < self->pos; i++)
        printf("%02X ", self->byte_msg[i]);
    printf("\n");
    //
    free(self->byte_msg);
}

int dbus_destroy(dbus_t *self) {
    return OK;
}
