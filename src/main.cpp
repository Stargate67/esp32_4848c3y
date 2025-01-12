#include <Arduino.h>
#include "OTA.h"
#include "time.h"
#include "My_Modbus.h"
#include "Globals.h"
#include "MainScreen.h"

//#include <ModbusIP_ESP8266.h>

void UpdateTickers(void);

int TmpDay;
struct tm timeinfo;

WiFiClient client;

String sClockHHMMSS;
String sClockHHMM;
String sPrintdate;
String sPrintShortdate;

lv_obj_t *IPLabel;
lv_obj_t *AlarmLabel;
lv_obj_t *ClockLabel;

/////////////////////////////////////////////////////////////////////////////////////////////////
//                         FIN DES DECLARATIONS 
/////////////////////////////////////////////////////////////////////////////////////////////////

/*
static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}
*/

void timeloop (int interval){ // the delay function
  int timer;

  unsigned long prevmillis = millis();
  do {
     timer = (millis() - prevmillis); 
  } while(timer < interval); 
}

void setTimezone(String timezone){
  if (SERDEBUG) Serial.printf(" Setting Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone){
  struct tm timeinfo;

  if (SERDEBUG) Serial.println("Setting up time");
  configTime(0, 0, "pool.ntp.org");    // First connect to NTP server, with 0 TZ offset
  if (!getLocalTime(&timeinfo)) {
    if (SERDEBUG) Serial.println(" Failed to obtain time");
    return;
  }
  if (SERDEBUG) Serial.println(" Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

void printLocalTime(){
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return;
  }
  if (SERDEBUG) Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
}

void startWifi(){
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Recherche WIFI......");
    lv_label_set_text(IPLabel, "Recherche WIFI......");
    delay(100);
    UpdateTickers();
  }

  Serial.print("Wifi RSSI=");
  Serial.println(WiFi.RSSI());
  //long rssi = WiFi.RSSI();
  Serial.println("");
  Serial.println(WiFi.localIP());
  String sLocalIP = WiFi.localIP().toString();
  lv_label_set_text(IPLabel, sLocalIP.c_str());
}

void setTime(int yr, int month, int mday, int hr, int minute, int sec, int isDst){
  struct tm tm;
  tm.tm_year = yr - 1900;   // Set date
  tm.tm_mon = month-1;
  tm.tm_mday = mday;
  tm.tm_hour = hr;      // Set time
  tm.tm_min = minute;
  tm.tm_sec = sec;
  tm.tm_isdst = isDst;  // 1 or 0
  time_t t = mktime(&tm);
  if (SERDEBUG) Serial.printf("Setting time: %s", asctime(&tm));
  struct timeval now = { .tv_sec = t };
  settimeofday(&now, NULL);
}

void UpdateTickers(){
// Update the ticker
  static auto lv_last_tick = millis();
  auto const now = millis();
  lv_tick_inc(now - lv_last_tick);
  lv_last_tick = now;
  // Update the UI
  lv_timer_handler();
}

//************************************************************************************************************/
//============================================================================================================/
void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);

  mb.client();
  
  InitUI();

  startWifi();
  
  setupOTA("ESP32_4848SD_FY", ssid, password);

  initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Paris/FR
}

void loop() {
  ArduinoOTA.handle();
  UpdateTickers();
  ReadModbus();

  if (!getLocalTime(&timeinfo)) {
    lv_label_set_text(AlarmLabel, "Pas de synchro Horloge!");
    return;
  }

  // Display clock
  sClockHHMMSS = 
    (String(timeinfo.tm_hour).length() > 1 ? String(timeinfo.tm_hour) : "0" + String(timeinfo.tm_hour))
    + ":" + 
    (String(timeinfo.tm_min).length() > 1 ? String(timeinfo.tm_min) : "0" + String(timeinfo.tm_min))
    + ":" + 
    (String(timeinfo.tm_sec).length() > 1 ? String(timeinfo.tm_sec) : "0" + String(timeinfo.tm_sec))
    ;
  
  sClockHHMM = 
    (String(timeinfo.tm_hour).length() > 1 ? String(timeinfo.tm_hour) : "0" + String(timeinfo.tm_hour))
    + ":" + 
    (String(timeinfo.tm_min).length() > 1 ? String(timeinfo.tm_min) : "0" + String(timeinfo.tm_min))
    ;
  lv_label_set_text(ClockLabel, sClockHHMMSS.c_str());

  // Display Date 
  if (timeinfo.tm_mday != TmpDay) {
    //printLocalTime();
    sPrintdate = (String(timeinfo.tm_mday).length() > 1 ? String(timeinfo.tm_mday) : "0" + String(timeinfo.tm_mday)) + "/" + (String(timeinfo.tm_mon+1).length() > 1 ? String(timeinfo.tm_mon+1) : "0" + String(timeinfo.tm_mon+1))+ "/" + String(timeinfo.tm_year+1900);

    sPrintShortdate = (String(timeinfo.tm_mday).length() > 1 ? String(timeinfo.tm_mday) : "0" + String(timeinfo.tm_mday)) + "/" + (String(timeinfo.tm_mon+1).length() > 1 ? String(timeinfo.tm_mon+1) : "0" + String(timeinfo.tm_mon+1));

    if (SERDEBUG) Serial.println("Date: " + sPrintdate);
    TmpDay = timeinfo.tm_mday;

  }
}

/*
  stext2.pushSprite(0, 56);
  //delay(5); // sped it up a little
  stext2.scroll(-1);    // scroll stext 1 pixel left, up/down default is 0
  tcount--;
  if (tcount <=0)
  { // If we have scrolled 240 pixels then redraw text
    tcount = 450;

    // print date
    stext2.setTextColor(TFT_GREENYELLOW, TFT_BLACK);
    stext2.drawString(sPrintdate, 240, 0, 4);
    // Temp Ext
    stext2.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    if (rTempExt >= 10.0 ) {
      stext2.setTextColor(TFT_GREEN, TFT_BLACK);
    } else if (rTempExt >= 25.0) {
      stext2.setTextColor(TFT_ORANGE, TFT_BLACK);
    } else if (rTempExt >= 30.0) {
      stext2.setTextColor(TFT_RED, TFT_BLACK);
    }
    stext2.drawString((sTempExt + sTrend).c_str(), 240, 25, 4);

    // Temp Salon
    stext2.setTextColor(TFT_BLUE, TFT_BLACK);
    stext2.drawString(sTempSal.c_str(), 240, 50, 4);

    // Temp Min
    stext2.setTextColor(TFT_ORANGE, TFT_BLACK);
    stext2.drawString("Ext. Min", 450, 0, 4);

    stext2.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    stext2.drawString(String(fnMin(rTempExt)) + " " + String((char)167) + "C", 450, 25, 4);

    stext2.drawString((sExtMinTimeStp), 450, 50, 4);

    // Temp Max
    stext2.setTextColor(TFT_ORANGE, TFT_BLACK);
    stext2.drawString("Ext. Max", 550, 0, 4);

    stext2.setTextColor(TFT_SKYBLUE, TFT_BLACK);
    stext2.drawString(String(fnMax(rTempExt)) + " " + String((char)167) + "C", 550, 25, 4);

    stext2.drawString((sExtMaxTimeStp), 550, 50, 4);

  }
*/

