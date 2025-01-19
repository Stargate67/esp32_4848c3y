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

    String sMessage;

    if ( MBAlarm ) {
        if ( MBAlarm != MemoHMBAlarme) {
            MemoHMBAlarme = MBAlarm;

            sMessage = "";
            if (MBAlarm & 0b1) { // Bit0
                sMessage = (sMessage + "...TEST 1 Alarm");
            }
            if (MBAlarm & 0b10) { // Bit1
                sMessage = (sMessage + "...ALARME CHAUDIERE");
            }
            if (MBAlarm & 0b100) { // Bit2
                sMessage = (sMessage + "...DEFAUT PRESSION CIRCUIT EAU");
            }
            if (MBAlarm & 0b1000) { // Bit3
                sMessage = (sMessage + "...DEFAUT PRESSION BASSE CIRCUIT EAU");
            }
            if (MBAlarm & 0b10000) { // Bit4
                sMessage = (sMessage + "...PORTE DE GARAGE 1 OUVERTE");
            }
            if (MBAlarm & 0b100000) { // Bit5
                sMessage = (sMessage + "...PROBLEME REGULATION PLANCHER");
            }
            if (MBAlarm & 0b1000000) { // Bit6
                sMessage = (sMessage + "...ALARME ModbusTcp");
            }
            if (MBAlarm & 0b10000000) { // Bit7
                sMessage = (sMessage + "...ALARME ModbusRtu");
            }
            if (MBAlarm & 0b100000000) { // Bit8
                sMessage = (sMessage + "...Temp Exterieure > Temp Salon. Fermer les volets!");
            }
            if (MBAlarm & 0b1000000000) { // Bit9
                sMessage = (sMessage + "...Temp Exterieure < Temp Salon. Ouvrir les fenêtres!");
            }
            if (MBAlarm & 0b10000000000) { // Bit10
                sMessage = (sMessage + "...Alarme Bit 10");
            }
            if (MBAlarm & 0b100000000000) { // Bit11
                sMessage = (sMessage + "...Alarme Bit 11");
            }
            if (MBAlarm & 0b1000000000000) { // Bit12
                sMessage = (sMessage + "...Alarme Bit 12");
            }
            if (MBAlarm & 0b10000000000000) { // Bit13
                sMessage = (sMessage + "...Alarme Bit 13");
            }
            if (MBAlarm & 0b100000000000000) { // Bit14
                sMessage = (sMessage + "...Alarme Bit 14");
            }
            if (MBAlarm & 0b1000000000000000) { // Bit15
                sMessage = (sMessage + "...Alarme Bit 15");
            }
            lv_label_set_text(AlarmLabel, sMessage.c_str());
        }
    } else {
        lv_label_set_text(AlarmLabel, "...Pas d'alarme...");
    }
}