#include "config.h"
#include "utils.h"
#include "ui.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>

uint16_t background_color = color565(10, 10, 255);
uint16_t text_color = color565(255, 255, 255);

const int app_margin = 20;
const int default_app_color = color565(252, 5, 227);
const int max_apps_per_rows = 2;
const int max_apps_per_columns = 3;
const int list_margin = 10;
const int margin = 30;
const int app_icon_size = 60;

bool debug = true;
bool touch = false;
const int tft_rotation = 2;
bool lock_screen_enabled = true;
uint16_t lock_screen_background_color = color565(0, 23, 178);

bool swapXY = false;
bool invertX = false;
bool invertY = false;

char device_name[] = "CYD-ZackPhone";

const char cpu_name[] = "esp32";
const char cpu_speed[4] = "244";

const char os_version[4] = "0.3";

int home_indicator_width = tft.width() / 2;
int home_indicator_height = 8;
int home_indicator_y = tft.height() - home_indicator_height;
bool home_indicator_touched = false;
int home_indicator_default_y = tft.height() - home_indicator_height;

bool top_bar_enabled = true;
const int top_bar_height = 15;
uint16_t top_bar_color = color565(100, 100, 100);

bool lock_screen_swipe = false;

int n_wifi = -1;
int max_wifis = 7;

const int keyWidth = 40;
const int keyHeight = 40;
const int keysPerRow = 10;

const int MAX_DISPLAY_WIFI = max_wifis;
String selectedSSID = "";
bool wifi_password_required = false;

String typed_text = "";
bool keyboard_active = false;
uint16_t keyboard_color = color565(100, 100, 100);
App* next_app_after_keyboard = &home;

char get_website_url[256] = "172.20.10.3/get_html";

long int balance = random(-638, 293829);

const int LONG_PRESS_THRESHOLD = 150;

bool swipe = false;
int swipe_direction;

int global_pos_x = -1;
int global_pos_y = -1;
bool screen_touched = false;

int current_page = 0;
int max_apps_per_page = max_apps_per_rows * max_apps_per_columns;
int total_pages = (app_list_size + max_apps_per_page - 1) / max_apps_per_page;