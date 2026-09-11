/*
    My_Modbus.cpp - ModbusIP 
    HomIS mangement with HomIS PLC
*/

#include "Globals.h"
#include "My_Modbus.h"
#include <ModbusIP_ESP8266.h>

#define NBAVGFIFO 29

ModbusIP mb;

//unsigned long prevmillis1;
unsigned long LastModbusRequest;  // Variable to track the last Modbus request time
//unsigned long TransactMillis1;    // Timeout Transaction

int iState = 0;

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

// Cache pour eviter d'appeler lv_obj_set_style_*_color quand la couleur n'a pas change:
// LVGL invalide l'objet (donc programme un flush) a chaque appel de style, sans comparer
// a la valeur precedente (verifie dans lib/lvgl/src/core/lv_obj_style.c).
struct ColorCache {
  lv_color_t color{};
  bool inited = false;
};

static bool colorChanged(ColorCache &cache, lv_color_t newColor){
  if (!cache.inited || cache.color.red != newColor.red || cache.color.green != newColor.green ||
      cache.color.blue != newColor.blue) {
    cache.color = newColor;
    cache.inited = true;
    return true;
  }
  return false;
}

// Meme principe pour les labels: lv_label_set_text() invalide aussi sans comparer au texte
// deja affiche (verifie dans lib/lvgl/src/widgets/label/lv_label.c).
static bool textChanged(String &cache, const String &newText){
  if (cache != newText) {
    cache = newText;
    return true;
  }
  return false;
}

// Applique l'etat anime d'un relais (bit de MBresultANIM1[0]) a son bool d'etat, sa commande,
// et la couleur du bouton (ecran Relais) + du voyant d'etat (ecran principal), en ne touchant
// le style LVGL que si la couleur a reellement change.
static void updateAnimatedRelay(uint16_t animReg, uint16_t mask, bool &stateFlag, bool &cmdFlag, lv_obj_t *btn,
                                 lv_color_t colorOn, lv_color_t colorOff, lv_obj_t *led,
                                 ColorCache &btnCache, ColorCache &ledCache){
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
  if (colorChanged(btnCache, color)) lv_obj_set_style_bg_color(btn, color, 0);
  if (colorChanged(ledCache, color)) lv_obj_set_style_bg_color(led, color, 0);
}

// Valeurs de l'ecran principal deja mises en forme (registres Modbus -> String/float),
// calculees dans case 20 et transmises telles quelles a UpdateLVGLFromModbus().
struct ModbusDisplayValues {
  float rTempExt;
  float rAvgTempExt;
  String sTempExt;
  String sTempExtMin;
  String sTempExtMax;
  String sTempExtTimeMin;
  String sTempExtTimeMax;
  String sTempSal;
  String sTempPlancher;
  String sConsPlancher;
  String sTempECS;
  String sTempRadiat;
  String sDebitRadiat;
  String sCourant;
  String sConsoInstEau;
  String sConsoInstElec;
  String sConsoInstGaz;
  String sConsoJEau;
  String sConsoJElec;
  String sConsoJGaz;
  String sConsoJ1Eau;
  String sConsoJ1Elec;
  String sConsoJ1Gaz;
};

