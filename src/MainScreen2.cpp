
#include "Globals.h"
#include "MainScreen2.h"

lv_obj_t * ui_Cont1Val;
lv_obj_t * ui_LblPlancher;
//lv_obj_t * ui_LblValPlancher;
lv_obj_t * ui_LblConsPlancher;
//lv_obj_t * ui_LblValConsPlancher;
lv_obj_t * ui_LblECS;
//lv_obj_t * ui_LblValECS;

lv_obj_t * ui_Cont2Val;
lv_obj_t * ui_LblRadiat;
//lv_obj_t * ui_LblValRadiat;
lv_obj_t * ui_LblDebitRadiat;
//lv_obj_t * ui_LblValDebitRadit;
lv_obj_t * ui_LblCourant;
//lv_obj_t * ui_LblValCourant;

lv_obj_t * ui_ContConso;
lv_obj_t * ui_LblConsoEau;
lv_obj_t * ui_LblConsoElec;
lv_obj_t * ui_LblConsoGaz;
//lv_obj_t * ui_LblValConsoJEau;
//lv_obj_t * ui_LblValConsoJElec;
//lv_obj_t * ui_LblValConsoJGaz;
//lv_obj_t * ui_LblValConsoJ1Elec;
//lv_obj_t * ui_LblValConsoJ1Eau;
//lv_obj_t * ui_LblValConsoJ1Gaz;
lv_obj_t * ui_LblTitreConso;

