
#include "Globals.h"

lv_obj_t * ui_Cont1Val;
lv_obj_t * ui_LblPlancher;
lv_obj_t * ui_LblConsPlancher;
lv_obj_t * ui_LblECS;

lv_obj_t * ui_Cont2Val;
lv_obj_t * ui_LblRadiat;
lv_obj_t * ui_LblDebitRadiat;
lv_obj_t * ui_LblCourant;

lv_obj_t * ui_ContConso;
lv_obj_t * ui_LblConsoEau;
lv_obj_t * ui_LblConsoElec;
lv_obj_t * ui_LblConsoGaz;
lv_obj_t * ui_LblTitreConso;

// Cree un label "valeur" standard: taille auto, position (+alignement optionnel), style de texte.
static lv_obj_t* createValueLabel(lv_obj_t *parent, lv_coord_t x, lv_coord_t y, lv_align_t align,
                                   const char *text, uint32_t colorHex, const lv_font_t *font){
    lv_obj_t *lbl = lv_label_create(parent);
    lv_obj_set_width(lbl, LV_SIZE_CONTENT);
    lv_obj_set_height(lbl, LV_SIZE_CONTENT);
    lv_obj_set_x(lbl, x);
    lv_obj_set_y(lbl, y);
    lv_obj_set_align(lbl, align);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_SCROLL);
    lv_label_set_text(lbl, text);
    lv_obj_set_scrollbar_mode(lbl, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(lbl, LV_DIR_LEFT);
    lv_obj_set_style_text_color(lbl, lv_color_hex(colorHex), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(lbl, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl, font, LV_PART_MAIN | LV_STATE_DEFAULT);
    return lbl;
}

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

    ui_LblPlancher = createValueLabel(ui_Cont1Val, 0, 10, LV_ALIGN_DEFAULT, "T. Plancher", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblValPlancher = createValueLabel(ui_Cont1Val, -5, 8, LV_ALIGN_TOP_RIGHT, "XX.X°C", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblConsPlancher = createValueLabel(ui_Cont1Val, 0, 38, LV_ALIGN_DEFAULT, "C. Plancher", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblValConsPlancher = createValueLabel(ui_Cont1Val, -5, 36, LV_ALIGN_TOP_RIGHT, "XX.X°C", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblECS = createValueLabel(ui_Cont1Val, 0, 66, LV_ALIGN_DEFAULT, "Ballon EC", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblValECS = createValueLabel(ui_Cont1Val, -5, 64, LV_ALIGN_TOP_RIGHT, "XX.X°C", 0x00FFFF, &lv_font_montserrat_22);

    // ===========  CONTAINER 2 Valeur ANA  =============================
    ui_Cont2Val = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_Cont2Val);
    lv_obj_set_width(ui_Cont2Val, 240);
    lv_obj_set_height(ui_Cont2Val, 100);
    lv_obj_set_x(ui_Cont2Val, 240);
    lv_obj_set_y(ui_Cont2Val, 152);
    lv_obj_set_style_bg_color(ui_Cont2Val, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Cont2Val, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblValRadiat = createValueLabel(ui_Cont2Val, -5, 8, LV_ALIGN_TOP_RIGHT, "XX.X°C", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblRadiat = createValueLabel(ui_Cont2Val, 5, 10, LV_ALIGN_DEFAULT, "T. Radiat.", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblDebitRadiat = createValueLabel(ui_Cont2Val, 5, 38, LV_ALIGN_DEFAULT, "Debit Rad.", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblValDebitRadit = createValueLabel(ui_Cont2Val, -5, 36, LV_ALIGN_TOP_RIGHT, "XX.X l/m", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblCourant = createValueLabel(ui_Cont2Val, 5, 66, LV_ALIGN_DEFAULT, "Courant", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblValCourant = createValueLabel(ui_Cont2Val, -5, 64, LV_ALIGN_TOP_RIGHT, "XX.X A", 0x00FFFF, &lv_font_montserrat_22);

    // ===========  CONTAINER CONSO  =============================
 
    ui_ContConso = lv_obj_create(parent);
    lv_obj_remove_style_all(ui_ContConso);
    lv_obj_set_width(ui_ContConso, 330);
    lv_obj_set_height(ui_ContConso, 105);
    lv_obj_set_x(ui_ContConso, 0);
    lv_obj_set_y(ui_ContConso, 254);
    lv_obj_set_style_bg_color(ui_ContConso, lv_color_hex(0x212121), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_ContConso, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_LblConsoEau = createValueLabel(ui_ContConso, 0, 21, LV_ALIGN_DEFAULT, "Eau", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblValConsoInstEau = createValueLabel(ui_ContConso, -220, 19, LV_ALIGN_TOP_RIGHT, "XXXX", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblValConsoJEau = createValueLabel(ui_ContConso, -130, 19, LV_ALIGN_TOP_RIGHT, "XXXX", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblValConsoJ1Eau = createValueLabel(ui_ContConso, -40, 19, LV_ALIGN_TOP_RIGHT, "XXXX L", 0x00FFFF, &lv_font_montserrat_22);

    ui_LblConsoElec = createValueLabel(ui_ContConso, 0, 49, LV_ALIGN_DEFAULT, "Elec", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblValConsoInstElec = createValueLabel(ui_ContConso, -220, 47, LV_ALIGN_TOP_RIGHT, "XXXX", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblValConsoJElec = createValueLabel(ui_ContConso, -130, 47, LV_ALIGN_TOP_RIGHT, "XXXX", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblValConsoJ1Elec = createValueLabel(ui_ContConso, -3, 47, LV_ALIGN_TOP_RIGHT, "XXXX Kwh", 0x00FFFF, &lv_font_montserrat_22);

    ui_LblConsoGaz = createValueLabel(ui_ContConso, 0, 77, LV_ALIGN_DEFAULT, "Gaz", 0xFFFFFF, &lv_font_montserrat_20);
    ui_LblValConsoInstGaz = createValueLabel(ui_ContConso, -220, 75, LV_ALIGN_TOP_RIGHT, "XXX", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblValConsoJGaz = createValueLabel(ui_ContConso, -130, 75, LV_ALIGN_TOP_RIGHT, "XXX", 0x00FFFF, &lv_font_montserrat_22);
    ui_LblValConsoJ1Gaz = createValueLabel(ui_ContConso, 0, 75, LV_ALIGN_TOP_RIGHT, "XXX Nm3", 0x00FFFF, &lv_font_montserrat_22);

    ui_LblTitreConso = lv_label_create(ui_ContConso);
    lv_obj_set_width(ui_LblTitreConso, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_LblTitreConso, LV_SIZE_CONTENT);    /// 1
    lv_label_set_text(ui_LblTitreConso, "Cons.   Inst.        Jour     Jour-1");
    lv_obj_set_style_text_color(ui_LblTitreConso, lv_color_hex(0x0069FF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_LblTitreConso, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
}