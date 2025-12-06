#include "utils.h"

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    uint16_t r5 = (r * 31) / 255;
    uint16_t g6 = (g * 63) / 255;
    uint16_t b5 = (b * 31) / 255;

    return (r5 << 11) | (g6 << 5) | b5;
}

uint8_t r_from565(uint16_t color) { return ((color >> 11) & 0x1F) * 255 / 31; }
uint8_t g_from565(uint16_t color) { return ((color >> 5) & 0x3F) * 255 / 63; }
uint8_t b_from565(uint16_t color) { return (color & 0x1F) * 255 / 31; }

uint16_t changeBrightness(uint16_t color, float factor) {
    uint8_t r = r_from565(color);
    uint8_t g = g_from565(color);
    uint8_t b = b_from565(color);

    r = (r * factor > 255) ? 255 : uint8_t(r * factor);
    g = (g * factor > 255) ? 255 : uint8_t(g * factor);
    b = (b * factor > 255) ? 255 : uint8_t(b * factor);

    return color565(r, g, b);
}