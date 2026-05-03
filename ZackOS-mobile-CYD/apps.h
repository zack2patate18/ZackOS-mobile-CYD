#pragma once
#include "config.h"

typedef void (*AppFunction)();

struct App {
    uint16_t color;
    AppFunction run;
    AppFunction drawner;
    char name[15];
    bool on_home;
    bool beta;
};

void home_handler();

void draw_home();

void settings_handler();

void handle_apps();

void startup();

void reboot_menu_handler();

void draw_reboot_menu();

void draw_sleep();

void draw_lock_screen();

void lock_screen_handler();

void draw_keyboard();

void keyboard_handler();

void setup_torch();

void torch_on();

void torch_off();

void torch_handler();

void draw_torch();

void draw_calculator();

void calculator_handler();

void init_calculator();

void draw_calculator_display();

extern App settings;
extern App home;
extern int current_app;
extern App app_list[];
extern int app_list_size;
extern App reboot_menu;
extern App sleep_app;
extern App lock;
extern App keyboard;
extern App themes;
extern App torch;
extern App calculator;