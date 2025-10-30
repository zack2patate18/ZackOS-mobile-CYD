#include "config.h"
#include "utils.h"
#include "ui.h"

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

bool swapXY = false;
bool invertX = false;
bool invertY = false;

char device_name[] = "CYD-ZackPhone";

const char cpu_name[] = "esp32";
const char cpu_speed[4] = "244";

const char os_version[4] = "0.1";

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