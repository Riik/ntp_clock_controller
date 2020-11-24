#include <WiFi.h>
#include "time.h"
#include "config.h"

// make sure config.h contains your wifi password and SSID
const char* ssid       = WIFI_SSID;
const char* password   = WIFI_PASSWORD;
const char* ntpServer = "pool.ntp.org";
const char* TZ_INFO = "CET-1CEST,M3.5.0/2,M10.5.0/3";
struct tm timeinfo;

void connectWifi(){
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
}

void disconnectWifi(){
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void getCurrentTime(){
 configTime(0, 0, ntpServer);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  setenv("TZ", TZ_INFO, 1);
  tzset();
}
void printCurrentTime(){
  char strftime_buf[64];
  time_t now;
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  Serial.printf("Current time: %s\n", strftime_buf);
}

void setup()
{
  Serial.begin(115200);
  connectWifi();
  getCurrentTime();
  disconnectWifi();
}

void loop()
{
  delay(1000);
  printCurrentTime();
}
