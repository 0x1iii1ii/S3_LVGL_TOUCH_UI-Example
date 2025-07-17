#ifndef __BRIGHT_TILE_H__
#define __BRIGHT_TILE_H__

#include "lvgl_ui.h"
#include "esp32-hal-ledc.h"

#define GFX_BL 46
#define LEDC_TIMER_10_BIT (10)

#ifdef __cplusplus
extern "C" {
#endif

void lvgl_brightness_ui_init(lv_obj_t *parent);


#ifdef __cplusplus
}
#endif



#endif