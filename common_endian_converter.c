#include "common_endian_converter.h"

void big_to_little(uint32_t data, uint8_t *new_data) {
    new_data[0] = (data & 0x000000FF);
    new_data[1] = (data & 0x0000FF00) >> 8;
    new_data[2] = (data & 0x00FF0000) >> 16;
    new_data[3] = (data & 0xFF000000) >> 24;
}

uint32_t little_to_big(uint8_t *data) {
    return (data[0]) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}
