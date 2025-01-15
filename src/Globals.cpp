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