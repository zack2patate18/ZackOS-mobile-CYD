#include "utils.h"

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t r5 = (r * 31) / 255;  // 8 bits → 5 bits
    uint16_t g6 = (g * 63) / 255;  // 8 bits → 6 bits
    uint16_t b5 = (b * 31) / 255;  // 8 bits → 5 bits

    return (r5 << 11) | (g6 << 5) | b5;
}