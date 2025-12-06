#pragma once

#include <cstdint>
#include <Arduino.h>

uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
uint16_t changeBrightness(uint16_t color, float factor);