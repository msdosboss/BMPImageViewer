#include <stdint.h>

uint32_t bitpack32(uint8_t *raw, int offset){
    return (uint32_t) raw[offset] | raw[offset + 1] << 8 | raw[offset + 2] << 16 | raw[offset + 3] << 24;
}

uint16_t bitpack16(uint8_t *raw, int offset){
    return (uint16_t) raw[offset] | raw[offset + 1] << 8;
}

uint32_t colorTableBitpack(uint8_t *raw, int offset){
        return (uint32_t) 0xff | raw[offset] << 8 | raw[offset + 1] << 16 | raw[offset + 2] << 24;
}

//extracts colors values from 16 bitPerPixel images
uint32_t byte2Packer(uint8_t *raw){
    int opacity = 0xff;
    uint16_t colorValue = raw[0] | raw[1];
    uint8_t blue = ((raw[1] & 0b01111100) << 3) | (((raw[1]) & 0b01110000) >> 4);
    uint8_t green = ((colorValue & 0b1111100000)  >> 2) | ((colorValue & 0b1110000000) >> 7);
    uint8_t red = ((raw[0] & 0b00011111) << 3) | (((raw[0]) & 0b00011100) >> 2);

    return opacity | red << 8 | green << 16 | blue << 24;
}