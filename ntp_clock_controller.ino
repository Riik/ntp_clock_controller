#include <WiFi.h>
#include "time.h"
#include "config.h"

// make sure config.h contains your wifi password and SSID
const char* ssid       = WIFI_SSID;
const char* password   = WIFI_PASSWORD;
const char* ntpServer = "pool.ntp.org";
const char* TZ_INFO = "CET-1CEST,M3.5.0/2,M10.5.0/3";
struct tm timeinfo;
int PIN_A = 12;
int PIN_B = 13;
int PIN_EN = 14;
int lastMinute = 0; 
int lastHour = 0;
int clockState = PIN_A;

void connectWifi(){
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  for( int timeOut = 0; timeOut < 100; timeOut++){
     if(WiFi.status() != WL_CONNECTED) {
      Serial.println(" CONNECTED");
      break;
     }
     delay(500);
     Serial.print(".");
  }
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

bool checkMinuteHasPassed(){
  int currentMinute = timeinfo.tm_min;
  if(currentMinute != lastMinute){
    lastMinute = currentMinute;
    return true;
  }
  return false; 
}

bool checkHourHasPassed(){
  int currentHour = timeinfo.tm_hour;
  if(currentHour != lastHour){
    lastHour = currentHour;
    return true;
  }
  return false; 
}


bool checkTimeIsSet(){
  int currentYear = timeinfo.tm_year;
  if(currentYear == 70) {
    return false;
  }
  return true;
}

void advanceClock(){
  digitalWrite(PIN_EN, HIGH);
  if(clockState == PIN_A){
    clockState = PIN_B;
  } else {
    clockState = PIN_A;
  }
  digitalWrite(clockState, HIGH);
  delay(1500);
}

void disableOutput(){
  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_EN, LOW);
}

void refreshTime() {
  connectWifi();
  getCurrentTime();
  disconnectWifi();
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  refreshTime();
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_EN, OUTPUT);
  advanceClock();
}

void loop()
{
  if(checkMinuteHasPassed()) {
    Serial.println("Advancing clock");
    advanceClock();    
  }
  if(checkHourHasPassed()) {
    Serial.println("Connecting to WiFi and refreshing time");
    refreshTime();
  }
  if( !checkTimeIsSet() ) {
    Serial.println("Year is 1970, time not set");
    Serial.println("Reconnecting to time server");
    refreshTime();
  }
  disableOutput();
  printCurrentTime();
  delay(1000);
}
