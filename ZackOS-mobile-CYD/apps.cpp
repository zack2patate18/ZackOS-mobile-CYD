#include "apps.h"
#include "config.h"
#include "ui.h"
#include "utils.h"
#include "system.h"
#include <cstdlib>
#include <cstring>
#include "system.h"
#include <esp_system.h>
#include <future>
#include "WiFi.h"
#include <ESP32Ping.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <stdlib.h>

int current_app = 0;

void draw_page_indicator();

void next_page() {
    if (current_page < total_pages - 1) {
        current_page++;
        draw_home();
    }
}

void previous_page() {
    if (current_page > 0) {
        current_page--;
        draw_home();
    }
}

void go_to_page(int page) {
    if (page >= 0 && page < total_pages) {
        current_page = page;
        draw_home();
    }
}

void home_handler() {
  int max_apps_per_row = max_apps_per_rows;
  int y_start = margin;
  int visible_index = 0;

  int max_apps_per_page = max_apps_per_row * max_apps_per_columns;
  
  int start_index = current_page * max_apps_per_page;
  int end_index = min((current_page + 1) * max_apps_per_page, app_list_size);

  TouchPoint p = get_pos();

  if (total_pages > 1) {
    if (swipe && swipe_direction == 2) {
        previous_page();
    }
    else if (swipe && swipe_direction == 4) {
        next_page();
    }
  }

  if (touch) {

      for (int i = start_index; i < end_index; i++) {
        if (!app_list[i].on_home) continue;
    
        int row = visible_index / max_apps_per_row;
        int col = visible_index % max_apps_per_row;
        int apps_in_row = min(max_apps_per_row, (end_index - start_index) - row * max_apps_per_row);
    
        int total_width = apps_in_row * app_icon_size + (apps_in_row - 1) * app_margin;
        int startX = (tft.width() - total_width) / 2;
    
        int x = startX + col * (app_icon_size + app_margin);
        int y = y_start + row * (app_icon_size + app_margin);
    
        if (p.x >= x && p.x <= x + app_icon_size && p.y >= y && p.y <= y + app_icon_size) {
          if (debug) {
            Serial.print("Selected App : ");
            Serial.println(app_list[i].name);
          }
    
          tft.drawRect(x, y, app_icon_size, app_icon_size, color565(255, 0, 0));
          tft.fillRect(x - 1, y + 1, app_icon_size - 1, app_icon_size - 1, color565(255, 255, 255));
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

    if(touch) {
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
    if (touch) {
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
    if (touch) {
        current_app = 0;
        need_to_be_redrawn = true;
        need_to_be_refreshed = true;
    }
    delay(125);
}


void draw_lock_screen() {
    tft.fillScreen(lock_screen_background_color);
    // draw_top_bar();
    tft.setTextColor(color565(0, 0, 0), lock_screen_background_color);
    tft.drawCentreString(current_time, tft.width() / 2, home_indicator_y - (tft.height() - 80), 6);
    draw_home_indicator();
}

void lock_screen_handler() {
    /* TouchPoint p = get_pos();
    if (touch) {
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
    } */
    if (swipe && swipe_direction == 1) {
        int i = 0;
        while (i <= tft.height()) {
            tft.fillRect(0, tft.height() - i, tft.width(), tft.height(), background_color);
            i += 5;
        }
        draw_home_indicator();
        swipe = false;
        touch = false;
        
        launch_app(home);
    }
}

bool wait_for_keyboard_to_stop = false;

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

    } else {
        tft.setTextDatum(MC_DATUM);
        tft.fillRect(tft.width() / 4, 80, tft.width() / 4 * 2, 50, color565(98, 201, 222));
        tft.drawCentreString("Wifi scan", tft.width() / 2, 105, 2);
    }
}

void connectivity_handler() {

    char password[256];

    if (!keyboard_active && wait_for_keyboard_to_stop) {
        typed_text.toCharArray(password, sizeof(password));
        if (debug) {
            Serial.print(selectedSSID.c_str());
            Serial.println("YOOOOOO wifi password entered ! now we need to test it");
        }
        WiFi.begin(selectedSSID.c_str(), password );
        bool connected = false;
        for (int i = 0; !connected; i++) {
            if (i % 2 == 0) {
                tft.fillScreen(color565(100, 0, 0));
            } else {
                tft.fillScreen(color565(0, 0, 0));
            }

            tft.setTextDatum(MC_DATUM);
            tft.drawString("Connecting...", tft.width() / 2, tft.height() / 2, 4);

            if (WiFi.status() == WL_CONNECTED) {
                top_bar_color = color565(255, 255, 0);
                connected = true;

                if (debug) Serial.println("Connected");

                tft.fillScreen(color565(255, 255, 0));
                tft.drawString("Connected!", tft.width()/2, tft.height()/2, 4);
                tft.drawString("Local IP : " + WiFi.localIP().toString(), tft.width()/2, tft.height()/2 + 20, 2);

                IPAddress remoteIP(8, 8, 8, 8);
                if (Ping.ping(remoteIP)) {
                    tft.drawString("Can access internet!", tft.width()/2, tft.height()/2 + 40, 2);
                    if (debug) Serial.println("Can acces to internet");
                } else {
                    tft.drawString("Cannot access internet :(", tft.width()/2, tft.height()/2 + 40, 2);
                    if (debug) Serial.println("Cannot acces to internet");
                }

                delay(1000);

                break;
            }

            n_wifi = 0;

            delay(1000);
        }

        if (!connected) {
            tft.fillScreen(color565(100, 0, 0));
            tft.setTextDatum(MC_DATUM);
            tft.drawCentreString("Error while connecting : Timed out", tft.width() / 2, tft.height() / 2, 2);
            delay(1000);
        }
        launch_app(home);
        return;
    }

    TouchPoint p = get_pos();
    if (!touch) return;

    if (debug) {
        Serial.println("HEY from connectivity handler");
    }

    if (typed_text == "") {

    int wifiX = tft.width() / 4;
    int wifiY = 80;
    int wifiW = tft.width() / 2;
    int wifiH = 50;

    if (p.x >= wifiX && p.x <= wifiX + wifiW &&
        p.y >= wifiY && p.y <= wifiY + wifiH && n_wifi < 0) {
            if (debug) Serial.println("Starting wifi scan");
            tft.fillRect(wifiX, wifiY, wifiW, wifiH, color565(89, 191, 212));
        tft.drawCentreString("Scanning...", tft.width() / 2, wifiY + wifiH / 2, 2);
        n_wifi = WiFi.scanNetworks();
        draw_connectivity();
        return;
    }

    int ssid_y = 90;
    int n_display = min(n_wifi, MAX_DISPLAY_WIFI);

    for (int i = 0; i < n_display; i++) {
        int ssidX_start = margin + list_margin;
        int ssidX_end   = tft.width() - margin;
        int ssidY_start = ssid_y;
        int ssidY_end   = ssid_y + 20;

        if (p.x >= ssidX_start && p.x <= ssidX_end &&
            p.y >= ssidY_start && p.y <= ssidY_end) {

            selectedSSID = WiFi.SSID(i);
            wifi_password_required = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);

            if (wifi_password_required) {
                if (debug) Serial.println("Password required for: " + selectedSSID);
                delay(100);
                launch_app(keyboard);
                wait_for_keyboard_to_stop = true;
                keyboard_active = true;
                next_app_after_keyboard = &connectivity;
            } else {
                if (debug) Serial.println("Connecting to open network: " + selectedSSID);
                WiFi.begin(selectedSSID.c_str());
                bool connected = false;
                for (int i = 0; i < 30; i++) {
                    if (i % 2 == 0) {
                        tft.fillScreen(color565(100, 0, 0));
                    } else {
                        tft.fillScreen(color565(0, 0, 0));
                    }
                    tft.setTextDatum(MC_DATUM);
                    tft.drawCentreString("Connecting...", tft.width() / 2, tft.height() / 2, 4);
                    if (WiFi.status() == WL_CONNECTED) {
                        top_bar_color = color565(255, 255, 0);
                        connected = true;
                        break;
                    }
                    delay(1000);
                }
                if (!connected) {
                    tft.fillScreen(color565(100, 0, 0));
                    tft.setTextDatum(MC_DATUM);
                    tft.drawCentreString("Error while connecting : Timed out", tft.width() / 2, tft.height() / 2, 2);
                }
                launch_app(home);
                return;
            }
            break;
        }
        ssid_y += 25;
    }
}
}

void draw_keyboard() {
    const int rows = 4;
    const int cols = 10;
    const int key_w = tft.width() / cols - 4;
    const int key_h = (tft.height() / 3) / rows - 4;
    const int corner_radius = 3;

    const uint16_t keyboard_color = color565(40, 40, 40);
    const uint16_t key_color = color565(60, 60, 60);
    const uint16_t text_color = color565(255, 255, 255);

    tft.fillRoundRect(0, tft.height() / 2, tft.width(), tft.height() / 2, 10, keyboard_color);
    tft.fillRect(0, tft.height() / 2 - 30, tft.width(), tft.height() / 2, keyboard_color);
    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(color565(255, 255, 255), keyboard_color);
    tft.drawString(typed_text, margin, (tft.height() / 2 )- 15, 2);

    const char* keyboard_keys[rows][cols] = {
        {"a","z","e","r","t","y","u","i","o","p"},
        {"q","s","d","f","g","h","j","k","l","m"},
        {"w","x","c","v","b","n","-","_","SC","BK"},
        {"RRN","","","","","","","","",""}
    };

    int start_y = tft.height() / 2 + 10;
    int font_h = tft.fontHeight();

    for(int row=0; row<rows; row++){
        int start_x = 2;
        for(int col=0; col<cols; col++){
            int x = start_x + col * (key_w + 4);
            int y = start_y + row * (key_h + 4);

            tft.fillRoundRect(x, y, key_w, key_h, corner_radius, key_color);

            tft.setTextDatum(MC_DATUM);
            tft.setTextColor(text_color, key_color);
            tft.drawString(keyboard_keys[row][col], x + key_w/2, y + key_h/2 - font_h/2, 2);
        }
    }
}


void keyboard_handler() {
    const int rows = 4;
    const int cols = 10;
    const int key_w = tft.width() / cols - 4;
    const int key_h = (tft.height() / 3) / rows - 4;

    const char* keyboard_keys[rows][cols] = {
        {"a","z","e","r","t","y","u","i","o","p"},
        {"q","s","d","f","g","h","j","k","l","m"},
        {"w","x","c","v","b","n","-","_","SPACE","BACK"},
        {"RETURN","","","","","","","","",""}
    };

    int start_y = tft.height() / 2 + 10;
    TouchPoint p = get_pos();

    if(!touch) return;

    for(int row=0; row<rows; row++){
        int start_x = 2;
        for(int col=0; col<cols; col++){
            int x = start_x + col * (key_w + 4);
            int y = start_y + row * (key_h + 4);

            if(p.x >= x && p.x <= x+key_w && p.y >= y && p.y <= y+key_h){
                const char* key = keyboard_keys[row][col];
                if(strcmp(key,"SPACE")==0){
                    typed_text += " ";
                    if (debug) Serial.println("ADDED SPACE TO TYPED TEXT");
                } else if(strcmp(key,"BACK")==0){
                    if(typed_text.length() > 0) typed_text.remove(typed_text.length()-1);
                    if (debug) Serial.println("REMOVED THE LAST CHARACTER FROM TYPED TEXT");
                } else if(strcmp(key,"RETURN")==0){
                    keyboard_active = false;
                    if (debug) Serial.println("KEYBOARD STOPPED LAUNCHING NEXT APP");
                    launch_app(*next_app_after_keyboard);
                    return;
                    if(next_app_after_keyboard != nullptr) launch_app(*next_app_after_keyboard);
                } else if(strlen(key) > 0){
                    typed_text += key;
                    if (debug) {
                        Serial.print("ADDED : ");
                        Serial.print(key);
                        Serial.println("TO TYPED TEXT");
                    }
                }
                draw_keyboard();
                delay(100);
                return;
            }
        }
    }
}


void draw_themes() {
    tft.fillScreen(color565(91, 91, 91));

    tft.setTextDatum(MC_DATUM);
    int margin = 20;
    tft.drawCentreString("Change wallpaper", tft.width() / 2, margin, 4);
    tft.drawCentreString("Color", tft.width() / 2, margin + 25, 4);

    uint16_t colors[8] = {
        color565(255, 0, 0),
        color565(0, 255, 0),
        color565(0, 0, 255),
        color565(255, 255, 0),
        color565(255, 0, 255),
        color565(0, 255, 255),
        color565(255, 128, 0),
        color565(128, 0, 255)
    };

    int squareSize = 40;
    int padding = 10;
    int startX = (tft.width() - (2 * squareSize + 3 * padding)) / 2;
    int startY = margin + 75;

    for (int i = 0; i < 8; i++) {
        int row = i % 4;
        int col = i / 4;
        int x = startX + col * (squareSize + padding);
        int y = startY + row * (squareSize + padding);

        tft.fillRoundRect(x, y, squareSize, squareSize, 5, colors[i]);
    }
}

void themes_handler() {
    uint16_t colors[8] = {
        color565(255, 0, 0),
        color565(0, 255, 0),
        color565(0, 0, 255),
        color565(255, 255, 0),
        color565(255, 0, 255),
        color565(0, 255, 255),
        color565(255, 128, 0),
        color565(128, 0, 255)
    };
    
    int squareSize = 40;
    int padding = 10;
    int margin = 20;
    int startX = (tft.width() - (2 * squareSize + 3 * padding)) / 2;
    int startY = margin + 75;
    if (true) {
        TouchPoint p = get_pos();

        if (!touch) return;

        for (int i = 0; i < 8; i++) {
            int row = i % 4;
            int col = i / 4;
            int x = startX + col * (squareSize + padding);
            int y = startY + row * (squareSize + padding);

            if (p.x >= x && p.x <= x + squareSize && p.y >= y && p.y <= y + squareSize) {
                
                uint16_t selected_color = colors[i];

                background_color = selected_color;

                if (debug) {
                    Serial.print("Selected wallpaper : ");
                    Serial.println(i);
                }

                break;
            }
        }
    }
}

void parse_objects(String playload);

void draw_z_browser() {
    tft.fillScreen(color565(239, 23, 172));
    draw_top_bar();
    draw_home_indicator();
    tft.fillRect(0, top_bar_height, tft.width(), 30, color565(80, 80, 80));
    tft.drawCentreString("Enter your url here", tft.width() / 2, top_bar_height + 15, 2);
}

void z_browser_handler() {
TouchPoint p = get_pos();
  if (typed_text != "" && !keyboard_active && wait_for_keyboard_to_stop) {
    if (debug) {
        Serial.println("Sending request");
    }
    String resp = request_website(typed_text);
    if (resp != "err") {
      parse_objects(resp);
    }
    typed_text = "";
  } else if (touch) {
    if (p.y > top_bar_height && p.y > (top_bar_height + 15)) {
        
        if (debug) Serial.println("Url touched");
        
        if (!keyboard_active) {
            launch_app(keyboard);
            wait_for_keyboard_to_stop = true;
            keyboard_active = true;
            next_app_after_keyboard = &z_browser;
        }
    }
  }
}

void parse_objects(String payload) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.println("Erreur parsing JSON !");
    return;
  }

  String status = doc["status"];
  if (debug) Serial.println("Status: " + status);

  JsonObject objects = doc["objects"];
  for (JsonPair obj : objects) {
    const char* id = obj.key().c_str();
    const char* type = obj.value()["type"];
    JsonArray attrs = obj.value()["attr"];

    if (debug) {
        Serial.print("Object "); Serial.println(id);
        Serial.print(" Type: "); Serial.println(type);
        Serial.print(" Attrs: ");
        
        for (const char* attr : attrs) {
            Serial.print(attr);
            Serial.print(" ");
        }
        Serial.println();
    }

  }
}

