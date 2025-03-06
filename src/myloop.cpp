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

void super_loop()
{
  // 如果在home页则检测按键，不在home页则计时清除状态
  if (lv_tabview_get_tab_act(objects.tabview) == 0)
  {
    if (lv_obj_has_state(objects.home_home_use, LV_STATE_PRESSED))
    {
      borrowing_status = "0";
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
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
  switch (lv_tabview_get_tab_act(objects.tabview))
  {
  case 1:
    if (lv_obj_has_state(objects.home_idcheck_ok, LV_STATE_PRESSED))
    {
      // mqtt发送输入框信息
      const char *idcheck_text = lv_textarea_get_text(objects.home_idcheck_textarea);
      // mqtt_publish((char*)idcheck_text);
      js_publish_id(idcheck_text, borrowing_status);
      // 等待mqtt返回信息，创建弹窗
      progress_bar_idcheck();
    }
    break;
  case 2:
    if (lv_obj_has_state(objects.home_select_ok, LV_STATE_PRESSED))
    {
        progress_bar_select(); // 调用新的进度条弹窗函数
    }
    break;
  case 3:
    //30秒后返回home页
    if (timeout_timer == NULL)
    {
      timeout_timer = lv_timer_create(timeout_callback_1, TIMEOUT_MS_OVERALL, NULL);
    }
    break;
  }
}

// 更新select页的四个物品（用图片按键表示）拿取状态
void update_select_page(const char *borrowing_status_things[4])
{
  // 更新select页的四个物品（用图片按键表示）拿取状态
  lv_obj_t *buttons[] = {
      objects.home_select_btn0,
      objects.home_select_btn1,
      objects.home_select_btn2,
      objects.home_select_btn3};

  for (int i = 0; i < 4; i++)
  {
    if (strcmp(borrowing_status_things[i], "0") == 0)
    {
      lv_imgbtn_set_state(buttons[i], LV_IMGBTN_STATE_PRESSED);
    }
    else if (strcmp(borrowing_status_things[i], "1") == 0)
    {
      lv_imgbtn_set_state(buttons[i], LV_IMGBTN_STATE_RELEASED);
    }
    else if (strcmp(borrowing_status_things[i], "2") == 0)
    {
      lv_imgbtn_set_state(buttons[i], LV_IMGBTN_STATE_RELEASED);
    }
  }
}

//刷新idcheck页进度条
void update_progress_bar(lv_timer_t *timer)
{
  lv_obj_t *bar = static_cast<lv_obj_t *>(timer->user_data);
  int32_t value = lv_bar_get_value(bar);

  // 根据当前阶段更新进度条的值
  if (current_stage == 0)
  {
    // 发送信息阶段
    if (value < 33)
    {
      lv_bar_set_value(bar, value + 1, LV_ANIM_ON); // 更新进度条的值
    }
    else
    {
      //检查是否收到mqtt返回信息
      if (user_id_received)
      {
        Serial.println("User ID received, moving to stage 1");
        current_stage = 1; // 进入下一个阶段
        user_id_received = false; // 重置状态
      }
    }
  }
  else if (current_stage == 1)
  {
    // 等待服务器信息阶段
    if (value < 66)
    {
      lv_bar_set_value(bar, value + 1, LV_ANIM_ON); // 更新进度条的值
    }
    else
    {
      //检查是否收到mqtt返回信息
      if (item_states_received)
      {
        Serial.println("Item states received, moving to stage 2");
        current_stage = 2; // 进入下一个阶段
        item_states_received = false; // 重置状态
      }
    }
  }
  else if (current_stage == 2)
  {
    // 处理完成阶段
    if (value < 100)
    {
      lv_bar_set_value(bar, value + 1, LV_ANIM_ON); // 更新进度条的值
    }
    else
    {
      lv_msgbox_close(lv_obj_get_parent(bar));   // 关闭消息框
      lv_timer_del(timer);                       // 删除定时器
      current_stage = 0;                         // 重置阶段
      update_select_page(borrowing_status_user); // 更新select页的四个物品（用图片按键表示）拿取状态
      // 清空输入框
      lv_textarea_set_text(objects.home_idcheck_textarea, "");
      lv_tabview_set_act(objects.tabview, 2, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
    }
  }
}

// 进度条超时返回
void timeout_callback(lv_timer_t *timer)
{
  // 如果lv_tabview_get_tab_act(objects.tabview) == 1则删除定时器
  if (lv_tabview_get_tab_act(objects.tabview) == 1)
  {
    lv_obj_t *msgbox = static_cast<lv_obj_t *>(timer->user_data);
    lv_msgbox_close(msgbox);                                 // 关闭消息框
    lv_timer_del(timer);                                     // 删除定时器
    current_stage = 0;                                       // 重置阶段
    lv_textarea_set_text(objects.home_idcheck_textarea, ""); // 清空输入框
    lv_tabview_set_act(objects.tabview, 0, LV_ANIM_ON);      // 切换至主页面
  }
  else
  {
    lv_timer_del(timer); // 删除定时器
  }
}

// 全局超时返回定时器
void timeout_callback_1(lv_timer_t *timer)
{
  // 如果lvgl界面两分钟无人操作且不在home页则返回home页
  if (lv_disp_get_inactive_time(NULL) > TIMEOUT_MS_OVERALL && lv_tabview_get_tab_act(objects.tabview) != 0)
  {
    // 执行相应操作，例如返回主页面
    lv_tabview_set_act(objects.tabview, 0, LV_ANIM_ON);
    lv_obj_invalidate(lv_scr_act());
    // 复位borrowing_status_user
    for (int i = 0; i < 4; i++)
    {
      borrowing_status_user[i] = "0";
    }
    // 清空输入框
    lv_textarea_set_text(objects.home_idcheck_textarea, "");
  }
}

// 创建一个进度条弹窗
void progress_bar_idcheck()
{
  // 创建一个消息框
  static const char *btns[] = {""}; // 空按钮数组，表示没有按钮
  lv_obj_t *msgbox = lv_msgbox_create(NULL, "Processing", "", btns, false);
  lv_obj_center(msgbox);

  // 在消息框中创建一个进度条
  lv_obj_t *bar = lv_bar_create(msgbox);
  lv_obj_set_width(bar, lv_pct(80));         // 设置进度条宽度为消息框宽度的80%
  lv_obj_align(bar, LV_ALIGN_CENTER, 0, 20); // 将进度条放置在消息框中心偏下的位置
  lv_bar_set_range(bar, 0, 100);             // 设置范围值
  lv_bar_set_value(bar, 0, LV_ANIM_OFF);     // 设置初始值

  // 创建一个超时定时器，超时后自动返回主页面
  lv_timer_create(timeout_callback, TIMEOUT_MS, msgbox); // 超时时间为30秒

  // 创建一个定时器，更新进度条的值
  lv_timer_create(update_progress_bar, 50, bar); // 每50毫秒更新一次进度条的值
}

// 刷新select页进度条
void update_progress_bar_select(lv_timer_t *timer)
{
    lv_obj_t *bar = static_cast<lv_obj_t *>(timer->user_data);
    int32_t value = lv_bar_get_value(bar);

    // 根据当前阶段更新进度条的值
    if (current_stage_s == 0)
    {
        // 第一个阶段
        if (value < 50)
        {
            lv_bar_set_value(bar, value + 1, LV_ANIM_ON); // 更新进度条的值
        }
        else
        {
            current_stage_s = 1; // 进入下一个阶段
        }
    }
    else if (current_stage_s == 1)
    {
        // 第二个阶段
        if (value < 100)
        {
            lv_bar_set_value(bar, value + 1, LV_ANIM_ON); // 更新进度条的值
        }
        else
        {
            lv_msgbox_close(lv_obj_get_parent(bar));   // 关闭消息框
            lv_timer_del(timer);                       // 删除定时器
            current_stage_s = 0;                       // 重置阶段
            // 复位borrowing_status_user
            for (int i = 0; i < 4; i++)
            {
              borrowing_status_user[i] = "0";
            }
            update_select_page(borrowing_status_user); // 更新select页的四个物品（用图片按键表示）拿取状态
            lv_tabview_set_act(objects.tabview, 3, LV_ANIM_ON);
            lv_obj_invalidate(lv_scr_act());
        }
    }
}

// 创建一个select页进度条弹窗
void progress_bar_select()
{
    // 创建一个消息框
    static const char *btns[] = {""}; // 空按钮数组，表示没有按钮
    lv_obj_t *msgbox = lv_msgbox_create(NULL, "Please take your belongings and close the door", "", btns, false);
    lv_obj_center(msgbox);

    // 在消息框中创建一个进度条
    lv_obj_t *bar = lv_bar_create(msgbox);
    lv_obj_set_width(bar, lv_pct(80));         // 设置进度条宽度为消息框宽度的80%
    lv_obj_align(bar, LV_ALIGN_CENTER, 0, 20); // 将进度条放置在消息框中心偏下的位置
    lv_bar_set_range(bar, 0, 100);             // 设置范围值
    lv_bar_set_value(bar, 0, LV_ANIM_OFF);     // 设置初始值

    // 创建一个超时定时器，超时后自动返回主页面
    lv_timer_create(timeout_callback, TIMEOUT_MS, msgbox); // 超时时间为30秒

    // 创建一个定时器，更新进度条的值
    lv_timer_create(update_progress_bar_select, 50, bar); // 每50毫秒更新一次进度条的值
}