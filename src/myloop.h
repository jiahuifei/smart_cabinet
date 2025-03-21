#ifndef MYLOOP_H
#define MYLOOP_H

#include <main.h>


void super_loop();
void progress_bar_idcheck();
void progress_bar_select();
void update_select_page(const char *borrowing_status_things[4]);
void timeout_callback_1(lv_timer_t *timer);
static void check_progress_complete(int32_t value);

struct MessageBoxInfo {
    lv_obj_t* msgbox;
    lv_timer_t* timer;
};

extern const char *borrowing_status;         // 拿取状态(领用0.归还1，维修2)
extern const char *borrowing_status_user[4]; // 物品拿取状态


#endif /*MAIN_H*/