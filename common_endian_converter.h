#ifndef ENDIAN_CONVERTER_H
#define ENDIAN_CONVERTER_H

#include <stdint.h>

// Converts the 'data' 4-byte number (in big endian)
// to a 'new_data' 4-size array of bytes (in little endian)
void big_to_little(uint32_t data, uint8_t *new_data);

// Converts the 'data' 4-size array of bytes (in little endian)
// to a 4-byte number (in big endian)
uint32_t little_to_big(uint8_t *data);

#endif // ENDIAN_CONVERTER_H
