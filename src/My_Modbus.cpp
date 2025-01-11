/*
    My_Modbus.cpp - ModbusIP 
    HomIS mangement with HomIS PLC
*/

#include "My_Modbus.h"

float fnMin(float fInput);
float fnMax(float fInput); 
float fnAverage(float fInput);

void ReadModbus() {
  unsigned long prevmillis1;
  int iState = 0;

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
        
        float rTempSal = round(MBresult[0] * 100.0 / 10.0)/100.0;
        String sTempSal = "T. Sal: " + String(rTempSal) + " °C";
        float rTmp = (MBresult[8] * 100.0 / 32764.0) - 50.0; // Mise a l'echelle
        float rTempExt = round(rTmp * 100.0)/100.0; // 2 digits 
        String sTempExt = "T. Ext: " + String(rTempExt) + " °C";
        // Calcul la moyenne 
        float rAvgTempExt = round(fnAverage(rTempExt) * 100.0) / 100.0;

        String sTrend = String(" =");
        if (rTempExt > rAvgTempExt) {
          sTrend = String(" /");
        } else if (rTempExt < rAvgTempExt){
          sTrend = String(" \\");
        }
        lv_label_set_text(lblScrolTxt_1, ("   " + sPrintdate + "            Ext. Min          Ext. Max").c_str());
        lv_label_set_text(lblScrolTxt_2, (" " + sTempExt + sTrend + "        " + String(fnMin(rTempExt)) + " °C" + "            " + String(fnMax(rTempExt)) + " °C").c_str());
        lv_label_set_text(lblScrolTxt_3, (" " + sTempSal + "         " + sExtMinTimeStp + "              " + sExtMaxTimeStp).c_str());

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