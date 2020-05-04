#define _POSIX_C_SOURCE 200112L

#include "client_dbus.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define OK 0
#define ERROR -1
#define PADDING 8
#define LATER 0

/* --------------------------------------------------- */
/* Private methods to fill the structs for one message */
/* --------------------------------------------------- */

static int dbus_parse_params(dbus_t *self, char *line);

static int dbus_build_header(dbus_t *self);

static int dbus_build_param_array(dbus_t *self);

static int dbus_build_param(param_t *param, uint8_t type,
                            uint8_t data_type, char *value);

static int dbus_build_firm(firm_t *firm, uint8_t type,
                           uint8_t data_type, char *value);

static uint8_t dbus_build_params_quant(char *value);

static char *dbus_build_params_types(uint8_t params_quant);

/* ------------------------------------------------------------- */
/* Private methods to transform the structs into a uint8_t array */
/* ------------------------------------------------------------- */

static int dbus_write_message(dbus_t *self);

static void dbus_write_header(dbus_t *self);

static void dbus_write_body(dbus_t *self);

static void dbus_write_params_array(dbus_t *self);

static void dbus_write_param(dbus_t *self, param_t param);

static void dbus_write_firm(dbus_t *self, firm_t firm);

static void dbus_write_body_length(dbus_t *self);

static void dbus_write_array_length(dbus_t *self);

static void dbus_write_uint32(dbus_t *self, uint32_t value);

static void dbus_overwrite_uint32(dbus_t *self, int pos, uint32_t value);

static void dbus_write_padding(dbus_t *self);

static void dbus_destroy_firm_types(dbus_t *self);


/* -------------- */
/* Public methods */
/* -------------- */

int dbus_create(dbus_t *self) {
    self->last_id = 0;
    return OK;
}

int dbus_destroy(dbus_t *self) {
    return OK;
}

byte_msg_t dbus_parse_line(dbus_t *self, char *line) {
    dbus_parse_params(self, line);
    dbus_build_header(self);
    dbus_write_message(self);
    self->byte_msg.length = self->byte_msg.pos;

    return self->byte_msg;
}

/* --------------------------------------------------- */
/* Private methods to fill the structs for one message */
/* --------------------------------------------------- */

static int dbus_parse_params(dbus_t *self, char *line) {
    char *rest = line;
    char *params = strtok_r(rest, "(", &rest);

    self->msg.destiny = strtok_r(params, " ", &params);
    if (! self->msg.destiny) return ERROR;

    self->msg.path = strtok_r(params, " ", &params);
    if (! self->msg.path) return ERROR;

    self->msg.interface = strtok_r(params, " ", &params);
    if (! self->msg.interface) return ERROR;

    self->msg.method = strtok_r(params, " ", &params);
    if (! self->msg.method) return ERROR;

    self->msg.firm = strtok_r(rest, ")", &rest);

    return OK;
}

static int dbus_build_header(dbus_t *self) {
    self->msg.header.endianness = 'l';
    self->msg.header.type = 1;
    self->msg.header.flags = 0;
    self->msg.header.version = 1;
    self->msg.header.body_length = LATER; // Filled in dbus_write_body_length
    self->msg.header.id = ++self->last_id;
    dbus_build_param_array(self);
    return OK;
}

