#ifndef DBUS_H
#define DBUS_H

#include <stdint.h>

#define MAX_PARAMS  50

typedef struct {
    char *type;             // 1st byte
    char *data_quant;       // 2nd byte (always '1')
    char *data_type;        // 3rd byte
    char *end;              // 4th byte (always '00')
    uint32_t length;        // 5th to 8th byte
    char *name;           // 8th to <8 + length> byte
} param_t;

typedef struct {
    char *type;             // 1st byte (always '08')
    char *data_quant;       // 2nd byte (always '1')
    char *data_type;        // 3rd byte (always 'g')
    char *end;              // 4th byte (always '00')
    uint8_t params_quant;   // 5th byte
    char *params_types;      // 6th to <6 + params_quant> byte
    char *end2;             // <6 + params_quant + 1> byte (always '00')
} firm_t;

typedef struct {
    uint32_t header_length; // 1st to 4th byte
    param_t destiny;        // 06 01 's' 00
    param_t path;           // 01 01 'o' 00
    param_t interface;      // 02 01 's' 00
    param_t method;         // 03 01 's' 00
    firm_t firm;            // 08 01 'g' 00
} array_t;

typedef struct {
    char *endianness;       // 1st byte (always 'l')
    char *type;             // 2nd byte (always '0x01')
    char *flags;            // 3rd byte (always '0x0')
    char *version;          // 4th byte (always '0x01')
    uint32_t body_length;   // 5th to 8 byte
    uint32_t id;            // 9th to 12nd byte (starts in '0x0001')
    array_t array;          // 13rd to ??? byte
} header_t;

typedef struct {
    uint32_t length;        // 1st to 4th byte
    char *name;           // 5th to <5 + length> byte
} body_param_t;

typedef struct {
    body_param_t params[MAX_PARAMS];
} body_t;

typedef struct {
    header_t *header;
    body_t *body;
} dbus_t;

#endif // DBUS_H
