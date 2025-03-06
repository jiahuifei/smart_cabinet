#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *tabview;
    lv_obj_t *home_home;
    lv_obj_t *home_home_use;
    lv_obj_t *home_home_return;
    lv_obj_t *home_home_maintain;
    lv_obj_t *home_idcheck;
    lv_obj_t *home_idcheck_ok;
    lv_obj_t *home_idcheck_keyboard;
    lv_obj_t *home_idcheck_textarea;
    lv_obj_t *home_select;
    lv_obj_t *home_select_btn0;
    lv_obj_t *home_select_btn1;
    lv_obj_t *home_select_btn2;
    lv_obj_t *home_select_btn3;
    lv_obj_t *home_select_text;
    lv_obj_t *home_select_ok;
    lv_obj_t *home_final;
    lv_obj_t *home_final_text;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/