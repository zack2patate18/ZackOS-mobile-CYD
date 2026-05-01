#include "ui.h"

LGFX tft;

void init_screen() {
    tft.init();
    tft.setRotation(2);
    tft.setBrightness(255);
}

void launch_app(App& app) {
    if (strcmp(app.name, "L_screen") != 0) {
        if (debug) Serial.println("Not lockscreen");
        for (int i = 0; i < app_list_size; i++) {
            if (strcmp(app_list[i].name, app.name) == 0) {
                current_app = i;
                if (debug) Serial.println("App found");
                break;
            }
        }
    }

    if (debug) {
        Serial.print("New app : ");
        Serial.println(app_list[current_app].name);
    }

    need_to_be_redrawn    = true;
    need_to_be_refreshed  = true;
    touch = false;
}

void draw_home_indicator() {
    int x = (tft.width() - home_indicator_width) / 2;
    tft.fillRect(x, home_indicator_y - home_indicator_height,
                 home_indicator_width, home_indicator_height,
                 color565(255, 255, 255));
    tft.fillCircle(x,
                   home_indicator_y - home_indicator_height + (home_indicator_height / 2),
                   home_indicator_height / 2,
                   color565(255, 255, 255));
    tft.fillCircle(x + home_indicator_width,
                   home_indicator_y + (home_indicator_height / 2) - home_indicator_height,
                   home_indicator_height / 2,
                   color565(255, 255, 255));
}

void draw_top_bar() {
    tft.fillRect(0, 0, tft.width(), top_bar_height, top_bar_color);
    tft.setTextColor(color565(0, 0, 0), top_bar_color);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(current_time, tft.width() / 2, top_bar_height / 2, 1);
    if (torch_enabled)
        tft.drawString("Torch", tft.width() / 6, top_bar_height / 2, 1);
}

void run_home() {
    draw_home();
    draw_home_indicator();
    draw_top_bar();
}

TouchPoint get_raw_pos() {
    uint16_t tx = 0, ty = 0;
    
    if (tft.getTouch(&tx, &ty)) {
        if (tx >= tft.width() || ty >= tft.height()) {
            return { 0, 0, false };
        }
        if (tx == 0 || ty == 0 || tx == tft.width()-1 || ty == tft.height()-1) {
            return { 0, 0, false };
        }
        return { (int)tx, (int)ty, true };
    }
    return { 0, 0, false };
}
static unsigned long last_touch = 0;


void remap_pos(uint16_t tx, uint16_t ty) {
    int x = (int)tx;
    int y = (int)ty;
    if (swapXY) { int tmp = x; x = y; y = tmp; }
    if (invertX) x = tft.width()  - 1 - x;
    if (invertY) y = tft.height() - 1 - y;
    global_pos_x   = x;
    global_pos_y   = y;
    screen_touched = true;
}

TouchPoint get_pos() {
    return { global_pos_x, global_pos_y, screen_touched };
}

void handle_touch() {
    TouchPoint p = get_pos();

    if (debug) {
        Serial.print("Touch at: ");
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

void handle_swipe(TouchPoint start, TouchPoint end) {
    int dx = end.x - start.x;
    int dy = end.y - start.y;

    if (debug)
        Serial.printf("dx=%d, dy=%d, abs(dx)=%d, abs(dy)=%d\n",
                      dx, dy, abs(dx), abs(dy));

    if (abs(dx) < 30 && abs(dy) < 30) {
        swipe = false;
        if (debug) Serial.println("No swipe");
        return;
    }

    if (abs(dx) > abs(dy)) {
        swipe           = true;
        swipe_direction = (dx > 0) ? 2 : 4;
        if (debug) Serial.println(dx > 0 ? "Swipe right" : "Swipe left");
    } else {
        swipe           = true;
        swipe_direction = (dy > 0) ? 3 : 1;
        if (debug) Serial.println(dy > 0 ? "Swipe down" : "Swipe up");
    }
}