#include "Globals.h"

bool bRelay_4;
bool bRelay_5;
bool bAcquitAlarme;

uint32_t compteur;

lv_obj_t *btnR1Chaudiere;     // Bouton relais Chaudiere (ecran Relais)
lv_obj_t *btnR2BoostCh;       // Bouton relais Boost Chaudiere (ecran Relais)
lv_obj_t *btnR3PpeRadiateur;  // Bouton relais Pompe Radiateur (ecran Relais)
lv_obj_t *btnPpePlancher;     // Bouton relais Pompe Plancher chauffant (ecran Relais)
lv_obj_t *btnArriveeEau;      // Bouton vanne Arrivee Eau (ecran Relais)

lv_obj_t *lblBtnR1Chaudiere;  // Texte du bouton R1 ("Chaud")
lv_obj_t *lblBtnR1small;      // Petite etiquette d'etat "R1=0/1"

lv_obj_t *lblBtnR2BoostCh;    // Texte du bouton R2 ("Boost")
lv_obj_t *lblBtnR2small;      // Petite etiquette d'etat "R2=0/1"

lv_obj_t *lblBtnR3PpeRadiateur; // Texte du bouton R3 ("Radiat")
lv_obj_t *lblBtnR3small;        // Petite etiquette d'etat "R3=0/1"

lv_obj_t *lblBtnPpePlancher;    // Texte du bouton Plancher
lv_obj_t *lblBtnArriveeEau;     // Texte du bouton Arrivee Eau

lv_obj_t * ui_Container1;   // Cadre "Temp exterieure" (haut gauche, ecran principal)
lv_obj_t * ui_Container2;   // Cadre "Temp mini du jour"
lv_obj_t * ui_LblMinExt;    // Titre fixe "Min Ext."
lv_obj_t * ui_Container3;   // Cadre "Temp maxi du jour"
lv_obj_t * ui_LblMaxExt;    // Titre fixe "Max Ext."
lv_obj_t * ui_Chart1;       // Non utilise actuellement

// Voyants d'etat sur l'ecran principal, refletant l'etat des relais dont les boutons
// de commande sont sur l'ecran Relais (mis a jour dans My_Modbus.cpp, case 20).
lv_obj_t *ledChaud;
lv_obj_t *ledBoost;
lv_obj_t *ledRadiat;
lv_obj_t *ledPlancher;
lv_obj_t *ledArriveeEau;

bool bCdeRelaisR1;  // demande de marche Relais 1
bool bCdeRelaisR2;  // demande de marche Relais 2
bool bCdeRelaisR3;  // demande de marche Relais 3

static const lv_color_t Btn_grad_colors[2] = {
    LV_COLOR_MAKE(10, 10, 10),
    LV_COLOR_MAKE(30, 30, 30),
};

static lv_obj_t *ui_ScreenMain;
static lv_obj_t *ui_ScreenRelais;
static lv_obj_t *ui_ScreenWifi;   // Ecran de configuration WiFi (SSID/mot de passe), accessible depuis l'ecran Relais
static lv_obj_t *ui_ScreenPLC;    // Ecran de configuration de l'adresse IP du PLC, accessible depuis l'ecran Relais

static lv_obj_t *taWifiSsid;
static lv_obj_t *taWifiPass;
static lv_obj_t *kbWifi;

static lv_obj_t *taPlcIp;
static lv_obj_t *kbPlc;

//************************************************************************************************************/
//============================================================================================================/

// Bascule la commande d'un relais et pose l'adresse de bobine Modbus a ecrire au prochain cycle.
static void toggleRelayCoil(bool &cmdFlag, uint16_t coilOn, uint16_t coilOff){
    if (cmdFlag) {
        mbWriteCoilAddress = coilOff;
        cmdFlag = 0;
    } else {
        mbWriteCoilAddress = coilOn;
        cmdFlag = 1;
    }
}

static void my_event_cb_R1Chaudiere (lv_event_t *e){
    compteur++;
    if (BP_DEBUG) Serial.println("compteur=" + String(compteur));

    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_RELEASED) {
        toggleRelayCoil(bCdeRelaisR1, BP_MARCHE_CHAUD, BP_ARRET_CHAUD);
        if (BP_DEBUG) Serial.println("BP CHAUD: event code=" + String(LV_EVENT_RELEASED) + "/ Etat Relais:" + digitalRead(RELAY_1));
    }
}

