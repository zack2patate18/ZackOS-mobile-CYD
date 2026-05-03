#include "ui_elements.h"

Button::Button(int x, int y, int width, int height, uint16_t color, char* text, uint16_t outline_color) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->color = color;
    this->text = text;
    this->outline_color = outline_color;
}

void Button::draw() {
    if (debug) Serial.printf("Button draw: x=%d y=%d w=%d h=%d\n", x, y, width, height);
    tft.fillRect(x, y, width, height, color);
    tft.drawRect(x, y, width, height, outline_color);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, color);
    tft.drawString(text, x + (width / 2), y + (height / 2), 2);
}

bool Button::collide(uint16_t pos_x, uint16_t pos_y) {
    if (debug) Serial.printf("collide: pos=%d,%d btn=%d,%d w=%d h=%d\n", pos_x, pos_y, x, y, width, height);
    return ((pos_x > x) && (pos_x < x + width) && (pos_y > y) && (pos_y < y + height));
}