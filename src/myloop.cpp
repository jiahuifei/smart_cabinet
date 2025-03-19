#include <main.h>

const char *borrowing_status = "NULL"; // 0状态是领用，1状态是归还，2状态是维护
// 创建一个用于存储四个物品拿取状态的数组
const char *borrowing_status_user[4] = {"0", "0", "0", "0"}; // 0状态是领用，1状态是归还，2状态是维护

// 全局变量来跟踪当前阶段
static int current_stage = 0;
static int current_stage_s = 0;
static lv_timer_t *timeout_timer = NULL;

// 超时时间（单位：毫秒），例如 30 秒
#define TIMEOUT_MS 30000
#define TIMEOUT_MS_OVERALL 300000

// 全局状态说明：
// borrowing_status - 当前操作类型："0"=领用，"1"=归还，"2"=维护
// borrowing_status_user - 四个物品的当前状态（索引0~3对应不同物品）
// current_stage - 流程阶段控制：0=初始，1=身份验证，2=物品选择，3=完成
// timeout_timer - 全局超时计时器句柄

// 主循环处理函数
void super_loop()
{
  // 主页按钮检测
  if (lv_tabview_get_tab_act(objects.tabview) == 0)
  {
    // 领用按钮按下处理
    if (lv_obj_has_state(objects.home_home_use, LV_STATE_PRESSED)) 
    {
      borrowing_status = "0";
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON); // 切换到身份验证页
      lv_obj_invalidate(lv_scr_act()); // 强制界面重绘
    }
    if (lv_obj_has_state(objects.home_home_return, LV_STATE_PRESSED))
    {

      borrowing_status = "1";
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
    }
    if (lv_obj_has_state(objects.home_home_maintain, LV_STATE_PRESSED))
    {
      borrowing_status = "2";
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
    }
  }
  
  // 分页状态机
  switch (lv_tabview_get_tab_act(objects.tabview))
  {
  case 1: // 身份验证页
    if (lv_obj_has_state(objects.home_idcheck_ok, LV_STATE_PRESSED))
    {
      const char *idcheck_text = lv_textarea_get_text(objects.home_idcheck_textarea);
      js_publish_id(idcheck_text, borrowing_status); // 通过MQTT发送用户ID
      lv_textarea_set_text(objects.home_idcheck_textarea, "");  // 清空输入框
      
      // 等待物品状态更新
      if (item_states_received) {
        current_stage = 2; // 推进到物品选择阶段
        item_states_received = false;
      }
    }
    break;
    
  case 2: // 物品选择页
    if (lv_obj_has_state(objects.home_select_ok, LV_STATE_PRESSED)) 
    {
      // 初始化所有物品为未选择状态
      for (int i = 0; i < 4; i++) {
          borrowing_status_user[i] = "0";
      }
      update_select_page(borrowing_status_user); // 更新界面状态
      lv_tabview_set_act(objects.tabview, 3, LV_ANIM_ON); // 进入完成页
    }
    break;
    
  case 3: // 完成页
    // 启动5分钟超时计时器（300,000毫秒）
    if (timeout_timer == NULL) {
      timeout_timer = lv_timer_create(timeout_callback_1, TIMEOUT_MS_OVERALL, NULL);
    }
    break;
  }
}

// 更新物品选择页的按钮状态
// 参数：borrowing_status_things - 包含4个物品状态的数组
void update_select_page(const char *borrowing_status_things[4])
{
  if (lvgl_port_lock(-1)) { // 获取LVGL硬件锁
    // 按钮对象数组
    lv_obj_t *buttons[] = {
        objects.home_select_btn0,
        objects.home_select_btn1,
        objects.home_select_btn2,
        objects.home_select_btn3};

    // 遍历更新每个按钮状态
    for (int i = 0; i < 4; i++) {
      // "0"=已选择（按下状态），其他=未选择（释放状态）
      if (strcmp(borrowing_status_things[i], "0") == 0) {
        lv_imgbtn_set_state(buttons[i], LV_IMGBTN_STATE_PRESSED);
      } else {
        lv_imgbtn_set_state(buttons[i], LV_IMGBTN_STATE_RELEASED);
      }
    }
    lvgl_port_unlock(); // 释放硬件锁
  }
}

// 全局超时回调函数
void timeout_callback_1(lv_timer_t * timer)
{
  // 检测5分钟无操作且不在主页时执行复位
  if (lv_disp_get_inactive_time(NULL) > TIMEOUT_MS_OVERALL && lv_tabview_get_tab_act(objects.tabview) != 0)
  {
    if (lvgl_port_lock(-1)) {
      lv_tabview_set_act(objects.tabview, 0, LV_ANIM_ON); // 返回主页
      
      // 复位所有状态
      for (int i = 0; i < 4; i++) {
        borrowing_status_user[i] = "0";
      }
      lv_textarea_set_text(objects.home_idcheck_textarea, ""); // 清空身份验证输入框
      lvgl_port_unlock();
    }
  }
}