static void my_event_cb_R2BoostCh (lv_event_t *e){
    compteur++;
    if (BP_DEBUG) Serial.println("compteur=" + String(compteur));

    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_RELEASED) {
        toggleRelayCoil(bCdeRelaisR2, BP_MARCHE_BOOST, BP_ARRET_BOOST);
        if (BP_DEBUG) Serial.println("BP BOOST: event code=" + String(LV_EVENT_RELEASED) + "/ Etat Relais:" + digitalRead(RELAY_2));
    }
}

static void my_event_cb_R3PpeRadiateur (lv_event_t *e){

    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_RELEASED) {
        compteur++;
        if (BP_DEBUG) Serial.println("compteur=" + String(compteur));

        toggleRelayCoil(bCdeRelaisR3, BP_MARCHE_RADIAT, BP_ARRET_RADIAT);
        if (BP_DEBUG) Serial.println("BP RADIAT: event code=" + String(LV_EVENT_RELEASED) + "/ Etat Relais:" + digitalRead(RELAY_3));
    }
}

static void my_event_cb_PpePlancher (lv_event_t *e){

    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_RELEASED) {
        compteur++;
        if (BP_DEBUG) Serial.println("compteur=" + String(compteur));

        toggleRelayCoil(bRelay_4, BP_MARCHE_PLANCHER, BP_ARRET_PLANCHER);
        if (BP_DEBUG) Serial.println("BP PLANCHER: event code=" + String(LV_EVENT_RELEASED) + "/ Etat Relais:" + bRelay_4);
    }
}

static void my_event_cb_ArriveeEau (lv_event_t *e){

    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_RELEASED) {
        compteur++;
        if (BP_DEBUG) Serial.println("compteur=" + String(compteur));

        toggleRelayCoil(bRelay_5, BP_MARCHE_ARRIVEEAU, BP_ARRET_ARRIVEEAU);
        if (BP_DEBUG) Serial.println("BP ARRIVEE EAU: event code=" + String(LV_EVENT_RELEASED) + "/ Etat Relais:" + bRelay_5);
    }
}

static void my_event_cb_GoRelaisScreen(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        lv_scr_load(ui_ScreenRelais);
    }
}

static void my_event_cb_BackMainScreen(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        lv_scr_load(ui_ScreenMain);
    }
}

static void my_event_cb_GoWifiScreen(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        lv_scr_load(ui_ScreenWifi);
    }
}

static void my_event_cb_BackFromWifiScreen(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        lv_obj_add_flag(kbWifi, LV_OBJ_FLAG_HIDDEN);
        lv_keyboard_set_textarea(kbWifi, NULL);
        lv_scr_load(ui_ScreenRelais);
    }
}

// Affiche le clavier LVGL et l'attache au textarea qui vient de prendre le focus.
static void my_event_cb_WifiTaFocused(lv_event_t *e){
    lv_obj_t *ta = (lv_obj_t*)lv_event_get_target(e);
    lv_keyboard_set_textarea(kbWifi, ta);
    lv_obj_clear_flag(kbWifi, LV_OBJ_FLAG_HIDDEN);
}

// Cache le clavier quand on quitte le champ ou qu'on valide/annule depuis le clavier lui-meme.
static void my_event_cb_WifiKbHide(lv_event_t *e){
    lv_obj_add_flag(kbWifi, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(kbWifi, NULL);
}

// Lit les 2 champs et applique/enregistre les nouveaux identifiants WiFi (reconnexion a chaud,
// voir applyWifiCredentials() dans main.cpp).
static void my_event_cb_WifiSave(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        applyWifiCredentials(lv_textarea_get_text(taWifiSsid), lv_textarea_get_text(taWifiPass));
    }
}

static void my_event_cb_GoPLCScreen(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        lv_scr_load(ui_ScreenPLC);
    }
}

static void my_event_cb_BackFromPLCScreen(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        lv_obj_add_flag(kbPlc, LV_OBJ_FLAG_HIDDEN);
        lv_keyboard_set_textarea(kbPlc, NULL);
        lv_scr_load(ui_ScreenRelais);
    }
}

