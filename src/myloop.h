#ifndef MYLOOP_H
#define MYLOOP_H

#include <main.h>


void super_loop();
void progress_bar_idcheck();
void progress_bar_select();
void update_select_page(const char *borrowing_status_things[4]);
void timeout_callback_1(lv_timer_t *timer);
static void check_progress_complete(int32_t value);
void create_progress_msgbox(const char *title, const char *message);
void update_progress(int value);
void close_progress_msgbox();

struct MessageBoxInfo {
    lv_obj_t* msgbox;
    lv_timer_t* timer;
};

extern const char *currentOperationType;// 当前执行的操作类型
extern const char *itemStatusList[4];// 四个物品的状态数组



#endif /*MAIN_H*/