#include "ui.h"
#include "apps.h"
#include "config.h"
#include "system.h"
#include "utils.h"
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <cstring>

#define X_MIN_RAW  200
#define X_MAX_RAW  3677
#define Y_MIN_RAW  250
#define Y_MAX_RAW  3800

#define XPT2046_CS   33
#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25

SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
TFT_eSPI tft = TFT_eSPI();

void launch_app(App& app) {
    app.drawner();
    app.run();
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
}

void run_home() {
    draw_home();
    home_handler();
    draw_home_indicator();
    draw_top_bar();
}

void handle_touch() {
    if (ts.touched()) {
        TouchPoint p = get_pos();
        if (debug) {
            tft.fillCircle(p.x, p.y, 1, color565(26, 47, 155));
            Serial.println("Remapped : ");
            Serial.println(p.x);
            Serial.println(p.y);
            TouchPoint raw_p = get_raw_pos();
            Serial.println("Raw : ");
            Serial.println(raw_p.x);
            Serial.println(raw_p.y);
        }

        int hi_x_start = (tft.width() - home_indicator_width) / 2;
        int hi_x_end   = hi_x_start + home_indicator_width;
        int hi_y_start = home_indicator_y;
        int hi_y_end   = home_indicator_y + home_indicator_height;

        if (p.x >= hi_x_start && p.x <= hi_x_end &&
            p.y >= hi_y_start && p.y <= hi_y_end) {
            home_indicator_touched = true;
            Serial.println("Home Indicator Touched!");
            if (strcmp(app_list[current_app].name, "home") != 0) {
                for (int i = 0; i < app_list_size; i++) {
                    if (strcmp(app_list[i].name, "home") == 0) {
                        current_app = i;
                        need_to_be_redrawn = true;
                        need_to_be_refreshed = true;
                        break;
                    }
                }
            }
        } else {
            home_indicator_touched = false;
        }
        if (strcmp(app_list[current_app].name, "home") == 0) {
            home_handler();
        }
    } else {
        home_indicator_touched = false;
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

TouchPoint get_pos() {
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

    return { x_mapped, y_mapped, raw.touched};
}