#include "config.h"

using namespace fs;

// SD card CYD (ESP32-2432S028) : bus VSPI dédié, SÉPARÉ de l'écran
// Ecran  → HSPI : CLK=14, MOSI=13, MISO=12, CS=15
// SD     → VSPI : CLK=18, MOSI=23, MISO=19, CS=5
#define SD_CS   5
#define SD_CLK  18
#define SD_MISO 19
#define SD_MOSI 23

SPIClass spi_sd(VSPI);

void setup() {
    Serial.begin(115200);
    randomSeed(analogRead(0));

    // ── Ecran d'abord (HSPI) ───────────────────────────────────────────────
    init_screen();

    // ── SD sur VSPI dédié — aucun conflit avec l'écran ────────────────────
    spi_sd.begin(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, spi_sd, 4000000)) {
        Serial.println("SD ERR");
    } else {
        Serial.println("SD OK");
    }

    tft.fillScreen(TFT_BLACK);
    Serial.println("Ready!");

    tft.drawCentreString("ZackOS Mobile", tft.width() / 2, tft.height() / 2, 2);

    startup();
    setup_torch();
    File root = SD.open("/");
    std::vector<String> r_paths;
    list_files(root, "", r_paths);
    root.close();
    for (String path : r_paths) {
        Serial.println(path);
    }
    launch_app(lock);
    if (debug) Serial.println("Launching lock screen");
}

// ── Etat du touch ──────────────────────────────────────────────────────────
unsigned long touchStartTime = 0;
bool          isTouched      = false;

TouchPoint start_p = { -1, -1, false };
TouchPoint end_p   = { -1, -1, false };

void loop() {
    touch = false;

    if (keyboard_active) {
        draw_keyboard();
        keyboard_handler();
    }

    // ── LGFX : getTouch() remplace ts.touched() ───────────────────────────
    uint16_t tx = 0, ty = 0;
    bool currently_touched = tft.getTouch(&tx, &ty);

    if (currently_touched) {

        // remap_pos() applique invertX / invertY / swapXY depuis config.cpp
        remap_pos(tx, ty);

        if (!isTouched) {
            touchStartTime = millis();
            isTouched      = true;
            start_p        = get_pos();
        }
        end_p = get_pos();

    } else {
        screen_touched = false;

        if (isTouched) {
            unsigned long pressDuration = millis() - touchStartTime;

            if (debug)
                tft.fillCircle(get_pos().x, get_pos().y, 10, color565(255, 0, 255));

            if (pressDuration >= LONG_PRESS_THRESHOLD) {
                if (debug) Serial.println("Long Touch detected");
                handle_swipe(start_p, end_p);
                if (swipe && strcmp(app_list[current_app].name, "Lock") == 0) {
                    lock_screen_handler();
                    if (debug) Serial.println("Screen handler called from swipe");
                } else {
                    if (start_p.y > tft.height() - 40) {
                        launch_app(home);
                    }
                }
                handle_touch();
            } else {
                if (debug)
                    tft.fillCircle(get_pos().x, get_pos().y, 10, color565(255, 0, 0));
                if (debug) Serial.println("Short Touch detected");

                touch = true;
                swipe = false;
                handle_touch();
                app_list[current_app].run();
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