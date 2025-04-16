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

static void event_handler_cb_main_home_home_maintain(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 7, 0, e);
    }
}

static void event_handler_cb_manage_obj0(lv_event_t *e) {
    lv_event_code_t event = lv_event_get_code(e);
    void *flowState = lv_event_get_user_data(e);
    (void)flowState;
    
    if (event == LV_EVENT_PRESSED) {
        e->user_data = (void *)0;
        flowPropagateValueLVGLEvent(flowState, 99, 0, e);
    }
}

void create_screen_main() {
    void *flowState = getFlowState(0, 0);
    (void)flowState;
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
            lv_obj_set_style_text_font(obj, &ui_font_111, LV_PART_MAIN | LV_STATE_DEFAULT);
            {
                lv_obj_t *parent_obj = obj;
                {
                    // home_home
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "主页");
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
                                    lv_obj_set_pos(obj, -3, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_222, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "领用");
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
                                    lv_obj_set_pos(obj, 2, 4);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_222, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "归还");
                                }
                            }
                        }
                        {
                            // home_home_maintain
                            lv_obj_t *obj = lv_btn_create(parent_obj);
                            objects.home_home_maintain = obj;
                            lv_obj_set_pos(obj, 630, 4);
                            lv_obj_set_size(obj, 121, 50);
                            lv_obj_add_event_cb(obj, event_handler_cb_main_home_home_maintain, LV_EVENT_ALL, flowState);
                            {
                                lv_obj_t *parent_obj = obj;
                                {
                                    lv_obj_t *obj = lv_label_create(parent_obj);
                                    lv_obj_set_pos(obj, 0, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "维护");
                                }
                            }
                        }
                    }
                }
                {
                    // home_idcheck
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "身份验证");
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
                                    lv_obj_set_pos(obj, 4, 0);
                                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "确定");
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
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
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
                            lv_obj_set_style_text_font(obj, &lv_font_montserrat_44, LV_PART_MAIN | LV_STATE_DEFAULT);
                        }
                    }
                }
                {
                    // home_select
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "选择");
                    objects.home_select = obj;
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // home_select_btn0
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.home_select_btn0 = obj;
                            lv_obj_set_pos(obj, 27, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 192);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_goggles_grey_, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_goggles, NULL);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // home_select_btn1
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.home_select_btn1 = obj;
                            lv_obj_set_pos(obj, 278, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 192);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_reflective_clothing__grey_, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_reflective_clothing, NULL);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // home_select_btn2
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.home_select_btn2 = obj;
                            lv_obj_set_pos(obj, 528, 0);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 192);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_toe_cap_grey_, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_toe_cap, NULL);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // home_select_btn3
                            lv_obj_t *obj = lv_imgbtn_create(parent_obj);
                            objects.home_select_btn3 = obj;
                            lv_obj_set_pos(obj, 27, 207);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, 192);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_RELEASED, NULL, &img_helmet__grey_, NULL);
                            lv_imgbtn_set_src(obj, LV_IMGBTN_STATE_PRESSED, NULL, &img_helmet, NULL);
                            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
                        }
                        {
                            // home_select_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_select_text = obj;
                            lv_obj_set_pos(obj, 278, 226);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "请拿取需要的劳防用品。");
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
                                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_obj_set_style_text_font(obj, &ui_font_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                                    lv_label_set_text(obj, "确定");
                                }
                            }
                        }
                    }
                }
                {
                    // home_final
                    lv_obj_t *obj = lv_tabview_add_tab(parent_obj, "结束");
                    objects.home_final = obj;
                    {
                        lv_obj_t *parent_obj = obj;
                        {
                            // home_final_text
                            lv_obj_t *obj = lv_label_create(parent_obj);
                            objects.home_final_text = obj;
                            lv_obj_set_pos(obj, 280, 196);
                            lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                            lv_obj_set_style_text_font(obj, &ui_font_30, LV_PART_MAIN | LV_STATE_DEFAULT);
                            lv_label_set_text(obj, "感谢使用......");
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
    (void)flowState;
}

void create_screen_manage() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
    lv_obj_t *obj = lv_obj_create(0);
    objects.manage = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 480);
    {
        lv_obj_t *parent_obj = obj;
        {
            // manage_btn_37
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_37 = obj;
            lv_obj_set_pos(obj, 443, 37);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "37");
                }
            }
        }
        {
            // manage_btn_38
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_38 = obj;
            lv_obj_set_pos(obj, 443, 105);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "38");
                }
            }
        }
        {
            // manage_btn_39
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_39 = obj;
            lv_obj_set_pos(obj, 443, 176);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "39");
                }
            }
        }
        {
            // manage_btn_40
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_40 = obj;
            lv_obj_set_pos(obj, 443, 248);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "40");
                }
            }
        }
        {
            // manage_btn_41
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_41 = obj;
            lv_obj_set_pos(obj, 443, 322);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "41");
                }
            }
        }
        {
            // manage_btn_42
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_42 = obj;
            lv_obj_set_pos(obj, 443, 394);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "42");
                }
            }
        }
        {
            // manage_btn_43
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_43 = obj;
            lv_obj_set_pos(obj, 577, 37);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "43");
                }
            }
        }
        {
            // manage_btn_44
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_44 = obj;
            lv_obj_set_pos(obj, 577, 105);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "44");
                }
            }
        }
        {
            // manage_btn_45
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_45 = obj;
            lv_obj_set_pos(obj, 577, 176);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "45");
                }
            }
        }
        {
            // manage_btn_46
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_46 = obj;
            lv_obj_set_pos(obj, 577, 248);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "46");
                }
            }
        }
        {
            // manage_btn_47
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_47 = obj;
            lv_obj_set_pos(obj, 577, 322);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "47");
                }
            }
        }
        {
            // manage_btn_48
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_48 = obj;
            lv_obj_set_pos(obj, 577, 394);
            lv_obj_set_size(obj, 100, 50);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "48");
                }
            }
        }
        {
            // manage_btn_25
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_25 = obj;
            lv_obj_set_pos(obj, 303, 43);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "25");
                }
            }
        }
        {
            // manage_btn_26
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_26 = obj;
            lv_obj_set_pos(obj, 303, 76);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "26");
                }
            }
        }
        {
            // manage_btn_27
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_27 = obj;
            lv_obj_set_pos(obj, 303, 110);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "27");
                }
            }
        }
        {
            // manage_btn_28
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_28 = obj;
            lv_obj_set_pos(obj, 303, 145);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "28");
                }
            }
        }
        {
            // manage_btn_29
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_29 = obj;
            lv_obj_set_pos(obj, 303, 179);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "29");
                }
            }
        }
        {
            // manage_btn_30
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_30 = obj;
            lv_obj_set_pos(obj, 303, 212);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "30");
                }
            }
        }
        {
            // manage_btn_31
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_31 = obj;
            lv_obj_set_pos(obj, 303, 248);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "31");
                }
            }
        }
        {
            // manage_btn_32
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_32 = obj;
            lv_obj_set_pos(obj, 303, 281);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "32");
                }
            }
        }
        {
            // manage_btn_33
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_33 = obj;
            lv_obj_set_pos(obj, 303, 315);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "33");
                }
            }
        }
        {
            // manage_btn_34
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_34 = obj;
            lv_obj_set_pos(obj, 303, 350);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "34");
                }
            }
        }
        {
            // manage_btn_35
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_35 = obj;
            lv_obj_set_pos(obj, 303, 384);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "35");
                }
            }
        }
        {
            // manage_btn_36
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_36 = obj;
            lv_obj_set_pos(obj, 303, 417);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "36");
                }
            }
        }
        {
            // manage_btn_13
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_13 = obj;
            lv_obj_set_pos(obj, 158, 43);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "13");
                }
            }
        }
        {
            // manage_btn_14
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_14 = obj;
            lv_obj_set_pos(obj, 158, 76);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "14");
                }
            }
        }
        {
            // manage_btn_15
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_15 = obj;
            lv_obj_set_pos(obj, 158, 110);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "15");
                }
            }
        }
        {
            // manage_btn_16
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_16 = obj;
            lv_obj_set_pos(obj, 158, 145);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "16");
                }
            }
        }
        {
            // manage_btn_17
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_17 = obj;
            lv_obj_set_pos(obj, 158, 179);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "17");
                }
            }
        }
        {
            // manage_btn_18
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_18 = obj;
            lv_obj_set_pos(obj, 158, 212);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "18");
                }
            }
        }
        {
            // manage_btn_19
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_19 = obj;
            lv_obj_set_pos(obj, 158, 248);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "19");
                }
            }
        }
        {
            // manage_btn_20
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_20 = obj;
            lv_obj_set_pos(obj, 158, 281);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "20");
                }
            }
        }
        {
            // manage_btn_21
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_21 = obj;
            lv_obj_set_pos(obj, 158, 315);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "21");
                }
            }
        }
        {
            // manage_btn_22
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_22 = obj;
            lv_obj_set_pos(obj, 158, 350);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "22");
                }
            }
        }
        {
            // manage_btn_23
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_23 = obj;
            lv_obj_set_pos(obj, 158, 384);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "23");
                }
            }
        }
        {
            // manage_btn_24
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_24 = obj;
            lv_obj_set_pos(obj, 158, 417);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "24");
                }
            }
        }
        {
            // manage_btn_1
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_1 = obj;
            lv_obj_set_pos(obj, 13, 44);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "1");
                }
            }
        }
        {
            // manage_btn_2
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_2 = obj;
            lv_obj_set_pos(obj, 13, 77);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "2");
                }
            }
        }
        {
            // manage_btn_3
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_3 = obj;
            lv_obj_set_pos(obj, 13, 111);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "3");
                }
            }
        }
        {
            // manage_btn_4
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_4 = obj;
            lv_obj_set_pos(obj, 13, 146);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "4");
                }
            }
        }
        {
            // manage_btn_5
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_5 = obj;
            lv_obj_set_pos(obj, 13, 180);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "5");
                }
            }
        }
        {
            // manage_btn_6
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_6 = obj;
            lv_obj_set_pos(obj, 13, 213);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "6");
                }
            }
        }
        {
            // manage_btn_7
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_7 = obj;
            lv_obj_set_pos(obj, 13, 249);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "7");
                }
            }
        }
        {
            // manage_btn_8
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_8 = obj;
            lv_obj_set_pos(obj, 13, 282);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "8");
                }
            }
        }
        {
            // manage_btn_9
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_9 = obj;
            lv_obj_set_pos(obj, 13, 316);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "9");
                }
            }
        }
        {
            // manage_btn_10
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_10 = obj;
            lv_obj_set_pos(obj, 13, 351);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "10");
                }
            }
        }
        {
            // manage_btn_11
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_11 = obj;
            lv_obj_set_pos(obj, 13, 385);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "11");
                }
            }
        }
        {
            // manage_btn_12
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_12 = obj;
            lv_obj_set_pos(obj, 13, 418);
            lv_obj_set_size(obj, 100, 25);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "12");
                }
            }
        }
        {
            // manage_btn_0
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.manage_btn_0 = obj;
            lv_obj_set_pos(obj, 700, 252);
            lv_obj_set_size(obj, 88, 190);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "ALL");
                }
            }
        }
        {
            lv_obj_t *obj = lv_btn_create(parent_obj);
            objects.obj0 = obj;
            lv_obj_set_pos(obj, 700, 43);
            lv_obj_set_size(obj, 88, 189);
            lv_obj_add_event_cb(obj, event_handler_cb_manage_obj0, LV_EVENT_ALL, flowState);
            {
                lv_obj_t *parent_obj = obj;
                {
                    lv_obj_t *obj = lv_label_create(parent_obj);
                    lv_obj_set_pos(obj, 0, 0);
                    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                    lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(obj, "HOME");
                }
            }
        }
    }
    
    tick_screen_manage();
}

