// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: Novo1

#include "ui.h"
#include "ui_helpers.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "ethernet.h"

#define TASK_PEROPD     100
#define ANI_TIME_SEC    5
static const char *TAG = "ui";

///////////////////// VARIABLES ////////////////////
lv_disp_t * dispp;
lv_disp_t * dispp_start;
lv_disp_t * dispp_hold;
lv_obj_t * ui_scrStart;
lv_obj_t * ui_imgLogo;
lv_obj_t * ui_lblPLaceHand;
lv_obj_t * ui_lblToStart;
lv_obj_t * ui_scrHold;
lv_obj_t * ui_pgrDone;
lv_obj_t * ui_imgStand;
lv_obj_t * ui_lblPose;
lv_obj_t * ui_lblTimeLeft;
lv_obj_t * ui_lblTime;
lv_obj_t * ui_Spinner1;
lv_obj_t * ui_scrDone;
lv_obj_t * ui_imgLogoDone;
lv_obj_t * ui_Label6;
lv_obj_t * ui_lblYouMayLeave;
lv_obj_t * ui_scrNotDone;
lv_obj_t * ui_imgLogoNotDone;
lv_obj_t * ui_Label7;
lv_obj_t * ui_scrDoors;
lv_obj_t * ui_imgLogoDoors;
lv_obj_t * ui_lblDoorsOpen;
lv_obj_t * ui_lblCloseDoors;
lv_obj_t * ui_scrDoors1;
lv_obj_t * ui_imgLogoDoors1;
lv_obj_t * ui_lblErrror;
lv_obj_t * ui_lblCloseDoors1;
lv_anim_t a;
lv_obj_t * bar;

