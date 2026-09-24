/*
    My_Modbus.cpp - ModbusIP 
    HomIS mangement with HomIS PLC
*/

#include "Globals.h"
#include "My_Modbus.h"
#include <ModbusIP_ESP8266.h>
#include <Preferences.h>
#include <map>

#define NBAVGFIFO 29

ModbusIP mb;

// Adresse IP du PLC WAGO, mutable: valeur par defaut ecrasee par loadPLCAddress() si une
// adresse a deja ete enregistree depuis l'ecran de configuration PLC (voir MainScreen.cpp).
IPAddress MBremote(192, 168, 0, 105);

static Preferences plcPrefs;

// Lit l'adresse IP du PLC depuis la NVS (Preferences); si rien n'est enregistre (premier
// boot), MBremote garde sa valeur par defaut ci-dessus.
void loadPLCAddress(){
  plcPrefs.begin("modbus", true);
  String saved = plcPrefs.getString("plcip", "");
  plcPrefs.end();
  IPAddress addr;
  if (saved.length() > 0 && addr.fromString(saved)) {
    MBremote = addr;
  }
}

// Enregistre la nouvelle adresse en NVS et l'applique immediatement: la machine a etats de
// MainModbus() (case 0/99) reconnecte automatiquement des le prochain cycle, sans reboot.
void applyPLCAddress(const IPAddress &newAddr){
  MBremote = newAddr;
  plcPrefs.begin("modbus", false);
  plcPrefs.putString("plcip", MBremote.toString());
  plcPrefs.end();
}

// Registre Modbus du %MW630 (INT PLC, valeur x10): documentation PLC "412919 => %MW630 =>
// CONSIGNE T° SALON en 1/10", convention Modicon standard (adresse = numero documente -
// 400001), soit 412919 - 400001 = 12918. Confirme sur site le 2026-09-24 (lecture affichait
// 18.2 avec l'ancienne adresse 12919 alors que la consigne reelle etait a son minimum 16.0).
// Ce registre est aussi relu en continu (case 5/20 ci-dessous) pour que le bouton "Consigne"
// affiche toujours la vraie valeur du PLC, jamais une simple copie locale de la derniere ecriture.
constexpr uint16_t REG_CONSIGNE_TEMP = 12918;  // %MW630
constexpr uint16_t NB_REGS_CONSIGNE = 1;

uint16_t MBresultConsigne[NB_REGS_CONSIGNE];  // Lecture du registre de consigne (%MW630)

float gConsigneTemp = 20.0;  // Valeur affichee: resynchronisee sur la lecture PLC a chaque cycle (case 20)

uint16_t mbWriteHregAddress;  // Adresse holding register a ecrire au prochain cycle (0 = rien a ecrire)
uint16_t mbWriteHregValue;    // Valeur a ecrire (INT PLC, x10: 205 = 20.5°C)

// Programme l'ecriture de la nouvelle consigne (%MW630): la machine a etats de MainModbus()
// (case 0/95) l'envoie au PLC des le prochain cycle. gConsigneTemp est mis a jour ici pour un
// retour visuel immediat sur l'ecran de saisie, puis resynchronise sur la valeur reellement
// relue du PLC des le cycle de lecture suivant (~1s, voir MB_READ_INTERVAL).
void applyConsigneTemp(float newTemp){
  gConsigneTemp = newTemp;
  mbWriteHregValue = (uint16_t)(int16_t)lround(newTemp * 10.0);
  mbWriteHregAddress = REG_CONSIGNE_TEMP;
}

//unsigned long prevmillis1;
unsigned long LastModbusRequest;  // Variable to track the last Modbus request time
//unsigned long TransactMillis1;    // Timeout Transaction

int iState = 0;

// Anti-blocage case 99: mb.connect() est un appel bloquant (TCP connect). Sans cette tempo,
// une IP PLC injoignable ferait boucler case 0 <-> case 99 en tentant une connexion a chaque
// passage (toutes les 10ms), gelant l'ecran tactile en continu. Meme pattern que TimerCheckWifi
// dans main.cpp: on limite les tentatives a une toutes les 3s.
Tempos TimerModbusReconnect(3000);

volatile unsigned long g_case20DurationUs = 0; // Diagnostic temporaire: duree du dernier passage en case 20

const char *sExtMaxTimeStp = "NA:NA";
const char *sExtMinTimeStp = "NA:NA";