void tick_screen_manage() {
    void *flowState = getFlowState(0, 1);
    (void)flowState;
}


static const char *screen_names[] = { "Main", "manage" };
static const char *object_names[] = { "main", "manage", "home_home_maintain", "obj0", "tabview", "home_home", "home_home_use", "home_home_return", "home_idcheck", "home_idcheck_ok", "home_idcheck_keyboard", "home_idcheck_textarea", "home_select", "home_select_btn0", "home_select_btn1", "home_select_btn2", "home_select_btn3", "home_select_text", "home_select_ok", "home_final", "home_final_text", "manage_btn_37", "manage_btn_38", "manage_btn_39", "manage_btn_40", "manage_btn_41", "manage_btn_42", "manage_btn_43", "manage_btn_44", "manage_btn_45", "manage_btn_46", "manage_btn_47", "manage_btn_48", "manage_btn_25", "manage_btn_26", "manage_btn_27", "manage_btn_28", "manage_btn_29", "manage_btn_30", "manage_btn_31", "manage_btn_32", "manage_btn_33", "manage_btn_34", "manage_btn_35", "manage_btn_36", "manage_btn_13", "manage_btn_14", "manage_btn_15", "manage_btn_16", "manage_btn_17", "manage_btn_18", "manage_btn_19", "manage_btn_20", "manage_btn_21", "manage_btn_22", "manage_btn_23", "manage_btn_24", "manage_btn_1", "manage_btn_2", "manage_btn_3", "manage_btn_4", "manage_btn_5", "manage_btn_6", "manage_btn_7", "manage_btn_8", "manage_btn_9", "manage_btn_10", "manage_btn_11", "manage_btn_12", "manage_btn_0" };


typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_main,
    tick_screen_manage,
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
    create_screen_manage();
}
