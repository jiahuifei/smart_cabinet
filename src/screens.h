#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *manage;
    lv_obj_t *home_home_maintain;
    lv_obj_t *obj0;
    lv_obj_t *tabview;
    lv_obj_t *home_home;
    lv_obj_t *home_home_use;
    lv_obj_t *home_home_return;
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
    lv_obj_t *grid_1_text;
    lv_obj_t *grid_2_text;
    lv_obj_t *home_select_ok;
    lv_obj_t *home_final;
    lv_obj_t *home_final_text;
    lv_obj_t *manage_btn_37;
    lv_obj_t *manage_btn_38;
    lv_obj_t *manage_btn_39;
    lv_obj_t *manage_btn_40;
    lv_obj_t *manage_btn_41;
    lv_obj_t *manage_btn_42;
    lv_obj_t *manage_btn_43;
    lv_obj_t *manage_btn_44;
    lv_obj_t *manage_btn_45;
    lv_obj_t *manage_btn_46;
    lv_obj_t *manage_btn_47;
    lv_obj_t *manage_btn_48;
    lv_obj_t *manage_btn_25;
    lv_obj_t *manage_btn_26;
    lv_obj_t *manage_btn_27;
    lv_obj_t *manage_btn_28;
    lv_obj_t *manage_btn_29;
    lv_obj_t *manage_btn_30;
    lv_obj_t *manage_btn_31;
    lv_obj_t *manage_btn_32;
    lv_obj_t *manage_btn_33;
    lv_obj_t *manage_btn_34;
    lv_obj_t *manage_btn_35;
    lv_obj_t *manage_btn_36;
    lv_obj_t *manage_btn_13;
    lv_obj_t *manage_btn_14;
    lv_obj_t *manage_btn_15;
    lv_obj_t *manage_btn_16;
    lv_obj_t *manage_btn_17;
    lv_obj_t *manage_btn_18;
    lv_obj_t *manage_btn_19;
    lv_obj_t *manage_btn_20;
    lv_obj_t *manage_btn_21;
    lv_obj_t *manage_btn_22;
    lv_obj_t *manage_btn_23;
    lv_obj_t *manage_btn_24;
    lv_obj_t *manage_btn_1;
    lv_obj_t *manage_btn_2;
    lv_obj_t *manage_btn_3;
    lv_obj_t *manage_btn_4;
    lv_obj_t *manage_btn_5;
    lv_obj_t *manage_btn_6;
    lv_obj_t *manage_btn_7;
    lv_obj_t *manage_btn_8;
    lv_obj_t *manage_btn_9;
    lv_obj_t *manage_btn_10;
    lv_obj_t *manage_btn_11;
    lv_obj_t *manage_btn_12;
    lv_obj_t *manage_btn_0;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_MANAGE = 2,
};

void create_screen_main();
void tick_screen_main();

void create_screen_manage();
void tick_screen_manage();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/