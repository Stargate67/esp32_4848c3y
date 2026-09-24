
#include <Arduino.h>
#include "ModbusIP_ESP8266.h"
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


// SCREEN: scr
void InitUI(void);
void ui_Screen1_screen_init(void);
void ui_Screen1_screen_init_1(lv_obj_t *parent);
//extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_Container1;    // Cadre "Temp exterieure" (haut gauche, ecran principal)
extern lv_obj_t * ui_LblTempExt;    // Grand label: temperature exterieure courante
extern lv_obj_t * ui_LblDate;       // Label: date du jour (haut du container 1)
extern lv_obj_t * ui_LblTempSalon;  // Label: temperature du salon (bas du container 1)
extern lv_obj_t * ui_Container2;    // Cadre "Temp mini du jour"
extern lv_obj_t * ui_LblTempMin;    // Valeur: temperature mini du jour
extern lv_obj_t * ui_LblMinExt;     // Titre fixe "Min Ext."
extern lv_obj_t * ui_LblHeureMin;   // Heure a laquelle le mini a ete releve
extern lv_obj_t * ui_Container3;    // Cadre "Temp maxi du jour"
extern lv_obj_t * ui_LblTempMax;    // Valeur: temperature maxi du jour
extern lv_obj_t * ui_LblMaxExt;     // Titre fixe "Max Ext."
extern lv_obj_t * ui_LblHeureMax;   // Heure a laquelle le maxi a ete releve


extern lv_obj_t * ui_LblValPlancher;      // Valeur: temperature plancher chauffant
extern lv_obj_t * ui_LblValConsPlancher;  // Valeur: consigne plancher chauffant
extern lv_obj_t * ui_LblValECS;           // Valeur: temperature ballon eau chaude sanitaire
extern lv_obj_t * ui_LblValRadiat;        // Valeur: temperature radiateur
extern lv_obj_t * ui_LblValDebitRadit;    // Valeur: debit du circuit radiateur (l/min)
extern lv_obj_t * ui_LblValCourant;       // Valeur: courant electrique consomme (A)

extern lv_obj_t * ui_LblValConsoInstEau;   // Consommation eau instantanee
extern lv_obj_t * ui_LblValConsoInstElec;  // Consommation electrique instantanee
extern lv_obj_t * ui_LblValConsoInstGaz;   // Consommation gaz instantanee
extern lv_obj_t * ui_LblValConsoJEau;      // Consommation eau cumulee du jour
extern lv_obj_t * ui_LblValConsoJElec;     // Consommation electrique cumulee du jour
extern lv_obj_t * ui_LblValConsoJGaz;      // Consommation gaz cumulee du jour
extern lv_obj_t * ui_LblValConsoJ1Elec;    // Consommation electrique totale de la veille (J-1)
extern lv_obj_t * ui_LblValConsoJ1Eau;     // Consommation eau totale de la veille (J-1)
extern lv_obj_t * ui_LblValConsoJ1Gaz;     // Consommation gaz totale de la veille (J-1)

// CUSTOM VARIABLES

void Relays();
void DisplayAlarms(uint16_t MBAlarm);
void AcquitMesAlarme();

// Alimente le buffer d'historique d'une mesure analogique (ecran Relais, graphique
// selectionnable): un echantillon toutes les 10 min par mesure, voir
// lv_createChartMeasure()/SampleMeasurementGraph() dans MainScreen.cpp. `key` doit
// correspondre a une des cles de GRAPH_MEASUREMENTS (sinon l'appel est ignore).
void SampleMeasurementGraph(const char *key, float value);

// Resynchronise l'affichage de l'ecran de saisie de consigne (valeur jaune) sur gConsigneTemp
// (valeur reellement relue du PLC), appelee a chaque cycle Modbus (voir case 20/
// UpdateLVGLFromModbus() dans My_Modbus.cpp). Sans appel, il fallait quitter puis revenir sur
// l'ecran pour forcer ce resync (seul my_event_cb_GoConsigneScreen le faisait). N'ecrase pas
// une saisie +/- en cours (voir bConsigneDirty dans MainScreen.cpp).
void RefreshConsigneDisplay();

// Recalcule les etiquettes HH:MM (heure reelle glissante) de l'axe X du graphique de mesures
// analogiques (ecran Relais). Voir MainScreen.cpp pour le detail; appelee a chaque cycle
// Modbus au meme titre que RefreshConsigneDisplay().
void UpdateGraphTimeAxis();

extern bool bAcquitAlarme;
extern int iState;
extern ModbusIP mb;  //ModbusIP object

//extern char *sClockHHMMSS;
extern char sClockHHMM[15];
extern char sDateDDMMYYYY[16];
extern char sShortDateDDMM[15];

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
extern bool bRelay_5;

extern IPAddress MBremote;   // Adresse IP du PLC WAGO, mutable: voir loadPLCAddress()/applyPLCAddress() dans My_Modbus.cpp
void applyPLCAddress(const IPAddress &newAddr);

// Consigne de temperature (bouton "Consigne" ecran Relais, rangee superieure): ecrite en INT
// (x10, ex 205 = 20.5°C) sur le registre Modbus du %MW630 PLC, et relue en continu depuis ce
// meme registre (case 5/20 dans My_Modbus.cpp): gConsigneTemp reflete donc toujours la valeur
// reelle du PLC (avec ~1s de latence apres une ecriture), pas une simple copie locale. Voir
// applyConsigneTemp() dans My_Modbus.cpp.
extern float gConsigneTemp;
void applyConsigneTemp(float newTemp);

