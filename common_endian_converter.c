#include "common_endian_converter.h"
#include <stdbool.h>

// Detects the current machine endianness
// Returns true if it's little endian, false if it's big endian
static bool host_is_little();

void host_to_little(uint32_t data, uint8_t *new_data) {
    if (host_is_little()) {
        new_data[0] = (data & 0x000000FF);
        new_data[1] = (data & 0x0000FF00) >> 8;
        new_data[2] = (data & 0x00FF0000) >> 16;
        new_data[3] = (data & 0xFF000000) >> 24;
    } else {
        new_data[0] = (data & 0x000000FF);
        new_data[1] = (data & 0x0000FF00);
        new_data[2] = (data & 0x00FF0000);
        new_data[3] = (data & 0xFF000000);
    }
}

uint32_t little_to_host(uint8_t *data) {
    uint32_t n;
    if (host_is_little()) {
        n = (data[0]) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    } else {
        n = (data[0]) | (data[1]) | (data[2]) | (data[3]);
    }
    return n;
}

static bool host_is_little() {
    uint32_t i = 1;
    uint32_t *p = &i;
    return (p[0] == 1);
}
