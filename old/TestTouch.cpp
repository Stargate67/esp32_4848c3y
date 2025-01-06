/*You code here*/
void btn_event_cb(lv_obj_t *btn, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    printf("Clicked\n");
  }
}

void set_button(void) {
  lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL); /*Add a button to the current screen*/
  //lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
  lv_obj_set_size(btn, 150, 100);         /*Set its size*/
  lv_obj_set_event_cb(btn, btn_event_cb); /*Assign a callback to the button*/
  lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 60);

  lv_obj_t *label = lv_label_create(btn, NULL); /*Add a label to the button*/
  lv_label_set_text(label, "Next");             /*Set the labels text*/
}

/*Return true is the touchpad is pressed*/
static bool kbpro_lvgl_touchpad_is_pressed(void) {
    return kbpro_touch_ztw622_event_status();
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void kbpro_lvgl_touchpad_get_xy(lv_coord_t * x, lv_coord_t * y) {
    (*x) = kbpro_touch_ztw622_get_x_coord();
    (*y) = kbpro_touch_ztw622_get_y_coord();
}

/* Will be called by the LVGL library to read the touchpad */
static bool kbpro_lvgl_touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static lv_coord_t last_x = 0;
    static lv_coord_t last_y = 0;

    /*Save the pressed coordinates and the state*/
    if(kbpro_lvgl_touchpad_is_pressed()) {
        kbpro_lvgl_touchpad_get_xy(&last_x, &last_y);
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Set the last pressed coordinates*/
    data->point.x = last_x;
    data->point.y = last_y;

    /*Return `false` because we are not buffering and no more data to read*/
    return false;
}

/*
  initialize lvgl input device driver (touch in our case)
*/
lv_indev_t * indev_touchpad;
lv_indev_drv_t indev_drv;

void kbpro_lvgl_indev_init(void) {
/*Initialize your touchpad if you have*/
#if KBPRO_ENABLE_TOUCH
  kbpro_ztw622_touch_init();
  kbpro_ztw622_touch_check_id();
#endif //KBPRO_ENABLE_TOUCH

  /*Register a touchpad input device*/
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = kbpro_lvgl_touchpad_read;
  indev_touchpad = lv_indev_drv_register(&indev_drv);
  if (!indev_touchpad) {
    printk("\n kbpro_lvgl_indev_init FAIL");
  }
}