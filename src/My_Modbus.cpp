/*
    My_Modbus.cpp - ModbusIP 
    HomIS mangement with HomIS PLC
*/

#include "Globals.h"
#include "My_Modbus.h"
#include <ModbusIP_ESP8266.h>

ModbusIP mb;

unsigned long prevmillis1;
unsigned long LastModbusRequest;  // Variable to track the last Modbus request time
unsigned long TransactMillis1;    // Timeout Transaction
int iState = 0;
String sExtMaxTimeStp = "NA:NA";
String sExtMinTimeStp = "NA:NA";
//const int iAvgMaxFifo;
const int iAvgMaxFifo = 10;
float fAvgFiFo[iAvgMaxFifo];
int iReadIndex;
int iStartIndex;
uint16_t MBresultANA1[NB_REGS];
uint16_t MBresultANIM1[NB_REGS_ANIM];
//bool MBresultCL1[NB_OUTPUT1];

lv_obj_t * ui_LblTempExt;
lv_obj_t * ui_LblDate;
lv_obj_t * ui_LblTempSalon;
lv_obj_t * ui_LblTempMin;
lv_obj_t * ui_LblHeureMin;
lv_obj_t * ui_LblTempMax;
lv_obj_t * ui_LblHeureMax;

lv_obj_t * ui_LblValPlancher;
lv_obj_t * ui_LblValECS;
lv_obj_t * ui_LblValRadiat;
lv_obj_t * ui_LblValDebitRadit;
lv_obj_t * ui_LblValCourant;

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

