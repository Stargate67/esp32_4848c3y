
#include <MainScreen.h>


void InitUI(void){

// Initialize the display
  smartdisplay_init();

  // Set backlight to full brightness
  smartdisplay_lcd_set_backlight(0.6); // 0 is off, 0.5 is half and 1 is full brightness.

  __attribute__((unused)) auto disp = lv_disp_get_default();
  // lv_disp_set_rotation(disp, LV_DISP_ROT_90);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_180);
  // lv_disp_set_rotation(disp, LV_DISP_ROT_270);

  // Get the active screen
  scr = lv_scr_act();

  // Set screen background color to dark blue
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x090909), 0);

  // Create the Hello World label
  lv_CreateIPLabel(scr);
  lv_CreateClock(scr);
  lv_CreateAlarm(scr);
  lv_CreateHorScrollArea(scr);
  lv_createButton_R1(scr);
  lv_createButton_R2(scr);
  lv_createButton_R3(scr);
  lv_createButton_R4(scr);
}

void lv_CreateHorScrollArea(lv_obj_t *parent){
    HorScrollArea = lv_obj_create(parent);
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


void lv_createButton_R1(lv_obj_t *parent){
    // Créer l'interface utilisateur
    btnRelay_1 = lv_btn_create(parent);    // Créer un bouton
    lv_obj_set_size(btnRelay_1, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_1, LV_ALIGN_TOP_LEFT, 0, 360);      // Centrer le bouton
    lv_obj_set_style_bg_color(btnRelay_1, lv_color_make( 90, 90, 90 ), 0 );

    // Ajouter une étiquette au bouton
    lblBtnRelay_1 = lv_label_create(btnRelay_1);
    lv_label_set_text(lblBtnRelay_1, "CHAUD");
    lv_obj_center(lblBtnRelay_1);

    // Ajouter une action au bouton
    lv_obj_add_event_cb(btnRelay_1, my_event_cb_Relay_1, LV_EVENT_RELEASED, NULL);
}

void lv_createButton_R2(lv_obj_t *parent){
    btnRelay_2 = lv_btn_create(parent);
    lv_obj_set_size(btnRelay_2, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_2, LV_ALIGN_TOP_LEFT, 120, 360);
    lv_obj_set_style_bg_color(btnRelay_2, lv_color_make( 100, 100, 100 ), 0 );

    lblBtnRelay_2 = lv_label_create(btnRelay_2);
    lv_label_set_text(lblBtnRelay_2, "BOOST");
    lv_obj_center(lblBtnRelay_2);

    lv_obj_add_event_cb(btnRelay_2, my_event_cb_Relay_2, LV_EVENT_RELEASED, NULL);
}

void lv_createButton_R3(lv_obj_t *parent){
    btnRelay_3 = lv_btn_create(parent);
    lv_obj_set_size(btnRelay_3, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_3, LV_ALIGN_TOP_LEFT, 240, 360);
    lv_obj_set_style_bg_color(btnRelay_3, lv_color_make( 110, 110, 110 ), 0 );

    lblBtnRelay_3 = lv_label_create(btnRelay_3);
    lv_label_set_text(lblBtnRelay_3, "PLANCHER");
    lv_obj_center(lblBtnRelay_3);

    lv_obj_add_event_cb(btnRelay_3, my_event_cb_Relay_3, LV_EVENT_RELEASED, NULL);
}

void lv_createButton_R4(lv_obj_t *parent){
    btnRelay_4 = lv_btn_create(parent);
    lv_obj_set_size(btnRelay_4, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_4, LV_ALIGN_TOP_LEFT, 360, 360);
    lv_obj_set_style_bg_color(btnRelay_4, lv_color_make( 120, 120, 120 ), 0 );

    lblBtnRelay_4 = lv_label_create(btnRelay_4);
    lv_label_set_text(lblBtnRelay_4, "RADIAT");
    lv_obj_center(lblBtnRelay_4);

    lv_obj_add_event_cb(btnRelay_4, my_event_cb_Relay_4, LV_EVENT_RELEASED, NULL);
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
    lv_label_set_text(AlarmLabel, "Display the alarms...");
    lv_obj_set_style_text_font(AlarmLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(AlarmLabel, lv_color_make(255, 0, 0), 0); //Red
    lv_obj_align(AlarmLabel, LV_ALIGN_TOP_LEFT, 80, 0); 
}

void lv_CreateClock(lv_obj_t * parent)
{    // Create a label
    ClockLabel = lv_label_create(parent); 
    lv_label_set_text(ClockLabel, "HH:MM:SS");
    lv_obj_set_style_text_font(ClockLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(ClockLabel, lv_color_white(), 0);
    lv_obj_align(ClockLabel, LV_ALIGN_TOP_RIGHT, 0, 0); 
}

//************************************************************************************************************/
//============================================================================================================/

static void my_event_cb_Relay_1 (lv_event_t *e){
    compteur++;
    //Serial.println(compteur);

    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {

        if (digitalRead(RELAY_1) == HIGH ) {
            lv_label_set_text(lblBtnRelay_1, "R2_ON");
            digitalWrite(RELAY_1, LOW);
        } else {
            lv_label_set_text(lblBtnRelay_1, "R2_OFF");
            digitalWrite(RELAY_1, HIGH);
        }
    }
}

static void my_event_cb_Relay_2 (lv_event_t *e){
    compteur++;
    //Serial.println(compteur);

    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {

        if (digitalRead(RELAY_2) == HIGH ) {
            lv_label_set_text(lblBtnRelay_2, "R2_ON");
            digitalWrite(RELAY_2, LOW);
        } else {
            lv_label_set_text(lblBtnRelay_2, "R2_OFF");
            digitalWrite(RELAY_2, HIGH);
        }
    }
}

static void my_event_cb_Relay_3 (lv_event_t *e){

    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {
        compteur++;
        //Serial.println(compteur);

        if (digitalRead(RELAY_3) == HIGH ) {
            lv_label_set_text(lblBtnRelay_3, "R3_ON");
            digitalWrite(RELAY_3, LOW);
        } else {
            lv_label_set_text(lblBtnRelay_3, "R3_OFF");
            digitalWrite(RELAY_3, HIGH);
        }
    }
}

static void my_event_cb_Relay_4 (lv_event_t *e){

    lv_event_code_t code = lv_event_get_code(e);
    //lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_RELEASED) {
        compteur++;
        //Serial.println(compteur);

        if (bRelay_4) {
            //lv_label_set_text(lblBtnRelay_4, "R4_OFF");
            lv_obj_set_style_bg_color(btnRelay_4, lv_color_make( 120, 120, 120 ), 0 );
            bRelay_4 = false;
        } else {
            //lv_label_set_text(lblBtnRelay_4, "R4_ON");
            lv_obj_set_style_bg_color(btnRelay_4, lv_color_make( 0, 160, 60 ), 0 );
            bRelay_4 = true;
        }
    }
}