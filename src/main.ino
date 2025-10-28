#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include <TFT_eSPI.h>
#include <cstdint>

#define XPT2046_CS   33
#define XPT2046_IRQ  36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK  25

SPIClass mySpi = SPIClass(VSPI);
XPT2046_Touchscreen ts(XPT2046_CS, XPT2046_IRQ);
TFT_eSPI tft = TFT_eSPI();

#define X_MIN_RAW  200
#define X_MAX_RAW  3677
#define Y_MIN_RAW  250
#define Y_MAX_RAW  3800

uint16_t background_color = TFT_BLACK;

void drawHome() {
  tft.fillScreen(TFT_BLACK);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  mySpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  ts.begin(mySpi);
  ts.setRotation(2);

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  Serial.println("Ready!");

  tft.drawCentreString("ZackOS Mobile", tft.width() / 2, tft.height() / 2, 2);
}

void loop() {

}