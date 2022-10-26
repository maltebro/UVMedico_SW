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
lv_obj_t * bar;
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

    //lv_disp_set_rotation(ui_imgLogo, 90); // changing rotation of img by 90 degrees?

    lv_img_set_src(ui_imgLogo, &ui_img_logo_std_small_png);

    lv_obj_set_width(ui_imgLogo, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_imgLogo, LV_SIZE_CONTENT);

    lv_obj_set_x(ui_imgLogo, 0);
    lv_obj_set_y(ui_imgLogo, -130);

    lv_obj_set_align(ui_imgLogo, LV_ALIGN_CENTER);

    lv_obj_add_flag(ui_imgLogo, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_imgLogo, LV_OBJ_FLAG_SCROLLABLE);

}

void ui_scrHold_screen_init(void)
{

    // ui_scrHold

    // ui_scrHold = lv_obj_create(NULL);
    ui_scrHold = lv_disp_get_scr_act(NULL);
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

void lv_start_menu(void)
{

    /*Create a menu object*/
    lv_obj_t * menu = lv_menu_create(lv_scr_act());
    lv_obj_set_size(menu, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_center(menu);

    /*Modify the header*/
    lv_obj_t * back_btn = lv_menu_get_main_header_back_btn(menu);
    lv_obj_t * back_btn_label = lv_label_create(back_btn);
    lv_label_set_text(back_btn_label, "Back");

    lv_obj_t * cont;
    lv_obj_t * label;

    /*Create sub pages*/
    lv_obj_t * sub_1_page = lv_menu_page_create(menu, "Page 1");

    cont = lv_menu_cont_create(sub_1_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Next 'OK' will activate the laser-indicators");

    lv_obj_t * sub_2_page = lv_menu_page_create(menu, "Page 2");

    cont = lv_menu_cont_create(sub_2_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Next 'OK' will activate the laser-indicators");

    lv_obj_t * sub_3_page = lv_menu_page_create(menu, "Page 3");

    cont = lv_menu_cont_create(sub_3_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Next 'OK' will activate the laser-indicators");

    /*Create a main page*/
    lv_obj_t * main_page = lv_menu_page_create(menu, NULL);

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Standard treatment");
    lv_menu_set_load_page_event(menu, cont, sub_1_page);

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Anti-Resistant treatment");
    lv_menu_set_load_page_event(menu, cont, sub_2_page);

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Operating room treatment");
    lv_menu_set_load_page_event(menu, cont, sub_3_page);

    lv_menu_set_page(menu, main_page);

} 

void ui_init(void)
{
    lv_disp_t * dispp = lv_disp_get_default();
    lv_theme_t * theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    bar = lv_obj_create(NULL);

    ui_scrStart_screen_init();

    //ui_scrHold_screen_init();

    // lv_disp_load_scr(ui_scrHold);
}


