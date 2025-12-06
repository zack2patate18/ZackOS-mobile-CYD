#include "ui.h"
#include "apps.h"
#include "config.h"
#include "system.h"
#include "utils.h"
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <cstring>

#define X_MIN_RAW  325
#define X_MAX_RAW  3776
#define Y_MIN_RAW  386
#define Y_MAX_RAW  3843

#define XPT2046_CS   33
#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25

SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
TFT_eSPI tft = TFT_eSPI();

void launch_app(App& app) {
    if (strcmp(app.name, "L_screen") != 0) {
        if (debug) Serial.println("Not lockscreen");
        for (int i = 0; i < app_list_size; i++) {
            if (strcmp(app_list[i].name, app.name) == 0) {
                current_app = i;
                if (debug) Serial.println("App founds");
                break;
            }
        }
    }

    if (debug) {
        Serial.print("New app : ");
        Serial.println(app_list[current_app].name);
    }

    // app.drawner();
    // app.run();
    need_to_be_redrawn = true;
    need_to_be_refreshed = true;
    touch = false;
}



void init_screen() {
    mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    ts.begin(mySpi);
    ts.setRotation(2);
    tft.init();
    tft.setRotation(2);
}

void draw_home_indicator() {
    int x = (tft.width() - home_indicator_width) / 2;
    tft.fillRect(x, home_indicator_y - home_indicator_height, home_indicator_width, home_indicator_height, color565(255, 255, 255));
    tft.fillCircle(x, home_indicator_y - home_indicator_height + (home_indicator_height / 2), home_indicator_height / 2, color565(255, 255, 255));
    tft.fillCircle(x + home_indicator_width, home_indicator_y + (home_indicator_height / 2) - home_indicator_height, home_indicator_height / 2, color565(255, 255, 255));
}

void draw_top_bar() {
    tft.fillRect(0, 0, tft.width(), top_bar_height, top_bar_color);
    tft.setTextColor(color565(0, 0, 0), top_bar_color);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(current_time, tft.width() / 2, top_bar_height / 2, 1);
    if (torch_enabled) tft.drawString("Torch", tft.width() / 6, top_bar_height / 2, 1);
}

void run_home() {
    draw_home();
    draw_home_indicator();
    draw_top_bar();
}

struct SwipeState {
    bool is_swiping;
    TouchPoint start_point;
    unsigned long start_time;
    TouchPoint last_point;
};

SwipeState swipe_state = {false, {0, 0}, 0, {0, 0}};

const int SWIPE_THRESHOLD = 50;
const int SWIPE_TIMEOUT = 500;
const int SWIPE_MAX_PERPENDICULAR = 40;

enum SwipeDirection {
    SWIPE_NONE,
    SWIPE_UP,
    SWIPE_DOWN,
    SWIPE_LEFT,
    SWIPE_RIGHT
};

SwipeDirection detect_swipe(TouchPoint start, TouchPoint end, unsigned long duration) {
    if (duration > SWIPE_TIMEOUT) {
        return SWIPE_NONE;
    }
    
    int delta_x = end.x - start.x;
    int delta_y = end.y - start.y;
    
    int abs_delta_x = abs(delta_x);
    int abs_delta_y = abs(delta_y);
    
    if (abs_delta_x > abs_delta_y) {
        if (abs_delta_x >= SWIPE_THRESHOLD && abs_delta_y <= SWIPE_MAX_PERPENDICULAR) {
            if (delta_x > 0) {
                return SWIPE_RIGHT;
            } else {
                return SWIPE_LEFT;
            }
        }
    }
    else {
        if (abs_delta_y >= SWIPE_THRESHOLD && abs_delta_x <= SWIPE_MAX_PERPENDICULAR) {
            if (delta_y > 0) {
                return SWIPE_DOWN;
            } else {
                return SWIPE_UP;
            }
        }
    }
    
    return SWIPE_NONE;
}