static int dbus_build_param_array(dbus_t *self) {
    self->msg.header.array.length = LATER; // Filled in dbus_write_header

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

/* ------------------------------------------------------------- */
/* Private methods to transform the structs into a uint8_t array */
/* ------------------------------------------------------------- */

static int dbus_write_message(dbus_t *self) {
    // TODO: usar el TDA dynamic_buffer
    self->byte_msg.pos = 0;
    self->byte_msg.value = malloc(1024 * sizeof(uint8_t));
    if (! self->byte_msg.value) return ERROR;

    dbus_write_header(self);

    if (self->msg.firm) {
        dbus_write_body(self);
        dbus_destroy_firm_types(self);
    }
    dbus_write_body_length(self);
    dbus_write_array_length(self);

    self->byte_msg.value[++self->byte_msg.pos] = '\0';

    return OK;
}

static void dbus_write_header(dbus_t *self) {
    self->byte_msg.value[self->byte_msg.pos] = self->msg.header.endianness;
    self->byte_msg.value[++self->byte_msg.pos] = self->msg.header.type;
    self->byte_msg.value[++self->byte_msg.pos] = self->msg.header.flags;
    self->byte_msg.value[++self->byte_msg.pos] = self->msg.header.version;

    // Body length will be filled in dbus_write_body_length
    for (int i = 0; i < 4; i ++)
        self->byte_msg.value[++self->byte_msg.pos] = LATER;

    // Id in little endian
    dbus_write_uint32(self, self->last_id);

    int prev_pos = self->byte_msg.pos;

    dbus_write_params_array(self);

    // Fills array length
    self->msg.header.array.length = self->byte_msg.array_last_pos - prev_pos;
}

static void dbus_write_body(dbus_t *self) {
    uint8_t param_length;
    int i, prev_pos = self->byte_msg.pos;
    char *param;
    char *rest = self->msg.firm;

    while ((param = strtok_r(rest, ",", &rest))) {
        param_length = strlen(param);

        // Param length in little endian
        dbus_write_uint32(self, param_length);

        for (i = 0; i < param_length; i ++)
            self->byte_msg.value[++self->byte_msg.pos] = param[i];

        self->byte_msg.value[++self->byte_msg.pos] = 0;
    }
    self->msg.header.body_length = self->byte_msg.pos - prev_pos;
}

static void dbus_write_params_array(dbus_t *self) {
    // Array length will be filled in dbus_write_array_length
    for (int i = 0; i < 4; i ++)
        self->byte_msg.value[++self->byte_msg.pos] = LATER;

    dbus_write_param(self, self->msg.header.array.destiny);
    dbus_write_param(self, self->msg.header.array.path);
    dbus_write_param(self, self->msg.header.array.interface);
    dbus_write_param(self, self->msg.header.array.method);

    if (self->msg.firm)
        dbus_write_firm(self, self->msg.header.array.firm);
}

static void dbus_write_param(dbus_t *self, param_t param) {
    self->byte_msg.value[++self->byte_msg.pos] = param.type;
    self->byte_msg.value[++self->byte_msg.pos] = param.data_quant;
    self->byte_msg.value[++self->byte_msg.pos] = param.data_type;
    self->byte_msg.value[++self->byte_msg.pos] = param.end;

    // Param length in little endian
    dbus_write_uint32(self, param.length);

    int i;
    for (i = 0; i < param.length; i ++)
        self->byte_msg.value[++self->byte_msg.pos] = param.value[i];

    self->byte_msg.value[++self->byte_msg.pos] = param.end2;

    // Stores the final position of the array (doesn't include last padding)
    self->byte_msg.array_last_pos = self->byte_msg.pos;

    dbus_write_padding(self);
}

static void dbus_write_firm(dbus_t *self, firm_t firm) {
    self->byte_msg.value[++self->byte_msg.pos] = firm.type;
    self->byte_msg.value[++self->byte_msg.pos] = firm.data_quant;
    self->byte_msg.value[++self->byte_msg.pos] = firm.data_type;
    self->byte_msg.value[++self->byte_msg.pos] = firm.end;
    self->byte_msg.value[++self->byte_msg.pos] = firm.params_quant;

    int i;
    for (i = 0; i < firm.params_quant; i ++)
        self->byte_msg.value[++self->byte_msg.pos] = firm.params_types[i];

    self->byte_msg.value[++self->byte_msg.pos] = firm.end2;

    // Stores the final position of the array (doesn't include last padding)
    self->byte_msg.array_last_pos = self->byte_msg.pos;

    dbus_write_padding(self);
}

static void dbus_write_body_length(dbus_t *self) {
    int body_length_pos = sizeof(self->msg.header.endianness) +
                          sizeof(self->msg.header.type) +
                          sizeof(self->msg.header.flags) +
                          sizeof(self->msg.header.version);

    // Body length in little endian
    uint32_t body_length = self->msg.header.body_length;
    dbus_overwrite_uint32(self, body_length_pos, body_length);
}

static void dbus_write_array_length(dbus_t *self) {
    int array_length_pos = sizeof(self->msg.header.endianness) +
                           sizeof(self->msg.header.type) +
                           sizeof(self->msg.header.flags) +
                           sizeof(self->msg.header.version) +
                           sizeof(self->msg.header.body_length) +
                           sizeof(self->msg.header.id);

    // Array length in little endian
    uint32_t array_length = self->msg.header.array.length;
    dbus_overwrite_uint32(self, array_length_pos, array_length);
}

static void dbus_write_uint32(dbus_t *self, uint32_t value) {
    uint8_t new_data[4];
    big_to_little(value, new_data);
    for (int i = 0; i < 4; i ++)
        self->byte_msg.value[++self->byte_msg.pos] = new_data[i];
}

static void dbus_overwrite_uint32(dbus_t *self, int pos, uint32_t value) {
    uint8_t new_data[4];
    big_to_little(value, new_data);
    int i, j = 0;
    for (i = pos; i < pos + 4; i ++) {
        self->byte_msg.value[i] = new_data[j];
        j ++;
    }
}

static void dbus_write_padding(dbus_t *self) {
    int n = PADDING - (self->byte_msg.pos % PADDING) - 1;
    int i;
    for (i = 0; i < n; i ++) self->byte_msg.value[++self->byte_msg.pos] = 0;
}

static void dbus_destroy_firm_types(dbus_t *self) {
    free(self->msg.header.array.firm.params_types);
}

void dbus_destroy_byte_msg(dbus_t *self) {
    free(self->byte_msg.value);
}
