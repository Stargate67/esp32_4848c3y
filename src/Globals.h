
#include <Arduino.h>
#include <ModbusIP_ESP8266.h>
#include "esp32_smartdisplay.h"

#define SERDEBUG false
#define BP_DEBUG false

class Tempos{
        unsigned long uInitTime;
        unsigned long uPreset;
    public:
        Tempos(unsigned long Preset); // Constructeur
        bool Q();
        bool Q(unsigned long Preset);
        bool Preset(unsigned long Preset);
        bool Reset();
};

//extern Tempos TpoMesAcquite();  // tempo pour Raz Message acquité

/*
constexpr char* sMsgAlarme[16]= {
    "TEST 1 Alarm"
    ,"ALARME CHAUDIERE"
    ,"DEFAUT PRESSION CIRCUIT EAU"
    ,"DEFAUT PRESSION BASSE CIRCUIT EAU"
    ,"PORTE DE GARAGE 1 OUVERTE"
    ,"PROBLEME REGULATION PLANCHER"
    ,"ALARME ModbusTcp"
    ,"ALARME ModbusRtu"
    ,"Temp Exterieure > Temp Salon"
    ,"Temp Exterieure < Temp Salon"
};
*/


// SCREEN: scr
void ui_Screen1_screen_init(void);
//extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_Container1;
extern lv_obj_t * ui_LblTempExt;
extern lv_obj_t * ui_LblDate;
extern lv_obj_t * ui_LblTempSalon;
extern lv_obj_t * ui_Container2;
extern lv_obj_t * ui_LblTempMin;
extern lv_obj_t * ui_LblMinExt;
extern lv_obj_t * ui_LblHeureMin;
extern lv_obj_t * ui_Container3;
extern lv_obj_t * ui_LblTempMax;
extern lv_obj_t * ui_LblMaxExt;
extern lv_obj_t * ui_LblHeureMax;


extern lv_obj_t * ui_LblValPlancher;
extern lv_obj_t * ui_LblValConsPlancher;
extern lv_obj_t * ui_LblValECS;
extern lv_obj_t * ui_LblValRadiat;
extern lv_obj_t * ui_LblValDebitRadit;
extern lv_obj_t * ui_LblValCourant;

extern lv_obj_t * ui_LblValConsoJEau;
extern lv_obj_t * ui_LblValConsoJElec;
extern lv_obj_t * ui_LblValConsoJGaz;
extern lv_obj_t * ui_LblValConsoJ1Elec;
extern lv_obj_t * ui_LblValConsoJ1Eau;
extern lv_obj_t * ui_LblValConsoJ1Gaz;

// CUSTOM VARIABLES

void Relays();
void DisplayAlarms(uint16_t MBAlarm);
void AcquitMesAlarme();

extern bool bAcquitAlarme;

extern ModbusIP mb;  //ModbusIP object

extern String sClockHHMMSS;
extern String sClockHHMM;
extern String sPrintdate;
extern String sPrintShortdate;

extern String sExtMaxTimeStp;
extern String sExtMinTimeStp;

extern const int iAvgMaxFifo;
extern float fAvgFiFo[];
extern int iReadIndex;
extern int iStartIndex;

extern bool bChaudiere;    // Modbus Etat Marche Chaudière
extern bool bBoostChaud;   // Modbus Etat Marche Boost Chaudière
extern bool bPpeRadiat;    // Modbus Etat Marche pompe Radiateur
extern bool bPpePlancher;  // Modbus Etat Marche pompe Plancher

extern bool bCdeRelaisR1;  // demande de marche Relais 1
extern bool bCdeRelaisR2;  // demande de marche Relais 2
extern bool bCdeRelaisR3;  // demande de marche Relais 3

extern bool bRelay_1;
extern bool bRelay_2;
extern bool bRelay_3;
extern bool bRelay_4;

const IPAddress MBremote(77, 204, 15, 6);   // Address of Internet Box => 192.168.0.105 PLC WAGO

extern lv_obj_t *btnR1Chaudiere;
extern lv_obj_t *btnR2BoostCh;
extern lv_obj_t *btnR3PpeRadiateur;
extern lv_obj_t *btnPpePlancher;

extern lv_obj_t *lblBtnR1Chaudiere;
extern lv_obj_t *lblBtnR1small;

