// SquareLine LVGL GENERATED FILE
// EDITOR VERSION: SquareLine Studio 1.0.5
// LVGL VERSION: 8.2
// PROJECT: Novo1

#ifndef _NOVO1_UI_H
#define _NOVO1_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "top.h"

#if __has_include("lvgl.h")
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "lvgl_helpers.h"

#define LV_TICK_PERIOD_MS 1
#define TEST_MODE       0

/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
SemaphoreHandle_t xGuiSemaphore;


enum LCD_Page_Types
{
    Display_Doors = 1,
    Display_Start = 2,
    Display_Hold = 3,
    Display_Done = 4,
    Display_Error = 5,
    Display_NotDone = 6
};

LV_IMG_DECLARE(ui_img_logo_std_small_png);    // assets\Logo_std_small.png
LV_IMG_DECLARE(ui_img_896891123);    // assets\man-007-512_small.png

void ui_clean(void);

static void tab_changer_task_cb(lv_timer_t * tmr);

void ui_init(void);

static void set_value(void *bar, int32_t v);

static void event_cb(lv_event_t * e);

static void slider_event_cb(lv_event_t * e);

void lv_example_get_started_3(void);

void lv_example_slider_1(void);

static void slider_event_cb(lv_event_t * e);

void lv_example_bar_6(void);

static void auto_del(lv_obj_t * obj, uint32_t delay);

static void ani_del(lv_obj_t * obj);

void ui_scrStart_screen_load(void);

void lv_tick_task(void *arg);

void guiTask(void *pvParameter);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
