#ifndef BITPACK_H
#define BITPACK_H
#include <stdint.h>

uint32_t bitpack32(uint8_t *raw, int offset);
uint16_t bitpack16(uint8_t *raw, int offset);
uint32_t colorTableBitpack(uint8_t *raw, int offset);

#endif
