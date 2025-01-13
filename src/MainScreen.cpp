#include "MainScreen.h"
#include "Globals.h"

bool bRelay_4;

uint32_t compteur;

lv_obj_t *btnR1Chaudiere;
lv_obj_t *btnR2BoostCh;
lv_obj_t *btnR3PpeRadiateur;
lv_obj_t *btnPpePlancher;

lv_obj_t *lblBtnR1Chaudiere;
lv_obj_t *lblBtnR2BoostCh;
lv_obj_t *lblBtnR3PpeRadiateur;
lv_obj_t *lblBtnPpePlancher;

// test divers widgets
void lv_example_label_1(void)
{
    lv_obj_t * label2 = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(label2, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
    lv_obj_set_width(label2, 150);
    lv_label_set_text(label2, "It is a circularly scrolling text. ");
    lv_obj_align(label2, LV_ALIGN_CENTER, 0, 40);
}

//************************************************************************************************************/
//============================================================================================================/

static void my_event_cb_R1Chaudiere (lv_event_t *e){
    compteur++;
    //Serial.println(compteur);

    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {

        if (digitalRead(RELAY_1) == HIGH ) {
            lv_label_set_text(lblBtnR1Chaudiere, "R2_ON");
            digitalWrite(RELAY_1, LOW);
        } else {
            lv_label_set_text(lblBtnR1Chaudiere, "R2_OFF");
            digitalWrite(RELAY_1, HIGH);
        }
    }
}

static void my_event_cb_R2BoostCh (lv_event_t *e){
    compteur++;
    //Serial.println(compteur);

    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {

        if (digitalRead(RELAY_2) == HIGH ) {
            //writeCoil(MBremote, 212, true, nullptr, 1);

            lv_label_set_text(lblBtnR2BoostCh, "R2_ON");
            digitalWrite(RELAY_2, LOW);
        } else {
            lv_label_set_text(lblBtnR2BoostCh, "R2_OFF");
            digitalWrite(RELAY_2, HIGH);
        }
    }
}

static void my_event_cb_R3PpeRadiateur (lv_event_t *e){

    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {
        compteur++;
        //Serial.println(compteur);

        if (digitalRead(RELAY_3) == HIGH ) {
            lv_label_set_text(lblBtnR3PpeRadiateur, "R3_ON");
            digitalWrite(RELAY_3, LOW);
        } else {
            lv_label_set_text(lblBtnR3PpeRadiateur, "R3_OFF");
            digitalWrite(RELAY_3, HIGH);
        }
    }
}

static void my_event_cb_PpePlancher (lv_event_t *e){

    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {
        compteur++;
        //Serial.println(compteur);

        if (bRelay_4) {
            //lv_label_set_text(lblBtnPpePlancher, "R4_OFF");
            lv_obj_set_style_bg_color(btnPpePlancher, lv_color_make( 120, 120, 120 ), 0 );
            bRelay_4 = false;
        } else {
            //lv_label_set_text(lblBtnPpePlancher, "R4_ON");
            lv_obj_set_style_bg_color(btnPpePlancher, lv_color_make( 0, 160, 60 ), 0 );
            bRelay_4 = true;
        }
    }
}

void lv_CreateHorScrollArea(lv_obj_t *parent){
    lv_obj_t * HorScrollArea = lv_obj_create(parent);
    lv_obj_set_size(HorScrollArea, 480, 130);
    lv_obj_set_pos(HorScrollArea, 0, 30);
    lv_obj_set_style_bg_color(HorScrollArea, lv_color_make( 30, 30, 30 ), 0 );
    lv_obj_set_style_border_width(HorScrollArea, 0, 0 );
    lv_obj_set_style_pad_all(HorScrollArea, 0, 0);
    //lv_obj_set_style_outline_color(HorScrollArea, lv_color_make( 125, 125, 235 ), 0 ); 
    //lv_obj_set_style_radius(HorScrollArea, 0, 0 );
    //lv_obj_set_style_outline_width(HorScrollArea, 0, 0 );
 
    // Ajouter une étiquette au bouton
    lblScrolTxt_1 = lv_label_create(HorScrollArea);
    lv_label_set_text(lblScrolTxt_1, "lblScrolTxt_1  init");
    lv_obj_set_style_text_font(lblScrolTxt_1, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(lblScrolTxt_1, lv_color_make(2, 40, 209), 0); 
    lv_obj_align(lblScrolTxt_1, LV_ALIGN_TOP_LEFT, 0, 10); 

    lblScrolTxt_2 = lv_label_create(HorScrollArea);
    lv_label_set_text(lblScrolTxt_2, "lblScrolTxt_2  init");
    lv_obj_set_style_text_font(lblScrolTxt_2, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(lblScrolTxt_2, lv_color_make(255, 255, 255), 0); 
    lv_obj_align(lblScrolTxt_2, LV_ALIGN_TOP_LEFT, 0, 50); 

    lblScrolTxt_3 = lv_label_create(HorScrollArea);
    lv_label_set_text(lblScrolTxt_3, "lblScrolTxt_2  init");
    lv_obj_set_style_text_font(lblScrolTxt_3, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(lblScrolTxt_3, lv_color_make(0, 255, 255), 0); 
    lv_obj_align(lblScrolTxt_3, LV_ALIGN_TOP_LEFT, 0, 90); 
}


void lv_createButton_CHAUD(lv_obj_t *parent){
    // Créer l'interface utilisateur
    btnR1Chaudiere = lv_btn_create(parent);    // Créer un bouton
    lv_obj_set_size(btnR1Chaudiere, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnR1Chaudiere, LV_ALIGN_TOP_LEFT, 0, 360);      // Centrer le bouton
    lv_obj_set_style_bg_color(btnR1Chaudiere, lv_color_make( 90, 90, 90 ), 0 );

    // Ajouter une étiquette au bouton
    lblBtnR1Chaudiere = lv_label_create(btnR1Chaudiere);
    lv_label_set_text(lblBtnR1Chaudiere, "CHAUD");
    lv_obj_center(lblBtnR1Chaudiere);

    // Ajouter une action au bouton
    lv_obj_add_event_cb(btnR1Chaudiere, my_event_cb_R1Chaudiere, LV_EVENT_RELEASED, NULL);
}

void lv_createButton_BOOSTCh(lv_obj_t *parent){
    btnR2BoostCh = lv_btn_create(parent);
    lv_obj_set_size(btnR2BoostCh, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnR2BoostCh, LV_ALIGN_TOP_LEFT, 120, 360);
    lv_obj_set_style_bg_color(btnR2BoostCh, lv_color_make( 100, 100, 100 ), 0 );

    lblBtnR2BoostCh = lv_label_create(btnR2BoostCh);
    lv_label_set_text(lblBtnR2BoostCh, "BOOST");
    lv_obj_center(lblBtnR2BoostCh);

    lv_obj_add_event_cb(btnR2BoostCh, my_event_cb_R2BoostCh, LV_EVENT_RELEASED, NULL);
}

void lv_createButton_RADIAT(lv_obj_t *parent){
    btnR3PpeRadiateur = lv_btn_create(parent);
    lv_obj_set_size(btnR3PpeRadiateur, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnR3PpeRadiateur, LV_ALIGN_TOP_LEFT, 240, 360);
    lv_obj_set_style_bg_color(btnR3PpeRadiateur, lv_color_make( 110, 110, 110 ), 0 );

    lblBtnR3PpeRadiateur = lv_label_create(btnR3PpeRadiateur);
    lv_label_set_text(lblBtnR3PpeRadiateur, "RADIAT");
    lv_obj_center(lblBtnR3PpeRadiateur);

    lv_obj_add_event_cb(btnR3PpeRadiateur, my_event_cb_R3PpeRadiateur, LV_EVENT_RELEASED, NULL);
}

void lv_createButton_PLANCHER(lv_obj_t *parent){
    btnPpePlancher = lv_btn_create(parent);
    lv_obj_set_size(btnPpePlancher, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnPpePlancher, LV_ALIGN_TOP_LEFT, 360, 360);
    lv_obj_set_style_bg_color(btnPpePlancher, lv_color_make( 120, 120, 120 ), 0 );

    lblBtnPpePlancher = lv_label_create(btnPpePlancher);
    lv_label_set_text(lblBtnPpePlancher, "PLANCHER");
    lv_obj_center(lblBtnPpePlancher);

    lv_obj_add_event_cb(btnPpePlancher, my_event_cb_PpePlancher, LV_EVENT_RELEASED, NULL);
}


void lv_CreateIPLabel(lv_obj_t * parent)
{
    IPLabel = lv_label_create(parent); 
    lv_label_set_text(IPLabel, "XXX.XXX.XXX.XXX");
    lv_obj_set_style_text_font(IPLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(IPLabel, lv_color_make(255, 250, 0), 0); //yellow
    lv_obj_align(IPLabel, LV_ALIGN_TOP_LEFT, 0, 3); 
}

void lv_CreateAlarm(lv_obj_t * parent)
{
    AlarmLabel = lv_label_create(parent);
    lv_obj_set_width(AlarmLabel, 310);
    lv_label_set_text(AlarmLabel, "...Pas d'alarme...");
    lv_obj_set_style_text_font(AlarmLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(AlarmLabel, lv_color_make(255, 0, 0), 0); //Red
    lv_obj_align(AlarmLabel, LV_ALIGN_TOP_LEFT, 80, 0); 
    lv_label_set_long_mode(AlarmLabel, LV_LABEL_LONG_SCROLL_CIRCULAR);
    //lv_obj_set_scroll_dir(AlarmLabel, LV_DIR_LEFT);
}

void lv_CreateClock(lv_obj_t * parent)
{    // Create a label
    ClockLabel = lv_label_create(parent); 
    lv_label_set_text(ClockLabel, "HH:MM:SS");
    lv_obj_set_style_text_font(ClockLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(ClockLabel, lv_color_white(), 0);
    lv_obj_align(ClockLabel, LV_ALIGN_TOP_RIGHT, 0, 0); 
}


void InitUI(){

// Initialize the display
  smartdisplay_init();

  // Set backlight to full brightness
  smartdisplay_lcd_set_backlight(0.6); // 0 is off, 0.5 is half and 1 is full brightness.

  __attribute__((unused)) auto disp = lv_disp_get_default();
  // lv_disp_set_rotation(disp, LV_DISP_ROT_90);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_180);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_270);

  // Get the active screen
  lv_obj_t * scr = lv_scr_act();

  // Set screen background color to dark blue
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x090909), 0);

  // Create the Hello World label
  lv_CreateIPLabel(scr);
  lv_CreateClock(scr);
  lv_CreateAlarm(scr);
  lv_CreateHorScrollArea(scr);
  lv_createButton_CHAUD(scr);
  lv_createButton_BOOSTCh(scr);
  lv_createButton_RADIAT(scr);
  lv_createButton_PLANCHER(scr);
  //lv_example_label_1();
}
