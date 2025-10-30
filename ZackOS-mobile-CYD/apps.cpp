#include "apps.h"
#include "config.h"
#include "ui.h"
#include "utils.h"
#include "system.h"
#include <cstring>
#include "system.h"
#include <esp_system.h>
#include <future>
#include "WiFi.h"

int current_app = 0;

void home_handler() {

    int max_apps_per_row = max_apps_per_rows;
    int y_start = margin;

    int visible_index = 0;


    TouchPoint p = get_pos();
    if (p.x == -1 || p.y == -1) return;

    for (int i = 0; i < app_list_size; i++) {
        if (strcmp(app_list[i].name, "home") == 0) continue;

        int row = visible_index / max_apps_per_row;
        int col = visible_index % max_apps_per_row;
        int apps_in_row = min(max_apps_per_row, app_list_size - row * max_apps_per_row);

        int total_width = apps_in_row * app_icon_size + (apps_in_row - 1) * app_margin;
        int startX = (tft.width() - total_width) / 2;

        int x = startX + col * (app_icon_size + app_margin);
        int y = y_start + row * (app_icon_size + app_margin);

        if (p.x >= x && p.x <= x + app_icon_size &&
            p.y >= y && p.y <= y + app_icon_size) {

            if (debug) {
                Serial.print("Selected App : ");
                Serial.println(app_list[i].name);
            }

            tft.drawRect(x, y, app_icon_size, app_icon_size, color565(255, 0, 0));
            tft.fillRect(x-1, y+1, app_icon_size - 1, app_icon_size - 1, color565(255, 255, 255));
            delay(150);

            need_to_be_refreshed = true;
            need_to_be_redrawn = true;

            app_list[i].run();
            current_app = i;
            return;
        }

        visible_index++;
    }
}

void draw_settings() {


    char time_text[14];
    strcpy(time_text, "Time : ");
    strcat(time_text, current_time);

    char chip_text[20];
    strcpy(chip_text, "Chip : ");
    strcat(chip_text, cpu_name);

    char chip_speed_text[20];
    strcpy(chip_speed_text, "Chip speed : ");
    strcat(chip_speed_text, cpu_speed);

    char dev_mode_text[15];
    strcpy(dev_mode_text, "DEV MODE : ");
    if (debug) {
        strcat(dev_mode_text, "on");
    } else {
        strcat(dev_mode_text, "off");
    }

    char os_name[30] = "OS name : ZackOS mobile (CYD)";
    
    char os_version_text[17];

    strcpy(os_version_text, "OS version : ");
    strcat(os_version_text, os_version);

    tft.fillScreen(color565(91, 91, 91));
    tft.setTextColor(color565(0, 0, 0), color565(91, 91, 91));
    tft.setTextDatum(TL_DATUM);
    tft.drawCentreString("Settings", tft.width() / 2, 30, 2);
    tft.drawString(dev_mode_text, margin, 90, 2);
    tft.drawString("Infos :", margin, 115, 2);
    tft.drawString(time_text, margin + list_margin, 140, 2);
    tft.drawString(chip_text, margin + list_margin, 165, 2);
    tft.drawString(chip_speed_text, margin + list_margin, 190, 2);
    tft.drawString(os_name, margin + list_margin, 215, 2);
    tft.drawString(os_version_text, margin + list_margin, 235, 2);

    draw_home_indicator();
    draw_top_bar();

    if (debug) Serial.println("SETTINGS draw");
}

void settings_handler() {

    if (need_to_be_redrawn) {
        draw_settings();
        need_to_be_redrawn = false;
        need_to_be_refreshed = false;
    }

    if (debug) Serial.println("SETTINGS handler");
    
    TouchPoint p = get_pos();

    if(p.touched) {
        if (p.y < (tft.fontHeight(2) + 90) && p.y > 90) {
            if (debug) Serial.println("DEV MODE CLICKED FROM SETTINGS");
            debug = !debug;
            draw_settings();
            delay(250);
        }
    }

    if (home_indicator_touched) {
        if (debug) Serial.println("HI TOUCHED FROM SETTINGS");

        for (int i = 0; i < app_list_size; i++) {
            if (strcmp(app_list[i].name, "home") == 0) {
                current_app = i;
            }
        }

    }

}