const char *sClock0000 = "00:00";

int iReadIndex;   // Index d'ecriture courant dans le buffer circulaire de moyenne (fnAverage)
int iStartIndex;  // Nombre d'echantillons deja accumules, jusqu'a NBAVGFIFO (buffer plein)
uint16_t MBresultANA1[NB_REGS];       // Buffer de lecture des registres analogiques (temperatures, conso, etc.)
uint16_t MBresultANIM1[NB_REGS_ANIM]; // Buffer de lecture des registres d'animation (etat marche relais + alarmes)
//bool MBresultCL1[NB_OUTPUT1];

uint16_t mbWriteCoilAddress;  // Adresse de bobine Modbus a ecrire au prochain cycle (0 = rien a ecrire)

// Labels de l'ecran principal, mis a jour depuis les registres Modbus dans MainModbus() (case 20)
lv_obj_t * ui_LblTempExt;   // Temperature exterieure courante
lv_obj_t * ui_LblDate;      // Date du jour
lv_obj_t * ui_LblTempSalon; // Temperature du salon
lv_obj_t * ui_LblTempMin;   // Temperature exterieure mini du jour
lv_obj_t * ui_LblHeureMin;  // Heure du releve mini
lv_obj_t * ui_LblTempMax;   // Temperature exterieure maxi du jour
lv_obj_t * ui_LblHeureMax;  // Heure du releve maxi

lv_obj_t * ui_LblValPlancher;      // Temperature plancher chauffant
lv_obj_t * ui_LblValConsPlancher;  // Consigne plancher chauffant
lv_obj_t * ui_LblValECS;           // Temperature ballon eau chaude sanitaire
lv_obj_t * ui_LblValRadiat;        // Temperature radiateur
lv_obj_t * ui_LblValDebitRadit;    // Debit du circuit radiateur
lv_obj_t * ui_LblValCourant;       // Courant electrique consomme
lv_obj_t * ui_LblValConsigneTemp;  // Consigne de temperature (bouton "Consigne", ecran Relais)

lv_obj_t * ui_LblValConsoInstEau;   // Consommation eau instantanee
lv_obj_t * ui_LblValConsoInstElec;  // Consommation electrique instantanee
lv_obj_t * ui_LblValConsoInstGaz;   // Consommation gaz instantanee
lv_obj_t * ui_LblValConsoJEau;      // Consommation eau cumulee du jour
lv_obj_t * ui_LblValConsoJElec;     // Consommation electrique cumulee du jour
lv_obj_t * ui_LblValConsoJGaz;      // Consommation gaz cumulee du jour
lv_obj_t * ui_LblValConsoJ1Elec;    // Consommation electrique totale de la veille (J-1)
lv_obj_t * ui_LblValConsoJ1Eau;     // Consommation eau totale de la veille (J-1)
lv_obj_t * ui_LblValConsoJ1Gaz;     // Consommation gaz totale de la veille (J-1)

bool bChaudiere;    // Modbus Etat Marche Chaudière
bool bBoostChaud;   // Modbus Etat Marche Boost Chaudière
bool bPpeRadiat;    // Modbus Etat Marche pompe Radiateur
bool bPpePlancher;  // Modbus Etat Marche pompe Plancher
bool bArriveeEau;   // Modbus Etat Marche arrivée eau

// Cache generique pour eviter d'appeler LVGL quand la valeur affichee n'a pas change: LVGL
// invalide l'objet (donc programme un flush) a chaque lv_label_set_text()/set_style_*_color(),
// meme si la valeur est identique (verifie dans lib/lvgl/src/widgets/label/lv_label.c et
// lib/lvgl/src/core/lv_obj_style.c). Le cache est indexe par le pointeur lv_obj_t* dans une
// map interne a chaque fonction: ajouter un nouveau label/couleur ne demande donc aucune
// declaration de cache ni modification de structure, juste un appel a la bonne fonction.
static uint32_t packColor(lv_color_t c){
  return (uint32_t(c.red) << 16) | (uint32_t(c.green) << 8) | c.blue;
}

static void setLabelTextIfChanged(lv_obj_t *obj, const String &newText){
  static std::map<lv_obj_t*, String> cache;
  auto it = cache.find(obj);
  if (it == cache.end() || it->second != newText) {
    cache[obj] = newText;
    lv_label_set_text(obj, newText.c_str());
  }
}

