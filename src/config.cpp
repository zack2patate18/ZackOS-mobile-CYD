#include "config.h"
#include <cstdint>

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F);
}


uint16_t background_color = color565(0, 0, 0);
uint16_t text_color = color565(255, 255, 255);

const int app_margin = 10;
const int default_app_color = color565(252, 5, 227);
const int max_apps_per_rows = 2;
const int max_apps_per_columns = 3;

bool debug = false;
bool touch = false;
const int tft_rotation = 2;
bool lock_screen = true;

char device_name[] = "CYD-ZackPhone";