bool pre_flag = false;
bool ani_flag = false;
char time_left[8];
uint8_t time_cnt = 0;
static uint32_t mem_free_start = 0;

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "#error LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////
void ui_scrStart_screen_init(void)
{

    // ui_scrStart

    // ui_scrStart = lv_obj_create(NULL);
    ui_scrStart = lv_disp_get_scr_act(NULL);
    lv_obj_clear_flag(ui_scrStart, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_scrStart, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_scrStart, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_imgLogo

    ui_imgLogo = lv_img_create(ui_scrStart);
    lv_img_set_src(ui_imgLogo, &ui_img_logo_std_small_png);

    lv_obj_set_width(ui_imgLogo, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_imgLogo, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_imgLogo, 0);
    lv_obj_set_y(ui_imgLogo, -130);

    lv_obj_set_align(ui_imgLogo, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_imgLogo, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_imgLogo, LV_OBJ_FLAG_SCROLLABLE);

    // ui_lblPLaceHand

    ui_lblPLaceHand = lv_label_create(ui_scrStart);

    lv_obj_set_width(ui_lblPLaceHand, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblPLaceHand, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblPLaceHand, 0);
    lv_obj_set_y(ui_lblPLaceHand, 0);

    lv_obj_set_align(ui_lblPLaceHand, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblPLaceHand, "Place hand over start sensor ");

    lv_obj_set_style_text_font(ui_lblPLaceHand, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_lblToStart

    ui_lblToStart = lv_label_create(ui_scrStart);

    lv_obj_set_width(ui_lblToStart, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblToStart, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblToStart, 0);
    lv_obj_set_y(ui_lblToStart, 30);

    lv_obj_set_align(ui_lblToStart, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblToStart, "to start disinfection");

    lv_obj_set_style_text_font(ui_lblToStart, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

}
void ui_scrHold_screen_init(void)
{

    // ui_scrHold

    // ui_scrHold = lv_obj_create(NULL);
    ui_scrHold = lv_disp_get_scr_act(NULL);
    lv_obj_clear_flag(ui_scrHold, LV_OBJ_FLAG_SCROLLABLE);

    // ui_pgrDone

    // ui_pgrDone = lv_bar_create(ui_scrHold);
    // lv_bar_set_range(ui_pgrDone, 0, 100);
    // lv_bar_set_value(ui_pgrDone, 25, LV_ANIM_OFF);


    ui_pgrDone = lv_bar_create(ui_scrHold);
    lv_obj_add_event_cb(ui_pgrDone, event_cb, LV_EVENT_DRAW_PART_END, NULL);

    
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui_pgrDone);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_time(&a, ANI_TIME_SEC * 1000);
    
    //lv_anim_set_playback_time(&a, 2000);
    //lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    

    lv_obj_set_width(ui_pgrDone, 240);
    lv_obj_set_height(ui_pgrDone, 25);

    lv_obj_set_x(ui_pgrDone, 0);
    lv_obj_set_y(ui_pgrDone, 140);

    lv_obj_set_align(ui_pgrDone, LV_ALIGN_CENTER);

    lv_obj_set_style_bg_color(ui_pgrDone, lv_color_hex(0x42BDDB), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pgrDone, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(ui_pgrDone, lv_color_hex(0x223A74), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_pgrDone, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    // ui_imgStand

    ui_imgStand = lv_img_create(ui_scrHold);
    lv_img_set_src(ui_imgStand, &ui_img_896891123);

    lv_obj_set_width(ui_imgStand, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_imgStand, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_imgStand, 0);
    lv_obj_set_y(ui_imgStand, -50);

    lv_obj_set_align(ui_imgStand, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_imgStand, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_imgStand, LV_OBJ_FLAG_SCROLLABLE);

    // ui_lblPose

    ui_lblPose = lv_label_create(ui_scrHold);

    lv_obj_set_width(ui_lblPose, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblPose, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblPose, 0);
    lv_obj_set_y(ui_lblPose, -200);

    lv_obj_set_align(ui_lblPose, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblPose, "Please Hold Pose");

    lv_obj_set_style_text_font(ui_lblPose, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_lblTimeLeft

    ui_lblTimeLeft = lv_label_create(ui_scrHold);

    lv_obj_set_width(ui_lblTimeLeft, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblTimeLeft, LV_SIZE_CONTENT);
    /*
    lv_obj_set_x(ui_lblTimeLeft, 0);
    lv_obj_set_y(ui_lblTimeLeft, 110);

    lv_obj_set_align(ui_lblTimeLeft, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblTimeLeft, "000");
    */
    lv_obj_set_style_text_font(ui_lblTimeLeft, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_lblTime

    ui_lblTime = lv_label_create(ui_scrHold);

    lv_obj_set_width(ui_lblTime, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblTime, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblTime, 0);
    lv_obj_set_y(ui_lblTime, 90);

    lv_obj_set_align(ui_lblTime, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblTime, "Time Left");

    lv_obj_set_style_text_font(ui_lblTime, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align_to(ui_lblTimeLeft, ui_lblTime, LV_ALIGN_OUT_BOTTOM_MID, 12, 4);
    // ui_Spinner1

    ui_Spinner1 = lv_spinner_create(ui_scrHold, 1000, 90);

    lv_obj_set_width(ui_Spinner1, 250);
    lv_obj_set_height(ui_Spinner1, 250);

    lv_obj_set_x(ui_Spinner1, 0);
    lv_obj_set_y(ui_Spinner1, -50);

    lv_obj_set_align(ui_Spinner1, LV_ALIGN_CENTER);

    lv_obj_clear_flag(ui_Spinner1, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_set_style_arc_width(ui_Spinner1, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_arc_color(ui_Spinner1, lv_color_hex(0x223A74), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_Spinner1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_Spinner1, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);

}

void ui_scrDone_screen_init(void)
{

    // ui_scrDone

    // ui_scrDone = lv_obj_create(NULL);
    ui_scrDone = lv_disp_get_scr_act(NULL);
    lv_obj_clear_flag(ui_scrDone, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_scrDone, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_scrDone, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_imgLogoDone

    ui_imgLogoDone = lv_img_create(ui_scrDone);
    lv_img_set_src(ui_imgLogoDone, &ui_img_logo_std_small_png);

    lv_obj_set_width(ui_imgLogoDone, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_imgLogoDone, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_imgLogoDone, 0);
    lv_obj_set_y(ui_imgLogoDone, -130);

    lv_obj_set_align(ui_imgLogoDone, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_imgLogoDone, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_imgLogoDone, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Label6

    ui_Label6 = lv_label_create(ui_scrDone);

    lv_obj_set_width(ui_Label6, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label6, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label6, 0);
    lv_obj_set_y(ui_Label6, 0);

    lv_obj_set_align(ui_Label6, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label6, "Disinfection Done");

    lv_obj_set_style_text_font(ui_Label6, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_lblYouMayLeave

    ui_lblYouMayLeave = lv_label_create(ui_scrDone);

    lv_obj_set_width(ui_lblYouMayLeave, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblYouMayLeave, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblYouMayLeave, 0);
    lv_obj_set_y(ui_lblYouMayLeave, 30);

    lv_obj_set_align(ui_lblYouMayLeave, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblYouMayLeave, "You may now leave booth");

    lv_obj_set_style_text_font(ui_lblYouMayLeave, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void ui_scrNotDone_screen_init(void)
{

    // ui_scrNotDone

    // ui_scrNotDone = lv_obj_create(NULL);
    ui_scrNotDone = lv_disp_get_scr_act(NULL);
    lv_obj_clear_flag(ui_scrNotDone, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_scrNotDone, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_scrNotDone, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_imgLogoNotDone

    ui_imgLogoNotDone = lv_img_create(ui_scrNotDone);
    lv_img_set_src(ui_imgLogoNotDone, &ui_img_logo_std_small_png);

    lv_obj_set_width(ui_imgLogoNotDone, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_imgLogoNotDone, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_imgLogoNotDone, 0);
    lv_obj_set_y(ui_imgLogoNotDone, -130);

    lv_obj_set_align(ui_imgLogoNotDone, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_imgLogoNotDone, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_imgLogoNotDone, LV_OBJ_FLAG_SCROLLABLE);

    // ui_Label7

    ui_Label7 = lv_label_create(ui_scrNotDone);

    lv_obj_set_width(ui_Label7, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_Label7, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_Label7, 0);
    lv_obj_set_y(ui_Label7, 0);

    lv_obj_set_align(ui_Label7, LV_ALIGN_CENTER);

    lv_label_set_text(ui_Label7, "Disinfection Not Done");

    lv_obj_set_style_text_font(ui_Label7, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);


}

void ui_scrDoors_screen_init(void)
{

    // ui_scrDoors

    // ui_scrDoors = lv_obj_create(NULL);
    ui_scrDoors = lv_disp_get_scr_act(NULL);
    lv_obj_clear_flag(ui_scrDoors, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_scrDoors, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_scrDoors, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_imgLogoDoors

    ui_imgLogoDoors = lv_img_create(ui_scrDoors);
    lv_img_set_src(ui_imgLogoDoors, &ui_img_logo_std_small_png);

    lv_obj_set_width(ui_imgLogoDoors, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_imgLogoDoors, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_imgLogoDoors, 0);
    lv_obj_set_y(ui_imgLogoDoors, -130);

    lv_obj_set_align(ui_imgLogoDoors, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_imgLogoDoors, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_imgLogoDoors, LV_OBJ_FLAG_SCROLLABLE);

    // ui_lblDoorsOpen

    ui_lblDoorsOpen = lv_label_create(ui_scrDoors);

    lv_obj_set_width(ui_lblDoorsOpen, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblDoorsOpen, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblDoorsOpen, 0);
    lv_obj_set_y(ui_lblDoorsOpen, 0);

    lv_obj_set_align(ui_lblDoorsOpen, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblDoorsOpen, "Doors are open");

    lv_obj_set_style_text_font(ui_lblDoorsOpen, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_lblCloseDoors

    ui_lblCloseDoors = lv_label_create(ui_scrDoors);

    lv_obj_set_width(ui_lblCloseDoors, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblCloseDoors, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblCloseDoors, 0);
    lv_obj_set_y(ui_lblCloseDoors, 30);

    lv_obj_set_align(ui_lblCloseDoors, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblCloseDoors, "Please close doors to enable booth");

    lv_obj_set_style_text_font(ui_lblCloseDoors, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

}
void ui_scrDoors1_screen_init(void)
{

    // ui_scrDoors1

    //ui_scrDoors1 = lv_obj_create(NULL);
    ui_scrDoors1= lv_disp_get_scr_act(NULL);
    lv_obj_clear_flag(ui_scrDoors1, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_scrDoors1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_scrDoors1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_imgLogoDoors1

    ui_imgLogoDoors1 = lv_img_create(ui_scrDoors1);
    lv_img_set_src(ui_imgLogoDoors1, &ui_img_logo_std_small_png);

    lv_obj_set_width(ui_imgLogoDoors1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_imgLogoDoors1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_imgLogoDoors1, 0);
    lv_obj_set_y(ui_imgLogoDoors1, -130);

    lv_obj_set_align(ui_imgLogoDoors1, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_imgLogoDoors1, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_imgLogoDoors1, LV_OBJ_FLAG_SCROLLABLE);

    // ui_lblErrror

    ui_lblErrror = lv_label_create(ui_scrDoors1);

    lv_obj_set_width(ui_lblErrror, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblErrror, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblErrror, 0);
    lv_obj_set_y(ui_lblErrror, 0);

    lv_obj_set_align(ui_lblErrror, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblErrror, "ERROR");

    lv_obj_set_style_text_font(ui_lblErrror, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_lblCloseDoors1

    ui_lblCloseDoors1 = lv_label_create(ui_scrDoors1);

    lv_obj_set_width(ui_lblCloseDoors1, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblCloseDoors1, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblCloseDoors1, 0);
    lv_obj_set_y(ui_lblCloseDoors1, 30);

    lv_obj_set_align(ui_lblCloseDoors1, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblCloseDoors1, "<ERRORTEXT>");

    lv_obj_set_style_text_font(ui_lblCloseDoors1, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

}



void ui_init(void)
{
    dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
/*
    lv_theme_t * theme_start = lv_theme_default_init(dispp_start, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp_start, theme_start);

    lv_theme_t * theme_hold = lv_theme_default_init(dispp_hold, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp_hold, theme_hold);
*/
    bar = lv_obj_create(NULL);
    //ui_scrHold_screen_init();

    // ui_scrStart_screen_load();
    // lv_obj_clear_flag(ui_scrStart, LV_OBJ_FLAG_SCROLLABLE);

    // lv_obj_set_style_bg_color(ui_scrStart, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_opa(ui_scrStart, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_scrStart_screen_init();
    /*
    ui_scrHold_screen_init();
    ui_scrDone_screen_init();
    ui_scrDoors_screen_init();
    ui_scrDoors1_screen_init();
    */
    /*
    ui_scrHold_screen_init();
    lv_disp_load_scr(ui_scrHold);
    vTaskDelay(100 / portTICK_RATE_MS);
    printf("uint32_t lv_anim_get_playtime(lv_anim_t * a);= %d",lv_anim_get_playtime(&a));
    */
    // lv_example_slider_1();
    //lv_example_bar_6();
    // lcd_test();
    // ui_scrStart_screen_load();
    // vTaskDelay(1000 / portTICK_RATE_MS);
    lv_timer_create(tab_changer_task_cb, TASK_PEROPD, NULL);
    //lv_timer_create(test_task_cb, TASK_PEROPD, NULL);
    

    
    
}

void ui_clean(void)
{
    
    lv_obj_clean(ui_scrStart);
    /*
    lv_obj_clean(ui_scrHold);
    lv_obj_clean(ui_scrDone);
    lv_obj_clean(ui_scrDoors);
    lv_obj_clean(ui_scrDoors1);
    */
    lv_obj_clean(lv_scr_act());
    
    ui_scrStart = NULL;
    ui_scrHold = NULL;
    ui_scrDone = NULL;
    ui_scrDoors = NULL;
    ui_scrDoors1 = NULL;
    
    
}
uint8_t move_flag = 1;
uint8_t start_trigger_flag = 0;
uint8_t buzzer_cnt = 0;
uint8_t buzzer_cnt_2 = 0;
uint8_t buzzer_cnt_3 = 0;
uint16_t NotDone_cnt = 0;
bool Open_door_while_Hold = false;
bool Disinfection_done_flag = false;
bool Buzzer_error_flag = false;
static void tab_changer_task_cb(lv_timer_t * tmr)
{
    (void) tmr;    /*Unused*/
    
    //if(TEST_MODE == 1)
    /*
    LCD_Page = Display_Hold;
    hall_sensor_flag = true;
    start_sensor_flag = true;
    stop_sensor_flag = false;
    */
    /*
    if((Hall1_voltage <= 950 || Hall1_voltage >= 1150 )&&\
    (Hall2_voltage <= 950 || Hall2_voltage >= 1150)) hall_sensor_flag = true;       //Door closed
    else hall_sensor_flag = false;          //Door open
    */
    hall_sensor_flag = true;
    if(Start_distance <= 100) start_trigger_flag = true;
    else start_trigger_flag = false;
    if(Stop_distance <= 100) stop_sensor_flag = true;
    else stop_sensor_flag = false;

    if(move_flag && start_trigger_flag == 1 && LCD_Page == Display_Start) //<100mm trigger
	{
		move_flag = 0;
		if(start_trigger_flag == 1) start_sensor_flag = true;
	}
	else if(start_trigger_flag == 0)move_flag = 1;
    
    //if(Start_distance <= 100) start_sensor_flag = true;
    //else start_sensor_flag = false;
    /*
    printf("hall_sensor_flag = %d,start_sensor_flag = %d,stop_sensor_flag = %d\n",\
    hall_sensor_flag,start_sensor_flag,stop_sensor_flag);
    */
    if(Disinfection_done_flag == true){
        buzzer_cnt_2 ++;
        if(buzzer_cnt_2 == 1)gpio_set_level(33 , 1);    //on
        else if(buzzer_cnt_2 == 11){
            gpio_set_level(33 , 0);                     //off
            buzzer_cnt_2 = 0;
            Disinfection_done_flag = false;
        }
    }
    if(Buzzer_error_flag == true){
        buzzer_cnt_3 ++;
        if(buzzer_cnt_3 % 2 == 0)gpio_set_level(33 , 1);            //on
        else if(buzzer_cnt_3 % 2 == 1)gpio_set_level(33 , 0);       //off
        if(buzzer_cnt_3 >= 21){
            gpio_set_level(33 , 0);                                 //off
            buzzer_cnt_3 = 0;
            Buzzer_error_flag = false;
        }
    }
    //printf("buzzer_cnt_3 = %d\n",buzzer_cnt_3);
    //printf("gpio = %d, Buzzer_error_flag = %d, buzzer_cnt_3 = %d, LCD_Page = %d\n",
    //gpio_get_level(33),Buzzer_error_flag,buzzer_cnt_3,LCD_Page);
    if(Device_missing_flag == true && pre_flag == false) LCD_Page = Display_Error;
    //printf("Disinfection_done_flag = %d gpio_get_level = %d\n", Disinfection_done_flag, gpio_get_level(33));
    switch(LCD_Page) {
        /*
        case 0:
        {
            lv_res_t res = lv_mem_test();
            if(res != LV_RES_OK) {
                LV_LOG_ERROR("Memory integrity error");
            }

            lv_mem_monitor_t mon;
            lv_mem_monitor(&mon);
            LV_LOG_USER("mem leak since start: %d, frag: %3d %%",  mem_free_start - mon.free_size, mon.frag_pct);
            printf("mem leak since start: %d, free_size: %d,frag: %3d %%\n",  mem_free_start - mon.free_size, mon.free_size, mon.frag_pct);
        }
            LCD_Page++;
            break;
        */
        case Display_Doors:
            if(pre_flag == false){
                LCD_Change_Flag = true;
                // ui_clean();
                ui_scrDoors_screen_init();
                // lv_disp_load_scr(ui_scrDoors);
                pre_flag = true;
            }
            if(hall_sensor_flag == true ||Device_missing_flag == true){
                pre_flag = false;
                // lv_obj_clean(ui_scrDoors);
                // lv_obj_del(ui_scrDoors);
                lv_obj_clean(lv_scr_act());
                ui_scrDoors = NULL;
                LCD_Page = Display_Start;
            }
            break;

        case Display_Start:
            if(pre_flag == false){
                LCD_Change_Flag = true;
                // ui_clean();
                ui_scrStart_screen_init();
                // lv_disp_load_scr(ui_scrStart);
                // ui_scrStart_screen_load();
                pre_flag = true;
            }
            if((start_sensor_flag == true && stop_sensor_flag == false)\
            ||hall_sensor_flag == false ||Device_missing_flag == true){
                pre_flag = false;
                start_sensor_flag = false;
                // lv_obj_clean(ui_scrStart);
                // lv_obj_del(ui_scrStart);
                lv_obj_clean(lv_scr_act());
                ui_scrStart = NULL;
                if(hall_sensor_flag == false)LCD_Page = Display_Doors;
                else LCD_Page = Display_Hold;
            }
            break;

        case Display_Hold:
            buzzer_cnt++;
            if(Disinfection_done_flag == false){
                if(buzzer_cnt >= 10){       
                    gpio_set_level(33 , 1);    //on
                    buzzer_cnt = 0;
                }
                else gpio_set_level(33 , 0);  //off
                //printf("buzzer_cnt = %d\n", buzzer_cnt);
            }
            
            if(pre_flag == false){
                LCD_Change_Flag = true;
                // ui_clean();
                ui_scrHold_screen_init();
                /*
                lv_obj_set_parent(ui_pgrDone, ui_scrHold);
                lv_obj_set_parent(ui_imgStand, ui_scrHold);
                lv_obj_set_parent(ui_lblPose, ui_scrHold);
                lv_obj_set_parent(ui_lblTimeLeft, ui_scrHold);
                lv_obj_set_parent(ui_lblTime, ui_scrHold);
                lv_obj_set_parent(ui_Spinner1, ui_scrHold);
                */
                //lv_disp_load_scr(ui_scrHold);
                //lv_anim_start(&a);
                //lv_anim_start(&a);
                pre_flag = true;
                ani_flag = true;
                UV_On_UDP_Flag = true;
            }
            if(ani_flag == false||stop_sensor_flag == 1||hall_sensor_flag == false||Device_missing_flag == true){
                pre_flag = false;
                //lv_obj_clean(ui_scrHold);
                // lv_obj_del(ui_scrHold);
                lv_obj_clean(lv_scr_act());
                ui_scrHold = NULL;
                lv_disp_clean_dcache(dispp);
                //ani_del(&a);
                //vTaskDelay(5000 / portTICK_PERIOD_MS);
                //lv_disp_load_scr(ui_scrStart);
                //lv_disp_clean_dcache(dispp_hold);
                //lv_obj_del(ui_scrHold);
                Disinfection_done_flag = true; 
                if(hall_sensor_flag == false) Open_door_while_Hold = true;
                else Open_door_while_Hold = false;
                //printf("Open_door_while_Hold = %d\n",Open_door_while_Hold);
                if(stop_sensor_flag == true || hall_sensor_flag == false)LCD_Page = Display_Start;
                //else if(hall_sensor_flag == false)LCD_Page = Display_NotDone;
                else LCD_Page = Display_Start;
                UV_On_UDP_Flag = false;
            }
            /*
            {
                lv_res_t res = lv_mem_test();
                if(res != LV_RES_OK) {
                    LV_LOG_ERROR("Memory integrity error");
                }

                lv_mem_monitor_t mon;
                lv_mem_monitor(&mon);
                LV_LOG_USER("mem leak since start: %d, frag: %3d %%",  mem_free_start - mon.free_size, mon.frag_pct);
                //printf("free_size: %d, ani_flag = %d, pre_flag= %d\n",mon.free_size,ani_flag,pre_flag);
            }
            */
            break;

        case Display_Done:
            if(pre_flag == false){
                LCD_Change_Flag = true;
                ui_scrDone_screen_init();
                pre_flag = true;
            }
            if(hall_sensor_flag == false ||Device_missing_flag == true){
                
                pre_flag = false;
                lv_obj_clean(lv_scr_act());
                ui_scrDone = NULL;
                LCD_Page = Display_Doors;
            }
            break;

        case Display_Error:
            if(pre_flag == false && Device_missing_flag == true){
                LCD_Change_Flag = true;
                ui_scrDoors1_screen_init();
                pre_flag = true;
                UV_On_UDP_Flag = false;
                Buzzer_error_flag = true;
                lv_label_set_text(ui_lblCloseDoors1, "<Missing Module>");
                
            }
            //if(pre_flag == true && Device_missing_flag == true){
                //lv_label_set_text(ui_lblCloseDoors1, "<Missing Module>");
            //}
            if(Device_missing_flag == false){
                
                pre_flag = false;
                lv_obj_clean(lv_scr_act());
                ui_scrDone = NULL;
                LCD_Page = Display_Doors;
            }
            break;

        case Display_NotDone:
            if(pre_flag == false){
                Buzzer_error_flag = true;
                LCD_Change_Flag = true;
                // ui_clean();
                ui_scrNotDone_screen_init();
                // lv_disp_load_scr(ui_scrNotDone);
                pre_flag = true;
            }
            if((hall_sensor_flag == true && Open_door_while_Hold == true)\
            || Device_missing_flag == true){   //Door close at the beginning
                
                pre_flag = false;
                lv_obj_clean(lv_scr_act());
                ui_scrNotDone = NULL;
                LCD_Page = Display_Doors;
                NotDone_cnt = 0;
            }
            if((hall_sensor_flag == false && Open_door_while_Hold == false)\
            || Device_missing_flag == true){   //Door open at the beginning
                
                pre_flag = false;
                lv_obj_clean(lv_scr_act());
                ui_scrNotDone = NULL;
                LCD_Page = Display_Doors;
                NotDone_cnt = 0;
            }
            NotDone_cnt ++;
            if(NotDone_cnt >= 300)
            {
                pre_flag = false;
                lv_obj_clean(lv_scr_act());
                ui_scrNotDone = NULL;
                LCD_Page = Display_Doors;
                NotDone_cnt = 0;
            }
            break;
        case 20:
            if(pre_flag == false){
                lv_example_bar_6();
                pre_flag = true;
            }
            else if(ani_flag == false){
                
                pre_flag = false;
                ani_del(bar);
                LCD_Page++;
            }
            /*
            lv_res_t res = lv_mem_test();
            if(res != LV_RES_OK) {
                LV_LOG_ERROR("Memory integrity error");
            }

            lv_mem_monitor_t mon;
            lv_mem_monitor(&mon);
            LV_LOG_USER("mem leak since start: %d, frag: %3d %%",  mem_free_start - mon.free_size, mon.frag_pct);
            printf("mem leak since start: %d, free_size: %d,frag: %3d %%\n",  mem_free_start - mon.free_size, mon.free_size, mon.frag_pct);
            */
            break;
        default:
            break;
    }
    //printf("LCD_Page: %d\n", LCD_Page);
}

static void event_cb(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
    if(dsc->part != LV_PART_INDICATOR) return;

    lv_obj_t * obj= lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", (int)lv_bar_get_value(obj));
    //if(buf[0]=='9'&&buf[1]=='9')ani_flag = false;
    //else 
    if(buf[0]=='1'&&buf[1]=='0'&&buf[2]=='0')ani_flag = false;
    else ani_flag = true;
    /*
    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);

    lv_area_t txt_area;
    //If the indicator is long enough put the text inside on the right
    if(lv_area_get_width(dsc->draw_area) > txt_size.x + 20) {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    //If the indicator is still short put the text out of it on the right
    else {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_black();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
    */
    float calculation = (100-atoi(buf)) * ANI_TIME_SEC /100;
    if(calculation < ANI_TIME_SEC) calculation++;
    if(ani_flag == false)calculation = 0;
    uvMiliLeft = (calculation * 1000);
    itoa((int)calculation,time_left,10);
    lv_label_set_text(ui_lblTimeLeft, time_left);
    //lv_obj_set_align(ui_lblTimeLeft, LV_ALIGN_CENTER);
    //lv_obj_align_to(ui_lblTimeLeft, ui_lblTime, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    //lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    //printf("buf = %s,calculation = %f, time_left = %s,atoi(buf)= %d\n",buf,calculation,time_left,atoi(buf));
}

static void set_value(void *bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_ON);
}
static void slider1_event_cb(lv_event_t * e);
static lv_obj_t * slider_label;
/**
 * A default slider with a label displaying the current value
 */
void lv_example_slider_1(void)
{
    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_center(slider);
    lv_obj_add_event_cb(slider, slider1_event_cb, LV_EVENT_DRAW_PART_END, NULL);

    /*Create a label below the slider*/
    slider_label = lv_label_create(lv_scr_act());
    // lv_label_set_text(slider_label, "0%");

    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, slider);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static void slider1_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

void lv_example_bar_6(void)
{
    bar = lv_bar_create(lv_scr_act());
    lv_obj_add_event_cb(bar, event_cb, LV_EVENT_DRAW_PART_END, NULL);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_center(bar);

    lv_anim_init(&a);
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_time(&a, TASK_PEROPD * 10);
    //lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    //auto_del(bar, TASK_PEROPD * 10);
    
}

static void auto_del(lv_obj_t * obj, uint32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 0);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);

}

static void ani_del(lv_obj_t * obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_time(&a, 0);
    lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
    lv_anim_start(&a);

}

void ui_scrStart_screen_load(void)
{

    // ui_scrStart

    // ui_scrStart = lv_obj_create(NULL);
    ui_scrStart = lv_disp_get_scr_act(NULL);
    lv_obj_clear_flag(ui_scrStart, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(ui_scrStart, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_scrStart, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_disp_load_scr(ui_scrStart);
    // ui_imgLogo

    ui_imgLogo = lv_img_create(ui_scrStart);
    lv_img_set_src(ui_imgLogo, &ui_img_logo_std_small_png);

    lv_obj_set_width(ui_imgLogo, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_imgLogo, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_imgLogo, 0);
    lv_obj_set_y(ui_imgLogo, -130);

    lv_obj_set_align(ui_imgLogo, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_imgLogo, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_imgLogo, LV_OBJ_FLAG_SCROLLABLE);

    // ui_lblPLaceHand

    ui_lblPLaceHand = lv_label_create(ui_scrStart);

    lv_obj_set_width(ui_lblPLaceHand, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblPLaceHand, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblPLaceHand, 0);
    lv_obj_set_y(ui_lblPLaceHand, 0);

    lv_obj_set_align(ui_lblPLaceHand, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblPLaceHand, "Place hand over start sensor ");

    lv_obj_set_style_text_font(ui_lblPLaceHand, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_lblToStart

    ui_lblToStart = lv_label_create(ui_scrStart);

    lv_obj_set_width(ui_lblToStart, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblToStart, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblToStart, 0);
    lv_obj_set_y(ui_lblToStart, 30);

    lv_obj_set_align(ui_lblToStart, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblToStart, "to start disinfection");

    lv_obj_set_style_text_font(ui_lblToStart, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

}

void guiTask(void *pvParameter)
{
    xGuiSemaphore = xSemaphoreCreateMutex();

    lv_init();

    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2 != NULL);
#else
    static lv_color_t *buf2 = NULL;
#endif

    static lv_disp_draw_buf_t disp_buf;

    uint32_t size_in_px = DISP_BUF_SIZE;

#if defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_IL3820 || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_JD79653A || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_UC8151D || defined CONFIG_LV_TFT_DISPLAY_CONTROLLER_SSD1306
    size_in_px *= 8;
#endif

    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;

    /* When using a monochrome display we need to register the callbacks:
     * - rounder_cb
     * - set_px_cb */
#ifdef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    disp_drv.rounder_cb = disp_driver_rounder;
    disp_drv.set_px_cb = disp_driver_set_px;
#endif

    disp_drv.draw_buf = &disp_buf;
    // disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = 480;
    ESP_LOGI(TAG, "disp_drv.hor_res = %d", disp_drv.hor_res);
    ESP_LOGI(TAG, "disp_drv.ver_res = %d", disp_drv.ver_res);
    lv_disp_drv_register(&disp_drv);

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000)); // 1ms

    ui_init();

    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        // vTaskDelay(pdMS_TO_TICKS(10));
        vTaskDelay(10 / portTICK_PERIOD_MS);
        /* Try to take the semaphore, call lvgl related function on success */
        //if (ota_start_flag == false && firmware_updating_flag == false && ota_running_flag == false)
        //{
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
        //}
    }

    /* A task should NEVER return */
    free(buf1);
#ifndef CONFIG_LV_TFT_DISPLAY_MONOCHROME
    free(buf2);
#endif
}

void lv_tick_task(void *arg)
{
    (void)arg;

    lv_tick_inc(LV_TICK_PERIOD_MS);
}
