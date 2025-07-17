#include "bright_tile.h"

lv_obj_t * ui_Screen0 = NULL;
lv_obj_t * ui_Slider1 = NULL;
lv_obj_t * ui_Label1 = NULL;
lv_obj_t * ui_Label0 = NULL;

void slider_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        // 获取当前滑块的值
        lv_obj_t *slider = lv_event_get_target(e);
        int value = lv_slider_get_value(slider);
        // printf("Slider value: %d\n", value);
        lv_label_set_text_fmt(ui_Label0, "%d %%", value);
        // bsp_lcd_brightness_set(value);
        ledcWrite(GFX_BL , (1 << LEDC_TIMER_10_BIT) / 100 * value);
        // 阻止事件向上传递
        lv_event_stop_bubbling(e);
    }
}

void lvgl_brightness_ui_init(lv_obj_t *parent)
{
    ui_Screen0 = lv_obj_create(parent);
    lv_obj_set_size(ui_Screen0, lv_pct(90), lv_pct(50));
    lv_obj_align(ui_Screen0, LV_ALIGN_CENTER, 0, 0);
    lv_obj_clear_flag(ui_Screen0, LV_OBJ_FLAG_SCROLLABLE);      /// Flags

    ui_Slider1 = lv_slider_create(ui_Screen0);
    lv_slider_set_range(ui_Slider1, 1, 100);
    lv_slider_set_value(ui_Slider1, 75, LV_ANIM_OFF);
    if(lv_slider_get_mode(ui_Slider1) == LV_SLIDER_MODE_RANGE) lv_slider_set_left_value(ui_Slider1, 0, LV_ANIM_OFF);
    lv_obj_set_width(ui_Slider1, 117);
    lv_obj_set_height(ui_Slider1, 13);
    lv_obj_set_x(ui_Slider1, 2);
    lv_obj_set_y(ui_Slider1, 14);
    lv_obj_set_align(ui_Slider1, LV_ALIGN_CENTER);

    ui_Label1 = lv_label_create(ui_Screen0);
    lv_obj_set_width(ui_Label1, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label1, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label1, 3);
    lv_obj_set_y(ui_Label1, -35);
    lv_obj_set_align(ui_Label1, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label1, "Brightness");

    ui_Label0 = lv_label_create(ui_Screen0);
    lv_obj_set_width(ui_Label0, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Label0, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_x(ui_Label0, 0);
    lv_obj_set_y(ui_Label0, -9);
    lv_obj_set_align(ui_Label0, LV_ALIGN_CENTER);
    lv_label_set_text(ui_Label0, "75%");

    lv_obj_add_event_cb(ui_Slider1, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    // lv_obj_t *obj = lv_obj_create(parent);
    // lv_obj_set_size(obj, lv_pct(90), lv_pct(50));
    // lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0); 
    // // 创建滑动条
    // lv_obj_t *slider = lv_slider_create(obj);

    // // 设置滑动条的方向为水平
    // lv_slider_set_range(slider, 1, 100);          
    // lv_slider_set_value(slider, 80, LV_ANIM_OFF); 

    // // 调整滑动条大小和位置
    // lv_obj_set_size(slider, lv_pct(90), 20);    
    // lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0); 

    // lv_obj_set_style_pad_top(obj, 20, 0);
    // lv_obj_set_style_pad_bottom(obj, 20, 0);
    // // lv_obj_set_style_pad_left(parent, 50, 0);
    // // lv_obj_set_style_pad_right(parent, 50, 0);
    // lv_obj_clear_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
    // // 添加事件回调（可选）
    // lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // label_brightness = lv_label_create(obj);
    // lv_label_set_text(label_brightness, "80%");
    // lv_obj_align(label_brightness, LV_ALIGN_TOP_MID, 0, 0);

}