void draw_zackpay() {
    tft.fillScreen(color565(125, 129, 203));
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(color565(0, 0, 0));
    draw_top_bar();
    draw_home_indicator();
    tft.drawCentreString("ZackPay", tft.width() / 2, 20, 4);
    char balance_text[50];
    strcpy(balance_text, "Balance : ");
    char balance_str[20];
    sprintf(balance_str, "%ld", balance);
    strcat(balance_text, balance_str);
    strcat(balance_text, "$");
    tft.drawCentreString(balance_text, tft.width() / 2, 50, 2);
    tft.setTextColor(color565(0, 0, 0));
    tft.fillRect(tft.width() / 4, tft.height() / 2 - 30, tft.width() / 2, 35, color565(255, 30, 30));
    tft.drawCentreString("pay", tft.width() / 2, tft.height() / 2 - 15, 2);
}

void zackpay_handler() {
    TouchPoint p = get_pos();
    if (touch) {
        tft.fillRect(tft.width() / 4, tft.height() / 2 - 30, tft.width() / 2, 35, color565(255, 30, 30));
        if (p.x > tft.width() / 2 && p.x < tft.width() / 4 * 3 && p.y > tft.height() / 2 - 35 && p.y < tft.height() / 2) {
            if (debug) Serial.println("Pay button pressed on zackpay");
            balance -= random(10, 123);
            draw_zackpay();
            delay(100);
        }
    }
}