static void setTextColorIfChanged(lv_obj_t *obj, lv_color_t color){
  static std::map<lv_obj_t*, uint32_t> cache;
  uint32_t packed = packColor(color);
  auto it = cache.find(obj);
  if (it == cache.end() || it->second != packed) {
    cache[obj] = packed;
    lv_obj_set_style_text_color(obj, color, LV_PART_MAIN | LV_STATE_DEFAULT);
  }
}

static void setBgColorIfChanged(lv_obj_t *obj, lv_color_t color){
  static std::map<lv_obj_t*, uint32_t> cache;
  uint32_t packed = packColor(color);
  auto it = cache.find(obj);
  if (it == cache.end() || it->second != packed) {
    cache[obj] = packed;
    lv_obj_set_style_bg_color(obj, color, 0);
  }
}

// Applique l'etat anime d'un relais (bit de MBresultANIM1[0]) a son bool d'etat, sa commande,
// et la couleur du bouton (ecran Relais) + du voyant d'etat (ecran principal), en ne touchant
// le style LVGL que si la couleur a reellement change.
static void updateAnimatedRelay(uint16_t animReg, uint16_t mask, bool &stateFlag, bool &cmdFlag, lv_obj_t *btn,
                                 lv_color_t colorOn, lv_color_t colorOff, lv_obj_t *led){
  lv_color_t color;
  if (animReg & mask) {
    stateFlag = 1;
    cmdFlag = 1;
    color = colorOn;
  } else {
    stateFlag = 0;
    cmdFlag = 0;
    color = colorOff;
  }
  setBgColorIfChanged(btn, color);
  setBgColorIfChanged(led, color);
}

// Valeurs de l'ecran principal deja mises en forme (registres Modbus -> String), transmises
// de case 20 a UpdateLVGLFromModbus() via une map cle/valeur generique plutot qu'une structure
// nommee: ajouter une nouvelle valeur ne demande qu'une ligne v["MaCle"] = ... cote case 20 et
// un get(v, "MaCle") cote affichage, sans jamais retoucher de declaration.
using ModbusDisplayValues = std::map<String, String>;

static String get(const ModbusDisplayValues &v, const char *key){
  auto it = v.find(key);
  return it != v.end() ? it->second : String();
}

