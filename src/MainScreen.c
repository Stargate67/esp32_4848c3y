
#include <MainScreen.h>
#include <MainScreen.h>

// Event handler for the button
void btn_event_handler(lv_event_t *e) {

    compteur++;

    //Serial.println(compteur);


    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {

        //Serial.println("btnRelay_1 Clicked !" + char(code));

        if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
            // Create an animation to increase the bar value gradually
            //Serial.println("btnRelay_1 Checked !") ;
        } else {
            // Create an animation to decrease the bar value gradually
            //Serial.println("btnRelay_1 Unchecked !");
        }
    }
}

void lv_example_button_R1(lv_obj_t *parent){
    // Créer l'interface utilisateur
    btnRelay_1 = lv_btn_create(parent);    // Créer un bouton
    lv_obj_set_size(btnRelay_1, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_1, LV_ALIGN_TOP_LEFT, 10, 360);      // Centrer le bouton

    // Ajouter une étiquette au bouton
    lblBtnRelay_1 = lv_label_create(btnRelay_1);
    lv_label_set_text(lblBtnRelay_1, "R1_OFF");
    lv_obj_center(lblBtnRelay_1);

    // Ajouter une action au bouton
    lv_obj_add_event_cb(btnRelay_1, my_event_cb_Relay_1, LV_EVENT_RELEASED, NULL);
}

void lv_example_button_R2(lv_obj_t *parent){
    btnRelay_2 = lv_btn_create(parent);
    lv_obj_set_size(btnRelay_2, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_2, LV_ALIGN_TOP_LEFT, 140, 360);
    //lv_obj_remove_flag(btnRelay_2, LV_OBJ_FLAG_PRESS_LOCK);

    lblBtnRelay_2 = lv_label_create(btnRelay_2);
    lv_label_set_text(lblBtnRelay_2, "R2_OFF");
    lv_obj_center(lblBtnRelay_2);

    lv_obj_add_event_cb(btnRelay_2, my_event_cb_Relay_2, LV_EVENT_RELEASED, NULL);
}

void lv_example_button_R3(lv_obj_t *parent){
    btnRelay_3 = lv_btn_create(parent);
    lv_obj_set_size(btnRelay_3, 120, 120);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_3, LV_ALIGN_TOP_LEFT, 270, 360);
    //lv_obj_remove_flag(btnRelay_2, LV_OBJ_FLAG_PRESS_LOCK);

    lblBtnRelay_3 = lv_label_create(btnRelay_3);
    lv_label_set_text(lblBtnRelay_3, "R3_OFF");
    lv_obj_center(lblBtnRelay_3);

    lv_obj_add_event_cb(btnRelay_3, my_event_cb_Relay_3, LV_EVENT_RELEASED, NULL);
}

void CreateIPLabel(lv_obj_t * parent)
{
    IPLabel = lv_label_create(parent); 
    lv_label_set_text(IPLabel, "XXX.XXX.XXX.XXX");
    lv_obj_set_style_text_font(IPLabel, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(IPLabel, lv_color_make(255, 250, 0), 0); //yellow
    lv_obj_align(IPLabel, LV_ALIGN_TOP_LEFT, 0, 3); 
}

void CreateAlarm(lv_obj_t * parent)
{
    AlarmLabel = lv_label_create(parent); 
    lv_label_set_text(AlarmLabel, "Dislay the alarms...");
    lv_obj_set_style_text_font(AlarmLabel, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(AlarmLabel, lv_color_make(255, 0, 0), 0); //Red
    lv_obj_align(AlarmLabel, LV_ALIGN_TOP_LEFT, 80, 0); 
}

void CreateClock(lv_obj_t * parent)
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
