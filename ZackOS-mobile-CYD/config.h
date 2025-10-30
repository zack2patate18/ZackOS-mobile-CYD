#pragma once

#include <TFT_eSPI.h>
#include <cstdint>
#include <cstring>

extern uint16_t background_color;
extern uint16_t text_color;

extern const int app_margin;
extern const int default_app_color;
extern const int max_apps_per_rows;
extern const int max_apps_per_columns;
extern const int list_margin;
extern const int margin;
extern const int app_icon_size;

extern bool debug;

extern bool touch;

extern const int tft_rotation;

extern bool swapXY;
extern bool invertX;
extern bool invertY;
extern bool lock_screen_enabled;

extern char device_name[];

extern const char cpu_speed[4];
extern const char cpu_name[];

extern const char os_version[4];

extern int home_indicator_width;
extern int home_indicator_height;
extern int home_indicator_y;
extern int home_indicator_default_y;
extern bool home_indicator_touched;

extern bool top_bar_enabled;
extern const int top_bar_height;
extern uint16_t top_bar_color;

extern bool lock_screen_swipe;

extern int n_wifi;