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

// Applique l'etat anime d'un relais (bit de MBresultANIM1[0]) a son bool d'etat, sa commande,
// la couleur du bouton (ecran Relais) et, si fourni, celle du voyant d'etat (ecran principal).
static void updateAnimatedRelay(uint16_t animReg, uint16_t mask, bool &stateFlag, bool &cmdFlag, lv_obj_t *btn, lv_color_t colorOn, lv_color_t colorOff, lv_obj_t *led = nullptr){
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
  lv_obj_set_style_bg_color(btn, color, 0);
  if (led) lv_obj_set_style_bg_color(led, color, 0);
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

    // Lecture des valeurs dans le buffer MB et mise ne forme
    case 20:
    {
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

      // Changement de couleur des valeurs Min ou Max selon tendance de la Temp ext.
      lv_obj_set_style_text_color(ui_LblTempMin, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
      lv_obj_set_style_text_color(ui_LblTempMax, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
      if (rTempExt > rAvgTempExt) {
        lv_obj_set_style_text_color(ui_LblTempMax, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
      } else if (rTempExt < rAvgTempExt){
        lv_obj_set_style_text_color(ui_LblTempMin, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
      }

      // Changement de couleur de la Température Exterieure selon seuils
      lv_obj_set_style_text_color(ui_LblTempExt, lv_color_hex(0xC2ED34), LV_PART_MAIN | LV_STATE_DEFAULT);
      if (rTempExt > 25.0) {
        lv_obj_set_style_text_color(ui_LblTempExt, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
      }
      if (rTempExt > 32.0) {
        lv_obj_set_style_text_color(ui_LblTempExt, lv_color_hex(0xFB2626), LV_PART_MAIN | LV_STATE_DEFAULT);
      }

      lv_label_set_text(ui_LblDate, sDateDDMMYYYY);
      lv_label_set_text(ui_LblTempExt, sTempExt.c_str());

      //Temp exterieure Mini
      lv_label_set_text(ui_LblTempMin, sTempExtMin.c_str());
      //Temp exterieure Maxi
      lv_label_set_text(ui_LblTempMax, sTempExtMax.c_str());

      // Affiche la moyenne exterieure a la place du salon pour essai
      //sTempSal = "Avg: " + String(rAvgTempExt, 4);

      lv_label_set_text(ui_LblTempSalon, sTempSal.c_str());

      lv_label_set_text(ui_LblHeureMin, sTempExtTimeMin.c_str());
      lv_label_set_text(ui_LblHeureMax, sTempExtTimeMax.c_str());

      lv_label_set_text(ui_LblValPlancher, sTempPlancher.c_str());
      lv_label_set_text(ui_LblValConsPlancher, sConsPlancher.c_str());
      lv_label_set_text(ui_LblValECS, sTempECS.c_str());
      lv_label_set_text(ui_LblValRadiat, sTempRadiat.c_str());
      lv_label_set_text(ui_LblValDebitRadit, sDebitRadiat.c_str());
      lv_label_set_text(ui_LblValCourant, sCourant.c_str());

      lv_label_set_text(ui_LblValConsoInstEau, String(iConsoEauInst).c_str());
      lv_label_set_text(ui_LblValConsoInstElec, String(iConsoElecInst/1000.0, 3).c_str());
      lv_label_set_text(ui_LblValConsoInstGaz, sConsoGazInst.c_str());
      lv_label_set_text(ui_LblValConsoJEau, String(iConsoEauJ).c_str());
      lv_label_set_text(ui_LblValConsoJElec, String(iConsoElecJ).c_str());
      lv_label_set_text(ui_LblValConsoJGaz, sConsoGazJ.c_str());
      lv_label_set_text(ui_LblValConsoJ1Eau, (String(iConsoEauJ1) + " L").c_str());
      lv_label_set_text(ui_LblValConsoJ1Elec, (String(iConsoElecJ1) + " Kwh").c_str());
      lv_label_set_text(ui_LblValConsoJ1Gaz, sConsoGazJ1.c_str());

      // Traitement animation des BPs sur retour MBus (bouton ecran Relais + voyant ecran principal)
      updateAnimatedRelay(MBresultANIM1[0], MASK_CHAUD, bChaudiere, bCdeRelaisR1, btnR1Chaudiere,
                           lv_color_make( 0, 160, 60 ), lv_color_make( 100, 100, 100 ), ledChaud);

      if (MBresultANIM1[0] & MASK_BOOST_ANIM) {
        bBoostChaud = 1;
        bCdeRelaisR2 = 1;
        // Couleur Boost differenciee sur passage d'eau reel
        lv_color_t colorBoost = (sDebitRadiat.toFloat() > 0.1) ? lv_color_make( 210, 16, 52 ) : lv_color_make( 255, 130, 0 );
        lv_obj_set_style_bg_color(btnR2BoostCh, colorBoost, 0 );
        lv_obj_set_style_bg_color(ledBoost, colorBoost, 0 );
      } else {
        bBoostChaud = 0;
        bCdeRelaisR2 = 0;
        lv_obj_set_style_bg_color(btnR2BoostCh, lv_color_make( 110, 110, 110 ), 0 );
        lv_obj_set_style_bg_color(ledBoost, lv_color_make( 110, 110, 110 ), 0 );
      }

      updateAnimatedRelay(MBresultANIM1[0], MASK_PPERADIAT, bPpeRadiat, bCdeRelaisR3, btnR3PpeRadiateur,
                           lv_color_make( 0, 160, 60 ), lv_color_make( 120, 120, 120 ), ledRadiat);

      updateAnimatedRelay(MBresultANIM1[0], MASK_PPEPLANCHER, bPpePlancher, bRelay_4, btnPpePlancher,
                           lv_color_make( 0, 160, 60 ), lv_color_make( 130, 130, 130 ), ledPlancher);

      updateAnimatedRelay(MBresultANIM1[0], MASK_ARRIVEEAU, bArriveeEau, bRelay_5, btnArriveeEau,
                           lv_color_make( 40, 112, 226 ), lv_color_make( 130, 130, 130 ), ledArriveeEau);

      // Traitement Affichage des alarmes.
      DisplayAlarms(MBresultANIM1[3]); // Registre des alarmes MD230  HR 412748

      // ******  DEBUG  ***********
      if (SERDEBUG) { 
        Serial.print("Avg T.Ext. = ");
        Serial.println(String(rAvgTempExt));
      }
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
        mb.disconnect(MBremote);
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