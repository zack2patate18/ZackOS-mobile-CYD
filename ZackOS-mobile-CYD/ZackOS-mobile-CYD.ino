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

void setup() {
  Serial.begin(115200);
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

void loop() {
  if (ts.touched()) {
    handle_touch();
    app_list[current_app].run();
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
