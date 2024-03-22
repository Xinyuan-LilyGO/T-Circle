
#include <TFT_eSPI.h>
#include "ui.h"
#include "lvgl_scr_mgr.h"
#include <TouchDrvCSTXXX.hpp>
// #include <lv_demos.h>
#include "utilities.h"

bool UI_TOUCH_LINE_EN = 0;
int ui_rotation = SCR_ROTATION_0;

static const uint16_t screenWidth = 160;
static const uint16_t screenHeight = 160;

/**    PIN
 * **************************** no touch
 * BL_EN --- 18
 * MOSI  --- 11
 * CS    --- 10
 * SCK   --- 12
 * DC    --- 17
 * **************************** touch
 * LEDA  --- 18
 * RESET --- 9
 * RS    --- 17
 * SDA   --- 11
 * SCL   --- 12
 * CS    --- 10
 * 
 * TP_RST --- 15
 * TP_SCL --- 6
 * TP_SDA --- 5
 * TP_INT --- 7
 */

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */
TouchDrvCSTXXX touch;

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

static lv_point_t line_x_points[] = { {0, 0}, {0, 0} };
static lv_point_t line_y_points[] = { {0, 0}, {0, 0} };
lv_obj_t *line_x;
lv_obj_t *line_y;
void touch_line(lv_indev_data_t *data)
{
    if (data->state == true) {
        // printf("x=%d\t y=%d\n", mouse_data.point.x, mouse_data.point.y);
        line_x_points[0].x = data->point.x;
        line_x_points[0].y = 0;
        line_x_points[1].x = data->point.x;
        line_x_points[1].y = screenWidth;
        lv_line_set_points(line_x, line_x_points, 2);
        line_y_points[0].x = 0;
        line_y_points[0].y = data->point.y;
        line_y_points[1].x = screenWidth;
        line_y_points[1].y = data->point.y;
        lv_line_set_points(line_y, line_y_points, 2);
        lv_obj_clear_flag(line_x, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(line_y, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(line_x, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(line_y, LV_OBJ_FLAG_HIDDEN);
    }
}

static void lv_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    static int16_t x[5], y[5];
    uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
    if (touched) {
        data->state = LV_INDEV_STATE_PR;
        if(ui_rotation == SCR_ROTATION_0){
            data->point.x = lv_map(x[0], 0, 350, 0, TFT_WIDTH);
            data->point.y = lv_map(y[0], 0, 350, 0, TFT_HEIGHT);
        } 
        else if(ui_rotation == SCR_ROTATION_90){
            data->point.x = lv_map(y[0], 0, 350, 0, TFT_WIDTH);;
            data->point.y = TFT_WIDTH - lv_map(x[0], 0, 350, 0, TFT_WIDTH);
        }
        else if(ui_rotation == SCR_ROTATION_180){
            data->point.x = TFT_WIDTH - lv_map(x[0], 0, 350, 0, TFT_WIDTH);
            data->point.y = TFT_HEIGHT - lv_map(y[0], 0, 350, 0, TFT_HEIGHT);
        }
        else if(ui_rotation == SCR_ROTATION_270) {
            data->point.x = TFT_HEIGHT - lv_map(y[0], 0, 350, 0, TFT_WIDTH);
            data->point.y = lv_map(x[0], 0, 350, 0, TFT_WIDTH);;
        }
        Serial.print("x=");Serial.print(data->point.x);
        Serial.print(", y=");Serial.println(data->point.y);
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
    if(UI_TOUCH_LINE_EN == 1)
    touch_line(data);
}

void touch_sleep(void)
{
    touch.sleep();
}

void touch_wakeup(void)
{
    touch.wakeup();
}

void setup()
{
  
  Serial.begin(115200);
  // delay(3000);

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

  // Serial.println(LVGL_Arduino);
  printf("I am LVGL_Arduino");

  tft.begin();        /* TFT init */
  tft.setRotation(ui_rotation); /* Landscape orientation, flipped */
  // tft.setSwapBytes(false);
  // tft.fillScreen(TFT_WHITE); 

  touch.setPins(BOARD_TOUCH_RST, BOARD_TOUCH_IRQ);
  bool hasTouch = touch.init(Wire, BOARD_I2C_SDA, BOARD_I2C_SCL, 0x15);
  if (!hasTouch) {
      Serial.println("Failed to find Capacitive Touch !");
  } else {
      Serial.println("Find Capacitive Touch");
  }

  lv_init();

  static lv_disp_draw_buf_t draw_buf;
  static lv_color_t buf1[screenWidth * screenHeight];
//   static lv_color_t buf2[screenWidth * screenHeight];

  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, screenWidth * screenHeight);
  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = lv_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 1);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_rounded(&style_line, false);

    /*Create a line and apply the new style*/
    line_x = lv_line_create(lv_layer_top());
    // lv_line_set_points(line_x, line_x_points, 2);     /*Set the points*/
    lv_obj_add_style(line_x, &style_line, 0);

    line_y = lv_line_create(lv_layer_top());
    // lv_line_set_points(line_x, line_x_points, 2);     /*Set the points*/
    lv_obj_add_style(line_y, &style_line, 0);
    lv_obj_add_flag(line_x, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(line_y, LV_OBJ_FLAG_HIDDEN);

  ui_entry();

  UI_TOUCH_LINE_EN = 1;
}

unsigned long previousMillis = 0;
int scr_id = 0;
void loop()
{
  // unsigned long currentMillis = millis();  

  // if (currentMillis - previousMillis >= 3000) {
    
  //   switch (scr_id) {
  //     case scr_mgr_id_main: ScrMgrPopToRoot(true);  scr_id = 1; break;
  //     case scr_mgr_id_clock1: ScrMgrPushScr(scr_mgr_id_clock1, true); scr_id = 2; break;
  //     case scr_mgr_id_clock2: ScrMgrPushScr(scr_mgr_id_clock2, true); scr_id = 0; break;
      
  //     default:
  //       break;
  //   }
  //   previousMillis = currentMillis;  //更新时间记录
  // }
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
}