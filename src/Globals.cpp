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

// Applique la tempo Marche/Arret d'un relais: cmd=demande de marche, feedback=etat retourne par le PLC.
static void applyRelayTiming(bool cmd, bool feedback, int pin, Tempos &onTimer, Tempos &offTimer){
    if (cmd){
        offTimer.Reset();
        if (onTimer.Q() && !feedback) {
            digitalWrite(pin, HIGH);
        }
    } else {
        onTimer.Reset();
        if (offTimer.Q() && feedback) {
            digitalWrite(pin, LOW);
        }
    }
}

void Relays(){

    applyRelayTiming(bCdeRelaisR1, bChaudiere, RELAY_1, Timer1, Timer2);
    applyRelayTiming(bCdeRelaisR2, bBoostChaud, RELAY_2, Timer3, Timer4);
    applyRelayTiming(bCdeRelaisR3, bPpeRadiat, RELAY_3, Timer5, Timer6);

    lv_label_set_text_fmt(lblBtnR1small, "R1=%d", digitalRead(RELAY_1));
    lv_label_set_text_fmt(lblBtnR2small, "R2=%d", digitalRead(RELAY_2));
    lv_label_set_text_fmt(lblBtnR3small, "R3=%d", digitalRead(RELAY_3));
}

void DisplayAlarms(const uint16_t MBAlarm){

    static String sMessage;
    static uint16_t memoXorAlarm;

    /*
    Alarmes PLC 

         '0 ALARME CHAUDIERE'
        ,'1 PROBLEME REGULATION PLANCHER'
        ,'2 PORTE DE GARAGE OUVERTE'
        ,'3 DEFAUT PRESSION EAU'
        ,'4 TEST MESSAGE'
        ,'5 ALARME ModbusTcp'
        ,'6 ALARME ModbusRtu'
        ,'7 Courant > 4A'
        ,'8 Fuite d eau probable'
        ,'9 Temp. Ext > Temp. Salon'
        ,'10 Temp. Ext < Temp. Salon'
    */

    if ( MBAlarm != MemoHMBAlarme) { // Une alarme se rajoute

        static u_int32_t Cptr=0;
        Cptr++;

        //String sPrefix = "\n/!\\ " + sClockHHMM + " "; // A activer pour scroll verticale
        String sPrefix = "#" + String(sClockHHMM) + " " + String(Cptr) + " ";

    //  memoXorAlarm = MBAlarm ^ MemoHMBAlarme; //Detection nouvelle alarme
        memoXorAlarm = MBAlarm; //Detection nouvelle alarme

        static const char* const AlarmTexts[16] = {
            "0 ALARME CHAUDIERE ",
            "1 PROBLEME REGULATION PLANCHER ",
            "2 PORTE DE GARAGE 1 OUVERTE ",
            "3 DEFAUT PRESSION EAU CHAUFFAGE ",
            "4 TEST MESSAGE ",
            "5 ALARME ModbusTcp ",
            "6 ALARME ModbusRtu ",
            "7 Courant > 4A ",
            "8 Fuite d eau probable ",
            "9 Temp. Ext > Temp. Fermer les volets! ",
            "10 Temp. Ext < Temp. Salon. Ouvrir les fenetres! ",
            "Alarme Bit 11 ",
            "Alarme Bit 12 ",
            "Alarme Bit 13 ",
            "Alarme Bit 14 ",
            "Alarme Bit 15 ",
        };

        sMessage = "";
        for (uint8_t bit = 0; bit < 16; bit++) {
            if (memoXorAlarm & (1 << bit)) {
                sMessage += sPrefix + AlarmTexts[bit];
            }
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