#include "lvgl_ui.h"

void lvgl_ui_init(void)
{
    lv_obj_t *tileview = lv_tileview_create(lv_scr_act());
    lv_obj_set_style_bg_opa(tileview, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(tileview, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);

    lv_obj_t *rgb_tile = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_RIGHT);
    lock_tile_init(rgb_tile);

    lv_obj_t *system_tile = lv_tileview_add_tile(tileview, 1, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    system_tile_init(system_tile);

    lv_obj_t *my_tile = lv_tileview_add_tile(tileview, 6, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    lvgl_brightness_ui_init(my_tile);

    lv_obj_t *s1_tile = lv_tileview_add_tile(tileview, 3, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    ui_Screen1_screen_init(s1_tile);

    lv_obj_t *s2_tile = lv_tileview_add_tile(tileview, 4, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    ui_Screen2_screen_init(s2_tile);

    lv_obj_t *s3_tile = lv_tileview_add_tile(tileview, 5, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    ui_Screen3_screen_init(s3_tile);

    lv_obj_t *wifi_tile = lv_tileview_add_tile(tileview, 2, 0, LV_DIR_LEFT | LV_DIR_RIGHT);
    wifi_tile_init(wifi_tile);

}