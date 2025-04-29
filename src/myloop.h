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
void handleManagementMode();
void handleIdleState();
void handleSelectionState();
void handleCompletionState();
void checkTimeout();

struct MessageBoxInfo {
    lv_obj_t* msgbox;
    lv_timer_t* timer;
};

extern const char *currentOperationType;// 当前执行的操作类型
typedef struct
{
  SystemState currentState; // 当前系统状态
  OperationType currentOp;  // 当前操作类型
  bool itemStates[4];       // 物品选中状态数组，4个物品
  bool doorStates[4];       // 柜门状态数组，4个柜门
  String userId;            // 当前用户ID
  lv_timer_t *timeoutTimer; // 超时计时器指针
  String reservationId;     // 新增：预约ID
  bool rfidVerified;        // 新增：RFID验证状态
  int progressValue;        // 新增：进度条数值
} SystemContext;

extern SystemContext sysCtx;  // 保持extern声明




#endif /*MAIN_H*/