// MAJ des labels/couleurs LVGL de l'ecran principal + des voyants relais, a partir des
// valeurs deja mises en forme par case 20. Isolee hors de MainModbus() pour ne pas
// encombrer la machine a etats. Chaque MAJ est filtree via textChanged()/colorChanged():
// LVGL invalide l'objet (donc programme un flush) meme si la valeur ne change pas
// (verifie dans lib/lvgl/src/widgets/label/lv_label.c et lib/lvgl/src/core/lv_obj_style.c),
// ce qui bloquait ~500ms sur ce panneau ST7701 (buffer plein-ecran unique en PSRAM) quand
// tous les labels etaient invalides en une fois.
static void UpdateLVGLFromModbus(const ModbusDisplayValues &v) {
  // --- Temp exterieure: couleur selon seuils + label ---
  static ColorCache cacheColorTempExt;
  static String cacheTempExt, cacheDate;
  lv_color_t colorTempExt = lv_color_hex(0xC2ED34);
  if (v.rTempExt > 25.0) colorTempExt = lv_color_hex(0xFF7D00);
  if (v.rTempExt > 32.0) colorTempExt = lv_color_hex(0xFB2626);
  if (colorChanged(cacheColorTempExt, colorTempExt)) {
    lv_obj_set_style_text_color(ui_LblTempExt, colorTempExt, LV_PART_MAIN | LV_STATE_DEFAULT);
  }
  if (textChanged(cacheTempExt, v.sTempExt)) lv_label_set_text(ui_LblTempExt, v.sTempExt.c_str());
  if (textChanged(cacheDate, String(sDateDDMMYYYY))) lv_label_set_text(ui_LblDate, sDateDDMMYYYY);

  // --- Couleur + labels Min/Max selon tendance de la Temp ext. ---
  static ColorCache cacheColorMin, cacheColorMax;
  lv_color_t colorMin = lv_color_hex(0x00FFFF);
  lv_color_t colorMax = lv_color_hex(0x00FFFF);
  if (v.rTempExt > v.rAvgTempExt) {
    colorMax = lv_color_hex(0xFF7D00);
  } else if (v.rTempExt < v.rAvgTempExt) {
    colorMin = lv_color_hex(0xFF7D00);
  }
  if (colorChanged(cacheColorMin, colorMin)) lv_obj_set_style_text_color(ui_LblTempMin, colorMin, LV_PART_MAIN | LV_STATE_DEFAULT);
  if (colorChanged(cacheColorMax, colorMax)) lv_obj_set_style_text_color(ui_LblTempMax, colorMax, LV_PART_MAIN | LV_STATE_DEFAULT);

  static String cacheTempMin, cacheTempMax, cacheHeureMin, cacheHeureMax;
  if (textChanged(cacheTempMin, v.sTempExtMin)) lv_label_set_text(ui_LblTempMin, v.sTempExtMin.c_str());
  if (textChanged(cacheTempMax, v.sTempExtMax)) lv_label_set_text(ui_LblTempMax, v.sTempExtMax.c_str());
  if (textChanged(cacheHeureMin, v.sTempExtTimeMin)) lv_label_set_text(ui_LblHeureMin, v.sTempExtTimeMin.c_str());
  if (textChanged(cacheHeureMax, v.sTempExtTimeMax)) lv_label_set_text(ui_LblHeureMax, v.sTempExtTimeMax.c_str());

  static String cacheTempSalon;
  if (textChanged(cacheTempSalon, v.sTempSal)) lv_label_set_text(ui_LblTempSalon, v.sTempSal.c_str());

  // --- Plancher chauffant: temperature + consigne ---
  static String cachePlancher, cacheConsPlancher;
  if (textChanged(cachePlancher, v.sTempPlancher)) lv_label_set_text(ui_LblValPlancher, v.sTempPlancher.c_str());
  if (textChanged(cacheConsPlancher, v.sConsPlancher)) lv_label_set_text(ui_LblValConsPlancher, v.sConsPlancher.c_str());

  // --- ECS / Radiateur / Courant ---
  static String cacheECS, cacheRadiat, cacheDebitRadiat, cacheCourant;
  if (textChanged(cacheECS, v.sTempECS)) lv_label_set_text(ui_LblValECS, v.sTempECS.c_str());
  if (textChanged(cacheRadiat, v.sTempRadiat)) lv_label_set_text(ui_LblValRadiat, v.sTempRadiat.c_str());
  if (textChanged(cacheDebitRadiat, v.sDebitRadiat)) lv_label_set_text(ui_LblValDebitRadit, v.sDebitRadiat.c_str());
  if (textChanged(cacheCourant, v.sCourant)) lv_label_set_text(ui_LblValCourant, v.sCourant.c_str());

  // --- Consommations instantanees Eau/Elec/Gaz ---
  static String cacheConsoInstEau, cacheConsoInstElec, cacheConsoInstGaz;
  if (textChanged(cacheConsoInstEau, v.sConsoInstEau)) lv_label_set_text(ui_LblValConsoInstEau, v.sConsoInstEau.c_str());
  if (textChanged(cacheConsoInstElec, v.sConsoInstElec)) lv_label_set_text(ui_LblValConsoInstElec, v.sConsoInstElec.c_str());
  if (textChanged(cacheConsoInstGaz, v.sConsoInstGaz)) lv_label_set_text(ui_LblValConsoInstGaz, v.sConsoInstGaz.c_str());

  // --- Consommations Eau/Elec/Gaz du jour et de la veille (J-1) ---
  static String cacheConsoJEau, cacheConsoJElec, cacheConsoJGaz, cacheConsoJ1Eau, cacheConsoJ1Elec, cacheConsoJ1Gaz;
  if (textChanged(cacheConsoJEau, v.sConsoJEau)) lv_label_set_text(ui_LblValConsoJEau, v.sConsoJEau.c_str());
  if (textChanged(cacheConsoJElec, v.sConsoJElec)) lv_label_set_text(ui_LblValConsoJElec, v.sConsoJElec.c_str());
  if (textChanged(cacheConsoJGaz, v.sConsoJGaz)) lv_label_set_text(ui_LblValConsoJGaz, v.sConsoJGaz.c_str());
  if (textChanged(cacheConsoJ1Eau, v.sConsoJ1Eau)) lv_label_set_text(ui_LblValConsoJ1Eau, v.sConsoJ1Eau.c_str());
  if (textChanged(cacheConsoJ1Elec, v.sConsoJ1Elec)) lv_label_set_text(ui_LblValConsoJ1Elec, v.sConsoJ1Elec.c_str());
  if (textChanged(cacheConsoJ1Gaz, v.sConsoJ1Gaz)) lv_label_set_text(ui_LblValConsoJ1Gaz, v.sConsoJ1Gaz.c_str());

  // --- Voyants relais (ecran Relais + ecran principal) ---
  // Traitement animation des BPs sur retour MBus (bouton ecran Relais + voyant ecran principal)
  static ColorCache cacheBtnChaud, cacheLedChaud;
  updateAnimatedRelay(MBresultANIM1[0], MASK_CHAUD, bChaudiere, bCdeRelaisR1, btnR1Chaudiere,
                       lv_color_make( 0, 160, 60 ), lv_color_make( 100, 100, 100 ), ledChaud,
                       cacheBtnChaud, cacheLedChaud);

  static ColorCache cacheBtnBoost, cacheLedBoost;
  lv_color_t colorBoost;
  if (MBresultANIM1[0] & MASK_BOOST_ANIM) {
    bBoostChaud = 1;
    bCdeRelaisR2 = 1;
    // Couleur Boost differenciee sur passage d'eau reel
    colorBoost = (v.sDebitRadiat.toFloat() > 0.1) ? lv_color_make( 210, 16, 52 ) : lv_color_make( 255, 130, 0 );
  } else {
    bBoostChaud = 0;
    bCdeRelaisR2 = 0;
    colorBoost = lv_color_make( 110, 110, 110 );
  }
  if (colorChanged(cacheBtnBoost, colorBoost)) lv_obj_set_style_bg_color(btnR2BoostCh, colorBoost, 0 );
  if (colorChanged(cacheLedBoost, colorBoost)) lv_obj_set_style_bg_color(ledBoost, colorBoost, 0 );

  static ColorCache cacheBtnRadiat, cacheLedRadiat;
  updateAnimatedRelay(MBresultANIM1[0], MASK_PPERADIAT, bPpeRadiat, bCdeRelaisR3, btnR3PpeRadiateur,
                       lv_color_make( 0, 160, 60 ), lv_color_make( 120, 120, 120 ), ledRadiat,
                       cacheBtnRadiat, cacheLedRadiat);

  static ColorCache cacheBtnPlancher, cacheLedPlancher;
  updateAnimatedRelay(MBresultANIM1[0], MASK_PPEPLANCHER, bPpePlancher, bRelay_4, btnPpePlancher,
                       lv_color_make( 0, 160, 60 ), lv_color_make( 130, 130, 130 ), ledPlancher,
                       cacheBtnPlancher, cacheLedPlancher);

  static ColorCache cacheBtnArriveeEau, cacheLedArriveeEau;
  updateAnimatedRelay(MBresultANIM1[0], MASK_ARRIVEEAU, bArriveeEau, bRelay_5, btnArriveeEau,
                       lv_color_make( 40, 112, 226 ), lv_color_make( 130, 130, 130 ), ledArriveeEau,
                       cacheBtnArriveeEau, cacheLedArriveeEau);

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
      v.rTempExt = rTempExt;
      v.rAvgTempExt = rAvgTempExt;
      v.sTempExt = sTempExt;
      v.sTempExtMin = sTempExtMin;
      v.sTempExtMax = sTempExtMax;
      v.sTempExtTimeMin = sTempExtTimeMin;
      v.sTempExtTimeMax = sTempExtTimeMax;
      v.sTempSal = sTempSal;
      v.sTempPlancher = sTempPlancher;
      v.sConsPlancher = sConsPlancher;
      v.sTempECS = sTempECS;
      v.sTempRadiat = sTempRadiat;
      v.sDebitRadiat = sDebitRadiat;
      v.sCourant = sCourant;
      v.sConsoInstEau = String(iConsoEauInst);
      v.sConsoInstElec = String(iConsoElecInst/1000.0, 3);
      v.sConsoInstGaz = sConsoGazInst;
      v.sConsoJEau = String(iConsoEauJ);
      v.sConsoJElec = String(iConsoElecJ);
      v.sConsoJGaz = sConsoGazJ;
      v.sConsoJ1Eau = String(iConsoEauJ1) + " L";
      v.sConsoJ1Elec = String(iConsoElecJ1) + " Kwh";
      v.sConsoJ1Gaz = sConsoGazJ1;

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

    case 99:
    {     
      if (SERDEBUG) Serial.println(String(iState));
      mb.connect(MBremote);
      iState = 0;
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