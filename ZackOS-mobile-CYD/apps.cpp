#include "apps.h"
#include "config.h"
#include "ui.h"
#include "utils.h"
#include "system.h"
#include <cstring>
#include "system.h"
#include <esp_system.h>

App app_list[] = {home, settings, reboot_menu, sleep_app};
int app_list_size = sizeof(app_list) / sizeof(app_list[0]);

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

            Serial.print("App touchée : ");
            Serial.println(app_list[i].name);

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

    Serial.println("SETTINGS draw");
}

void settings_handler() {

    if (need_to_be_redrawn) {
        draw_settings();
        need_to_be_redrawn = false;
        need_to_be_refreshed = false;
    }
    Serial.println("SETTINGS handler");

    if (home_indicator_touched) {
        Serial.println("HI TOUCHED FROM SETTINGS");

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
    Serial.println("HEY from reboot_menu_handler");
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
    bool temp = true;
    while (temp) {
        for (int r = 0; r <= 250; r += 10) {
            if (!temp) {
                break;
            }
            for (int g = 0; g <= 250; g += 10) {
                if (!temp) {
                    break;
                }
                for (int b = 0; b <= 250; b += 10) {
                    if (!temp) {
                        break;
                    }
                    tft.fillScreen(color565(r, g, b));
                    delay(500);
                    TouchPoint p = get_pos();
                    if (p.touched) {
                        for (int i = 0; i < app_list_size; i++) {
                            if (strcmp(app_list[current_app].name, "home") == 0) {
                                current_app = i;
                                need_to_be_redrawn = true;
                                need_to_be_refreshed = true;
                                run_home();
                                temp = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

App settings = {
    color565(91, 91, 91), settings_handler, draw_settings, "Settings"
};

App reboot_menu = {
    color565(91, 91, 91), reboot_menu_handler, draw_reboot_menu, "Restart"
};

App sleep_app = {
    color565(100, 0, 0), draw_sleep, draw_sleep, "Sleep"
};

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

        tft.fillRect(x, y, app_icon_size, app_icon_size, app_list[i].color);
        tft.drawRect(x, y, app_icon_size, app_icon_size, color565(255, 255, 255));

        tft.setTextColor(TFT_WHITE);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(app_list[i].name, x + app_icon_size / 2, y + app_icon_size + 10, 1);

        visible_index++;
    }
}

App home = {
    background_color, run_home, draw_home, "home"
};