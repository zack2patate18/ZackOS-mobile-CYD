#pragma once

#include <TFT_eSPI.h>
#include <cstdint>

extern uint16_t background_color;
extern uint16_t text_color;

extern const int app_margin;
extern const int default_app_color;
extern const int max_apps_per_rows;
extern const int max_apps_per_columns;

extern bool debug;

extern bool touch;

extern const int tft_rotation;

extern bool swapXY;
extern bool invertX;
extern bool invertY;
extern bool need_to_be_refresh;
extern bool lock_screen;

extern char device_name[];