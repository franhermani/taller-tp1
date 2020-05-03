#ifndef ENDIAN_CONVERTER_H
#define ENDIAN_CONVERTER_H

#include <stdint.h>

void big_to_little(uint32_t data, uint8_t *new_data);

#endif // ENDIAN_CONVERTER_H