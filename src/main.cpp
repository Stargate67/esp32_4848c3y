#include <Arduino.h>
#include "OTA.h"
#include "time.h"
#include "My_Modbus.h"
#include "Globals.h"
#include <Preferences.h>

void UpdateTickers(void);

int TmpDay;
struct tm timeinfo;

// Attribue une adresse Ip fixe, la passerelle par défaut et le masque de sous-réseau.
IPAddress IP(192, 168, 0, 115);      //adresse fixe
IPAddress gateway(192, 168, 0, 254);      //passerelle par défaut
IPAddress subnet(255, 255, 255, 0);     //masque de sous réseau
IPAddress dns(192, 168, 0, 254);  //DNS

WiFiClient client;

// Identifiants WiFi persistants (NVS): charges au boot par loadWifiCredentials(), modifiables
// a chaud depuis l'ecran de configuration WiFi (voir MainScreen.cpp) via applyWifiCredentials().
// credentials.h ne sert plus que de valeur par defaut au tout premier boot (NVS vide).
static Preferences wifiPrefs;
String gWifiSsid;
String gWifiPassword;

//char *sClockHHMMSS;
char sClockHHMM[15];
char sDateDDMMYYYY[16];
char sShortDateDDMM[15];

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

void setTimezone(String timezone){
  if (SERDEBUG) Serial.printf(" Setting Timezone to %s\n",timezone.c_str());
  setenv("TZ",timezone.c_str(),1);  //  Now adjust the TZ.  Clock settings are adjusted to show the new local time
  tzset();
}

void initTime(String timezone){
  struct tm timeinfo;

  if (SERDEBUG) Serial.println("Setting up time");
    configTime(0, 0, "fr.pool.ntp.org", "time.windows.com");    // First connect to NTP server, with 0 TZ offset
  if (!getLocalTime(&timeinfo)) {
    if (SERDEBUG) Serial.println(" Failed to obtain time");
    return;
  }

  if (SERDEBUG) Serial.println(" Got the time from NTP");
  // Now we can set the real timezone
  setTimezone(timezone);
}

// Lit SSID/mot de passe depuis la NVS (Preferences); au tout premier boot (rien en NVS),
// se rabat sur les valeurs par defaut de credentials.h.
void loadWifiCredentials(){
  wifiPrefs.begin("wifi", true);
  gWifiSsid = wifiPrefs.getString("ssid", ssid);
  gWifiPassword = wifiPrefs.getString("pass", password);
  wifiPrefs.end();
}

// Enregistre les nouveaux identifiants en NVS et reconnecte a chaud (pas de reboot):
// WiFi.begin() est non bloquant, la reconnexion est ensuite suivie par le TimerCheckWifi
// existant dans loop() qui reflete l'etat via WiFi.status() (IPLabel/AlarmLabel).
void applyWifiCredentials(const String &newSsid, const String &newPassword){
  wifiPrefs.begin("wifi", false);
  wifiPrefs.putString("ssid", newSsid);
  wifiPrefs.putString("pass", newPassword);
  wifiPrefs.end();

  gWifiSsid = newSsid;
  gWifiPassword = newPassword;

  WiFi.disconnect();
  WiFi.begin(gWifiSsid.c_str(), gWifiPassword.c_str());
}

void startWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.config(IP, gateway, subnet, dns);
  WiFi.begin(gWifiSsid.c_str(), gWifiPassword.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Recherche WIFI......");
    lv_label_set_text(IPLabel, "Recherche WIFI......");
    delay(100);
    UpdateTickers();
  }

  Serial.print("Wifi RSSI=");
  Serial.println(WiFi.RSSI());
  Serial.println("");
  Serial.println(WiFi.localIP());
  LocalIP = WiFi.localIP();
  lv_label_set_text_fmt(IPLabel, ".%d.%d\nWifi:%d", LocalIP[2], LocalIP[3], WiFi.status());
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

  loadWifiCredentials();
  loadPLCAddress();

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);

  InitUI();

  // Diagnostic temporaire: verifie si le pool memoire LVGL (LV_MEM_SIZE, lib/lv_conf.h) est
  // sature apres construction de tous les ecrans - a retirer une fois le blocage au 1er
  // rendu (ecran noir apres "Recherche WIFI......") diagnostique.
  {
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    Serial.printf("LVGL mem: used=%u%% frag=%u%% free_size=%u free_biggest=%u max_used=%u\n",
                  mon.used_pct, mon.frag_pct, (unsigned)mon.free_size,
                  (unsigned)mon.free_biggest_size, (unsigned)mon.max_used);
  }

  startWifi();
  
  mb.client();

  // A TESTER    Set up Modbus registers
  //mb.config();
  //mb.addHreg(0, 123); // Example holding register value

  setupOTA("ESP32_4848SD_HOMIS_FY", gWifiSsid.c_str(), gWifiPassword.c_str());

  initTime("CET-1CEST,M3.5.0,M10.5.0/3");   // Set for Paris/FR
}


//************************************************************************************************************/
//                                           MAIN LOOP 
//============================================================================================================/

void loop() {
  // Diagnostic temporaire: detecte les iterations de loop() anormalement longues (saccade LVGL)
  static unsigned long tPrevLoop = micros();
  unsigned long tNowLoop = micros();
  unsigned long loopGap = tNowLoop - tPrevLoop;
  tPrevLoop = tNowLoop;
  if (loopGap > 15000) {
    Serial.printf("LOOP GAP %lu us, iState=%d\n", loopGap, iState);
  }

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
    Relays();
    AcquitMesAlarme();
    
    if (WiFi.status() == WL_CONNECTED) {
      // Affichage top left corner adresse IP et Déroulement étapes Modbus
      //lv_label_set_text_fmt(IPLabel, LV_SYMBOL_WIFI".%d.%d\ni:%02d", LocalIP[2], LocalIP[3], iState);
      lv_label_set_text_fmt(IPLabel, ".%d.%d\ni:%02d", LocalIP[2], LocalIP[3], iState);

      if (!getLocalTime(&timeinfo)) {
        lv_label_set_text(AlarmLabel, "Pas de synchro Horloge!");
      } else {
        // Display clock
        lv_label_set_text_fmt(ClockLabel, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        strftime(sClockHHMM, sizeof(sClockHHMM), "%H:%M", &timeinfo);

        // Display Date 
        if (timeinfo.tm_mday != TmpDay) {
        strftime(sDateDDMMYYYY, sizeof(sDateDDMMYYYY), "%d/%m/%Y", &timeinfo);
        strftime(sShortDateDDMM, sizeof(sShortDateDDMM), "%d/%m", &timeinfo);
        
        if (SERDEBUG) Serial.println("Date: " + String(sDateDDMMYYYY));
          TmpDay = timeinfo.tm_mday;
        }
      }
    } else {
      // Si pas de connexion WIFI => Message plus complet top left corner
      lv_label_set_text_fmt(IPLabel, ".%d.%d\nW:%d i:%d", LocalIP[2], LocalIP[3], WiFi.status(), iState);
    }
    TimerScan100ms.Reset();
  }

  // ========  Check Wifi toutes les 3s tant que non connecte  ===========
  if (TimerCheckWifi.Q()) { // 3000ms (Tempos TimerCheckWifi(3000))
    if (WiFi.status() != WL_CONNECTED) {
      String sPrefix = "# " + String(sClockHHMM) + " ";
      String sMessage = sPrefix + "Pas de WIFI " + gWifiSsid + ". En attente de reconnexion.";
      lv_label_set_text(AlarmLabel, sMessage.c_str());
      WiFi.reconnect(); // Non bloquant: ne gele pas l'ecran/OTA pendant l'attente
    }
    TimerCheckWifi.Reset();
  }
}
