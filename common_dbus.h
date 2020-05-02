#ifndef DBUS_H
#define DBUS_H

#include <stdint.h>

typedef struct {
    uint8_t type;             // 1st byte
    uint8_t data_quant;       // 2nd byte (always '1')
    uint8_t data_type;        // 3rd byte
    uint8_t end;              // 4th byte (always '00')
    uint32_t length;          // 5th to 8th byte
    char *value;              // 8th to <8 + length> byte
    uint8_t end2;             // End Of String ("\0")
} param_t;

typedef struct {
    uint8_t type;             // 1st byte (always '08')
    uint8_t data_quant;       // 2nd byte (always '1')
    uint8_t data_type;        // 3rd byte (always 'g')
    uint8_t end;              // 4th byte (always '00')
    uint8_t params_quant;     // 5th byte
    char *params_types;       // 6th to <6 + params_quant> byte
    uint8_t end2;             // <6 + params_quant + 1> byte (always '00')
} firm_t;

typedef struct {
    uint32_t length;          // 1st to 4th byte
    param_t destiny;          // 06 01 's' 00
    param_t path;             // 01 01 'o' 00
    param_t interface;        // 02 01 's' 00
    param_t method;           // 03 01 's' 00
    firm_t firm;              // 08 01 'g' 00
} array_t;

typedef struct {
    uint8_t endianness;       // 1st byte (always 'l')
    uint8_t type;             // 2nd byte (always '0x01')
    uint8_t flags;            // 3rd byte (always '0x0')
    uint8_t version;          // 4th byte (always '0x01')
    uint32_t body_length;     // 5th to 8 byte
    uint32_t id;              // 9th to 12nd byte (starts in '0x0001')
    array_t array;            // 13rd to ??? byte
} header_t;

typedef struct {
    uint32_t length;          // 1st to 4th byte
    char *value;              // 5th to <5 + length> byte
} body_param_t;

typedef struct {
    body_param_t *params;
} body_t;

typedef struct {
    header_t header;
    body_t body;
    char *destiny;
    char *path;
    char *interface;
    char *method;
    char *firm;
} message_t;

typedef struct {
    uint8_t *value;
    int length;
    int pos;
    int array_last_pos;
} byte_msg_t;

typedef struct {
    uint32_t last_id;
    message_t msg;
    byte_msg_t byte_msg;
} dbus_t;

int dbus_create(dbus_t *self);

int dbus_destroy(dbus_t *self);

byte_msg_t dbus_parse_line(dbus_t *self, char *line);

void dbus_destroy_byte_msg(dbus_t *self);

int dbus_get_array_length(dbus_t *self, char *first_bytes);

int dbus_get_body_length(dbus_t *self, char *first_bytes);

#endif // DBUS_H
