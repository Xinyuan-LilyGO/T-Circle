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

/**
 * A clock from a meter
 */
void ui_entry(void)
{
    ScrMgrInit();
    ScrMgrSwitchScr(scr_mgr_id_main, true);
}