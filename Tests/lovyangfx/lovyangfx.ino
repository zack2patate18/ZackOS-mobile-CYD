#include <XPT2046_Touchscreen.h>
#include <SPI.h>

#define CS_PIN 33
#define TIRQ_PIN  255 // non utilisé

XPT2046_Touchscreen ts(CS_PIN, TIRQ_PIN); 

void setup() {
  Serial.begin(115200);
  SPI.begin(13, 12, 14); // MOSI, MISO, SCLK
  ts.begin();
  ts.setRotation(2);
}

void loop() {
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    Serial.print("X: "); Serial.print(p.x);
    Serial.print(" Y: "); Serial.println(p.y);
  }
  delay(100);
}
