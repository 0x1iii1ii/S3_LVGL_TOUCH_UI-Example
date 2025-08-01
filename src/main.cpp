#include <Arduino.h>
/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/get-started/platforms/arduino.html  */

#include <lvgl.h>
#include "esp_lcd_touch_axs5106l.h"
#include "lvgl_ui.h"

/*To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 *You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 Note that the `lv_examples` library is for LVGL v7 and you shouldn't install it for this version (since LVGL v8)
 as the examples and demos are now part of the main LVGL library. */

// #include <examples/lv_examples.h>
#include <demos/lv_demos.h>

// #define DIRECT_RENDER_MODE // Uncomment to enable full frame buffer

#include "fx_init.h"

#define ROTATION 0
// #define ROTATION 1
// #define ROTATION 2
// #define ROTATION 3

#define Touch_I2C_SDA 42
#define Touch_I2C_SCL 41
#define Touch_RST     47
#define Touch_INT     48

#define LEDC_FREQ             5000

#define LOCK_PIN GPIO_NUM_0

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_disp_draw_buf_t draw_buf;
lv_color_t *disp_draw_buf;
lv_disp_drv_t disp_drv;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf)
{
  Serial.printf(buf);
  Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
#ifndef DIRECT_RENDER_MODE
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#else
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)&color_p->full, w, h);
#endif
#endif // #ifndef DIRECT_RENDER_MODE

  lv_disp_flush_ready(disp_drv);
}