void handle_swipe(SwipeDirection direction) {
    if (debug) {
        Serial.print("Swipe detected: ");
        switch(direction) {
            case SWIPE_UP:
                Serial.println("UP");
                break;
            case SWIPE_DOWN:
                Serial.println("DOWN");
                break;
            case SWIPE_LEFT:
                Serial.println("LEFT");
                break;
            case SWIPE_RIGHT:
                Serial.println("RIGHT");
                break;
            default:
                Serial.println("NONE");
                break;
        }
    }
}

void handle_touch() {
    TouchPoint p = get_pos();
    
    if (debug) {
        Serial.print("Touch detected at: ");
        Serial.print(p.x);
        Serial.print(", ");
        Serial.println(p.y);
    }
    
    int hi_x_start = (tft.width() - home_indicator_width) / 2;
    int hi_x_end   = hi_x_start + home_indicator_width;
    int hi_y_start = home_indicator_y - home_indicator_height;
    int hi_y_end   = home_indicator_y;
    
    if (p.x >= hi_x_start && p.x <= hi_x_end &&
        p.y >= hi_y_start && p.y <= hi_y_end) {
        home_indicator_touched = true;
        if (debug) Serial.println("Home Indicator Touched!");
        if (strcmp(app_list[current_app].name, "home") != 0 && 
            strcmp(app_list[current_app].name, "lock") != 0) {
            launch_app(home);
            return;
        }
    } else {
        home_indicator_touched = false;
    }
    
    if (strcmp(app_list[current_app].name, "home") == 0) {
        home_handler();
    }
}


TouchPoint get_raw_pos() {
    if (ts.touched()) {
        TS_Point p = ts.getPoint();
        TouchPoint raw_pos;
        raw_pos.x = p.x;
        raw_pos.y = p.y;
        raw_pos.touched = true;
        return raw_pos;
    } else {
        TouchPoint raw_pos;
        raw_pos.touched = false;
        return raw_pos;
    }
}

void remap_pos() {
    TouchPoint raw = get_raw_pos();
    int rawX = raw.x;
    int rawY = raw.y;

    if (swapXY) {
        int tmp = rawX; rawX = rawY; rawY = tmp;
    }

    int x_mapped = map(rawX, X_MIN_RAW, X_MAX_RAW, 0, tft.width());
    int y_mapped = map(rawY, Y_MIN_RAW, Y_MAX_RAW, 0, tft.height());

    if (invertX) x_mapped = tft.width() - x_mapped;
    if (invertY) y_mapped = tft.height() - y_mapped;
    global_pos_x = x_mapped;
    global_pos_y = y_mapped;
    screen_touched = raw.touched;
}

TouchPoint get_pos() {
    return {global_pos_x, global_pos_y, screen_touched};
}

void handle_swipe(TouchPoint start, TouchPoint end) {
    int dx = end.x - start.x;
    int dy = end.y - start.y;

    if (debug) {
        Serial.print("Start x : ");
        Serial.print(start.x);
        Serial.print(", start y : ");
        Serial.println(start.y);
        Serial.print("");
        Serial.print("End x : ");
        Serial.print(end.x);
        Serial.print(", end y : ");
        Serial.println(end.y);
    }

    if (debug) Serial.printf("dx=%d, dy=%d, abs(dx)=%d, abs(dy)=%d\n", dx, dy, abs(dx), abs(dy));

    if (abs(dx) < 30 && abs(dy) < 30) {
        swipe = false;
        if (debug) Serial.println("No swipe");
        return;
    }

    if (abs(dx) > abs(dy)) {
        if (dx > 0) {
            if (debug) Serial.println("Swipe right");
            swipe = true;
            swipe_direction = 2;
        } else {
            if (debug) Serial.println("Swipe left");
            swipe = true;
            swipe_direction = 4;
        }
    } else {
        if (dy > 0) {
            if (debug) Serial.println("Swipe down");
            swipe = true;
            swipe_direction = 3;
        } else {
            if (debug) Serial.println("Swipe up");
            swipe = true;
            swipe_direction = 1;
        }
    }
}