void ReadModbus() {
  
  //mb.task();

  if (mb.isConnected(MBremote)) {  
    //if (SERDEBUG) Serial.println(String(iState));
    switch (iState) {
      case 0:
      {
        TransactMillis1 = millis();
        iState = 5;
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

        //mb.task();
        //delay(5);
        prevmillis1 = millis();
        iState = 10;
        if (SERDEBUG) Serial.println("iState="+String(iState));

/*
        if (SERDEBUG) Serial.println("iState="+String(iState));
        if (SERDEBUG) Serial.println("MBTransact.="+String(MBTransactionANA1));
        if (SERDEBUG) Serial.println("MBTransact.="+String(MBTransactionANIM1));

        // Wait for the transaction to complete
        if (mb.isTransaction(MBTransactionANA1) || mb.isTransaction(MBTransactionANIM1)) {
          mb.task();
          if (millis() >= TransactMillis1 +2000){ // TimeOut Transaction
            lv_label_set_text(AlarmLabel, "Erreur transaction Modbus"); // A faire.. rajouter le code erreur Modbus
            LastModbusRequest = millis();
            iState = 30;
            if (SERDEBUG) Serial.println("iState="+String(iState));
            break;
          }
        } else {
          // Transaction MB OK
          prevmillis1 = millis();
          iState = 20;
          if (SERDEBUG) Serial.println("iState="+String(iState));
          if (SERDEBUG) Serial.println("MBTransactHR="+String(MBTransactionANA1));
          //if (SERDEBUG) Serial.println("MBTransactCoils="+String(MBTransactionCL1));
        }
        */
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
        float rTempSal = round(MBresultANA1[0] * 100.0 / 10.0)/100.0;
        String sTempSal = "Sal:  " + String(rTempSal) + " °C";

        float rTempPlancher;
        rTempPlancher = round(MBresultANA1[1]*1.0)/10.0;
        String sTempPlancher = String(rTempPlancher) + "°C";

        float rTempECS = round(MBresultANA1[2]*1.0)/10.0; // 1 digits 
        String sTempECS = String(rTempECS) + "°C";

        float rTempRadiat = int(MBresultANA1[5])/10.0; // 1 digits 
        float rDebitRadiat = int(MBresultANA1[7])/10.0; // 1 digits 

        float rCourant = round(MBresultANA1[15]/100.0)/10.0; // 1 digits
        if (SERDEBUG) Serial.println("sTempPlancher " + String(rTempPlancher));
        if (SERDEBUG) Serial.println("TempECS " + String(rTempECS));
        if (SERDEBUG) Serial.println("Courant " + String(rCourant));

        int rConsoEauJ = int(MBresultANA1[16]); 
        int rConsoEauJ1 = int(MBresultANA1[17]); 
        int rConsoElecJ = int(MBresultANA1[18]); 
        int rConsoElecJ1 = int(MBresultANA1[19]);

        float rConsoGazJ = round(MBresultANA1[20])/100;
        String sConsoGazJ = String(rConsoGazJ);

        float rConsoGazJ1 = MBresultANA1[21]/100; 
        String sConsoGazJ1 = String(rConsoGazJ1) + "Nm3";

        float rTmp = (MBresultANA1[8] * 100.0 / 32764.0) - 50.0; // Mise a l'echelle
        float rTempExt = round(rTmp * 100.0)/100.0; // 2 digits 
        String sTempExt = String(rTempExt) + " °C";
        // Calcul la moyenne 
        float rAvgTempExt = round(fnAverage(rTempExt) * 100.0) / 100.0;

        // Changement de couleur des valeurs Min ou Max selon tendance de la temp ext.
        lv_obj_set_style_text_color(ui_LblTempMin, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_LblTempMax, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
        if (rTempExt > rAvgTempExt) {
          lv_obj_set_style_text_color(ui_LblTempMax, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
        } else if (rTempExt < rAvgTempExt){
          lv_obj_set_style_text_color(ui_LblTempMin, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }

        // Changement de couleur de la Temp Ext. selon 
        lv_obj_set_style_text_color(ui_LblTempExt, lv_color_hex(0xC2ED34), LV_PART_MAIN | LV_STATE_DEFAULT);
        if (rTempExt > 25.0) {
          lv_obj_set_style_text_color(ui_LblTempExt, lv_color_hex(0xFF7D00), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
        if (rTempExt > 32.0) {
          lv_obj_set_style_text_color(ui_LblTempExt, lv_color_hex(0xFB2626), LV_PART_MAIN | LV_STATE_DEFAULT);
        }

        lv_label_set_text(ui_LblDate, sPrintdate.c_str());
        lv_label_set_text(ui_LblTempExt, sTempExt.c_str());
        lv_label_set_text(ui_LblTempMin, (String(fnMin(rTempExt)) + " °C").c_str());
        lv_label_set_text(ui_LblTempMax, (String(fnMax(rTempExt)) + " °C").c_str());
        lv_label_set_text(ui_LblTempSalon, (sTempSal).c_str());
        lv_label_set_text(ui_LblHeureMin, (sExtMinTimeStp).c_str());
        lv_label_set_text(ui_LblHeureMax, (sExtMaxTimeStp).c_str());

        lv_label_set_text(ui_LblValPlancher, sTempPlancher.c_str());
        lv_label_set_text(ui_LblValECS, sTempECS.c_str());
        lv_label_set_text(ui_LblValRadiat, (String(rTempRadiat) + " °C").c_str());
        lv_label_set_text(ui_LblValDebitRadit, (String(rDebitRadiat) + " l/m").c_str());
        lv_label_set_text(ui_LblValCourant, (String(rCourant) + " A").c_str());

        lv_label_set_text(ui_LblValConsoJEau, String(rConsoEauJ).c_str());
        lv_label_set_text(ui_LblValConsoJElec, String(rConsoElecJ).c_str());
        lv_label_set_text(ui_LblValConsoJGaz, sConsoGazJ.c_str());
        lv_label_set_text(ui_LblValConsoJ1Eau, (String(rConsoEauJ1) + " l").c_str());
        lv_label_set_text(ui_LblValConsoJ1Elec, (String(rConsoElecJ1) + " Kwh").c_str());
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
          lv_obj_set_style_bg_color(btnR2BoostCh, lv_color_make( 0, 160, 0 ), 0 );
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
        LastModbusRequest = millis();
        iState = 30;
        
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
          /*
          for (int i = 0; i < NB_OUTPUT1; i++) {
            Serial.print("Coil ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(MBresultCL1[i]);
          }
          */
          Serial.println(" ");
          Serial.println("Animations Values:");
          for (int i = 0; i < NB_REGS_ANIM; i++) {
            Serial.print("Registers ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(MBresultANIM1[i]);
          }
        }
      }
      break;

      case 30:
      {     // Wait MB_READ_INTERVAL sec
        if (millis() - LastModbusRequest >= MB_READ_INTERVAL) {
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
  total = total - fAvgFiFo[iReadIndex];
  // read the sensor:
  fAvgFiFo[iReadIndex] = fInput;
  // add value to total:
  total = total + fAvgFiFo[iReadIndex];
  // handle index
  iReadIndex ++;
  if (iReadIndex >= iAvgMaxFifo) {
    iReadIndex = 0;
  }
  if (iStartIndex < iAvgMaxFifo) {
    iStartIndex++;
  }

  // calculate the average:
  average = total / iAvgMaxFifo;

  if (iStartIndex >= iAvgMaxFifo) {
    return average;
  } else {
    return fInput;
  }
}