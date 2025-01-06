#include <Arduino.h>
#include <esp32_smartdisplay.h>
//#include <ui\ui.h>

#define LV_TICK_PERIOD_MS 5

lv_obj_t *btnRelay_1;
lv_obj_t *btnRelay_2;
lv_obj_t *btnRelay_3;
lv_obj_t *lblBtnRelay_1;
lv_obj_t *lblBtnRelay_2;
lv_obj_t *lblBtnRelay_3;
lv_obj_t *scr;
lv_obj_t *label;

uint32_t compteur;

static void my_event_cb_Relay_1 (lv_event_t *event);
static void my_event_cb_Relay_2 (lv_event_t *event);
static void my_event_cb_Relay_3 (lv_event_t *event);

// Event handler for the button
void btn_event_handler(lv_event_t *e) {

    compteur++;
    Serial.println(compteur);


    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = (lv_obj_t*)lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {

        Serial.println("btnRelay_1 Clicked !" + char(code));

        if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
            // Create an animation to increase the bar value gradually
            Serial.println("btnRelay_1 Checked !") ;
        } else {
            // Create an animation to decrease the bar value gradually
            Serial.println("btnRelay_1 Unchecked !");
        }
    }
}

void lv_example_button_R1(lv_obj_t *parent){
    // Créer l'interface utilisateur
    btnRelay_1 = lv_btn_create(parent);    // Créer un bouton
    lv_obj_set_size(btnRelay_1, 120, 200);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_1, LV_ALIGN_TOP_LEFT, 10, 50);      // Centrer le bouton

    // Ajouter une étiquette au bouton
    lblBtnRelay_1 = lv_label_create(btnRelay_1);
    lv_label_set_text(lblBtnRelay_1, "R1_OFF");
    lv_obj_center(lblBtnRelay_1);

    // Ajouter une action au bouton
    lv_obj_add_event_cb(btnRelay_1, my_event_cb_Relay_1, LV_EVENT_RELEASED, NULL);
}

void lv_example_button_R2(lv_obj_t *parent){
    btnRelay_2 = lv_btn_create(parent);
    lv_obj_set_size(btnRelay_2, 120, 200);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_2, LV_ALIGN_TOP_LEFT, 140, 50);
    //lv_obj_remove_flag(btnRelay_2, LV_OBJ_FLAG_PRESS_LOCK);

    lblBtnRelay_2 = lv_label_create(btnRelay_2);
    lv_label_set_text(lblBtnRelay_2, "R2_OFF");
    lv_obj_center(lblBtnRelay_2);

    lv_obj_add_event_cb(btnRelay_2, my_event_cb_Relay_2, LV_EVENT_RELEASED, NULL);
}

void lv_example_button_R3(lv_obj_t *parent){
    btnRelay_3 = lv_btn_create(parent);
    lv_obj_set_size(btnRelay_3, 120, 200);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_3, LV_ALIGN_TOP_LEFT, 270, 50);
    //lv_obj_remove_flag(btnRelay_2, LV_OBJ_FLAG_PRESS_LOCK);

    lblBtnRelay_3 = lv_label_create(btnRelay_3);
    lv_label_set_text(lblBtnRelay_3, "R3_OFF");
    lv_obj_center(lblBtnRelay_3);

    lv_obj_add_event_cb(btnRelay_3, my_event_cb_Relay_3, LV_EVENT_RELEASED, NULL);
}

void create_hello_world(lv_obj_t * parent)
{
    // Create a label
    label = lv_label_create(parent);
    
    // Set the text
    lv_label_set_text(label, "Hello, World!");
    
    // Increase the font size (multiplier for the default font size)
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
    
    // Set text color to white
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    
    // Center the label on the screen
    //lv_obj_center(label);
    lv_obj_set_pos(label, 180, 10); 
}

void setup()
{
    Serial.begin(115200);

    pinMode(RELAY_1, OUTPUT);
    pinMode(RELAY_2, OUTPUT);
    pinMode(RELAY_3, OUTPUT);
    digitalWrite(RELAY_1, LOW);
    digitalWrite(RELAY_2, LOW);
    digitalWrite(RELAY_3, LOW);

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
    create_hello_world(scr);
    lv_example_button_R1(scr);
    lv_example_button_R2(scr);
    lv_example_button_R3(scr);
    // Add the event handler to the button

}

void loop() {
 
    // Update the ticker
    static auto lv_last_tick = millis();

    auto const now = millis();
    lv_tick_inc(now - lv_last_tick);
    lv_last_tick = now;
    // Update the UI
    lv_timer_handler();

}



//************************************************************************************************************/
//============================================================================================================/

static void my_event_cb_Relay_1 (lv_event_t *e){
    compteur++;
    Serial.println(compteur);

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
    Serial.println(compteur);

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
        Serial.println(compteur);

        if (digitalRead(RELAY_3) == HIGH ) {
            lv_label_set_text(lblBtnRelay_3, "R3_ON");
            digitalWrite(RELAY_3, LOW);
        } else {
            lv_label_set_text(lblBtnRelay_3, "R3_OFF");
            digitalWrite(RELAY_3, HIGH);
        }
    }
}
