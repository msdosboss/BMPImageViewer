#include <stdint.h>

uint32_t bitpack32(uint8_t *raw, int offset){
    return (uint32_t) raw[offset] | raw[offset + 1] << 8 | raw[offset + 2] << 16 | raw[offset + 3] << 24;
}
uint16_t bitpack16(uint8_t *raw, int offset){
    return (uint16_t) raw[offset] | raw[offset + 1] << 8;
}
