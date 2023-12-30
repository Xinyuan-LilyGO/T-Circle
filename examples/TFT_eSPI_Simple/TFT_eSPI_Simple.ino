
#include <TFT_eSPI.h>
#include "ui.h"
#include "lvgl_scr_mgr.h"
// #include <lv_demos.h>

static const uint16_t screenWidth = 160;
static const uint16_t screenHeight = 160;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors((uint16_t *)&color_p->full, w * h, false);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

void setup()
{
  Serial.begin(115200);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  // Serial.println(LVGL_Arduino);
  printf("I am LVGL_Arduino");

  tft.begin();        /* TFT init */
  tft.setRotation(0); /* Landscape orientation, flipped */
  tft.setSwapBytes(false);
  // tft.fillScreen(TFT_WHITE); 

  lv_init();

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);
  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  ui_entry();
}

unsigned long previousMillis = 0;
int scr_id = 0;
void loop()
{
  unsigned long currentMillis = millis();  

  if (currentMillis - previousMillis >= 3000) {
    
    switch (scr_id) {
      case scr_mgr_id_main: ScrMgrPopToRoot(true);  scr_id = 1; break;
      case scr_mgr_id_clock1: ScrMgrPushScr(scr_mgr_id_clock1, true); scr_id = 2; break;
      case scr_mgr_id_clock2: ScrMgrPushScr(scr_mgr_id_clock2, true); scr_id = 0; break;
      
      default:
        break;
    }
    previousMillis = currentMillis;  //更新时间记录
  }
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}