// MAJ des labels/couleurs LVGL de l'ecran principal + des voyants relais, a partir des
// valeurs deja mises en forme par case 20. Isolee hors de MainModbus() pour ne pas
// encombrer la machine a etats. Chaque MAJ est filtree via textChanged()/colorChanged():
// LVGL invalide l'objet (donc programme un flush) meme si la valeur ne change pas
// (verifie dans lib/lvgl/src/widgets/label/lv_label.c et lib/lvgl/src/core/lv_obj_style.c),
// ce qui bloquait ~500ms sur ce panneau ST7701 (buffer plein-ecran unique en PSRAM) quand
// tous les labels etaient invalides en une fois.
static void UpdateLVGLFromModbus(const ModbusDisplayValues &v) {
  // --- Temp exterieure: couleur selon seuils + label ---
  float rTempExt = get(v, "TempExtRaw").toFloat();
  float rAvgTempExt = get(v, "AvgTempExtRaw").toFloat();
  lv_color_t colorTempExt = lv_color_hex(0xC2ED34);
  if (rTempExt > 25.0) colorTempExt = lv_color_hex(0xFF7D00);
  if (rTempExt > 32.0) colorTempExt = lv_color_hex(0xFB2626);
  setTextColorIfChanged(ui_LblTempExt, colorTempExt);
  setLabelTextIfChanged(ui_LblTempExt, get(v, "TempExt"));
  setLabelTextIfChanged(ui_LblDate, sDateDDMMYYYY);

  // --- Graphique selectionnable (ecran Relais): echantillonne toutes les mesures en continu,
  // voir GRAPH_MEASUREMENTS/SampleMeasurementGraph() dans MainScreen.cpp ---
  SampleMeasurementGraph("TempExtRaw", rTempExt);
  SampleMeasurementGraph("TempSalonRaw", get(v, "TempSalonRaw").toFloat());
  SampleMeasurementGraph("TempPlancherRaw", get(v, "TempPlancherRaw").toFloat());
  SampleMeasurementGraph("TempECSRaw", get(v, "TempECSRaw").toFloat());
  SampleMeasurementGraph("TempRadiatRaw", get(v, "TempRadiatRaw").toFloat());

  // --- Couleur + labels Min/Max selon tendance de la Temp ext. ---
  lv_color_t colorMin = lv_color_hex(0x00FFFF);
  lv_color_t colorMax = lv_color_hex(0x00FFFF);
  if (rTempExt > rAvgTempExt) {
    colorMax = lv_color_hex(0xFF7D00);
  } else if (rTempExt < rAvgTempExt) {
    colorMin = lv_color_hex(0xFF7D00);
  }
  setTextColorIfChanged(ui_LblTempMin, colorMin);
  setTextColorIfChanged(ui_LblTempMax, colorMax);

  setLabelTextIfChanged(ui_LblTempMin, get(v, "TempExtMin"));
  setLabelTextIfChanged(ui_LblTempMax, get(v, "TempExtMax"));
  setLabelTextIfChanged(ui_LblHeureMin, get(v, "HeureMin"));
  setLabelTextIfChanged(ui_LblHeureMax, get(v, "HeureMax"));

  setLabelTextIfChanged(ui_LblTempSalon, get(v, "TempSalon"));

  // --- Plancher chauffant: temperature + consigne ---
  setLabelTextIfChanged(ui_LblValPlancher, get(v, "TempPlancher"));
  setLabelTextIfChanged(ui_LblValConsPlancher, get(v, "ConsPlancher"));

  // --- ECS / Radiateur / Courant ---
  setLabelTextIfChanged(ui_LblValECS, get(v, "TempECS"));
  setLabelTextIfChanged(ui_LblValRadiat, get(v, "TempRadiat"));
  setLabelTextIfChanged(ui_LblValDebitRadit, get(v, "DebitRadiat"));
  setLabelTextIfChanged(ui_LblValCourant, get(v, "Courant"));
  setLabelTextIfChanged(ui_LblValConsigneTemp, get(v, "ConsigneTemp"));
  RefreshConsigneDisplay(); // Ecran de saisie (valeur jaune): meme resync, sans devoir quitter/revenir
  UpdateGraphTimeAxis();    // Etiquettes HH:MM de l'axe X du graphique (heure glissante)

  // --- Consommations instantanees Eau/Elec/Gaz ---
  setLabelTextIfChanged(ui_LblValConsoInstEau, get(v, "ConsoInstEau"));
  setLabelTextIfChanged(ui_LblValConsoInstElec, get(v, "ConsoInstElec"));
  setLabelTextIfChanged(ui_LblValConsoInstGaz, get(v, "ConsoInstGaz"));

  // --- Consommations Eau/Elec/Gaz du jour et de la veille (J-1) ---
  setLabelTextIfChanged(ui_LblValConsoJEau, get(v, "ConsoJEau"));
  setLabelTextIfChanged(ui_LblValConsoJElec, get(v, "ConsoJElec"));
  setLabelTextIfChanged(ui_LblValConsoJGaz, get(v, "ConsoJGaz"));
  setLabelTextIfChanged(ui_LblValConsoJ1Eau, get(v, "ConsoJ1Eau"));
  setLabelTextIfChanged(ui_LblValConsoJ1Elec, get(v, "ConsoJ1Elec"));
  setLabelTextIfChanged(ui_LblValConsoJ1Gaz, get(v, "ConsoJ1Gaz"));

  // --- Voyants relais (ecran Relais + ecran principal) ---
  // Traitement animation des BPs sur retour MBus (bouton ecran Relais + voyant ecran principal)
  updateAnimatedRelay(MBresultANIM1[0], MASK_CHAUD, bChaudiere, bCdeRelaisR1, btnR1Chaudiere,
                       lv_color_make( 0, 160, 60 ), lv_color_make( 100, 100, 100 ), ledChaud);

  lv_color_t colorBoost;
  if (MBresultANIM1[0] & MASK_BOOST_ANIM) {
    bBoostChaud = 1;
    bCdeRelaisR2 = 1;
    // Couleur Boost differenciee sur passage d'eau reel
    colorBoost = (get(v, "DebitRadiat").toFloat() > 0.1) ? lv_color_make( 210, 16, 52 ) : lv_color_make( 255, 130, 0 );
  } else {
    bBoostChaud = 0;
    bCdeRelaisR2 = 0;
    colorBoost = lv_color_make( 110, 110, 110 );
  }
  setBgColorIfChanged(btnR2BoostCh, colorBoost);
  setBgColorIfChanged(ledBoost, colorBoost);

  updateAnimatedRelay(MBresultANIM1[0], MASK_PPERADIAT, bPpeRadiat, bCdeRelaisR3, btnR3PpeRadiateur,
                       lv_color_make( 0, 160, 60 ), lv_color_make( 120, 120, 120 ), ledRadiat);

  updateAnimatedRelay(MBresultANIM1[0], MASK_PPEPLANCHER, bPpePlancher, bRelay_4, btnPpePlancher,
                       lv_color_make( 0, 160, 60 ), lv_color_make( 130, 130, 130 ), ledPlancher);

  updateAnimatedRelay(MBresultANIM1[0], MASK_ARRIVEEAU, bArriveeEau, bRelay_5, btnArriveeEau,
                       lv_color_make( 40, 112, 226 ), lv_color_make( 130, 130, 130 ), ledArriveeEau);

  // Alarmes: DisplayAlarms() ne touche deja AlarmLabel que si le registre change
  // (voir Globals.cpp), pas besoin de filtrage supplementaire ici.
  DisplayAlarms(MBresultANIM1[3]); // Registre des alarmes MD230  HR 412748
}