void draw_reboot_menu() {
    int boxX = tft.width() / 4;
    int boxY = tft.height() / 2;
    int boxW = tft.width() / 2;
    int boxH = 100;

    tft.fillRect(boxX, boxY, boxW, boxH, color565(91, 91, 91));

    tft.setTextColor(color565(0, 0, 0));
    tft.drawCentreString("Restart", tft.width() / 2, boxY + (boxH / 2) - 8, 2);
}

void reboot_menu_handler() {
    TouchPoint p = get_pos();
    if (p.touched) {
        int boxX = tft.width() / 4;
        int boxY = tft.height() / 2;
        int boxW = tft.width() / 2;
        int boxH = 100;

        if (debug) {
            Serial.print("boxX ");
            Serial.println(boxX);
            Serial.print("boxY ");
            Serial.println(boxY);
            Serial.print("boxW ");
            Serial.println(boxW);
            Serial.print("boxH ");
            Serial.println(boxH);
            
            Serial.println("==========================");
            Serial.println(p.x);
            Serial.println(p.y);

        }

        if (p.x >= boxX && p.x <= boxX + boxW &&
            p.y >= boxY && p.y <= boxY + boxH) {
            
            tft.fillRect(boxX, boxY, boxW, boxH, color565(50, 200, 50));
            tft.setTextColor(TFT_BLACK);
            tft.drawCentreString("Restarting...", tft.width() / 2, boxY + (boxH / 2) - 8, 2);
            need_to_be_redrawn = true;

            for (int i = 0; i < 255; i += 15) {
                tft.fillScreen(color565(i, i, i));
                delay(20);
            }
            ESP.restart();
        }
    }
    if (debug) Serial.println("HEY from reboot_menu_handler");
    need_to_be_refreshed = true;
}

void handle_apps() {
    app_list[current_app].run();
    need_to_be_refreshed = true;
    need_to_be_redrawn = true;
}

void startup() {
    tft.fillScreen(color565(0, 0, 0));
    delay(500);
    for (int i = 1; i <= 10; i++) {
        tft.fillScreen(color565(i * 10, i * 10, i * 10));
        tft.setTextColor(color565(250 / i, 250 / i, 250 / i));
        tft.drawCentreString("ZackOS", tft.width() / 2, tft.height() / 2, 2);
        tft.drawCentreString("By Zack", tft.width() / 2, tft.height() / 2 + 30, 2);
        delay(250 + ( i * 3));
    }
    delay(2000);
}

void draw_sleep() {
    static int r=0, g=0, b=0;

    r += 10; if (r > 250) { r = 0; g += 10; }
    if (g > 250) { g = 0; b += 10; }
    if (b > 250) { b = 0; }

    tft.fillScreen(color565(r, g, b));

    TouchPoint p = get_pos();
    if (p.touched) {
        current_app = 0;
        need_to_be_redrawn = true;
        need_to_be_refreshed = true;
    }
    delay(125);
}


void draw_lock_screen() {
    tft.fillScreen(background_color);
    // draw_top_bar();
    tft.setTextColor(color565(0, 0, 0), background_color);
    tft.drawCentreString(current_time, tft.width() / 2, home_indicator_y - (tft.height() - 80), 6);
    draw_home_indicator();
}

void lock_screen_handler() {
    TouchPoint p = get_pos();
    if (p.touched) {
        if (p.y > (tft.height() - 30) && !lock_screen_swipe) {
            lock_screen_swipe = true;
        }
        if (lock_screen_swipe) {
            // int x = (tft.width() - home_indicator_width) / 2;
            // tft.fillRect(x, home_indicator_y - home_indicator_height, home_indicator_width, home_indicator_height, background_color);
            // tft.fillCircle(x, home_indicator_y - home_indicator_height + (home_indicator_height / 2), home_indicator_height / 2, background_color);
            // tft.fillCircle(x + home_indicator_width, home_indicator_y + (home_indicator_height / 2) - home_indicator_height, home_indicator_height / 2, background_color);
            // tft.drawCentreString(current_time, tft.width() / 2, (tft.height() - 80) + home_indicator_y, 6);
            home_indicator_y = p.y;
            draw_lock_screen();
        }
        if (lock_screen_swipe && p.y < (tft.height() / 2)) {
            lock_screen_swipe = false;
            if (debug) Serial.println("Lockscreen swipe ended. Launching home");
            delay(500);
            home_indicator_y = home_indicator_default_y;
            draw_home_indicator();
            launch_app(home);
        }
    }
}

