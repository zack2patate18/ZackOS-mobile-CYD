#pragma once

#include "config.h"

// ── Instances globales (définies dans ui.cpp) ──────────────────────────────
extern LGFX tft;

struct App;

// ── Ecran ──────────────────────────────────────────────────────────────────
void init_screen();
void run_home();
void draw_top_bar();
void draw_home_indicator();
void draw_home();

// ── Touch ──────────────────────────────────────────────────────────────────
struct TouchPoint {
    int  x;
    int  y;
    bool touched;
};

TouchPoint get_raw_pos();
void       remap_pos();
TouchPoint get_pos();

// ── Navigation ─────────────────────────────────────────────────────────────
void launch_app(App& app);
void handle_touch();
void handle_swipe(TouchPoint start, TouchPoint end);