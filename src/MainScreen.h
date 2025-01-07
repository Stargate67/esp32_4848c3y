/**
 * @file MainScreen.h
 * This file exists only to be compatible with Arduino's library structure
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <esp32_smartdisplay.h>

uint32_t compteur;

lv_obj_t *btnRelay_1;
lv_obj_t *btnRelay_2;
lv_obj_t *btnRelay_3;
lv_obj_t *lblBtnRelay_1;
lv_obj_t *lblBtnRelay_2;
lv_obj_t *lblBtnRelay_3;
lv_obj_t *scr;
lv_obj_t *IPLabel;
lv_obj_t *AlarmLabel;
lv_obj_t *ClockLabel;

static void my_event_cb_Relay_1 (lv_event_t * e);
static void my_event_cb_Relay_2 (lv_event_t * e);
static void my_event_cb_Relay_3 (lv_event_t * e);

void CreateIPLabel(lv_obj_t * parent);
void CreateAlarm(lv_obj_t * parent);
void CreateClock(lv_obj_t * parent);
void lv_example_button_R1(lv_obj_t * parent);
void lv_example_button_R2(lv_obj_t * parent);
void lv_example_button_R3(lv_obj_t * parent);

#ifdef __cplusplus
} /*extern "C"*/
#endif
