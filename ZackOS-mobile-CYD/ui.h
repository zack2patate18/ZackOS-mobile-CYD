#pragma once

#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include "apps.h"

extern SPIClass mySpi;
extern XPT2046_Touchscreen ts;
extern TFT_eSPI tft;

struct App;

void init_screen();

void run_home();

void handle_touch();

void launch_app(App& app);

void draw_top_bar();

void draw_home_indicator();

void draw_home();

struct TouchPoint {
    int x;
    int y;
    bool touched;
};

TouchPoint get_raw_pos();

TouchPoint get_pos();