void draw_connectivity() {
    tft.fillScreen(color565(128, 231, 252));
    draw_top_bar();
    draw_home_indicator();
    tft.setTextColor(color565(255, 255, 255), color565(128, 231, 252));
    tft.drawCentreString("Connectivity", tft.width() / 2, 50, 4);
    tft.setTextColor(color565(255, 255, 255), color565(98, 201, 222));
    if (n_wifi >= 0) {
        tft.fillScreen(color565(128, 231, 252));
        tft.setTextColor(color565(255, 255, 255), color565(128, 231, 252));
        tft.drawCentreString("Connectivity", tft.width() / 2, 50, 4);
        tft.setTextDatum(TL_DATUM);
        tft.drawString("Wifi :", margin, 70, 2);

        int ssid_y = 90;

        for (int i = 0; i < n_wifi; i++) {
            tft.drawString(WiFi.SSID(i).c_str(), margin + list_margin, ssid_y, 2);
            if (debug) Serial.println(WiFi.SSID(i).c_str());
            ssid_y += 25;
        }

        delay(4000);

        n_wifi = -1;
        draw_connectivity();
    } else {
        tft.setTextDatum(MC_DATUM);
        tft.fillRect(tft.width() / 4, 80, tft.width() / 4 * 2, 50, color565(98, 201, 222));
        tft.drawCentreString("Wifi scan", tft.width() / 2, 105, 2);
    }
}

void connectivity_handler() {
    TouchPoint p = get_pos();

    if (!p.touched) return;

    int wifiX = tft.width() / 4;
    int wifiY = 80;
    int wifiW = tft.width() / 2;
    int wifiH = 50;

    int bleX = tft.width() / 4;
    int bleY = 150;
    int bleW = tft.width() / 2;
    int bleH = 50;

    if (p.x >= wifiX && p.x <= wifiX + wifiW &&
        p.y >= wifiY && p.y <= wifiY + wifiH) {
        if (debug) Serial.println("Starting wifi scan");
        tft.fillRect(wifiX, wifiY, wifiW, wifiH, color565(89, 191, 212));
        tft.drawCentreString("Scanning...", tft.width() / 2, wifiY + wifiH/2, 2);
        n_wifi = WiFi.scanNetworks();
        draw_connectivity();
    }
}


App settings = {
    color565(91, 91, 91), settings_handler, draw_settings, "Settings"
};

App reboot_menu = {
    color565(203, 249, 94), reboot_menu_handler, draw_reboot_menu, "Restart"
};

App sleep_app = {
    color565(255, 0, 0), draw_sleep, draw_sleep, "Sleep"
};

App lock = {
    color565(0, 0, 0), lock_screen_handler, draw_lock_screen, "Lock"
};

App connectivity = {
    color565(128, 231, 252), connectivity_handler, draw_connectivity, "Connectivity"
};

App app_list[] = {home, settings, reboot_menu, sleep_app, lock, connectivity};
int app_list_size = sizeof(app_list) / sizeof(app_list[0]);

void draw_home() {

    tft.fillScreen(background_color);

    int max_apps_per_row = max_apps_per_rows;
    int y_start = margin;

    int visible_index = 0;

    for (int i = 0; i < app_list_size; i++) {
        if (strcmp(app_list[i].name, "home") == 0) continue;

        int row = visible_index / max_apps_per_row;
        int col = visible_index % max_apps_per_row;

        int apps_in_row = min(max_apps_per_row, app_list_size - row * max_apps_per_row);
        int total_width = apps_in_row * app_icon_size + (apps_in_row - 1) * app_margin;
        int startX = (tft.width() - total_width) / 2;

        int x = startX + col * (app_icon_size + app_margin);
        int y = y_start + row * (app_icon_size + app_margin);

        tft.fillRoundRect(x, y, app_icon_size, app_icon_size, 10, app_list[i].color);
        if (debug) {
            char text[100]; // assez grand pour contenir tout
            sprintf(text, "Application : %s, index : %d, color : %d", 
            app_list[i].name, i, app_list[i].color);
            Serial.println(text);

        }

        tft.setTextColor(TFT_WHITE);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(app_list[i].name, x + app_icon_size / 2, y + app_icon_size + 10, 1);

        visible_index++;
    }
}

App home = {
    background_color, home_handler, run_home, "home"
};