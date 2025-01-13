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
String sExtMaxTimeStp;
String sExtMinTimeStp;
//const int iAvgMaxFifo;
const int iAvgMaxFifo = 10;
float fAvgFiFo[iAvgMaxFifo];
int iReadIndex;
int iStartIndex;
uint16_t MBresultANA1[NB_REGS];
uint16_t MBresultANIM1[NB_REGS_ANI];
bool MBresultCL1[NB_OUTPUT1];

uint8_t MBTransactionANA1; // Transcation for HRs Range 1
uint8_t MBTransactionANIM1; // Transcation for HRs Range 1
uint8_t MBTransactionCL1; // Transcation for Coils Range 1
String sTrend;

lv_obj_t *lblScrolTxt_1;
lv_obj_t *lblScrolTxt_2;
lv_obj_t *lblScrolTxt_3;

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
        mb.task();
        delay(5);
        // Read holding registers from Modbus Slave
        MBTransactionANA1 = mb.readHreg(MBremote, START_REG, MBresultANA1, NB_REGS, nullptr, 1);
        MBTransactionANIM1 = mb.readHreg(MBremote, START_REG_ANI, MBresultANIM1, NB_REGS_ANI, nullptr, 1);
        //MBTransactionCL1 = mb.readCoil(MBremote, START_OUTPUT1, MBresultCL1, NB_OUTPUT1, nullptr, 1);    
        
        if (SERDEBUG) Serial.println("iState="+String(iState));
        if (SERDEBUG) Serial.println("MBTransact.="+String(MBTransactionANA1));
        if (SERDEBUG) Serial.println("MBTransact.="+String(MBTransactionANIM1));
        // Wait for the transaction to complete
        if (mb.isTransaction(MBTransactionANA1) || mb.isTransaction(MBTransactionCL1) || mb.isTransaction(MBTransactionANIM1)) {
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
          //if (SERDEBUG) Serial.println("MBTransactCoils="+String(MBTransactionANA1));
        }
      } 
      break;

      case 10:
      { //  ***********    Etape SPARE    **************
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
        
        float rTempSal = round(MBresultANA1[0] * 100.0 / 10.0)/100.0;
        String sTempSal = "T. Sal: " + String(rTempSal) + " °C";
        float rTmp = (MBresultANA1[8] * 100.0 / 32764.0) - 50.0; // Mise a l'echelle
        float rTempExt = round(rTmp * 100.0)/100.0; // 2 digits 
        String sTempExt = "T. Ext: " + String(rTempExt) + " °C";
        // Calcul la moyenne 
        float rAvgTempExt = round(fnAverage(rTempExt) * 100.0) / 100.0;

        sTrend = String(" =");
        if (rTempExt > rAvgTempExt) {
          sTrend = String(" /");
        } else if (rTempExt < rAvgTempExt){
          sTrend = String(" \\");
        }
        lv_label_set_text(lblScrolTxt_1, ("   " + sPrintdate + "            Ext. Min          Ext. Max").c_str());
        lv_label_set_text(lblScrolTxt_2, (" " + sTempExt + sTrend + "        " + String(fnMin(rTempExt)) + " °C" + "            " + String(fnMax(rTempExt)) + " °C").c_str());
        lv_label_set_text(lblScrolTxt_3, (" " + sTempSal + "         " + sExtMinTimeStp + "              " + sExtMaxTimeStp).c_str());

        // Traitement animation des BPs sur retour MBus
        if (MBresultANIM1[0] & MASK_PPERADIAT) {
          lv_obj_set_style_bg_color(btnR3PpeRadiateur, lv_color_make( 0, 160, 60  ), 0 );
        } else {
          lv_obj_set_style_bg_color(btnR3PpeRadiateur, lv_color_make( 100, 100, 100 ), 0 );
        }
        
        if (MBresultANIM1[0] & MASK_PPEPLANCHER) {
          lv_obj_set_style_bg_color(btnPpePlancher, lv_color_make( 0, 160, 60  ), 0 );
        } else {
          lv_obj_set_style_bg_color(btnPpePlancher, lv_color_make( 100, 100, 100 ), 0 );
        }
          
        if (MBresultANIM1[0] & MASK_CHAUD) {
          lv_obj_set_style_bg_color(btnR1Chaudiere, lv_color_make( 0, 160, 60  ), 0 );
        } else {
          lv_obj_set_style_bg_color(btnR1Chaudiere, lv_color_make( 100, 100, 100 ), 0 );
        }

        if (MBresultANIM1[0] & MASK_BOOST) {
          lv_obj_set_style_bg_color(btnR2BoostCh, lv_color_make( 0, 160, 60  ), 0 );
        } else {
          lv_obj_set_style_bg_color(btnR2BoostCh, lv_color_make( 100, 100, 100 ), 0 );
        }

        // ******  DEBUG  ***********
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
          for (int i = 0; i < NB_REGS; i++) {
            Serial.print("Register ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(MBresultANA1[i]);
          }
          Serial.println(" ");
          Serial.println("Coils Values:");
          for (int i = 0; i < NB_OUTPUT1; i++) {
            Serial.print("Coil ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(MBresultCL1[i]);
          }
          Serial.println(" ");
          Serial.println("Animations Values:");
          for (int i = 0; i < NB_REGS_ANI; i++) {
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