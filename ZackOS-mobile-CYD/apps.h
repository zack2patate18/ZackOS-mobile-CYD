#pragma once
#include "ui.h"
#include <cstdint>

typedef void (*AppFunction)();

struct App {
    uint16_t color;
    AppFunction run;
    AppFunction drawner;
    char name[9];
};

void home_handler();

void draw_home();

void settings_handler();

void handle_apps();

void startup();

void reboot_menu_handler();

void draw_reboot_menu();

void draw_sleep();

extern App settings;
extern App home;
extern int current_app;
extern App app_list[];
extern int app_list_size;
extern App reboot_menu;
extern App sleep_app;