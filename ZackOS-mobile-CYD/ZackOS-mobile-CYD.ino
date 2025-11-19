#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include <cstdint>
#include <cstring>
#include "config.h"
#include "ui.h"
#include "apps.h"
#include "system.h"
#include "WiFi.h"
#include "utils.h"

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  init_screen();

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  tft.fillScreen(TFT_BLACK);

  Serial.println("Ready!");

  tft.drawCentreString("ZackOS Mobile", tft.width() / 2, tft.height() / 2, 2);

  startup();

  // launchApp(settings);

  // launch_app(home);

  launch_app(lock);
  if (debug) {
    Serial.println("Launching lock screen");
  }

  // launch_app(settings);

}

unsigned long touchStartTime = 0;
bool isTouched = false;

TouchPoint start_p = {-1, -1, false};
TouchPoint end_p = {-1, -1, false};

void loop() {
  touch = false;
  if (keyboard_active) {
    keyboard_handler();
  }

  if (ts.touched()) {
    remap_pos();
    if (!isTouched) {
      touchStartTime = millis();
      isTouched = true;
      start_p = get_pos();
    }
    end_p = get_pos();

  } else {
    screen_touched = false;
    if (isTouched) {
      unsigned long pressDuration = millis() - touchStartTime;

      if (debug) tft.fillCircle(get_pos().x, get_pos().y, 10, color565(255, 0, 255));
      
      if (pressDuration >= LONG_PRESS_THRESHOLD) {
        if (debug) Serial.println("Long Touch detected");
        handle_swipe(start_p, end_p);
        if (swipe && strcmp(app_list[current_app].name, "Lock") == 0) {
          lock_screen_handler();
          if (debug) Serial.println("Screen handler called from swipe");
        }
        handle_touch();
      } else {
        if (debug) tft.fillCircle(get_pos().x, get_pos().y, 10, color565(255, 0, 0));
        if (debug) Serial.println("Short Touch detected");

        touch = true;
      
        int dx = abs(end_p.x - start_p.x);
        int dy = abs(end_p.y - start_p.y);
        
        swipe = false;
        
        handle_touch();
        
        if (!home_indicator_touched && !swipe && touch) {
          app_list[current_app].run();
        }
      }

      isTouched = false;
    }
  }

  if (need_to_be_redrawn) {
    app_list[current_app].drawner();
    need_to_be_redrawn = false;
  }

  if (need_to_be_refreshed) {
    app_list[current_app].run();
    need_to_be_refreshed = false;
  }
}