static void my_event_cb_PlcTaFocused(lv_event_t *e){
    lv_obj_t *ta = (lv_obj_t*)lv_event_get_target(e);
    lv_keyboard_set_textarea(kbPlc, ta);
    lv_obj_clear_flag(kbPlc, LV_OBJ_FLAG_HIDDEN);
}

static void my_event_cb_PlcKbHide(lv_event_t *e){
    lv_obj_add_flag(kbPlc, LV_OBJ_FLAG_HIDDEN);
    lv_keyboard_set_textarea(kbPlc, NULL);
}

// Valide et applique la nouvelle adresse IP du PLC (voir applyPLCAddress() dans My_Modbus.cpp).
// Une saisie invalide (mauvais format) est simplement ignoree, le champ garde le texte saisi.
static void my_event_cb_PlcSave(lv_event_t *e){
    if (lv_event_get_code(e) == LV_EVENT_RELEASED) {
        IPAddress addr;
        if (addr.fromString(lv_textarea_get_text(taPlcIp))) {
            applyPLCAddress(addr);
        }
    }
}

static void my_event_cb_AcqAlarmes (lv_event_t *e){

    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_RELEASED) {
        compteur++;
        if (BP_DEBUG) Serial.println("compteur=" + String(compteur));

        bAcquitAlarme = 1;

        if (BP_DEBUG) Serial.println("Acquit Alarmes: event code=" + String(LV_EVENT_RELEASED));
    }
}

