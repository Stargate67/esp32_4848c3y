#include <Arduino.h>
#include <esp32_smartdisplay.h>
#include <MainScreen.h>
#include "OTA.h"
#include "time.h"
#include <ModbusIP_ESP8266.h>

void UpdateTickers(void);

uint32_t targetTime = 0;       // for next 1 second timeout

#define SERDEBUG false

byte omm = 99;
boolean initial = 1;
byte xcolon = 0;
unsigned int colour = 0;
String sPrintdate;
String sPrintShortdate;
String sClockHHMMSS;
String sClockHHMM;
String sExtMaxTimeStp = "00:00";
String sExtMinTimeStp = "00:00";
String sLocalIP;
String sTempExt;
String sTempSal;
String sTrend;
float rAvgTempExt = 0;
float rTmp = 0;
bool bTempDisplay = true; 
unsigned long time_now = 0;
//unsigned long time1_now = 0;
unsigned long readMillis;
unsigned long prevmillis = 0; //used to hold previous value of currmillis
unsigned long prevmillis1 = 0; //used to hold previous value of currmillis
int timer = 0;     //used in the delay function, difference between currmillis and prevmillis
int TmpDay;
struct tm timeinfo;
int iState = 0;

const int numReadings = 10;
float readings [numReadings];
int iReadIndex = 0;
int iStartIndex = 0;
float total = 0;
int aisVal = 0;

WiFiClient client;
ModbusIP mb;  //ModbusIP object
//IPAddress MBremote(192, 168, 0, 105);  // Address of Modbus Slave device
IPAddress MBremote(77, 204, 15, 6);   // Address of Internet Box 
const int START_REG = 12688;       // Starting holding register
const int NUM_REGS = 10;           // Number of holding registers to read
const int INTERVAL = 5000;         // Interval between reads (in milliseconds)

uint16_t MBresult[NUM_REGS];
uint8_t show = NUM_REGS;  // Counter for displaying values
uint32_t LastModbusRequest = 0;  // Variable to track the last Modbus request time
float rTempExt;
float rTempSal;

/////////////////////////////////////////////////////////////////////////////////////////////////
//                         FIN DES DECLARATIONS 
/////////////////////////////////////////////////////////////////////////////////////////////////

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

//uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

float fnMax(float fInput) { /* function fnMax */
  //Perform Max Value 
  static float fMaxValue = fInput;

  if (fInput > fMaxValue || sClockHHMM == "00:00" ) { 
    fMaxValue = fInput;
    sExtMaxTimeStp = sClockHHMM;
  }
  return fMaxValue;
}

float fnMin(float fInput) { /* function fnMax */
  //Perform Max Value 
  static float fMinValue = fInput;

  if (fInput < fMinValue || sClockHHMM == "00:00") { 
    fMinValue = fInput;
    sExtMinTimeStp = sClockHHMM;
  }
  return fMinValue;
}

float fnAverage(float fInput) { /* function fnAverage */
  //Perform average on sensor readings
  float average;
  static float total;
  // subtract the last reading:
  total = total - readings[iReadIndex];
  // read the sensor:
  readings[iReadIndex] = fInput;
  // add value to total:
  total = total + readings[iReadIndex];
  // handle index
  iReadIndex = iReadIndex + 1;
  if (iReadIndex >= numReadings) {
    iReadIndex = 0;
  }
  if (iStartIndex < numReadings) {
    iStartIndex = iStartIndex + 1;
  }

  // calculate the average:
  average = total / numReadings;

  if (iStartIndex >= numReadings) {
    return average;
  } else {
    return fInput;
  }
}

void ReadModbus() {
  mb.task();
  if (mb.isConnected(MBremote)) {  
    //if (SERDEBUG) Serial.println(String(iState));
    switch (iState) {
      case 0:
      {
        // Read holding registers from Modbus Slave
        uint8_t transaction = mb.readHreg(MBremote, START_REG, MBresult, NUM_REGS, nullptr, 1);        
        prevmillis1 = millis();
        iState = 10;
        if (SERDEBUG) Serial.println("iState="+String(iState));
      } 
      break;

      case 10:
      {
        // Wait for the transaction to complete
        if (millis() >= prevmillis1 + 50){ //Process MB client request each second
          prevmillis1 = millis();
          iState = 20;
          if (SERDEBUG) Serial.println("iState="+String(iState));
        }
      }
      break;

      // Lecture des valeurs dans le buffer MB et mise ne forme
      case 20:
      {
        mb.disconnect(MBremote);
        rTempSal = round(MBresult[0] * 100.0 / 10.0)/100.0;
        sTempSal = "T. Sal: " + String(rTempSal) + " " + String((char)167) + "C";
        rTmp = (MBresult[8] * 100.0 / 32764.0) - 50.0; // Mise a l'echelle
        rTempExt = round(rTmp * 100.0)/100.0; // 2 digits 
        sTempExt = "T. Ext: " + String(rTempExt) + " " + String((char)167) + "C";
        // Calcul la moyenne 
        rAvgTempExt = round(fnAverage(rTempExt) * 100.0) / 100.0;

        sTrend = String("  =");
        if (rTempExt > rAvgTempExt) {
          sTrend = String("  /");
        } else if (rTempExt < rAvgTempExt){
          sTrend = String("  \\");
        }

        if (SERDEBUG) { 
          Serial.print("Avg T.Ext. = ");
          Serial.print(String(rAvgTempExt));
          Serial.println(sTrend);
        }
        LastModbusRequest = millis();
        iState = 30;
        
        if (SERDEBUG) {
          Serial.println("iState=" + String(iState));
        // Print holding register values
          Serial.println("Holding Register Values:");
          for (int i = 0; i < NUM_REGS; i++) {
            Serial.print("Register ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(MBresult[i]);
          }
        }
      }
      break;

      case 30:
      {     // Wait 5 sec
        if (millis() - LastModbusRequest >= INTERVAL) {
          LastModbusRequest = millis();
          iState = 0;  // On recommence
          if (SERDEBUG) Serial.println(String(iState));
        }
      }
      break;

      default:
      {
        iState = 0;
        if (SERDEBUG) Serial.println(String(iState));
      }
      break;
      }

  } else {
    // If not connected, try to connect
    mb.connect(MBremote);
  }
}

void timeloop (int interval){ // the delay function
  prevmillis = millis();
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
  sLocalIP = WiFi.localIP().toString();
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

  // Initialize the display
  smartdisplay_init();

  // Set backlight to full brightness
  smartdisplay_lcd_set_backlight(0.6); // 0 is off, 0.5 is half and 1 is full brightness.

  __attribute__((unused)) auto disp = lv_disp_get_default();
  // lv_disp_set_rotation(disp, LV_DISP_ROT_90);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_180);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_270);

  // Get the active screen
  scr = lv_scr_act();

  // Set screen background color to dark blue
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x090909), 0);

  // Create the Hello World label
  CreateIPLabel(scr);
  CreateClock(scr);
  CreateAlarm(scr);
  lv_example_button_R1(scr);
  lv_example_button_R2(scr);
  lv_example_button_R3(scr);

  startWifi();
  initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Paris/FR
  mb.client();
  setupOTA("ESP32_4848SD_FY", ssid, password);
}

void loop() {
  ArduinoOTA.handle();
  UpdateTickers();
  ReadModbus();
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



