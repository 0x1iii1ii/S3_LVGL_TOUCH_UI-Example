#ifndef __LVGL_UI_H__
#define __LVGL_UI_H__

#include <stdio.h>
#include "lvgl.h"
#include "system_tile.h"
#include "rgb_tile.h"
#include "image_tile.h"
#include "bright_tile.h"
#include "ui_Screen1.h"
#include "ui_Screen2.h"
#include "ui_Screen3.h"
#include "wifi_tile.h"
#include "bsp_wifi.h"
#include "bsp_sdcard.h"
#include "bsp_battery.h"

#ifdef __cplusplus
extern "C" {
#endif

void lvgl_ui_init(void);

#ifdef __cplusplus
}
#endif

#endif