void ui_Screen1_screen_init(lv_obj_t *parent)
{
    //ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(parent, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Container1 = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_Container1);
    lv_obj_set_width(ui_Container1, 180);
    lv_obj_set_height(ui_Container1, 120);
    lv_obj_set_x(ui_Container1, 0);
    lv_obj_set_y(ui_Container1, 30);

    lv_obj_set_scrollbar_mode(ui_Container1, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_Container1, LV_DIR_LEFT);
    lv_obj_set_style_bg_color(ui_Container1, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblTempExt = lv_label_create(ui_Container1);
    lv_obj_set_width(ui_LblTempExt, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblTempExt, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_LblTempExt, LV_ALIGN_CENTER);
    lv_label_set_long_mode(ui_LblTempExt, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblTempExt, "-99.99 °C");
    
    lv_obj_set_scrollbar_mode(ui_LblTempExt, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblTempExt, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblTempExt, lv_color_hex(0xC2ED34), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblTempExt, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblTempExt, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblDate = lv_label_create(ui_Container1);
    lv_obj_set_width(ui_LblDate, LV_SIZE_CONTENT);   /// 100
    lv_obj_set_height(ui_LblDate, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_LblDate, LV_ALIGN_TOP_MID);
    lv_label_set_long_mode(ui_LblDate, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblDate, "00/00/0000");
    
    lv_obj_set_scrollbar_mode(ui_LblDate, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblDate, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblDate, lv_color_hex(0x00FCFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblDate, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblDate, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblTempSalon = lv_label_create(ui_Container1);
    lv_obj_set_width(ui_LblTempSalon, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblTempSalon, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblTempSalon, 0);
    lv_obj_set_y(ui_LblTempSalon, -3);
    lv_obj_set_align(ui_LblTempSalon, LV_ALIGN_BOTTOM_MID);
    lv_label_set_long_mode(ui_LblTempSalon, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblTempSalon, "-99.99 °C");
    
    lv_obj_set_scrollbar_mode(ui_LblTempSalon, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblTempSalon, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblTempSalon, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblTempSalon, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblTempSalon, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Container2 = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_Container2);
    lv_obj_set_width(ui_Container2, 149);
    lv_obj_set_height(ui_Container2, 120);
    lv_obj_set_x(ui_Container2, 181);
    lv_obj_set_y(ui_Container2, 30);
    
    lv_obj_set_scrollbar_mode(ui_Container2, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_Container2, LV_DIR_LEFT);
    lv_obj_set_style_bg_color(ui_Container2, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_Container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_Container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_Container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_Container2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblTempMin = lv_label_create(ui_Container2);
    lv_obj_set_width(ui_LblTempMin, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblTempMin, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_LblTempMin, LV_ALIGN_CENTER);
    lv_label_set_long_mode(ui_LblTempMin, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblTempMin, "-99.99 °C");
    
    lv_obj_set_scrollbar_mode(ui_LblTempMin, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblTempMin, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblTempMin, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblTempMin, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblTempMin, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblMinExt = lv_label_create(ui_Container2);
    lv_obj_set_width(ui_LblMinExt, LV_SIZE_CONTENT);   /// 100
    lv_obj_set_height(ui_LblMinExt, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_LblMinExt, LV_ALIGN_TOP_MID);
    lv_label_set_long_mode(ui_LblMinExt, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblMinExt, "Min Ext.");
    
    lv_obj_set_scrollbar_mode(ui_LblMinExt, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblMinExt, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblMinExt, lv_color_hex(0x0069FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblMinExt, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblMinExt, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblHeureMin = lv_label_create(ui_Container2);
    lv_obj_set_width(ui_LblHeureMin, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblHeureMin, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblHeureMin, 0);
    lv_obj_set_y(ui_LblHeureMin, -3);
    lv_obj_set_align(ui_LblHeureMin, LV_ALIGN_BOTTOM_MID);
    lv_label_set_long_mode(ui_LblHeureMin, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblHeureMin, "00:00");
    
    lv_obj_set_scrollbar_mode(ui_LblHeureMin, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblHeureMin, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblHeureMin, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblHeureMin, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblHeureMin, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Container3 = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_Container3);
    lv_obj_set_width(ui_Container3, 150);
    lv_obj_set_height(ui_Container3, 120);
    lv_obj_set_x(ui_Container3, 331);
    lv_obj_set_y(ui_Container3, 30);
    
    lv_obj_set_scrollbar_mode(ui_Container3, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_Container3, LV_DIR_LEFT);
    lv_obj_set_style_bg_color(ui_Container3, lv_color_hex(0x282828), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Container3, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblTempMax = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_LblTempMax, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblTempMax, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_LblTempMax, LV_ALIGN_CENTER);
    lv_label_set_long_mode(ui_LblTempMax, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblTempMax, "-99.99 °C");
    
    lv_obj_set_scrollbar_mode(ui_LblTempMax, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblTempMax, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblTempMax, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblTempMax, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblTempMax, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblMaxExt = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_LblMaxExt, LV_SIZE_CONTENT);   /// 100
    lv_obj_set_height(ui_LblMaxExt, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_align(ui_LblMaxExt, LV_ALIGN_TOP_MID);
    lv_label_set_long_mode(ui_LblMaxExt, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblMaxExt, "Max Ext.");
    
    lv_obj_set_scrollbar_mode(ui_LblMaxExt, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblMaxExt, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblMaxExt, lv_color_hex(0x0069FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblMaxExt, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblMaxExt, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblHeureMax = lv_label_create(ui_Container3);
    lv_obj_set_width(ui_LblHeureMax, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblHeureMax, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblHeureMax, 0);
    lv_obj_set_y(ui_LblHeureMax, -3);
    lv_obj_set_align(ui_LblHeureMax, LV_ALIGN_BOTTOM_MID);
    lv_label_set_long_mode(ui_LblHeureMax, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblHeureMax, "00:00");

    lv_obj_set_scrollbar_mode(ui_LblHeureMax, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblHeureMax, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblHeureMax, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblHeureMax, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblHeureMax, &lv_font_montserrat_24, LV_PART_MAIN | LV_STATE_DEFAULT);

}

// Cree un bouton relais avec son degrade standard, son etiquette de titre centree et son event cb.
static lv_obj_t* createRelayButtonBase(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h,
                                        const char *text, lv_obj_t **outLabel, lv_event_cb_t eventCb){
    lv_obj_t *btn = lv_btn_create(parent);
    lv_obj_set_size(btn, w, h);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, x, y);
    lv_obj_set_style_bg_color(btn, Btn_grad_colors[0], 0);
    lv_obj_set_style_bg_grad_color(btn, Btn_grad_colors[1], 0);
    lv_obj_set_style_bg_grad_dir(btn, LV_GRAD_DIR_VER, 0);

    *outLabel = lv_label_create(btn);
    lv_label_set_text(*outLabel, text);
    lv_obj_center(*outLabel);

    lv_obj_add_event_cb(btn, eventCb, LV_EVENT_RELEASED, NULL);
    return btn;
}

// Ajoute la petite etiquette d'etat "Rx=0/1" en bas a gauche d'un bouton relais.
static lv_obj_t* createRelaySmallLabel(lv_obj_t *btn, const char *text){
    lv_obj_t *lbl = lv_label_create(btn);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_12, 0);
    lv_obj_align(lbl, LV_ALIGN_BOTTOM_LEFT, 3, 3);
    lv_label_set_text(lbl, text);
    return lbl;
}

// Cree un voyant rectangulaire (couleur grise par defaut) avec son libelle centre a l'interieur.
// Les voyants sont accoles (bordure fine) pour former un seul bloc rectangulaire.
// La couleur est mise a jour par updateAnimatedRelay() dans My_Modbus.cpp.
static lv_obj_t* createStatusLed(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h, const char *text){
    lv_obj_t *led = lv_obj_create(parent);
    lv_obj_remove_style_all(led);
    lv_obj_set_size(led, w, h);
    lv_obj_set_x(led, x);
    lv_obj_set_y(led, y);
    lv_obj_clear_flag(led, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(led, lv_color_make(128, 128, 128), 0);
    lv_obj_set_style_bg_opa(led, 255, 0);
    lv_obj_set_style_border_width(led, 1, 0);
    lv_obj_set_style_border_color(led, lv_color_black(), 0);

    lv_obj_t *lbl = lv_label_create(led);
    lv_obj_set_style_text_font(lbl, &lv_font_montserrat_22, 0);
    lv_obj_set_style_text_color(lbl, lv_color_white(), 0);
    lv_obj_center(lbl);
    lv_label_set_text(lbl, text);

    return led;
}

// Bloc rectangulaire de 5 voyants d'etat en grille 3 colonnes x 2 lignes, occupant toute
// la hauteur restante en bas de l'ecran principal (les boutons de commande sont sur l'ecran
// Relais). La 6e cellule (bas-droite) est reservee au bouton "Cde >", cree dans InitUI().
void lv_createStatusLeds(lv_obj_t *parent){
    const lv_coord_t xA = 5, colW = 156, gap = 1;
    const lv_coord_t xB = xA + colW + gap;
    const lv_coord_t xC = xB + colW + gap;
    const lv_coord_t y1 = 362, rowH = 57, rowGap = 1;
    const lv_coord_t y2 = y1 + rowH + rowGap;

    ledChaud      = createStatusLed(parent, xA, y1, colW, rowH, "Chaud");
    ledBoost      = createStatusLed(parent, xB, y1, colW, rowH, "Boost");
    ledRadiat     = createStatusLed(parent, xC, y1, colW, rowH, "Radiat");
    ledPlancher   = createStatusLed(parent, xA, y2, colW, rowH, "Plancher");
    ledArriveeEau = createStatusLed(parent, xB, y2, colW, rowH, "Eau");
}

void lv_createButton_CHAUD(lv_obj_t *parent){
    btnR1Chaudiere = createRelayButtonBase(parent, 0, 360, 120, 120, "Chaud", &lblBtnR1Chaudiere, my_event_cb_R1Chaudiere);
    lblBtnR1small = createRelaySmallLabel(btnR1Chaudiere, "R1=0");
}

void lv_createButton_BOOSTCh(lv_obj_t *parent){
    btnR2BoostCh = createRelayButtonBase(parent, 120, 360, 120, 120, "Boost", &lblBtnR2BoostCh, my_event_cb_R2BoostCh);
    lblBtnR2small = createRelaySmallLabel(btnR2BoostCh, "R2=0");
}

void lv_createButton_RADIAT(lv_obj_t *parent){
    btnR3PpeRadiateur = createRelayButtonBase(parent, 240, 360, 120, 120, "Radiat", &lblBtnR3PpeRadiateur, my_event_cb_R3PpeRadiateur);
    lblBtnR3small = createRelaySmallLabel(btnR3PpeRadiateur, "R3=0");
}

void lv_createButton_PLANCHER(lv_obj_t *parent){
    btnPpePlancher = createRelayButtonBase(parent, 360, 360, 120, 120, "Plancher", &lblBtnPpePlancher, my_event_cb_PpePlancher);
    lv_obj_set_style_bg_color(btnPpePlancher, lv_color_make( 120, 120, 120 ), 0 );
}

void lv_createButton_ArriveeEau(lv_obj_t *parent){
    btnArriveeEau = createRelayButtonBase(parent, 360, 250, 120, 110, "Arrivee\nEau", &lblBtnArriveeEau, my_event_cb_ArriveeEau);
    lv_obj_set_style_bg_color(btnArriveeEau, lv_color_make( 120, 120, 120 ), 0 );
    lv_obj_set_style_text_align(lblBtnArriveeEau, LV_TEXT_ALIGN_CENTER, 0);
}

// Cree un ecran de configuration WiFi: 2 champs (SSID en clair, mot de passe masque),
// pre-remplis avec les identifiants courants (gWifiSsid/gWifiPassword), un clavier LVGL
// attache dynamiquement au champ ayant le focus, et les boutons Enregistrer/Retour.
void lv_createScreenWifi(lv_obj_t *parent){
    // y de depart choisi pour rester sous le bouton "< Retour" (0,30,120,60 => bas a y=90)
    lv_obj_t *lblSsid = lv_label_create(parent);
    lv_label_set_text(lblSsid, "SSID");
    lv_obj_set_style_text_color(lblSsid, lv_color_white(), 0);
    lv_obj_set_style_text_font(lblSsid, &lv_font_montserrat_20, 0);
    lv_obj_set_pos(lblSsid, 20, 110);

    taWifiSsid = lv_textarea_create(parent);
    lv_obj_set_size(taWifiSsid, 300, 50);
    lv_obj_set_pos(taWifiSsid, 20, 140);
    lv_textarea_set_one_line(taWifiSsid, true);
    lv_textarea_set_text(taWifiSsid, gWifiSsid.c_str());
    lv_obj_add_event_cb(taWifiSsid, my_event_cb_WifiTaFocused, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(taWifiSsid, my_event_cb_WifiKbHide, LV_EVENT_DEFOCUSED, NULL);

    lv_obj_t *lblPass = lv_label_create(parent);
    lv_label_set_text(lblPass, "Mot de passe");
    lv_obj_set_style_text_color(lblPass, lv_color_white(), 0);
    lv_obj_set_style_text_font(lblPass, &lv_font_montserrat_20, 0);
    lv_obj_set_pos(lblPass, 20, 210);

    taWifiPass = lv_textarea_create(parent);
    lv_obj_set_size(taWifiPass, 300, 50);
    lv_obj_set_pos(taWifiPass, 20, 240);
    lv_textarea_set_one_line(taWifiPass, true);
    lv_textarea_set_password_mode(taWifiPass, true);
    lv_textarea_set_text(taWifiPass, gWifiPassword.c_str());
    lv_obj_add_event_cb(taWifiPass, my_event_cb_WifiTaFocused, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(taWifiPass, my_event_cb_WifiKbHide, LV_EVENT_DEFOCUSED, NULL);

    lv_obj_t *lblUnused;
    lv_obj_t *btnSave = createRelayButtonBase(parent, 340, 140, 120, 50, "Enregistrer", &lblUnused, my_event_cb_WifiSave);
    lv_obj_set_style_bg_color(btnSave, lv_color_make(0, 120, 40), 0);
    lv_obj_set_style_bg_grad_color(btnSave, lv_color_make(0, 120, 40), 0);

    lv_obj_t *btnBack = createRelayButtonBase(parent, 0, 30, 120, 60, "< Retour", &lblUnused, my_event_cb_BackFromWifiScreen);
    lv_obj_set_style_bg_color(btnBack, lv_color_make(60, 60, 60), 0);
    lv_obj_set_style_bg_grad_color(btnBack, lv_color_make(60, 60, 60), 0);

    kbWifi = lv_keyboard_create(parent);
    lv_obj_add_flag(kbWifi, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(kbWifi, my_event_cb_WifiKbHide, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(kbWifi, my_event_cb_WifiKbHide, LV_EVENT_CANCEL, NULL);
}

// Cree un ecran de configuration de l'adresse IP du PLC WAGO: un champ pre-rempli avec
// l'adresse courante (MBremote), un clavier numerique LVGL, boutons Enregistrer/Retour.
void lv_createScreenPLC(lv_obj_t *parent){
    // y de depart choisi pour rester sous le bouton "< Retour" (0,30,120,60 => bas a y=90)
    lv_obj_t *lblIp = lv_label_create(parent);
    lv_label_set_text(lblIp, "Adresse IP du PLC");
    lv_obj_set_style_text_color(lblIp, lv_color_white(), 0);
    lv_obj_set_style_text_font(lblIp, &lv_font_montserrat_20, 0);
    lv_obj_set_pos(lblIp, 20, 110);

    taPlcIp = lv_textarea_create(parent);
    lv_obj_set_size(taPlcIp, 220, 50);
    lv_obj_set_pos(taPlcIp, 20, 140);
    lv_textarea_set_one_line(taPlcIp, true);
    lv_textarea_set_text(taPlcIp, MBremote.toString().c_str());
    lv_obj_add_event_cb(taPlcIp, my_event_cb_PlcTaFocused, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(taPlcIp, my_event_cb_PlcKbHide, LV_EVENT_DEFOCUSED, NULL);

    lv_obj_t *lblUnused;
    lv_obj_t *btnSave = createRelayButtonBase(parent, 260, 140, 120, 50, "Enregistrer", &lblUnused, my_event_cb_PlcSave);
    lv_obj_set_style_bg_color(btnSave, lv_color_make(0, 120, 40), 0);
    lv_obj_set_style_bg_grad_color(btnSave, lv_color_make(0, 120, 40), 0);

    lv_obj_t *btnBack = createRelayButtonBase(parent, 0, 30, 120, 60, "< Retour", &lblUnused, my_event_cb_BackFromPLCScreen);
    lv_obj_set_style_bg_color(btnBack, lv_color_make(60, 60, 60), 0);
    lv_obj_set_style_bg_grad_color(btnBack, lv_color_make(60, 60, 60), 0);

    kbPlc = lv_keyboard_create(parent);
    lv_keyboard_set_mode(kbPlc, LV_KEYBOARD_MODE_NUMBER);
    lv_obj_add_flag(kbPlc, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(kbPlc, my_event_cb_PlcKbHide, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(kbPlc, my_event_cb_PlcKbHide, LV_EVENT_CANCEL, NULL);
}

void lv_CreateIPLabel(lv_obj_t * parent)
{
    IPLabel = lv_label_create(parent); 
    lv_label_set_text(IPLabel, ".X.XXX");
    lv_obj_set_style_text_font(IPLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(IPLabel, lv_color_make(255, 250, 0), 0); //yellow
    lv_obj_align(IPLabel, LV_ALIGN_TOP_LEFT, 0, 3); 
}

void lv_CreateAlarm(lv_obj_t * parent)
{
    AlarmLabel = lv_label_create(parent);
    lv_obj_set_size(AlarmLabel, 350, 25);
    lv_label_set_text(AlarmLabel, "");
    lv_obj_set_style_text_font(AlarmLabel, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(AlarmLabel, lv_color_make(255, 0, 0), 0); //Red
    lv_obj_align(AlarmLabel, LV_ALIGN_TOP_LEFT, 35, 0);
    lv_label_set_long_mode(AlarmLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    //lv_obj_set_scroll_dir(AlarmLabel, LV_DIR_LEFT);

    lv_obj_add_flag(AlarmLabel, LV_OBJ_FLAG_CLICKABLE);     /// Flags
    lv_obj_add_event_cb(AlarmLabel, my_event_cb_AcqAlarmes, LV_EVENT_RELEASED, NULL);
}

void lv_CreateClock(lv_obj_t * parent)
{    // Create a label
    ClockLabel = lv_label_create(parent); 
    lv_label_set_text(ClockLabel, "HH:MM:SS");
    lv_obj_set_style_text_font(ClockLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(ClockLabel, lv_color_white(), 0);
    lv_obj_align(ClockLabel, LV_ALIGN_TOP_LEFT, 400, 3); 
}

void InitUI(){

// Initialize the display
  smartdisplay_init();

  // Set backlight to full brightness
  smartdisplay_lcd_set_backlight(0.8); // 0 is off, 0.5 is half and 1 is full brightness.

  __attribute__((unused)) auto disp = lv_disp_get_default();
  // lv_disp_set_rotation(disp, LV_DISP_ROT_90);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_180);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_270);

  // Ecran principal = l'ecran actif par defaut
  ui_ScreenMain = lv_scr_act();
  lv_obj_set_style_bg_color(ui_ScreenMain, lv_color_hex(0x090909), 0);

  // 2eme ecran dedie aux boutons relais
  ui_ScreenRelais = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(ui_ScreenRelais, lv_color_hex(0x090909), 0);

  // 3eme ecran: configuration WiFi (SSID/mot de passe), accessible depuis l'ecran Relais
  ui_ScreenWifi = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(ui_ScreenWifi, lv_color_hex(0x090909), 0);

  // 4eme ecran: configuration de l'adresse IP du PLC, accessible depuis l'ecran Relais
  ui_ScreenPLC = lv_obj_create(NULL);
  lv_obj_set_style_bg_color(ui_ScreenPLC, lv_color_hex(0x090909), 0);

  // Statut com / horloge / alarme: zone commune, superposee a l'ecran actif quel qu'il
  // soit (lv_layer_top() est transparente hors des widgets qu'on y place).
  lv_CreateIPLabel(lv_layer_top());
  lv_CreateClock(lv_layer_top());
  lv_CreateAlarm(lv_layer_top());
  ui_Screen1_screen_init(ui_ScreenMain);
  ui_Screen1_screen_init_1(ui_ScreenMain);

  lv_createButton_CHAUD(ui_ScreenRelais);
  lv_createButton_BOOSTCh(ui_ScreenRelais);
  lv_createButton_RADIAT(ui_ScreenRelais);
  lv_createButton_PLANCHER(ui_ScreenRelais);
  lv_createButton_ArriveeEau(ui_ScreenRelais);

  // Voyants d'etat des relais, visibles sans quitter l'ecran principal
  lv_createStatusLeds(ui_ScreenMain);

  // Bouton "Cde >" : occupe la 6e cellule (bas-droite) de la grille de voyants 3x2
  // (memes coordonnees que dans lv_createStatusLeds : xC/y2/colW/rowH). Fond gris fonce uni.
  lv_obj_t *lblUnused;
  lv_obj_t *btnCde = createRelayButtonBase(ui_ScreenMain, 319, 420, 156, 57, "Cde >", &lblUnused, my_event_cb_GoRelaisScreen);
  lv_obj_set_style_bg_color(btnCde, lv_color_make(60, 60, 60), 0);
  lv_obj_set_style_bg_grad_color(btnCde, lv_color_make(60, 60, 60), 0);

  // y=30: laisse la place a la zone commune (IP/horloge/alarme, sur lv_layer_top())
  // desormais visible aussi sur cet ecran.
  lv_obj_t *btnRetour = createRelayButtonBase(ui_ScreenRelais, 0, 30, 120, 60, "< Retour", &lblUnused, my_event_cb_BackMainScreen);
  lv_obj_set_style_bg_color(btnRetour, lv_color_make(60, 60, 60), 0);
  lv_obj_set_style_bg_grad_color(btnRetour, lv_color_make(60, 60, 60), 0);

  // Bouton d'acces a l'ecran de config WiFi, a cote du bouton Retour
  lv_obj_t *btnWifi = createRelayButtonBase(ui_ScreenRelais, 130, 30, 120, 60, "WiFi >", &lblUnused, my_event_cb_GoWifiScreen);
  lv_obj_set_style_bg_color(btnWifi, lv_color_make(60, 60, 60), 0);
  lv_obj_set_style_bg_grad_color(btnWifi, lv_color_make(60, 60, 60), 0);

  // Bouton d'acces a l'ecran de config adresse IP du PLC, a cote du bouton WiFi
  lv_obj_t *btnPlc = createRelayButtonBase(ui_ScreenRelais, 260, 30, 120, 60, "PLC >", &lblUnused, my_event_cb_GoPLCScreen);
  lv_obj_set_style_bg_color(btnPlc, lv_color_make(60, 60, 60), 0);
  lv_obj_set_style_bg_grad_color(btnPlc, lv_color_make(60, 60, 60), 0);

  lv_createScreenWifi(ui_ScreenWifi);
  lv_createScreenPLC(ui_ScreenPLC);

  lv_scr_load(ui_ScreenMain);
}
