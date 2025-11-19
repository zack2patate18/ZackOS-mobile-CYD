#include "system.h"
#include <HTTPClient.h>
#include "WiFi.h"
#include "config.h"

char current_time[6] = "15:11";

bool need_to_be_refreshed = false;
bool need_to_be_redrawn = false;

String request_website(String url) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(get_website_url);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"url\":\"" + url + "\"}"; 
    int http_code = http.POST(jsonData);

    if (http_code > 0) {
      String resp = http.getString();
      if (debug) {
        Serial.print("RESP : ");
        Serial.println(resp);
      }
      http.end();
      return resp;
    } else {
      http.end();
      return "err";
    }
  }
  return "err";
}