void ui_Screen1_screen_init_1(lv_obj_t *parent) {

    // ===========  CONTAINER 1 Val ANA  =============================
    ui_Cont1Val = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_Cont1Val);
    lv_obj_set_width(ui_Cont1Val, 239);
    lv_obj_set_height(ui_Cont1Val, 100);
    lv_obj_set_x(ui_Cont1Val, 0);
    lv_obj_set_y(ui_Cont1Val, 152);
     lv_obj_set_style_bg_color(ui_Cont1Val, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Cont1Val, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblPlancher = lv_label_create(ui_Cont1Val);
    lv_obj_set_width(ui_LblPlancher, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblPlancher, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblPlancher, 0);
    lv_obj_set_y(ui_LblPlancher, 10);
    lv_label_set_long_mode(ui_LblPlancher, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblPlancher, "T. Plancher");
    lv_obj_set_scrollbar_mode(ui_LblPlancher, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblPlancher, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblPlancher, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblPlancher, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblPlancher, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValPlancher = lv_label_create(ui_Cont1Val);
    lv_obj_set_width(ui_LblValPlancher, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValPlancher, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValPlancher, -5);
    lv_obj_set_y(ui_LblValPlancher, 8);
    lv_obj_set_align(ui_LblValPlancher, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValPlancher, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValPlancher, "XX.X°C");
     lv_obj_set_scrollbar_mode(ui_LblValPlancher, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValPlancher, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValPlancher, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValPlancher, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValPlancher, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblConsPlancher = lv_label_create(ui_Cont1Val);
    lv_obj_set_width(ui_LblConsPlancher, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblConsPlancher, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblConsPlancher, 0);
    lv_obj_set_y(ui_LblConsPlancher, 38);
    lv_label_set_long_mode(ui_LblConsPlancher, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblConsPlancher, "C. Plancher");
    lv_obj_set_scrollbar_mode(ui_LblConsPlancher, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblConsPlancher, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblConsPlancher, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblConsPlancher, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblConsPlancher, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValConsPlancher = lv_label_create(ui_Cont1Val);
    lv_obj_set_width(ui_LblValConsPlancher, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValConsPlancher, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValConsPlancher, -5);
    lv_obj_set_y(ui_LblValConsPlancher, 36);
    lv_obj_set_align(ui_LblValConsPlancher, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValConsPlancher, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValConsPlancher, "XX.X°C");
    lv_obj_set_scrollbar_mode(ui_LblValConsPlancher, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValConsPlancher, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValConsPlancher, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValConsPlancher, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValConsPlancher, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblECS = lv_label_create(ui_Cont1Val);
    lv_obj_set_width(ui_LblECS, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblECS, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblECS, 0);
    lv_obj_set_y(ui_LblECS, 66);
    lv_label_set_long_mode(ui_LblECS, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblECS, "Ballon EC");
    lv_obj_set_scrollbar_mode(ui_LblECS, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblECS, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblECS, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblECS, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblECS, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValECS = lv_label_create(ui_Cont1Val);
    lv_obj_set_width(ui_LblValECS, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValECS, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValECS, -5);
    lv_obj_set_y(ui_LblValECS, 64);
    lv_obj_set_align(ui_LblValECS, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValECS, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValECS, "XX.X°C");
     lv_obj_set_scrollbar_mode(ui_LblValECS, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValECS, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValECS, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValECS, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValECS, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ===========  CONTAINER 2 Valeur ANA  =============================
    ui_Cont2Val = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_Cont2Val);
    lv_obj_set_width(ui_Cont2Val, 240);
    lv_obj_set_height(ui_Cont2Val, 100);
    lv_obj_set_x(ui_Cont2Val, 240);
    lv_obj_set_y(ui_Cont2Val, 152);
    lv_obj_set_style_bg_color(ui_Cont2Val, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Cont2Val, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValRadiat = lv_label_create(ui_Cont2Val);
    lv_obj_set_width(ui_LblValRadiat, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValRadiat, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValRadiat, -5);
    lv_obj_set_y(ui_LblValRadiat, 8);
    lv_obj_set_align(ui_LblValRadiat, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValRadiat, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValRadiat, "XX.X°C");
    lv_obj_set_scrollbar_mode(ui_LblValRadiat, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValRadiat, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValRadiat, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValRadiat, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValRadiat, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblRadiat = lv_label_create(ui_Cont2Val);
    lv_obj_set_width(ui_LblRadiat, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblRadiat, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblRadiat, 5);
    lv_obj_set_y(ui_LblRadiat, 10);
    lv_label_set_long_mode(ui_LblRadiat, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblRadiat, "T. Radiat.");
    lv_obj_set_scrollbar_mode(ui_LblRadiat, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblRadiat, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblRadiat, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblRadiat, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblRadiat, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblDebitRadiat = lv_label_create(ui_Cont2Val);
    lv_obj_set_width(ui_LblDebitRadiat, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblDebitRadiat, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblDebitRadiat, 5);
    lv_obj_set_y(ui_LblDebitRadiat, 38);
    lv_label_set_long_mode(ui_LblDebitRadiat, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblDebitRadiat, "Debit Rad.");
    lv_obj_set_scrollbar_mode(ui_LblDebitRadiat, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblDebitRadiat, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblDebitRadiat, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblDebitRadiat, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblDebitRadiat, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValDebitRadit = lv_label_create(ui_Cont2Val);
    lv_obj_set_width(ui_LblValDebitRadit, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValDebitRadit, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValDebitRadit, -5);
    lv_obj_set_y(ui_LblValDebitRadit, 36);
    lv_obj_set_align(ui_LblValDebitRadit, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValDebitRadit, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValDebitRadit, "XX.X l/m");
    lv_obj_set_scrollbar_mode(ui_LblValDebitRadit, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValDebitRadit, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValDebitRadit, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValDebitRadit, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValDebitRadit, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblCourant = lv_label_create(ui_Cont2Val);
    lv_obj_set_width(ui_LblCourant, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblCourant, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblCourant, 5);
    lv_obj_set_y(ui_LblCourant, 66);
    lv_label_set_long_mode(ui_LblCourant, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblCourant, "Courant");
    lv_obj_set_scrollbar_mode(ui_LblCourant, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblCourant, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblCourant, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblCourant, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblCourant, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValCourant = lv_label_create(ui_Cont2Val);
    lv_obj_set_width(ui_LblValCourant, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValCourant, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValCourant, -5);
    lv_obj_set_y(ui_LblValCourant, 64);
    lv_obj_set_align(ui_LblValCourant, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValCourant, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValCourant, "XX.X A");
    lv_obj_set_scrollbar_mode(ui_LblValCourant, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValCourant, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValCourant, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValCourant, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValCourant, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ===========  CONTAINER CONSO  =============================
 
    ui_ContConso = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_ContConso);
    lv_obj_set_width(ui_ContConso, 330);
    lv_obj_set_height(ui_ContConso, 105);
    lv_obj_set_x(ui_ContConso, 0);
    lv_obj_set_y(ui_ContConso, 254);
    lv_obj_set_style_bg_color(ui_ContConso, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ContConso, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblConsoEau = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblConsoEau, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblConsoEau, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblConsoEau, 0);
    lv_obj_set_y(ui_LblConsoEau, 21);
    lv_label_set_long_mode(ui_LblConsoEau, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblConsoEau, "Eau");
    lv_obj_set_scrollbar_mode(ui_LblConsoEau, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblConsoEau, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblConsoEau, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblConsoEau, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblConsoEau, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValConsoJEau = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblValConsoJEau, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValConsoJEau, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValConsoJEau, -170);
    lv_obj_set_y(ui_LblValConsoJEau, 19);
    lv_obj_set_align(ui_LblValConsoJEau, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValConsoJEau, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValConsoJEau, "XXXX");
    lv_obj_set_scrollbar_mode(ui_LblValConsoJEau, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValConsoJEau, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValConsoJEau, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValConsoJEau, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValConsoJEau, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValConsoJ1Eau = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblValConsoJ1Eau, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValConsoJ1Eau, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValConsoJ1Eau, -45);
    lv_obj_set_y(ui_LblValConsoJ1Eau, 19);
    lv_obj_set_align(ui_LblValConsoJ1Eau, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValConsoJ1Eau, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValConsoJ1Eau, "XXXX l");
    lv_obj_set_scrollbar_mode(ui_LblValConsoJ1Eau, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValConsoJ1Eau, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValConsoJ1Eau, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValConsoJ1Eau, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValConsoJ1Eau, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblConsoElec = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblConsoElec, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblConsoElec, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblConsoElec, 0);
    lv_obj_set_y(ui_LblConsoElec, 49);
    lv_label_set_long_mode(ui_LblConsoElec, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblConsoElec, "Elec");
    lv_obj_set_scrollbar_mode(ui_LblConsoElec, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblConsoElec, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblConsoElec, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblConsoElec, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblConsoElec, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValConsoJElec = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblValConsoJElec, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValConsoJElec, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValConsoJElec, -170);
    lv_obj_set_y(ui_LblValConsoJElec, 47);
    lv_obj_set_align(ui_LblValConsoJElec, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValConsoJElec, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValConsoJElec, "XXXX");
   lv_obj_set_scrollbar_mode(ui_LblValConsoJElec, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValConsoJElec, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValConsoJElec, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValConsoJElec, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValConsoJElec, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValConsoJ1Elec = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblValConsoJ1Elec, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValConsoJ1Elec, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValConsoJ1Elec, 0);
    lv_obj_set_y(ui_LblValConsoJ1Elec, 47);
    lv_obj_set_align(ui_LblValConsoJ1Elec, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValConsoJ1Elec, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValConsoJ1Elec, "XXXX Kwh");
    lv_obj_set_scrollbar_mode(ui_LblValConsoJ1Elec, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValConsoJ1Elec, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValConsoJ1Elec, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValConsoJ1Elec, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValConsoJ1Elec, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblConsoGaz = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblConsoGaz, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblConsoGaz, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblConsoGaz, 0);
    lv_obj_set_y(ui_LblConsoGaz, 77);
    lv_label_set_long_mode(ui_LblConsoGaz, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblConsoGaz, "Gaz");
   lv_obj_set_scrollbar_mode(ui_LblConsoGaz, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblConsoGaz, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblConsoGaz, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblConsoGaz, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblConsoGaz, &lv_font_montserrat_20, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValConsoJGaz = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblValConsoJGaz, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValConsoJGaz, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValConsoJGaz, -170);
    lv_obj_set_y(ui_LblValConsoJGaz, 75);
    lv_obj_set_align(ui_LblValConsoJGaz, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValConsoJGaz, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValConsoJGaz, "XXX");
    lv_obj_set_scrollbar_mode(ui_LblValConsoJGaz, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValConsoJGaz, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValConsoJGaz, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValConsoJGaz, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValConsoJGaz, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValConsoJ1Gaz = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblValConsoJ1Gaz, LV_SIZE_CONTENT);   /// 480
    lv_obj_set_height(ui_LblValConsoJ1Gaz, LV_SIZE_CONTENT);    /// 50
    lv_obj_set_x(ui_LblValConsoJ1Gaz, 0);
    lv_obj_set_y(ui_LblValConsoJ1Gaz, 75);
    lv_obj_set_align(ui_LblValConsoJ1Gaz, LV_ALIGN_TOP_RIGHT);
    lv_label_set_long_mode(ui_LblValConsoJ1Gaz, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(ui_LblValConsoJ1Gaz, "XXX Nm3");
    lv_obj_set_scrollbar_mode(ui_LblValConsoJ1Gaz, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(ui_LblValConsoJ1Gaz, LV_DIR_LEFT);
    lv_obj_set_style_text_color(ui_LblValConsoJ1Gaz, lv_color_hex(0x00FFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblValConsoJ1Gaz, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_LblValConsoJ1Gaz, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblTitreConso = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblTitreConso, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LblTitreConso, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ui_LblTitreConso, "Conso.           Jour            Jour-1");
    lv_obj_set_style_text_color(ui_LblTitreConso, lv_color_hex(0x0069FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblTitreConso, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}