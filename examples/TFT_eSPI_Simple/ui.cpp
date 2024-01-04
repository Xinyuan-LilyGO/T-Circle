#include "ui.h"
#include "lvgl_scr_mgr.h"

/*********************************************************************************
 *                              clock screen
 * *******************************************************************************/
static lv_obj_t *meter;
lv_meter_indicator_t *indic_min;
lv_meter_indicator_t *indic_hour;

static void set_value(void *indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter, (lv_meter_indicator_t *)indic, v);
}

lv_obj_t *gui_mainCreate(lv_obj_t *parent)
{
    meter = lv_meter_create(parent);
    lv_obj_set_size(meter, 160, 160);
    lv_obj_center(meter);

    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_scale_t *scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 270);

    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_palette_main(LV_PALETTE_GREY)); /*12 ticks*/
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 20, lv_color_black(), 10);       /*Every tick is major*/
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300);                            /*[1..12] values in an almost full circle*/

    LV_IMG_DECLARE(img_hand)

    /*Add a the hands from images*/
    indic_min = lv_meter_add_needle_img(meter, scale_min, &img_hand, 5, 5);
    indic_hour = lv_meter_add_needle_img(meter, scale_min, &img_hand, 5, 5);

    /*Create an animation to set the value*/
    return meter;
}

void gui_mainEnter() {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_values(&a, 0, 60);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&a, 2000); /*2 sec for 1 turn of the minute hand (1 hour)*/
    lv_anim_set_var(&a, indic_min);
    lv_anim_start(&a);

    lv_anim_set_var(&a, indic_hour);
    lv_anim_set_time(&a, 24000); /*24 sec for 1 turn of the hour hand*/
    lv_anim_set_values(&a, 0, 60);
    lv_anim_start(&a);
}

void gui_mainExit() {}

void gui_mainDestory() {}

ScrRegister(main, scr_mgr_id_main);
//------------------------------------------------------------------------------
static lv_obj_t *meter1;

static void set_value1(void *indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter1, (lv_meter_indicator_t *)indic, v);
}

lv_obj_t *gui_clock1Create(lv_obj_t *parent)
{
    meter1 = lv_meter_create(parent);
    lv_obj_center(meter1);
    lv_obj_set_size(meter1, 160, 160);

    /*Remove the circle from the middle*/
    lv_obj_remove_style(meter1, NULL, LV_PART_INDICATOR);

    /*Add a scale first*/
    lv_meter_scale_t *scale = lv_meter_add_scale(meter1);
    lv_meter_set_scale_ticks(meter1, scale, 11, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter1, scale, 1, 2, 30, lv_color_hex3(0xeee), 15);
    lv_meter_set_scale_range(meter1, scale, 0, 100, 270, 90);

    /*Add a three arc indicator*/
    lv_meter_indicator_t *indic1 = lv_meter_add_arc(meter1, scale, 10, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_indicator_t *indic2 = lv_meter_add_arc(meter1, scale, 10, lv_palette_main(LV_PALETTE_GREEN), -10);
    lv_meter_indicator_t *indic3 = lv_meter_add_arc(meter1, scale, 10, lv_palette_main(LV_PALETTE_BLUE), -20);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value1);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_var(&a, indic1);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, indic2);
    lv_anim_start(&a);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic3);
    lv_anim_start(&a);
    return meter1;
}

void gui_clock1Enter() {}

void gui_clock1Exit() {}

void gui_clock1Destory() {lv_anim_del_all();}

ScrRegister(clock1, scr_mgr_id_clock1);
//------------------------------------------------------------------------------
static lv_obj_t *meter2;

static void set_value2(void * indic, int32_t v)
{
    lv_meter_set_indicator_value(meter2, (lv_meter_indicator_t *)indic, v);
}

lv_obj_t *gui_clock2Create(lv_obj_t *parent)
{
    meter2 = lv_meter_create(parent);
    lv_obj_center(meter2);
    lv_obj_set_size(meter2, 160, 160);

    /*Add a scale first*/
    lv_meter_scale_t * scale = lv_meter_add_scale(meter2);
    lv_meter_set_scale_ticks(meter2, scale, 41, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter2, scale, 8, 4, 15, lv_color_black(), 10);

    lv_meter_indicator_t * indic;

    /*Add a blue arc to the start*/
    indic = lv_meter_add_arc(meter2, scale, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter2, indic, 0);
    lv_meter_set_indicator_end_value(meter2, indic, 20);

    /*Make the tick lines blue at the start of the scale*/
    indic = lv_meter_add_scale_lines(meter2, scale, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(meter2, indic, 0);
    lv_meter_set_indicator_end_value(meter2, indic, 20);

    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(meter2, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter2, indic, 80);
    lv_meter_set_indicator_end_value(meter2, indic, 100);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(meter2, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false,
                                     0);
    lv_meter_set_indicator_start_value(meter2, indic, 80);
    lv_meter_set_indicator_end_value(meter2, indic, 100);

    /*Add a needle line indicator*/
    indic = lv_meter_add_needle_line(meter2, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value2);
    lv_anim_set_var(&a, indic);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    return meter2;
}

void gui_clock2Enter() {}

void gui_clock2Exit() {}

void gui_clock2Destory() {lv_anim_del_all();}

ScrRegister(clock2, scr_mgr_id_clock2);
//------------------------------------------------------------------------------
lv_obj_t * btn_scr;

extern void touch_sleep(void);
extern void touch_wakeup(void);

static void touch_text_timer(lv_timer_t *t)
{
    static int sec = 0;
    static bool dir = false;

    sec++;

    if(sec >= 10){
        if(dir){
            touch_wakeup();
            Serial.println("touch_wakeup!");
        }else{
            touch_sleep();
            Serial.println("touch_sleep!");
        }
        sec = 0;
        dir = !dir;
    }
    Serial.println(sec);
}

static void btn_event_cb(lv_event_t * e)
{
    static bool flag = false;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;
        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Btn: %d", cnt);
        // touch_sleep();
        // if(!flag){
        //     lv_timer_create(touch_text_timer, 1000, NULL);
        //     flag = true;
        // }
    }
}

