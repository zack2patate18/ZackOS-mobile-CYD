#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include "lgfx_setup.h"
#include <cstdint>

extern bool debug;
extern LGFX tft;

class Button {
    public:
    int x, y, width, height;
    uint16_t color;
    uint16_t outline_color;
    char* text;

    Button(int x, int y, int width, int height, uint16_t color, char* text, uint16_t outline_color);
    void draw();
    bool collide(uint16_t pos_x, uint16_t pos_y);
};

#endif