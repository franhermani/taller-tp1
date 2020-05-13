#define _POSIX_C_SOURCE 200112L

#include "server_dbus.h"

#define OK 0
#define ERROR -1

/* ---------------------------------------------------- */
/* Private methods to build the structs for one message */
/* ---------------------------------------------------- */

// Calls the proper build method by reading the first byte of
// the 'array_req' received from the client
static void dbus_build_params(dbus_t *self, char *array_req);

// Obtains the parameter length and value and stores it in the proper
// dbus structure attribute according to the 'type' received
static void dbus_build_param(dbus_t *self, char *array_req, uint8_t type);

// Obtains the firm's parameters quantity and stores it in the dbus structure
static void dbus_build_firm(dbus_t *self, char *array_req);

// Converts the 4-size array of bytes at 'pos' in 'array_req'
// (in little endian) to a 4-byte number (in big endian)
static uint32_t dbus_build_uint32(dbus_t *self, int pos, char *array_req);

// Advances the padding bytes according to the actual position
static void dbus_advance_padding(dbus_t *self);


/* -------------- */
/* Public methods */
/* -------------- */

void dbus_create(dbus_t *self) {
    self->last_id = 0;
}

void dbus_destroy(dbus_t *self) {
    // Do nothing
}

void dbus_destroy_array(dbus_t *self) {
    free(self->msg.destiny);
    free(self->msg.path);
    free(self->msg.interface);
    free(self->msg.method);
}

void dbus_destroy_body(dbus_t *self) {
    int i;
    for (i = 0; i < self->msg.header.array.firm.params_quant; i ++)
        free(self->msg.body.params[i].value);

    free(self->msg.body.params);
}

void dbus_set_message_id(dbus_t *self, char *first_req) {
    int message_id_pos = sizeof(self->msg.header.endianness) +
                         sizeof(self->msg.header.type) +
                         sizeof(self->msg.header.flags) +
                         sizeof(self->msg.header.version) +
                         sizeof(self->msg.header.body_length);

    self->last_id = dbus_build_uint32(self, message_id_pos, first_req);
}

int dbus_get_array_length(dbus_t *self, char *first_req) {
    int array_length_pos = sizeof(self->msg.header.endianness) +
                           sizeof(self->msg.header.type) +
                           sizeof(self->msg.header.flags) +
                           sizeof(self->msg.header.version) +
                           sizeof(self->msg.header.body_length) +
                           sizeof(self->msg.header.id);

    self->msg.header.array.length = dbus_build_uint32(self, array_length_pos,
                                                      first_req);
    return self->msg.header.array.length;
}

int dbus_get_body_length(dbus_t *self, char *first_req) {
    int body_length_pos = sizeof(self->msg.header.endianness) +
                          sizeof(self->msg.header.type) +
                          sizeof(self->msg.header.flags) +
                          sizeof(self->msg.header.version);

    self->msg.header.body_length = dbus_build_uint32(self, body_length_pos,
                                                     first_req);
    return self->msg.header.body_length;
}

int dbus_get_array_last_padding(dbus_t *self, int array_len) {
    return ((array_len % PADDING == 0) ? 0 : PADDING - (array_len % PADDING));
}

void dbus_build_array(dbus_t *self, char *array_req, size_t array_size) {
    self->byte_msg.pos = 0;

    while (self->byte_msg.pos < array_size)
        dbus_build_params(self, array_req);
}

void dbus_build_body(dbus_t *self, char *body_req) {
    body_t body;
    self->byte_msg.pos = 0;

    uint8_t params_quant = self->msg.header.array.firm.params_quant;
    body_param_t *params = malloc(params_quant * sizeof(body_param_t));
    self->msg.body.params = params;

    int i;
    for (i = 0; i < params_quant; i ++) {
        uint32_t len = dbus_build_uint32(self, self->byte_msg.pos, body_req);
        char *value = malloc((len * sizeof(char)) + 1);

        self->byte_msg.pos += sizeof(body.params->length);

        int j;
        for (j = 0; j < len; j ++) {
            value[j] = body_req[self->byte_msg.pos];
            self->byte_msg.pos ++;
        }
        value[len] = '\0';
        self->msg.body.params[i].length = len;
        self->msg.body.params[i].value = value;
        self->byte_msg.pos ++;
    }
}

/* ---------------------------------------------------- */
/* Private methods to build the structs for one message */
/* ---------------------------------------------------- */

static void dbus_build_params(dbus_t *self, char *array_req) {
    uint8_t type = array_req[self->byte_msg.pos];

    if (type == TYPE_FIRM) {
        dbus_build_firm(self, array_req);
    } else {
        dbus_build_param(self, array_req, type);
    }
}

static void dbus_build_firm(dbus_t *self, char *array_req) {
    firm_t firm;

    self->byte_msg.pos += sizeof(firm.type) + sizeof(firm.data_quant) +
                          sizeof(firm.data_type) + sizeof(firm.end);

    uint8_t params_quant = array_req[self->byte_msg.pos];
    self->msg.header.array.firm.params_quant = params_quant;

    int i;
    for (i = 0; i < params_quant; i ++) self->byte_msg.pos ++;

    self->byte_msg.pos += sizeof(firm.end2);
    dbus_advance_padding(self);
}

static void dbus_build_param(dbus_t *self, char *array_req, uint8_t type) {
    param_t param;
    self->byte_msg.pos += sizeof(param.type) + sizeof(param.data_quant) +
                          sizeof(param.data_type) + sizeof(param.end);

    uint32_t len = dbus_build_uint32(self, self->byte_msg.pos, array_req);
    char *value = malloc((len * sizeof(char)) + 1);
    self->byte_msg.pos += sizeof(param.length);

    int i;
    for (i = 0; i < len; i ++) {
        value[i] = array_req[self->byte_msg.pos];
        self->byte_msg.pos ++;
    }
    value[len] = '\0';
    self->byte_msg.pos += sizeof(param.end2);
    dbus_advance_padding(self);

    if (type == TYPE_PATH) self->msg.path = value;
    if (type == TYPE_INTERFACE) self->msg.interface = value;
    if (type == TYPE_METHOD) self->msg.method = value;
    if (type == TYPE_DESTINY) self->msg.destiny = value;
}

static uint32_t dbus_build_uint32(dbus_t *self, int pos, char *array_req) {
    uint8_t data[4];
    int i, j = 0;
    for (i = pos; i < pos + 4; i ++) {
        data[j] = array_req[i];
        j ++;
    }
    return little_to_host(data);
}

static void dbus_advance_padding(dbus_t *self) {
    if (self->byte_msg.pos % PADDING != 0)
        self->byte_msg.pos += PADDING - (self->byte_msg.pos % PADDING);
}