lv_obj_t *gui_btnCreate(lv_obj_t *parent)
{
    /*Init the style for the default state*/
    btn_scr = lv_obj_create(parent);
    lv_obj_set_size(btn_scr, lv_pct(100), lv_pct(100));

    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, LV_RADIUS_CIRCLE);

    lv_style_set_bg_opa(&style, LV_OPA_100);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

    lv_style_set_border_opa(&style, LV_OPA_40);
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_ofs_y(&style, 8);

    lv_style_set_outline_opa(&style, LV_OPA_COVER);
    lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_pad_all(&style, 10);

    /*Init the pressed style*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_ofs_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_BLUE, 4));

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_INV};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&style_pr, &trans);

    lv_obj_t * btn = lv_btn_create(btn_scr);     /*Add a button the current screen*/
    // lv_obj_set_size(btn, 100, 100);                          /*Set its size*/
    lv_obj_remove_style_all(btn);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(btn, &style, 0);
    lv_obj_add_style(btn, &style_pr, LV_STATE_PRESSED);

    lv_obj_set_size(btn, 100, 100);
    // lv_obj_set_style_radius(btn_scr, LV_RADIUS_CIRCLE, 0);
    lv_obj_center(btn);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Btn");                     /*Set the labels text*/
    lv_obj_center(label);

    return btn_scr;
}

void gui_btnEnter() {}

void gui_btnExit() {}

void gui_btnDestory() {}

ScrRegister(btn, scr_mgr_id_btn);
//------------------------------------------------------------------------------
lv_obj_t *scroll_scr;

static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        }
        else {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = r * r - diff_y * diff_y;
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
    }
}

lv_obj_t *gui_scrollCreate(lv_obj_t *parent)
{
    scroll_scr = lv_obj_create(parent);
    lv_obj_set_size(scroll_scr, lv_pct(100), lv_pct(100));

    lv_obj_t * cont = lv_obj_create(scroll_scr);
    lv_obj_set_size(cont, 160, 160);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cont, true, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    uint32_t i;
    for(i = 0; i < 20; i++) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_width(btn, lv_pct(100));

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Button %"LV_PRIu32, i);
    }

    /*Update the buttons position manually for first*/
    lv_event_send(cont, LV_EVENT_SCROLL, NULL);

    /*Be sure the fist button is in the middle*/
    lv_obj_scroll_to_view(lv_obj_get_child(cont, 0), LV_ANIM_OFF);

    return scroll_scr;
}

void gui_scrollEnter() {}

void gui_scrollExit() {}

void gui_scrollDestory() {}

ScrRegister(scroll, scr_mgr_id_scroll);
//------------------------------------------------------------------------------

void indev_get_gesture_dir(lv_timer_t *t)
{
    lv_indev_data_t data;
    lv_indev_t * indev_pointer = lv_indev_get_next(NULL);
    lv_dir_t dir = lv_indev_get_gesture_dir(indev_pointer);
    static lv_dir_t curr_dir = LV_DIR_NONE;
    static int test_id = 0;

    _lv_indev_read(indev_pointer, &data);

    if(data.state == LV_INDEV_STATE_PR){
        curr_dir = dir;
    }else{
        switch (curr_dir)
        {
            case LV_DIR_LEFT: 
                Serial.println("scroll left\n");
                test_id++;
                if(test_id < scr_mgr_id_max)
                    ScrMgrPushScr(test_id % scr_mgr_id_max, true);
                break;
            case LV_DIR_RIGHT: 
                Serial.println("scroll right\n");
                if(ScrMgrPopScr(true)){
                    test_id--;
                }   
                break;
            case LV_DIR_TOP:
                Serial.println("scroll top\n");
                break;
            case LV_DIR_BOTTOM:
                Serial.println("scroll bottom\n");
                break;
            default:
                break;
        }
        curr_dir = LV_DIR_NONE;
        dir = LV_DIR_NONE;
    }
}

void ui_entry(void)
{

    // ui_btn();
    // lv_scroll_6();

    ScrMgrInit();
    ScrMgrSwitchScr(scr_mgr_id_main, true);
    lv_timer_create(indev_get_gesture_dir, 20, NULL);
}