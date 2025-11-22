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

//const int iAvgMaxFifo;
//const int iAvgMaxFifo = 10;
//float fAvgFiFo[iAvgMaxFifo];
int iReadIndex;
int iStartIndex;
uint16_t MBresultANA1[NB_REGS];
uint16_t MBresultANIM1[NB_REGS_ANIM];
//bool MBresultCL1[NB_OUTPUT1];

uint16_t mbWriteCoilAddress;

lv_obj_t * ui_LblTempExt;
lv_obj_t * ui_LblDate;
lv_obj_t * ui_LblTempSalon;
lv_obj_t * ui_LblTempMin;
lv_obj_t * ui_LblHeureMin;
lv_obj_t * ui_LblTempMax;
lv_obj_t * ui_LblHeureMax;

lv_obj_t * ui_LblValPlancher;
lv_obj_t * ui_LblValConsPlancher;
lv_obj_t * ui_LblValECS;
lv_obj_t * ui_LblValRadiat;
lv_obj_t * ui_LblValDebitRadit;
lv_obj_t * ui_LblValCourant;

lv_obj_t * ui_LblValConsoInstEau;
lv_obj_t * ui_LblValConsoInstElec;
lv_obj_t * ui_LblValConsoInstGaz;
lv_obj_t * ui_LblValConsoJEau;
lv_obj_t * ui_LblValConsoJElec;
lv_obj_t * ui_LblValConsoJGaz;
lv_obj_t * ui_LblValConsoJ1Elec;
lv_obj_t * ui_LblValConsoJ1Eau;
lv_obj_t * ui_LblValConsoJ1Gaz;

bool bChaudiere;    // Modbus Etat Marche Chaudière
bool bBoostChaud;   // Modbus Etat Marche Boost Chaudière
bool bPpeRadiat;    // Modbus Etat Marche pompe Radiateur
bool bPpePlancher;  // Modbus Etat Marche pompe Plancher

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
      //uint16_t MBTransactionANA1 = mb.readHreg(MBremote, START_REG, MBresultANA1, NB_REGS, nullptr, 1);
      //uint16_t MBTransactionANIM1 = mb.readHreg(MBremote, START_REG_ANIM, MBresultANIM1, NB_REGS_ANIM, nullptr, 1);

      //uint16_t MBTransactionCL1 = mb.readCoil(MBremote, START_OUTPUT1, MBresultCL1, NB_OUTPUT1, nullptr, 1);

      mb.readHreg(MBremote, START_REG, MBresultANA1, NB_REGS, nullptr, 1);
      mb.readHreg(MBremote, START_REG_ANIM, MBresultANIM1, NB_REGS_ANIM, nullptr, 1);

      //prevmillis1 = millis();
      iState = 10;
      if (SERDEBUG) Serial.println("iState="+String(iState));
    } 
    break;

    case 10:
    { //  ***********    Etape 10    **************
      // Wait for the transaction to complete
      //if (millis() >= prevmillis1 + 50){ //Process MB client request each second
        //prevmillis1 = millis();
        iState = 20;
        if (SERDEBUG) Serial.println("iState="+String(iState));
      //}
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

      // Changement de couleur de la Temp Ext. selon seuils
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

      // Traitement animation des BPs sur retour MBus
      if (MBresultANIM1[0] & MASK_CHAUD) {
        bChaudiere=1;
        bCdeRelaisR1=1;
        lv_obj_set_style_bg_color(btnR1Chaudiere, lv_color_make( 0, 160, 60 ), 0 );
      } else {
        bChaudiere=0;
        bCdeRelaisR1=0;
        lv_obj_set_style_bg_color(btnR1Chaudiere, lv_color_make( 100, 100, 100 ), 0 );
      }

      if (MBresultANIM1[0] & MASK_BOOST_ANIM) {
        bBoostChaud = 1;
        bCdeRelaisR2 = 1;
        lv_obj_set_style_bg_color(btnR2BoostCh, lv_color_make( 255, 130, 0 ), 0 );
      } else {
        bBoostChaud = 0;
        bCdeRelaisR2 = 0;
        lv_obj_set_style_bg_color(btnR2BoostCh, lv_color_make( 110, 110, 110 ), 0 );
      }

      if (MBresultANIM1[0] & MASK_PPERADIAT) {
        bPpeRadiat = 1;
        bCdeRelaisR3 = 1;
        lv_obj_set_style_bg_color(btnR3PpeRadiateur, lv_color_make( 0, 160, 60 ), 0 );
      } else {
        bPpeRadiat = 0;
        bCdeRelaisR3 = 0;
        lv_obj_set_style_bg_color(btnR3PpeRadiateur, lv_color_make( 120, 120, 120 ), 0 );
      }
      
      if (MBresultANIM1[0] & MASK_PPEPLANCHER) {
        bPpePlancher = 1;
        bRelay_4 = 1;
        lv_obj_set_style_bg_color(btnPpePlancher, lv_color_make( 0, 160, 60 ), 0 );
      } else {
        bPpePlancher = 0;
        bRelay_4 = 0;
        lv_obj_set_style_bg_color(btnPpePlancher, lv_color_make( 130, 130, 130 ), 0 );
      }
      
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

float fnMax(float fInput) { /* function fnMax */
  //Perform Max Value 
  static float fMaxValue = fInput;

  if (fInput > fMaxValue || (strcmp(sClockHHMM, sClock0000)==0)) { 

    fMaxValue = fInput;
    sExtMaxTimeStp = sClockHHMM;
  }
  return fMaxValue;
}

float fnMin(float fInput) { /* function fnMax */
  //Perform Max Value 
  static float fMinValue = fInput;

  if (fInput < fMinValue || (strcmp(sClockHHMM, sClock0000)==0)) { 
    fMinValue = fInput;
    sExtMinTimeStp = sClockHHMM;
  }
  return fMinValue;
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