App settings = {
    color565(91, 91, 91), settings_handler, draw_settings, "Settings", true
};

App reboot_menu = {
    color565(203, 249, 94), reboot_menu_handler, draw_reboot_menu, "Restart", true
};

App sleep_app = {
    color565(255, 0, 0), draw_sleep, draw_sleep, "Sleep", true
};

App lock = {
    color565(0, 0, 0), lock_screen_handler, draw_lock_screen, "Lock", true
};

App connectivity = {
    color565(128, 231, 252), connectivity_handler, draw_connectivity, "Connectivity", true
};

App themes = {
    color565(126, 252, 42), themes_handler, draw_themes, "Themes", true
};

App z_browser = {
    color565(255, 255, 255), z_browser_handler, draw_z_browser, "Z-Browser", true
};

App keyboard = {
    color565(0, 0, 0), keyboard_handler, draw_keyboard, "keyboard", true
};

App zackpay = {
    color565(125, 129, 203), zackpay_handler, draw_zackpay, "ZackPay", true
};

App app_list[] = {home, settings, reboot_menu, sleep_app, lock, connectivity, themes, zackpay, z_browser};
int app_list_size = sizeof(app_list) / sizeof(app_list[0]);


void draw_home() {
  tft.fillScreen(background_color);

  int max_apps_per_row = max_apps_per_rows;
  int y_start = margin;
  int visible_index = 0;

  int max_apps_per_page = max_apps_per_row * max_apps_per_columns;

  
  int start_index = current_page * max_apps_per_page;
  int end_index = min((current_page + 1) * max_apps_per_page, app_list_size);

  for (int i = start_index; i < end_index; i++) {
    
    if (!app_list[i].on_home) continue;

    int row = visible_index / max_apps_per_row;
    int col = visible_index % max_apps_per_row;

    int apps_in_row = min(max_apps_per_row, (end_index - start_index) - row * max_apps_per_row);
    int total_width = apps_in_row * app_icon_size + (apps_in_row - 1) * app_margin;
    int startX = (tft.width() - total_width) / 2;

    int x = startX + col * (app_icon_size + app_margin);
    int y = y_start + row * (app_icon_size + app_margin);

    tft.fillRoundRect(x, y, app_icon_size, app_icon_size, 10, app_list[i].color);
    
    if (debug) {
      char text[100];
      sprintf(text, "Application: %s, index: %d, page: %d/%d", 
              app_list[i].name, i, current_page + 1, total_pages);
      Serial.println(text);
    }

    tft.setTextColor(TFT_WHITE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(app_list[i].name, x + app_icon_size / 2, y + app_icon_size + 10, 1);

    visible_index++;
  }

  draw_page_indicator();
}

void draw_page_indicator() {
  if (total_pages > 1) {
    int dot_radius = 3;
    int dot_spacing = 10;
    int total_width = total_pages * dot_spacing;
    int start_x = (tft.width() - total_width) / 2;
    int y_pos = tft.height() - 20;

    for (int i = 0; i < total_pages; i++) {
      if (i == current_page) {
        tft.fillCircle(start_x + i * dot_spacing, y_pos, dot_radius, TFT_WHITE);
      } else {
        tft.fillCircle(start_x + i * dot_spacing, y_pos, dot_radius, TFT_DARKGREY);
      }
    }
  }
}

App home = {
    background_color, home_handler, run_home, "home"
};