void MainModbus() {
  //mb.task();
  switch (iState) {
    case 0:
    {
      if (SERDEBUG) Serial.println(String(iState));
      if (mb.isConnected(MBremote)) {
        // Si demande d'ecriture Modbus Coil on passe à l'etape 90
        if (mbWriteCoilAddress) {
          iState = 90;
        } else if (mbWriteHregAddress) {
          iState = 95;
        } else {
          // Lecture Modbus en continue
          iState = 5;
        }
      } else {
        iState = 99;
      }
    } 
    break;

    case 5:
    {
      // Read holding registers from Modbus Slave
      mb.readHreg(MBremote, START_REG, MBresultANA1, NB_REGS, nullptr, 1);
      mb.readHreg(MBremote, START_REG_ANIM, MBresultANIM1, NB_REGS_ANIM, nullptr, 1);
      mb.readHreg(MBremote, REG_CONSIGNE_TEMP, MBresultConsigne, NB_REGS_CONSIGNE, nullptr, 1);

      iState = 20;
      if (SERDEBUG) Serial.println("iState="+String(iState));
    }
    break;

    case 10:
    { //  ***********    Etape 10    **************
      // Wait for the transaction to complete
      iState = 20;
      if (SERDEBUG) Serial.println("iState="+String(iState));
    }
    break;

    // Lecture des valeurs dans le buffer MB et mise en forme. L'ecran (ST7701 RGB, buffer
    // plein-ecran unique en PSRAM, voir lib/esp32_smartdisplay) fusionne tout objet invalide
    // en un seul flush qui bloque le temps de le transferer: invalider ~15 labels disperses
    // en une fois bloquait ~500ms (mesure sur site, confirme present meme sur le commit
    // "Nouvelle box" avant cette session). LVGL n'evite pas ca tout seul: lv_label_set_text()
    // et lv_obj_set_style_*_color() invalident sans comparer a la valeur deja affichee (voir
    // lib/lvgl/src/widgets/label/lv_label.c et lib/lvgl/src/core/lv_obj_style.c). On filtre
    // donc ici via textChanged()/colorChanged(): la plupart des cycles Modbus ne changent que
    // quelques valeurs, donc plus besoin d'etaler le travail sur plusieurs ticks (subStep20).
    case 20:
    {
      unsigned long t0diag = micros(); // Diagnostic temporaire: mesure duree case 20
      float rTempExt = (MBresultANA1[8] * 100.0 / 32764.0) - 50.0; // Mise a l'echelle
      //float rTempExt = round(rTmp * 100.0)/100.0; // 2 digits
      String sTempExt = String(rTempExt, 2) + " °C";

      String sTempExtMin = String(((MBresultANA1[30] * 100.0 / 32764.0)-50.0), 1) + " °C";
      // Transforme un entier ex:1236 en heure '12:36'
      char cBuffer[10];
      sprintf(cBuffer, "%04d", MBresultANA1[31]);
      String sBuffer = String(cBuffer);
      String sTempExtTimeMin = sBuffer.substring(0, 2) + ":" + sBuffer.substring(2);

      String sTempExtMax = String(((MBresultANA1[32] * 100.0 / 32764.0)-50.0), 1) + " °C";
      sprintf(cBuffer, "%04d", MBresultANA1[33]);
      sBuffer = String(cBuffer);
      String sTempExtTimeMax = sBuffer.substring(0, 2) + ":" + sBuffer.substring(2);

      // Calcul la moyenne
      float rAvgTempExt = fnAverage(rTempExt);

      String sTempSal = "Sal: " + String(MBresultANA1[0]/10.0, 2) + " °C";

      // Container Val ANA1
      String sTempPlancher = String(MBresultANA1[1]/10.0, 1) + " °C";
      String sConsPlancher = String(MBresultANA1[12]/10.0, 1) + " °C";
      String sTempECS = String(MBresultANA1[2]/10.0, 1)+ " °C"; // 1 digits

      // Container Val ANA2
      String sTempRadiat = String(MBresultANA1[5]/10.0, 1)+ " °C"; // 1 digits
      String sDebitRadiat = String(MBresultANA1[7]/10.0, 1)+ " l/m"; // 1 digits
      String sCourant = String(MBresultANA1[15]/1000.0, 1)+ " A"; // 3 digits vers 1 digits

      // Container CONSO
      int iConsoElecInst = int(MBresultANA1[16]);
      int iConsoElecJ = int(MBresultANA1[17]);
      int iConsoElecJ1 = int(MBresultANA1[18]);
      int iConsoEauInst = int(MBresultANA1[19]);
      int iConsoEauJ = int(MBresultANA1[20]);
      int iConsoEauJ1 = int(MBresultANA1[21]);
      String sConsoGazInst = String(MBresultANA1[22]/100.0, 2);
      String sConsoGazJ = String(MBresultANA1[23]/100.0, 2);
      String sConsoGazJ1 = String(MBresultANA1[24]/100.0, 2) + " Nm3";

      // Consigne de temperature (%MW630): resynchronise gConsigneTemp sur la valeur reelle du
      // PLC a chaque cycle (voir applyConsigneTemp() plus haut pour le detail du pourquoi).
      gConsigneTemp = MBresultConsigne[0] / 10.0;
      {
        // Diagnostic temporaire: valeur brute du registre lue en Serial (contourne tout cache
        // d'affichage LVGL), pour verifier si le PLC clampe reellement la consigne ecrite.
        static uint16_t lastRawConsigne = 0xFFFF;
        if (MBresultConsigne[0] != lastRawConsigne) {
          lastRawConsigne = MBresultConsigne[0];
          Serial.printf("[Consigne] registre brut=%u -> %.1f C\n", MBresultConsigne[0], gConsigneTemp);
        }
      }
      String sConsigneTemp = String(gConsigneTemp, 1) + " °C";

      //if (SERDEBUG) Serial.println("sTempPlancher " + sTempPlancher);
      //if (SERDEBUG) Serial.println("TempECS " + sTempECS);
      //if (SERDEBUG) Serial.println("Courant " + sCourant);

      if (SERDEBUG) {
        Serial.print("Avg T.Ext. = ");
        Serial.println(String(rAvgTempExt));
      }

      // Mise en forme terminee: transmission a UpdateLVGLFromModbus() pour la partie
      // affichage (seule cette partie est filtree via textChanged()/colorChanged()).
      ModbusDisplayValues v;
      v["TempExtRaw"] = String(rTempExt, 4);
      v["AvgTempExtRaw"] = String(rAvgTempExt, 4);
      v["TempExt"] = sTempExt;
      v["TempExtMin"] = sTempExtMin;
      v["TempExtMax"] = sTempExtMax;
      v["HeureMin"] = sTempExtTimeMin;
      v["HeureMax"] = sTempExtTimeMax;
      v["TempSalon"] = sTempSal;
      v["TempPlancher"] = sTempPlancher;
      v["ConsPlancher"] = sConsPlancher;
      v["TempECS"] = sTempECS;
      v["TempRadiat"] = sTempRadiat;
      v["DebitRadiat"] = sDebitRadiat;
      v["Courant"] = sCourant;
      v["ConsoInstEau"] = String(iConsoEauInst);
      v["ConsoInstElec"] = String(iConsoElecInst/1000.0, 3);
      v["ConsoInstGaz"] = sConsoGazInst;
      v["ConsoJEau"] = String(iConsoEauJ);
      v["ConsoJElec"] = String(iConsoElecJ);
      v["ConsoJGaz"] = sConsoGazJ;
      v["ConsoJ1Eau"] = String(iConsoEauJ1) + " L";
      v["ConsoJ1Elec"] = String(iConsoElecJ1) + " Kwh";
      v["ConsoJ1Gaz"] = sConsoGazJ1;
      v["ConsigneTemp"] = sConsigneTemp;

      // Valeurs brutes pour le graphique selectionnable (ecran Relais): voir GRAPH_MEASUREMENTS
      // dans MainScreen.cpp, ces cles doivent correspondre a celles utilisees la-bas.
      v["TempSalonRaw"] = String(MBresultANA1[0] / 10.0, 4);
      v["TempPlancherRaw"] = String(MBresultANA1[1] / 10.0, 4);
      v["TempECSRaw"] = String(MBresultANA1[2] / 10.0, 4);
      v["TempRadiatRaw"] = String(MBresultANA1[5] / 10.0, 4);

      UpdateLVGLFromModbus(v);

      // ******  DEBUG  ***********
      if (SERDEBUG) {
        Serial.println("iState=" + String(iState));
        // Print holding register values
        Serial.println("Holding Register Values:");
        for (int i = 0; i < NB_REGS; i++) {
          Serial.print("Register ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(MBresultANA1[i]);
        }
        Serial.println(" ");
        Serial.println("Coils Values:");
        Serial.println(" ");
        Serial.println("Animations Values:");
        for (int i = 0; i < NB_REGS_ANIM; i++) {
          Serial.print("Registers ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(MBresultANIM1[i]);
        }
      }

      g_case20DurationUs = micros() - t0diag; // Diagnostic temporaire

      LastModbusRequest = millis();
      iState = 30;
    }
    break;

    case 30:
    {     // Wait MB_READ_INTERVAL sec OR process Write modbus
      if (mbWriteCoilAddress) {
        iState = 90;
      } else if (mbWriteHregAddress) {
        iState = 95;
      }
      if (millis() - LastModbusRequest >= MB_READ_INTERVAL) {
        LastModbusRequest = millis();
        iState = 0;  // On recommence
        if (SERDEBUG) Serial.println(String(iState));
        // On garde la connexion TCP ouverte (pas de disconnect ici): un connect() refait a
        // chaque cycle bloque brievement loop() (donc l'animation LVGL) toutes les secondes.
        // case 99 se charge deja de reconnecter si la connexion tombe reellement.
      }
    }
    break;

    case 90:
    {     // Ecriture Modbus
      if (SERDEBUG) Serial.println(String(iState));
      mb.writeCoil(MBremote, mbWriteCoilAddress, 1, nullptr, 1);
      mbWriteCoilAddress = 0;
      iState = 0;
    }
    break;

    case 95:
    {     // Ecriture Modbus: consigne de temperature (%MW630, voir applyConsigneTemp())
      if (SERDEBUG) Serial.println(String(iState));
      mb.writeHreg(MBremote, mbWriteHregAddress, mbWriteHregValue, nullptr, 1);
      mbWriteHregAddress = 0;
      iState = 0;
    }
    break;

    case 99:
    {
      if (SERDEBUG) Serial.println(String(iState));
      // Ne retente une connexion que toutes les 3s (voir TimerModbusReconnect ci-dessus):
      // mb.connect() bloque jusqu'a MODBUSIP_CONNECT_TIMEOUT (voir platformio.ini) si l'IP
      // du PLC est injoignable, il ne faut donc pas l'appeler a chaque passage de la boucle.
      if (TimerModbusReconnect.Q()) {
        mb.connect(MBremote);
        TimerModbusReconnect.Reset();
        iState = 0;
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
}

float fnAverage(float fInput) { /* function fnAverage */
   
  //Perform average on sensor readings
  float average;
  static float total;
  static float fAvgFiFo[NBAVGFIFO];

  // subtract the last reading:
  total = total - fAvgFiFo[iReadIndex];
  // read the sensor:
  fAvgFiFo[iReadIndex] = fInput;
  // add value to total:
  total = total + fAvgFiFo[iReadIndex];
  // handle index
  iReadIndex ++;
  if (iReadIndex >= NBAVGFIFO) {
    iReadIndex = 0;
  }
  if (iStartIndex < NBAVGFIFO) {
    iStartIndex++;
  }

  // calculate the average:
  average = total / NBAVGFIFO;

  if (iStartIndex >= NBAVGFIFO) {
    return average;
  } else {
    return fInput;
  }
}