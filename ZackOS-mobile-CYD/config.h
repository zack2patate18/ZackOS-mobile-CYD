#pragma once

#include <cstdint>
#include <cstring>
#include <SPI.h>
#include <cstdlib>
#include <stdlib.h>
#include <SD.h>
#include <FS.h>
#include <vector>
#include <esp_system.h>
#include <future>
// #include <ESP32Ping.h>
// #include <ArduinoJson.h>
// #include <HTTPClient.h>
#include <Arduino.h>
// #include "WiFi.h"

#include "lgfx_setup.h"
#include "ui.h"
#include "apps.h"
#include "system.h"
#include "utils.h"
#include "ui_elements.h"

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
extern uint16_t lock_screen_background_color;

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

extern const int keyWidth;
extern const int keyHeight;
extern const int keysPerRow;

extern String typed_text;
extern bool keyboard_active;
extern uint16_t keyboard_color;
extern App* next_app_after_keyboard;

extern char get_website_url[256];

extern long int balance;

extern const int LONG_PRESS_THRESHOLD;

extern bool swipe;
extern int swipe_direction;

extern int global_pos_x;
extern int global_pos_y;
extern bool screen_touched;

extern int current_page;
extern int total_pages;
extern int max_apps_per_page;

extern bool torch_enabled;
extern int led_pin_r;
extern int led_pin_g;
extern int led_pin_b;

extern bool notification;
extern char* notification_text;
extern uint16_t notification_color;
extern double notification_time;

extern Button calculator_equal_button;
extern Button calculator_divide_button;
extern Button calculator_multiply_button;
extern Button calculator_minus_button;
extern Button calculator_plus_button;
extern Button calculator_clear_button;
extern Button calculator_digit_buttons[10];
extern char calculator_calcul[30];

extern Button hidden_menu_buttons[3];
extern bool op_mode;