#define _POSIX_C_SOURCE 200112L

#include "common_dbus.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1
#define PADDING 8

static int dbus_parse_params(dbus_t *self, char *line);


/* Methods to fill the structs for one message */

static int dbus_build_header(dbus_t *self);

static int dbus_build_body(dbus_t *self);

static int dbus_build_param_array(dbus_t *self);

static int dbus_build_param(param_t *param, uint8_t type,
                            uint8_t data_type, char *value);

static int dbus_build_firm(firm_t *firm, uint8_t type,
                           uint8_t data_type, char *value);

static uint8_t dbus_build_params_quant(char *value);

static char *dbus_build_params_types(uint8_t params_quant);


/* Methods to transform the structs into a uint8_t array */

static int dbus_write_message(dbus_t *self);

static void dbus_write_header(dbus_t *self);

static void dbus_write_body(dbus_t *self);

static void dbus_write_params_array(dbus_t *self);

static void dbus_write_param(dbus_t *self, param_t param);

static void dbus_write_firm(dbus_t *self, firm_t firm);


/* Method to apply padding to the uin8_t array */

static void dbus_apply_padding(dbus_t *self);


/* Methods to destroy */

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

    dbus_build_param(&self->msg.header.array.destiny, 6, 's',
                     self->msg.destiny);
    dbus_build_param(&self->msg.header.array.path, 1, 'o',
                     self->msg.path);
    dbus_build_param(&self->msg.header.array.interface, 2, 's',
                     self->msg.interface);
    dbus_build_param(&self->msg.header.array.method, 3, 's',
                     self->msg.method);

    if (self->msg.firm)
        dbus_build_firm(&self->msg.header.array.firm, 9, 'g', self->msg.firm);

    return OK;
}

static int dbus_build_param(param_t *param, uint8_t type,
                            uint8_t data_type, char *value) {
    param->type = type;
    param->data_quant = 1;
    param->data_type = data_type;
    param->end = 0;
    param->length = strlen(value);
    param->value = value;
    param->end2 = '\0';
    return OK;
}

static int dbus_build_firm(firm_t *firm, uint8_t type,
                           uint8_t data_type, char *value) {
    firm->type = type;
    firm->data_quant = 1;
    firm->data_type = data_type;
    firm->end = 0;
    firm->params_quant = dbus_build_params_quant(value);
    firm->params_types = dbus_build_params_types(firm->params_quant);
    firm->end2 = 0;
    return OK;
}

static uint8_t dbus_build_params_quant(char *value) {
    uint8_t n = 1;
    int i;
    for (i = 0; i < strlen(value); i ++) {
        if (value[i] == ',') n ++;
    }
    return n;
}

static char *dbus_build_params_types(uint8_t params_quant) {
    char *params_types = malloc(params_quant * sizeof(char*));

    int i;
    for (i = 0; i < params_quant; i ++) {
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

    // TODO: llamar a una funcion que tome un uint32_t y lo escriba en little endian
    // Longitud del cuerpo
    int i;
    for (i=0; i < 4; i ++)
        self->byte_msg[++self->pos] = 0;    // To be filled later...

    // TODO: llamar a una funcion que tome un uint32_t y lo escriba en little endian
    // ID del mensaje
    self->byte_msg[++self->pos] = self->last_id;
    for (i=0; i < 3; i ++)
        self->byte_msg[++self->pos] = 0;

    dbus_write_params_array(self);
}

static void dbus_write_body(dbus_t *self) {

}

static void dbus_write_params_array(dbus_t *self) {
    // TODO: llamar a una funcion que tome un uint32_t y lo escriba en little endian
    // Longitud del array
    int i;
    for (i=0; i < 4; i ++)
        self->byte_msg[++self->pos] = 0;    // To be filled later...

    dbus_write_param(self, self->msg.header.array.destiny);
    dbus_write_param(self, self->msg.header.array.path);
    dbus_write_param(self, self->msg.header.array.interface);
    dbus_write_param(self, self->msg.header.array.method);

    if (self->msg.firm)
        dbus_write_firm(self, self->msg.header.array.firm);
}

static void dbus_write_param(dbus_t *self, param_t param) {
    // TODO: ...
    dbus_apply_padding(self);
}

static void dbus_write_firm(dbus_t *self, firm_t firm) {

}

static void dbus_apply_padding(dbus_t *self) {

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
