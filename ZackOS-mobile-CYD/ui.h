#pragma once

#include "config.h"

extern LGFX tft;

struct App;

void init_screen();
void run_home();
void draw_top_bar();
void draw_home_indicator();
void draw_home();

struct TouchPoint {
    int  x;
    int  y;
    bool touched;
};

TouchPoint get_raw_pos();
void remap_pos(uint16_t tx, uint16_t ty);
TouchPoint get_pos();

void launch_app(App& app);
void handle_touch();
void handle_swipe(TouchPoint start, TouchPoint end);
void notification_handler();