/*Read the touchpad*/
void touchpad_read_cb(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
  touch_data_t touch_data;
  // uint16_t touchpad_x[1] = { 0 };
  // uint16_t touchpad_y[1] = { 0 };
  uint8_t touchpad_cnt = 0;

  /* Read touch controller data */
  // esp_lcd_touch_read_data(touch_handle);
  bsp_touch_read();
  /* Get coordinates */
  // bool touchpad_pressed = esp_lcd_touch_get_coordinates(touch_handle, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);
  bool touchpad_pressed = bsp_touch_get_coordinates(&touch_data);

  if (touchpad_pressed) {
    data->point.x = touch_data.coords[0].x;
    data->point.y = touch_data.coords[0].y;
    data->state = LV_INDEV_STATE_PRESSED;
    // printf("x:%03d, y:%03d\r\n", touchpad_x[0], touchpad_y[0]);
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup()
{
#ifdef DEV_DEVICE_INIT
  DEV_DEVICE_INIT();
#endif

  Serial.begin(115200);
  // Serial.setDebugOutput(true);
  // while(!Serial);
  Serial.println("Arduino_GFX LVGL_Arduino_v8 example ");
  String LVGL_Arduino = String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println(LVGL_Arduino);

  // Init Display
  if (!gfx->begin())
  {
    Serial.println("gfx->begin() failed!");
  }
  lcd_reg_init();
  gfx->setRotation(ROTATION);
  gfx->fillScreen(RGB565_BLACK);

#ifdef GFX_BL

  ledcAttach(GFX_BL , LEDC_FREQ, LEDC_TIMER_10_BIT);
  ledcWrite(GFX_BL , (1 << LEDC_TIMER_10_BIT) / 100 * 80);
#endif

  // Init touch device
  //touch_init(gfx->width(), gfx->height(), gfx->getRotation());
  Wire.begin(Touch_I2C_SDA, Touch_I2C_SCL);
  bsp_touch_init(&Wire, Touch_RST, Touch_INT, gfx->getRotation(), gfx->width(), gfx->height());
  bsp_wifi_init("Lab Team", "VP.Start@168");
  bsp_battery_init();
  bsp_sdcard_init();
  lv_init();
  pinMode(LOCK_PIN, INPUT_PULLUP);
  esp_sleep_enable_ext0_wakeup(LOCK_PIN, 0); // Wake on LOW
  // Detect if we woke from deep sleep
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Woke up from deep sleep!");
  } else {
    Serial.println("Normal boot");
  }
#if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

  screenWidth = gfx->width();
  screenHeight = gfx->height();

#ifdef DIRECT_RENDER_MODE
  bufSize = screenWidth * screenHeight;
#else
  bufSize = screenWidth * 40;
#endif

#ifdef ESP32
#if defined(DIRECT_RENDER_MODE) && (defined(CANVAS) || defined(RGB_PANEL) || defined(DSI_PANEL))
  disp_draw_buf = (lv_color_t *)gfx->getFramebuffer();
#else  // !(defined(DIRECT_RENDER_MODE) && (defined(CANVAS) || defined(RGB_PANEL) || defined(DSI_PANEL)))
  disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!disp_draw_buf)
  {
    // remove MALLOC_CAP_INTERNAL flag try again
    disp_draw_buf = (lv_color_t *)heap_caps_malloc(bufSize * 2, MALLOC_CAP_8BIT);
  }
#endif // !(defined(DIRECT_RENDER_MODE) && (defined(CANVAS) || defined(RGB_PANEL) || defined(DSI_PANEL)))
#else // !ESP32
  Serial.println("LVGL disp_draw_buf heap_caps_malloc failed! malloc again...");
  disp_draw_buf = (lv_color_t *)malloc(bufSize * 2);
#endif // !ESP32
  if (!disp_draw_buf)
  {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  }
  else
  {
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, bufSize);

    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
#ifdef DIRECT_RENDER_MODE
    disp_drv.direct_mode = true;
#endif
    lv_disp_drv_register(&disp_drv);

    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read_cb;
    lv_indev_drv_register(&indev_drv);
    
    /* Option 2: Try an example. See all the examples
     * online: https://docs.lvgl.io/master/examples.html
     * source codes: https://github.com/lvgl/lvgl/tree/e7f88efa5853128bf871dde335c0ca8da9eb7731/examples */
    // lv_example_btn_1();

    /* Option 3: Or try out a demo. Don't forget to enable the demos in lv_conf.h. E.g. LV_USE_DEMOS_WIDGETS*/
    // lvgl_brightness_ui_init(lv_scr_act());
    lvgl_ui_init();
    // lv_demo_widgets();
    // lv_demo_benchmark();
    // lv_demo_keypad_encoder();
    // lv_demo_music();
    // lv_demo_stress();
  }

  Serial.println("Setup done");
}
bool sleeping = false;
#define LONG_PRESS_TIME 300
void loop()
{
  if (!sleeping) {
    lv_timer_handler(); // Let GUI update only when awake
  }

  // Detect long press
  if (digitalRead(LOCK_PIN) == LOW) {
    unsigned long pressStart = millis();

    while (digitalRead(LOCK_PIN) == LOW) {
      if (millis() - pressStart >= LONG_PRESS_TIME) {
        Serial.println("Long press detected");

        // Wait for release BEFORE entering sleep
        while (digitalRead(LOCK_PIN) == LOW) {
          delay(10);
        }

        Serial.println("Button released → entering light sleep");
        sleeping = true;

        // Setup wake source (LOW = pressed)
        esp_sleep_enable_ext0_wakeup(LOCK_PIN, 0);
        delay(100); // Flush serial

        // Enter light sleep
        ledcWrite(GFX_BL, 0);
        esp_light_sleep_start();

        // Woke up!
        Serial.println("Woke up from light sleep!");
        sleeping = false;
        ledcWrite(GFX_BL, (1 << LEDC_TIMER_10_BIT) / 100 * 50); // 100% brightness
        break;
      }
      delay(10);
    }
  }
#ifdef DIRECT_RENDER_MODE
#if defined(CANVAS) || defined(RGB_PANEL) || defined(DSI_PANEL)
  gfx->flush();
#else // !(defined(CANVAS) || defined(RGB_PANEL) || defined(DSI_PANEL))
#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(0, 0, (uint16_t *)disp_draw_buf, screenWidth, screenHeight);
#else
  gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)disp_draw_buf, screenWidth, screenHeight);
#endif
#endif // !(defined(CANVAS) || defined(RGB_PANEL) || defined(DSI_PANEL))
#else  // !DIRECT_RENDER_MODE
#ifdef CANVAS
  gfx->flush();
#endif
#endif // !DIRECT_RENDER_MODE

  delay(5);
}