extern lv_obj_t *lblBtnR2BoostCh;
extern lv_obj_t *lblBtnR2small;

extern lv_obj_t *lblBtnR3PpeRadiateur;
extern lv_obj_t *lblBtnR3small;

extern lv_obj_t *lblBtnPpePlancher;

extern lv_obj_t *lblScrolTxt_1;
extern lv_obj_t *lblScrolTxt_2;
extern lv_obj_t *lblScrolTxt_3;

extern lv_obj_t *IPLabel;
extern lv_obj_t *AlarmLabel;
extern lv_obj_t *ClockLabel;
extern lv_obj_t *HorScrollArea;

constexpr uint16_t MB_READ_INTERVAL = 1000; 

constexpr uint16_t START_REG = 12688;           // === Requete de Lectuer Modbus principale ===  Starting holding register
constexpr uint16_t NB_REGS = 22;                // Number of holding registers to read

constexpr uint16_t START_REG_ANIM = 12745;      // === Requete de Lectuer Modbus ==== Retour etat pour animation pompes, ch. lampes, etc 
constexpr uint16_t NB_REGS_ANIM = 6;            // Number of holding registers to read  y compris les Alarmes 12748,12749

constexpr uint16_t MASK_CHAUD = 0x1;            // Bit 0
constexpr uint16_t MASK_BOOST_ANIM = 0x4000;    // Bit 14  Pourretour etat BOOST
constexpr uint16_t MASK_BOOST_MA = 0x10;        // Bit 5   Pour Commande Marche Arret


constexpr uint16_t MASK_PPERADIAT = 0x2;        // Bit 1
constexpr uint16_t MASK_PPEPLANCHER = 0x4;      // Bit 2

//constexpr uint16_t START_REG_BP_MARCHE = 12743; // registre des cde Marches bit à bit
//constexpr uint16_t START_REG_BP_ARRET = 12744; // registre des cde Arret bit à bit

constexpr uint16_t BP_MARCHE_CHAUD = 19568;     // %MX455.0  Bobine cde Marche
constexpr uint16_t BP_ARRET_CHAUD = 19584;      // %MX457.0 Bobine cde Arret

constexpr uint16_t BP_MARCHE_BOOST = 19573;     // %MX455.5 Bobine cde Marche
constexpr uint16_t BP_ARRET_BOOST = 19589;      // %MX457.5 Bobine cde Arret

constexpr uint16_t BP_MARCHE_RADIAT = 19569;    // %MX455.1  Bobine cde Marche
constexpr uint16_t BP_ARRET_RADIAT = 19585;     // %MX457.1 Bobine cde Arret

constexpr uint16_t BP_MARCHE_PLANCHER = 19570;  // %MX455.2 Bobine cde Marche
constexpr uint16_t BP_ARRET_PLANCHER = 19586;   // %MX457.2 Bobine cde Arret

constexpr uint16_t BP_LAMPE_SSOL = 19488;       // BP Marche lampe sous sol
constexpr uint16_t BP_LAMPE_RDC = 19489;        // BP Marche lampe RdC
constexpr uint16_t BP_LAMPE_1ETAGE = 19490;     // BP Marche lampe 1erEtage

constexpr uint16_t BP_OUVERT_GARAGE = 19491;    // BP ouverture porte de garage
constexpr uint16_t PORTE_GARAGE1_FERME = 19492; // Etat porte de garage fermé

//Lecture des Bits de Sortie 
// de "000512   , 16 ); 

//$OutPuts["000513"] =  Porte de Garage
//$OutPuts["000514"] =  Ppe Radiateur
//$OutPuts["000515"] =  Ppe Plancher
//$OutPuts["000516"] =  Vanne Ouvre
//$OutPuts["000517"] =  Vanne ferme
//$OutPuts["000518"] =  Chaudière
//$OutPuts["000519"] =  Boot Chaudière

//Ou bien Registre 12745 pour les retours Animation 
// Voir ci-dessous

/*
 === Ecriture Modbus ===

Registre 12745 pour les retours Animation Equipements en Marche

Bit de chaque Mot (de 0 à 15)

CHAUD           = Bit 0  
BOOST           = bit 5 pour BPs M/A
BOOST           = bit 14 pour animation
PPE Radiateur   = Bit 1
PPE Plancher    = Bit 2

*/
