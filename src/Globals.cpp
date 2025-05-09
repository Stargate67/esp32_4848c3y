#include "Globals.h"

Tempos::Tempos(unsigned long Preset) { // Constructeur initialise la tempo
    this->uPreset = Preset;
    this->uInitTime = millis();
    if (SERDEBUG) Serial.println("Tempo Construct. InitTime= " + String(this->uInitTime));
};

bool Tempos::Q() {
    if (millis() > this->uInitTime + this->uPreset){
        return true;
    } else {
        return false;
    }
};

bool Tempos::Q(unsigned long Preset) {
    if (millis() > this->uInitTime + Preset){
        return true;
    } else {
        return false;
    }
};

bool Tempos::Preset(unsigned long Preset) {
    this->uInitTime = Preset;
    return true;
};

bool Tempos::Reset() {
    this->uInitTime = millis();
    return true;
};

Tempos Timer1(2000);
Tempos Timer2(2000);
Tempos Timer3(2000);
Tempos Timer4(2000);
Tempos Timer5(2000);
Tempos Timer6(2000);

Tempos TpoMesAcquite(2000);  // tempo pour Raz Message acquité

uint16_t MemoHMBAlarme;

void Relays(){

    if (bCdeRelaisR1){
        Timer2.Reset();
        if (Timer1.Q() && !bChaudiere) {
            digitalWrite(RELAY_1, HIGH);
        }
    } else {
        Timer1.Reset();
        if (Timer2.Q() && bChaudiere) {
            digitalWrite(RELAY_1, LOW);
        }
    }

    if (bCdeRelaisR2){
         Timer4.Reset();
         //if (Timer3.Q(2000) && !bBoostChaud) {
        if (Timer3.Q()) {
            digitalWrite(RELAY_2, HIGH);
        }
    } else {
         Timer3.Reset();
        //if (Timer4.Q() && bBoostChaud) {
        if (Timer4.Q()) {
            digitalWrite(RELAY_2, LOW);
        }
    }

    if (bCdeRelaisR3){
        Timer6.Reset();
        if (Timer5.Q() && !bPpeRadiat) {
            digitalWrite(RELAY_3, HIGH);
        }
    } else {
        Timer5.Reset();
        if (Timer6.Q() && bPpeRadiat) {
            digitalWrite(RELAY_3, LOW);
        }
    }

    lv_label_set_text_fmt(lblBtnR1small, "R1=%d", digitalRead(RELAY_1));
    lv_label_set_text_fmt(lblBtnR2small, "R2=%d", digitalRead(RELAY_2));
    lv_label_set_text_fmt(lblBtnR3small, "R3=%d", digitalRead(RELAY_3));
}


void DisplayAlarms(const uint16_t MBAlarm){

    static String sMessage;
    static uint16_t memoXorAlarm;

    /*
    Alarmes PLC 

        '/!\ 0 ALARME CHAUDIERE'
        ,'/!\ 1 PROBLEME REGULATION PLANCHER'
        ,'/!\ 2 PORTE DE GARAGE OUVERTE'
        ,'/!\ 3 DEFAUT PRESSION EAU'
        ,'/!\ 4 TEST MESSAGE'
        ,'/!\ 5 ALARME ModbusTcp'
        ,'/!\ 6 ALARME ModbusRtu'
        ,'/!\ 7 Courant > 4A'
        ,'/!\ 8 Fuite d eau probable'
        ,'/!\ 9 Temp. Ext > Temp. Salon'
        ,'/!\ 10 Temp. Ext < Temp. Salon'
    */

    if ( MBAlarm ) {
        if ( MBAlarm != MemoHMBAlarme) { // Une alarme se rajoute
            //String sPrefix = "\n/!\\ " + sClockHHMM + " "; // A activer pour scroll verticale
            String sPrefix = "/!\\ " + sClockHHMM + " ";

            memoXorAlarm = MBAlarm ^ MemoHMBAlarme; //Detection nouvelle alarme

            sMessage = "";
            if (memoXorAlarm & 0b1) { // Bit0
                sMessage = (sMessage + sPrefix + "0 ALARME CHAUDIERE ");
            }
            if (memoXorAlarm & 0b10) { // Bit1
                sMessage = (sMessage + sPrefix + "1 PROBLEME REGULATION PLANCHER ");
            }
            if (memoXorAlarm & 0b100) { // Bit2
                sMessage = (sMessage + sPrefix + "2 PORTE DE GARAGE 1 OUVERTE ");
            }
            if (memoXorAlarm & 0b1000) { // Bit3
                sMessage = (sMessage + sPrefix + "3 DEFAUT PRESSION EAU CHAUFFAGE ");
            }
            if (memoXorAlarm & 0b10000) { // Bit4
                sMessage = (sMessage + sPrefix + "4 TEST MESSAGE ");
            }
            if (memoXorAlarm & 0b100000) { // Bit5
                sMessage = (sMessage + sPrefix + "5 ALARME ModbusTcp ");
            }
            if (memoXorAlarm & 0b1000000) { // Bit6
                sMessage = (sMessage + sPrefix + "6 ALARME ModbusRtu ");
            }
            if (memoXorAlarm & 0b10000000) { // Bit7
                sMessage = (sMessage + sPrefix + "7 Courant > 4A ");
            }
            if (memoXorAlarm & 0b100000000) { // Bit8
                sMessage = (sMessage + sPrefix + "8 Fuite d eau probable ");
            }
            if (memoXorAlarm & 0b1000000000) { // Bit9
                sMessage = (sMessage + sPrefix + "9 Temp. Ext > Temp. Fermer les volets! ");
            }
            if (memoXorAlarm & 0b10000000000) { // Bit10
                sMessage = (sMessage + sPrefix + "10 Temp. Ext < Temp. Salon. Ouvrir les fenetres! ");
            }
            if (memoXorAlarm & 0b100000000000) { // Bit11
                sMessage = (sMessage + sPrefix + "Alarme Bit 11 ");
            }
            if (memoXorAlarm & 0b1000000000000) { // Bit12
                sMessage = (sMessage + sPrefix + "Alarme Bit 12 ");
            }
            if (memoXorAlarm & 0b10000000000000) { // Bit13
                sMessage = (sMessage + sPrefix + "Alarme Bit 13 ");
            }
            if (memoXorAlarm & 0b100000000000000) { // Bit14
                sMessage = (sMessage + sPrefix + "Alarme Bit 14 ");
            }
            if (memoXorAlarm & 0b1000000000000000) { // Bit15
                sMessage = (sMessage + sPrefix + "Alarme Bit 15 ");
            }

            lv_label_set_text(AlarmLabel, sMessage.c_str());
            MemoHMBAlarme = MBAlarm;
        }
        if (bAcquitAlarme){
            memoXorAlarm = 0;
            MemoHMBAlarme = 0;
            sMessage = "";
        }
    }
}

void AcquitMesAlarme(){
    static bool bBitmemo;
    if (bAcquitAlarme){
        if (!bBitmemo) { 
            TpoMesAcquite.Reset();
            bBitmemo = 1;
            lv_label_set_text(AlarmLabel, "         Alarmes Acquitees");
        }
        
        if (TpoMesAcquite.Q()) {
            lv_label_set_text(AlarmLabel, "");
            bAcquitAlarme = 0;
            bBitmemo = 0;
        }
    }
 }