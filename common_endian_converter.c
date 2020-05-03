#include "common_endian_converter.h"

void big_to_little(uint32_t data, uint8_t *new_data) {
    new_data[0] = (data & 0x000000FF);
    new_data[1] = (data & 0x000000FF) >> 8;
    new_data[2] = (data & 0x000000FF) >> 16;
    new_data[3] = (data & 0x000000FF) >> 24;
}
