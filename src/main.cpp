#include <Arduino.h>
#include <esp32_smartdisplay.h>
#include <MainScreen.h>



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



