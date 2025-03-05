#include <string.h>

#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

#include <string.h>

objects_t objects;
lv_obj_t *tick_value_change_obj;

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    lv_obj_t *obj = lv_obj_create(0);
    objects.main = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    {
        lv_obj_t *parent_obj = obj;
        {
            // tabview
            lv_obj_t *obj = lv_tabview_create(parent_obj, LV_DIR_TOP, 32);
            objects.tabview = obj;
            lv_obj_set_pos(obj, 0, 0);
            lv_obj_set_size(obj, 800, 480);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // home_home
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "HOME");
                    objects.home_home = obj;
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // home_home_use
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_home_use = obj;
                            lv_obj_set_pos(obj, 10, 78);
                            lv_obj_set_size(obj, 320, 320);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, "Use");
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                            }
                        }
                        {
                            // home_home_return
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_home_return = obj;
                            lv_obj_set_pos(obj, 431, 78);
                            lv_obj_set_size(obj, 320, 320);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, "Return");
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                            }
                        }
                        {
                            // home_home_maintain
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_home_maintain = obj;
                            lv_obj_set_pos(obj, 630, 4);
                            lv_obj_set_size(obj, 121, 50);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, "Maintain");
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                            }
                        }
                    }
                }
                {
                    // home_idcheck
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "ID_CHECK");
                    objects.home_idcheck = obj;
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // home_idcheck_ok
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_idcheck_ok = obj;
                            lv_obj_set_pos(obj, 594, 0);
                            lv_obj_set_size(obj, 131, 114);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, "OK");
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                            }
                        }
                        {
                            // home_idcheck_keyboard
                            lv_obj_t *obj = lv_keyboard_create(parent_obj);
                            objects.home_idcheck_keyboard = obj;
                            lv_obj_set_pos(obj, 35, 128);
                            lv_obj_set_size(obj, 690, 251);
                            lv_obj_set_style_align(obj, LV_ALIGN_DEFAULT, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                        {
                            // home_idcheck_textarea
                            lv_obj_t *obj = lv_textarea_create(parent_obj);
                            objects.home_idcheck_textarea = obj;
                            lv_obj_set_pos(obj, 35, -1);
                            lv_obj_set_size(obj, 526, 112);
                            lv_textarea_set_max_length(obj, 128);
                            lv_textarea_set_one_line(obj, false);
                            lv_textarea_set_password_mode(obj, false);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                        }
                    }
                }
                {
                    // home_select
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "SELECT");
                    objects.home_select = obj;
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // home_select_btn1
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.home_select_btn1 = obj;
                            lv_obj_set_pos(obj, 27, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 192);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_goggles_grey_, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_goggles, NULL);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // home_select_btn0
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.home_select_btn0 = obj;
                            lv_obj_set_pos(obj, 27, 207);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 192);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_helmet__grey_, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_helmet, NULL);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // home_select_btn2
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.home_select_btn2 = obj;
                            lv_obj_set_pos(obj, 278, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 192);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_reflective_clothing__grey_, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_reflective_clothing, NULL);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // home_select_btn3
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.home_select_btn3 = obj;
                            lv_obj_set_pos(obj, 528, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 192);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_toe_cap_grey_, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_toe_cap, NULL);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // home_select_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_select_text = obj;
                            lv_obj_set_pos(obj, 278, 226);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Text");
                        }
                        {
                            // home_select_ok
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_select_ok = obj;
                            lv_obj_set_pos(obj, 614, 289);
                            lv_obj_set_size(obj, 127, 85);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_label_set_text(obj, "OK");
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                }
                            }
                        }
                    }
                }
                {
                    // home_final
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "FINISH");
                    objects.home_final = obj;
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // home_final_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_final_text = obj;
                            lv_obj_set_pos(obj, 316, 196);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_label_set_text(obj, "Thanks for using...");
                        }
                    }
                }
            }
        }
    }
    lv_keyboard_set_textarea(objects.home_idcheck_keyboard, objects.home_idcheck_textarea);
    
    tick_screen_main();
}

void tick_screen_main() {
    void *flowState = getFlowState(0, 0);
}


static const char *screen_names[] = { "Main" };
static const char *object_names[] = { "main", "tabview", "home_home", "home_home_use", "home_home_return", "home_home_maintain", "home_idcheck", "home_idcheck_ok", "home_idcheck_keyboard", "home_idcheck_textarea", "home_select", "home_select_btn1", "home_select_btn0", "home_select_btn2", "home_select_btn3", "home_select_text", "home_select_ok", "home_final", "home_final_text" };


typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

void create_screens() {
    eez_flow_init_screen_names(screen_names, sizeof(screen_names) / sizeof(const char *));
    eez_flow_init_object_names(object_names, sizeof(object_names) / sizeof(const char *));
    
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), false, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);
    
    create_screen_main();
}