extern lv_obj_t * ui_LblValConsigneTemp;  // Valeur: consigne de temperature (bouton "Consigne", ecran Relais)

extern lv_obj_t *btnR1Chaudiere;     // Bouton relais Chaudiere (ecran Relais)
extern lv_obj_t *btnR2BoostCh;       // Bouton relais Boost Chaudiere (ecran Relais)
extern lv_obj_t *btnR3PpeRadiateur;  // Bouton relais Pompe Radiateur (ecran Relais)
extern lv_obj_t *btnPpePlancher;     // Bouton relais Pompe Plancher chauffant (ecran Relais)
extern lv_obj_t *btnArriveeEau;      // Bouton vanne Arrivee Eau (ecran Relais)

extern lv_obj_t *lblBtnR1Chaudiere;  // Texte du bouton R1 ("Chaud")
extern lv_obj_t *lblBtnR1small;      // Petite etiquette d'etat "R1=0/1"

extern lv_obj_t *lblBtnR2BoostCh;    // Texte du bouton R2 ("Boost")
extern lv_obj_t *lblBtnR2small;      // Petite etiquette d'etat "R2=0/1"

extern lv_obj_t *lblBtnR3PpeRadiateur; // Texte du bouton R3 ("Radiat")
extern lv_obj_t *lblBtnR3small;        // Petite etiquette d'etat "R3=0/1"

extern lv_obj_t *lblBtnPpePlancher;    // Texte du bouton Plancher
extern lv_obj_t *lblBtnArriveeEau;     // Texte du bouton Arrivee Eau

extern lv_obj_t *lblScrolTxt_1;
extern lv_obj_t *lblScrolTxt_2;
extern lv_obj_t *lblScrolTxt_3;

extern lv_obj_t *IPLabel;      // Coin haut-gauche: derniere IP + etape Modbus/etat wifi
extern lv_obj_t *AlarmLabel;   // Bandeau defilant des messages d'alarme (cliquable pour acquitter)
extern lv_obj_t *ClockLabel;   // Horloge HH:MM:SS coin haut-droit
extern lv_obj_t *HorScrollArea;

// Voyants d'etat (petits ronds colores) affiches sur l'ecran principal pour visualiser
// en un coup d'oeil l'etat des relais dont les boutons de commande sont sur l'ecran Relais.
extern lv_obj_t *ledChaud;        // Voyant Chaudiere
extern lv_obj_t *ledBoost;        // Voyant Boost Chaudiere
extern lv_obj_t *ledRadiat;       // Voyant Pompe Radiateur
extern lv_obj_t *ledPlancher;     // Voyant Pompe Plancher
extern lv_obj_t *ledArriveeEau;   // Voyant Arrivee Eau

extern uint16_t mbWriteCoilAddress;

// Identifiants WiFi persistants (NVS), voir main.cpp: loadWifiCredentials()/applyWifiCredentials().
extern String gWifiSsid;
extern String gWifiPassword;
void applyWifiCredentials(const String &newSsid, const String &newPassword);

constexpr uint16_t MB_READ_INTERVAL = 1000; 

constexpr uint16_t START_REG = 12688;           // === Requete de Lectuer Modbus principale ===  Starting holding register %MW399
constexpr uint16_t NB_REGS = 40;                // Number of holding registers to read

constexpr uint16_t START_REG_ANIM = 12745;      // === Requete de Lectuer Modbus ==== Retour etat pour animation pompes, ch. lampes, etc 
constexpr uint16_t NB_REGS_ANIM = 6;            // Number of holding registers to read  y compris les Alarmes 12748,12749

constexpr uint16_t MASK_CHAUD = 0x1;            // Bit 0
constexpr uint16_t MASK_BOOST_ANIM = 0x4000;    // Bit 14  Pour retour etat BOOST
constexpr uint16_t MASK_BOOST_MA = 0x10;        // Bit 5   Pour Commande Marche Arret
constexpr uint16_t MASK_ARRIVEEAU = 0x80;      // Bit 7   Pour retour ouverte arrivée eau

constexpr uint16_t MASK_PPERADIAT = 0x2;        // Bit 1
constexpr uint16_t MASK_PPEPLANCHER = 0x4;      // Bit 2

//constexpr uint16_t START_REG_BP_MARCHE = 12743; // registre des cde Marches bit à bit
//constexpr uint16_t START_REG_BP_ARRET = 12744; // registre des cde Arret bit à bit

constexpr uint16_t BP_MARCHE_CHAUD = 19568;     // %MX455.0  Bobine cde Marche
constexpr uint16_t BP_ARRET_CHAUD = 19584;      // %MX456.0 Bobine cde Arret

constexpr uint16_t BP_MARCHE_BOOST = 19573;     // %MX455.5 Bobine cde Marche
constexpr uint16_t BP_ARRET_BOOST = 19589;      // %MX456.5 Bobine cde Arret

constexpr uint16_t BP_MARCHE_ARRIVEEAU = 19574;     // %MX455.6 Bobine cde Marche
constexpr uint16_t BP_ARRET_ARRIVEEAU = 19590;      // %MX456.6 Bobine cde Arret


constexpr uint16_t BP_MARCHE_RADIAT = 19569;    // %MX455.1  Bobine cde Marche
constexpr uint16_t BP_ARRET_RADIAT = 19585;     // %MX456.1 Bobine cde Arret

constexpr uint16_t BP_MARCHE_PLANCHER = 19570;  // %MX455.2 Bobine cde Marche
constexpr uint16_t BP_ARRET_PLANCHER = 19586;   // %MX456.2 Bobine cde Arret

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
