//#include <Arduino.h>
#include <esp32_smartdisplay.h>

lv_obj_t *btnRelay_1;
lv_obj_t *btnRelay_2;
lv_obj_t *btnRelay_3;
lv_obj_t *lblBtnRelay_1;
lv_obj_t *lblBtnRelay_2;
lv_obj_t *lblBtnRelay_3;

lv_obj_t *label;

static void my_event_cb_Relay_1 (lv_event_t * event);
static void my_event_cb_Relay_2 (lv_event_t * event);

// https://www.youtube.com/nishad2m8
// https://buymeacoffee.com/nishad2m8

void lv_example_button_R2(lv_obj_t * parent)
{
    btnRelay_2 = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btnRelay_2, my_event_cb_Relay_2, LV_EVENT_ALL, NULL);
    lv_obj_align(btnRelay_2, LV_ALIGN_CENTER, 0, -40);
    lv_obj_remove_flag(btnRelay_2, LV_OBJ_FLAG_PRESS_LOCK);

    lblBtnRelay_2 = lv_label_create(btnRelay_2);
    lv_label_set_text(lblBtnRelay_2, "R2_OFF");
    lv_obj_center(lblBtnRelay_2);
}

void lv_example_button_R1(lv_obj_t * parent){
    // Créer l'interface utilisateur
    btnRelay_1 = lv_btn_create(parent);    // Créer un bouton
    lv_obj_set_size(btnRelay_1, 100, 50);                 // Définir la taille du bouton
    lv_obj_align(btnRelay_1, LV_ALIGN_CENTER, 0, 0);      // Centrer le bouton

    // Ajouter une étiquette au bouton
    lblBtnRelay_1 = lv_label_create(btnRelay_1);
    lv_label_set_text(lblBtnRelay_1, "R1_OFF");

    // Ajouter une action au bouton
    lv_obj_add_event_cb(btnRelay_1, my_event_cb_Relay_1, LV_EVENT_CLICKED, NULL);
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
    lv_obj_center(label);
}

void setup()
{
    Serial.begin(115200);
    
    // Initialize the display
    smartdisplay_init();
    
    // Set backlight to full brightness
    smartdisplay_lcd_set_backlight(1); // 0 is off, 0.5 is half and 1 is full brightness.

    __attribute__((unused)) auto disp = lv_disp_get_default();
    // lv_disp_set_rotation(disp, LV_DISP_ROT_90);
    // lv_disp_set_rotation(disp, LV_DISP_ROT_180);
    // lv_disp_set_rotation(disp, LV_DISP_ROT_270);
    
    // Get the active screen
    lv_obj_t * scr = lv_scr_act();
    
    // Set screen background color to dark blue
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0000A0), 0);
    
    // Create the Hello World label
    create_hello_world(scr);
    lv_example_button_R1(scr);
    lv_example_button_R2(scr);
}

void loop()
{
    // Handle LVGL tasks
    lv_timer_handler();
    delay(5);
}

//************************************************************************************************************/
//============================================================================================================/

static void my_event_cb_Relay_1 (lv_event_t * event){
    pinMode(RELAY_1, OUTPUT);
    if (digitalRead(RELAY_1) == HIGH ) {
        lv_label_set_text(lblBtnRelay_1, "R1_ON");
        digitalWrite(RELAY_1, LOW);
    } else {
        lv_label_set_text(lblBtnRelay_1, "R1_OFF");
        digitalWrite(RELAY_1, HIGH);
    }

    //ledState = !ledState;
    //digitalWrite(RELAY_1, RELAY_1 ? HIGH : LOW);
    Serial.println("Clicked !");
}

static void my_event_cb_Relay_2 (lv_event_t * event){
    pinMode(RELAY_2, OUTPUT);
    if (digitalRead(RELAY_2) == HIGH ) {
        lv_label_set_text(lblBtnRelay_1, "R2_ON");
        digitalWrite(RELAY_2, LOW);
    } else {
        lv_label_set_text(lblBtnRelay_1, "R2_OFF");
        digitalWrite(RELAY_2, HIGH);
    }
}
