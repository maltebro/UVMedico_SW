// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: Novo1

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
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
lv_obj_t * ui_scrDoors;
lv_obj_t * ui_imgLogoDoors;
lv_obj_t * ui_lblDoorsOpen;
lv_obj_t * ui_lblCloseDoors;
lv_obj_t * ui_scrDoors1;
lv_obj_t * ui_imgLogoDoors1;
lv_obj_t * ui_lblErrror;
lv_obj_t * ui_lblCloseDoors1;

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

    ui_scrStart = lv_obj_create(NULL);

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

    ui_scrHold = lv_obj_create(NULL);

    lv_obj_clear_flag(ui_scrHold, LV_OBJ_FLAG_SCROLLABLE);

    // ui_pgrDone

    ui_pgrDone = lv_bar_create(ui_scrHold);
    lv_bar_set_range(ui_pgrDone, 0, 100);
    lv_bar_set_value(ui_pgrDone, 25, LV_ANIM_OFF);

    lv_obj_set_width(ui_pgrDone, 240);
    lv_obj_set_height(ui_pgrDone, 25);

    lv_obj_set_x(ui_pgrDone, 0);
    lv_obj_set_y(ui_pgrDone, 200);

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
    lv_obj_set_y(ui_imgStand, 0);

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

    lv_obj_set_x(ui_lblTimeLeft, 0);
    lv_obj_set_y(ui_lblTimeLeft, 160);

    lv_obj_set_align(ui_lblTimeLeft, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblTimeLeft, "20");

    lv_obj_set_style_text_font(ui_lblTimeLeft, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_lblTime

    ui_lblTime = lv_label_create(ui_scrHold);

    lv_obj_set_width(ui_lblTime, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_lblTime, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_lblTime, 0);
    lv_obj_set_y(ui_lblTime, 140);

    lv_obj_set_align(ui_lblTime, LV_ALIGN_CENTER);

    lv_label_set_text(ui_lblTime, "Time Left");

    lv_obj_set_style_text_font(ui_lblTime, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // ui_Spinner1

    ui_Spinner1 = lv_spinner_create(ui_scrHold, 1000, 90);

    lv_obj_set_width(ui_Spinner1, 250);
    lv_obj_set_height(ui_Spinner1, 250);

    lv_obj_set_x(ui_Spinner1, 0);
    lv_obj_set_y(ui_Spinner1, 0);

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

    ui_scrDone = lv_obj_create(NULL);

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
void ui_scrDoors_screen_init(void)
{

    // ui_scrDoors

    ui_scrDoors = lv_obj_create(NULL);

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

    ui_scrDoors1 = lv_obj_create(NULL);

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
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    ui_scrStart_screen_init();
    ui_scrHold_screen_init();
    ui_scrDone_screen_init();
    ui_scrDoors_screen_init();
    ui_scrDoors1_screen_init();
    // lv_disp_load_scr(ui_scrHold);
}

