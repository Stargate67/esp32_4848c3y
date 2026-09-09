#include "Globals.h"

bool bRelay_4;
bool bRelay_5;
bool bAcquitAlarme;

uint32_t compteur;

lv_obj_t *btnR1Chaudiere;
lv_obj_t *btnR2BoostCh;
lv_obj_t *btnR3PpeRadiateur;
lv_obj_t *btnPpePlancher;
lv_obj_t *btnArriveeEau;

lv_obj_t *lblBtnR1Chaudiere;
lv_obj_t *lblBtnR1small;

lv_obj_t *lblBtnR2BoostCh;
lv_obj_t *lblBtnR2small;

lv_obj_t *lblBtnR3PpeRadiateur;
lv_obj_t *lblBtnR3small;

lv_obj_t *lblBtnPpePlancher;
lv_obj_t *lblBtnArriveeEau;

lv_obj_t * ui_Container1;
lv_obj_t * ui_Container2;
lv_obj_t * ui_LblMinExt;
lv_obj_t * ui_Container3;
lv_obj_t * ui_LblMaxExt;
lv_obj_t * ui_Chart1;

bool bCdeRelaisR1;  // demande de marche Relais 1
bool bCdeRelaisR2;  // demande de marche Relais 2
bool bCdeRelaisR3;  // demande de marche Relais 3

static const lv_color_t Btn_grad_colors[2] = {
    LV_COLOR_MAKE(10, 10, 10),
    LV_COLOR_MAKE(30, 30, 30),
};

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

  // Get the active screen
  lv_obj_t * ui_scr = lv_scr_act();

  // Set screen background color to dark blue
  lv_obj_set_style_bg_color(ui_scr, lv_color_hex(0x090909), 0);

  lv_CreateIPLabel(ui_scr);
  lv_CreateClock(ui_scr);
  lv_CreateAlarm(ui_scr);
  ui_Screen1_screen_init(ui_scr);
  ui_Screen1_screen_init_1(ui_scr);
  lv_createButton_CHAUD(ui_scr);
  lv_createButton_BOOSTCh(ui_scr);
  lv_createButton_RADIAT(ui_scr);
  lv_createButton_PLANCHER(ui_scr);
  lv_createButton_ArriveeEau(ui_scr);
}
