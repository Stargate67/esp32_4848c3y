#include <Arduino.h>
#include "OTA.h"
#include "time.h"
#include "My_Modbus.h"
#include "Globals.h"
#include "MainScreen.h"
#include "MainScreen2.h"

//#include <ModbusIP_ESP8266.h>

void UpdateTickers(void);

int TmpDay;
struct tm timeinfo;

// Attribue une adresse Ip fixe, la passerelle par défaut et le masque de sous-réseau.
IPAddress IP(192, 168, 0, 115);      //adresse fixe
IPAddress gateway(192, 168, 0, 254);      //passerelle par défaut
IPAddress subnet(255, 255, 255, 0);     //masque de sous réseau
IPAddress dns(192, 168, 0, 254);  //DNS

WiFiClient client;

String sClockHHMMSS;
String sClockHHMM;
String sPrintdate;
String sPrintShortdate;

lv_obj_t *IPLabel;
lv_obj_t *AlarmLabel;
lv_obj_t *ClockLabel;

Tempos TimerScan10ms(10);
Tempos TimerScan50ms(50);
Tempos TimerScan100ms(100);
Tempos TimerCheckWifi(3000);

IPAddress LocalIP;

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
  WiFi.mode(WIFI_STA);
  WiFi.config(IP, gateway, subnet, dns);  
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
  //String sLocalIP = WiFi.localIP().toString();
  LocalIP = WiFi.localIP();
  lv_label_set_text_fmt(IPLabel, ".%d.%d\nWifi:%d", LocalIP[2], LocalIP[3], WiFi.status());
  //(sLocalIP.substring( strlen(sLocalIP), -6)).c_str());
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
//                                         MAIN SETUP
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

  InitUI();

  startWifi();
  
  mb.client();

  // A TESTER    Set up Modbus registers
  //mb.config();
  //mb.addHreg(0, 123); // Example holding register value

  setupOTA("ESP32_4848SD_HOMIS_FY", ssid, password);

  initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Paris/FR
}


//************************************************************************************************************/
//                                           MAIN LOOP 
//============================================================================================================/

void loop() {
  ArduinoOTA.handle();
  UpdateTickers();

  // ========  Main Tasks à 10ms  =========== 
  if (TimerScan10ms.Q()){ // 10ms
    if (WiFi.status() == WL_CONNECTED) {
      MainModbus();
      mb.task(); // Tache principale Traitement ModbusTCP
    }
    TimerScan10ms.Reset();
  }

  // ========  Main Tasks à 100ms  =========== 
  if (TimerScan100ms.Q()){ // 100ms
    String sMesAlarme = "";

    Relays();
    AcquitMesAlarme();
    
    if (WiFi.status() == WL_CONNECTED) {
      // Affichage top left corner Déroulement étapes Modbus et ad. IP
      lv_label_set_text_fmt(IPLabel, ".%d.%d\ni:%d", LocalIP[2], LocalIP[3], iState);

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
    } else {
      // Si pas de connexion WIFI => Message plus complet top left corner
      lv_label_set_text_fmt(IPLabel, ".%d.%d\nW:%d i:%d", LocalIP[2], LocalIP[3], WiFi.status(), iState);
    }
    TimerScan100ms.Reset();
  }

  // ========  Check Wifi each minute  =========== 
  if (TimerCheckWifi.Q()) { // 1mn
    if (WiFi.status() != WL_CONNECTED) {
      //Serial.print(millis());
      //Serial.println("Reconnecting to WiFi...");
      String sPrefix = "/!\\ " + sClockHHMM + " ";
      String sMessage = sPrefix + "Plus de WIFI Stargate. Reconnexion en cours.";
      lv_label_set_text(AlarmLabel, sMessage.c_str());
      delay(100);
      UpdateTickers();
      WiFi.disconnect();
      delay(2000);
      WiFi.reconnect();
    }
    TimerCheckWifi.Reset();
  }
}

