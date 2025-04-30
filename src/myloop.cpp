#include <main.h>

/* 
 * 系统操作状态枚举
 * STATUS_NULL - 初始空闲状态
 * STATUS_BORROW - 物品领用状态
 * STATUS_RETURN - 物品归还状态
 * STATUS_MAINTAIN - 设备维护状态
 */
#define STATUS_NULL "NULL"
#define STATUS_BORROW "0"
#define STATUS_RETURN "1"
#define STATUS_MAINTAIN "2"

/* 系统全局状态变量 */
const char *currentOperationType = STATUS_NULL;  // 当前执行的操作类型
const char *itemStatusList[4] = {STATUS_BORROW, STATUS_BORROW, STATUS_BORROW, STATUS_BORROW}; // 四个物品的状态数组

/* 流程控制变量 */
static int currentWorkflowStage = 0;  // 当前工作流阶段
static lv_timer_t *operationTimeoutTimer = NULL;  // 操作超时计时器

/* 超时配置(毫秒) */
#define TIMEOUT_MS_OVERALL 3000000      // 单步操作超时时间(5分钟)
#define TOTAL_PROCESS_TIMEOUT_MS 30000000    // 整体流程超时时间(50分钟)

/* 进度条弹窗相关变量 */
static lv_obj_t *progress_msgbox = NULL;
static lv_obj_t *progress_slider = NULL;
static lv_obj_t *progress_label = NULL;

/**
 * 处理主页按钮点击事件
 * @param operationName 操作名称(用于日志)
 * @param operationType 操作类型(STATUS_BORROW/STATUS_RETURN/STATUS_MAINTAIN)
 */
static void handleHomePageButton(const char* operationName, const char* operationType) 
{
  Serial.printf("[系统操作] 用户选择: %s\n", operationName);
  currentOperationType = operationType;
  lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON); // 切换到身份验证页
  lv_obj_invalidate(lv_scr_act()); // 刷新屏幕
}

/**
 * 处理身份验证页逻辑
 * 1. 获取用户输入的ID
 * 2. 通过MQTT发送验证请求
 * 3. 等待服务器返回物品状态
 */
void processIdentityVerification()
{
  const char *userInputId = lv_textarea_get_text(objects.home_idcheck_textarea);
  Serial.printf("[Action] 用户ID: %s, 操作类型: %s\n", userInputId, currentOperationType);
  
  // 发送验证请求到服务器
  send_user_auth(userInputId, currentOperationType);
  delay(50); // 短暂延迟确保发送完成
  
  // 清空输入框
  lv_textarea_set_text(objects.home_idcheck_textarea, "");
  lv_obj_invalidate(lv_scr_act()); // 刷新屏幕

}

// 处理选择页物品按钮点击状态
static void handle_item_buttons() {
  for(int i=0; i<4; i++){
      lv_obj_t *btn = ((lv_obj_t*[]){objects.home_select_btn0, objects.home_select_btn1, 
                    objects.home_select_btn2, objects.home_select_btn3})[i];
      if(lv_obj_has_state(btn, LV_STATE_PRESSED)){
          itemStatusList[i] = (strcmp(itemStatusList[i], STATUS_BORROW) == 0) ? STATUS_RETURN : STATUS_BORROW;
          lv_obj_clear_state(btn, LV_STATE_PRESSED);
          Serial.printf("[Action] Item %d status changed to %s\n", i, itemStatusList[i]);
      }
  }
  
}

/**
 * @brief 等待锁状态变化，带超时功能
 * @param door1 第一个锁ID
 * @param door2 第二个锁ID
 * @param timeout_ms 超时时间(毫秒)
 * @return true-锁状态已变化，false-超时
 */
bool waitForLockStateChangeWithTimeout(int door1, int door2, unsigned long timeout_ms) {
  unsigned long startTime = millis();
  
  // 更新初始进度
  update_progress(33);
  
  while (!isLockOpen(door1) && !isLockOpen(door2)) {
    Serial.println(isLockOpen(door1));
    Serial.println(isLockOpen(door2));
    // 检查是否超时
    if (millis() - startTime > timeout_ms) {
      Serial.println("[Warning] 等待锁状态变化超时");
      return false;
    }
  
    // 短暂延迟
    delay(500);
  }
      // 更新进度
      update_progress(66);
      delay(1000);
  
  // 锁状态已变化
  update_progress(100);
  return true;
}

// 处理选择页确认按钮点击
static void handle_selection_confirmation() {
  // 有效性检查
  bool hasSelection = false;
  for(int i=0; i<4; i++){
      if(strcmp(itemStatusList[i], STATUS_RETURN) == 0){
          hasSelection = true;
          break;
      }
  }
  
  if(!hasSelection) {
      Serial.println("[Warning] No items selected");
      //return;
  }
  
  Serial.println("[Action] Selection confirmed, proceeding to completion page");
  
  // 开启柜门
  int door1 = 5;//这里需要查询数据库修改
  int door2 = 37;//这里需要查询数据库修改



      if (strcmp(itemStatusList[0], STATUS_RETURN) == 0||strcmp(itemStatusList[1], STATUS_RETURN) == 0||strcmp(itemStatusList[2], STATUS_RETURN) == 0) {

          bool result = directOpenLockById(door1);
          delay(1000);
          if (result) {
              Serial.printf("[Action] Door %d opened successfully\n", door1 + 1);
          } else {
              Serial.printf("[Error] Failed to open door %d\n", door1 + 1);
          }
      }
      if (strcmp(itemStatusList[3], STATUS_RETURN) == 0) {

          bool result = directOpenLockById(door2);
          delay(1000);
          if (result) {
              Serial.printf("[Action] Door %d opened successfully\n", door2 + 1);
          } else {
              Serial.printf("[Error] Failed to open door %d\n", door2 + 1);
          }
      }
      create_progress_msgbox("Plase take things!", "Plase take things, and close the door!");
      //进度条到33%
      update_progress(33);
        // 使用带超时的等待函数，设置30秒超时
      bool doorsClosed = waitForLockStateChangeWithTimeout(door1, door2, 30000);
      if (!doorsClosed) {
        // 超时处理待完善
        Serial.println("[Warning] 等待关门超时，强制继续流程");
      } else {
        Serial.println("[Action] 所有柜门已关闭");
      }
      close_progress_msgbox();

  // 重置状态
  for (int i = 0; i < 4; i++) {
      itemStatusList[i] = STATUS_BORROW;
  }
  update_select_page(itemStatusList); 
  lv_tabview_set_act(objects.tabview, 3, LV_ANIM_ON);
}

// 主循环处理函数
void super_loop()
{
  // 检查当前是否在管理页面
  if (lv_scr_act() == objects.manage) {
    Serial.println("当前在管理页面");
    // 定义管理按钮数组
    lv_obj_t* manage_buttons[] = {
        objects.manage_btn_0, objects.manage_btn_1, objects.manage_btn_2, objects.manage_btn_3,
        objects.manage_btn_4, objects.manage_btn_5, objects.manage_btn_6, objects.manage_btn_7,
        objects.manage_btn_8, objects.manage_btn_9, objects.manage_btn_10, objects.manage_btn_11,
        objects.manage_btn_12, objects.manage_btn_13, objects.manage_btn_14, objects.manage_btn_15,
        objects.manage_btn_16, objects.manage_btn_17, objects.manage_btn_18, objects.manage_btn_19,
        objects.manage_btn_20, objects.manage_btn_21, objects.manage_btn_22, objects.manage_btn_23,
        objects.manage_btn_24, objects.manage_btn_25, objects.manage_btn_26, objects.manage_btn_27,
        objects.manage_btn_28, objects.manage_btn_29, objects.manage_btn_30, objects.manage_btn_31,
        objects.manage_btn_32, objects.manage_btn_33, objects.manage_btn_34, objects.manage_btn_35,
        objects.manage_btn_36, objects.manage_btn_37, objects.manage_btn_38, objects.manage_btn_39,
        objects.manage_btn_40, objects.manage_btn_41, objects.manage_btn_42, objects.manage_btn_43,
        objects.manage_btn_44, objects.manage_btn_45, objects.manage_btn_46, objects.manage_btn_47,
        objects.manage_btn_48
    };
    
    // 遍历所有管理按钮(0-48)
    for(int i = 0; i <= 48; i++) {
        if(manage_buttons[i] && lv_obj_has_state(manage_buttons[i], LV_STATE_PRESSED)) {
            directOpenLockById(i);  // 按钮编号直接对应锁ID
            Serial.printf("[Action] 按钮 %d 按下，打开锁 %d\n", i, i);
            break;  // 一次只处理一个按钮
        }
    }
  }
    
  // 主页（Tab 0）按钮处理
  if (lv_tabview_get_tab_act(objects.tabview) == 0 && lv_scr_act() == objects.main)
  {
    Serial.println("当前在主页");
    // 领用按钮处理
    if (lv_obj_has_state(objects.home_home_use, LV_STATE_PRESSED)) {
      handleHomePageButton("Borrow", STATUS_BORROW);
    }
    // 归还按钮处理
    if (lv_obj_has_state(objects.home_home_return, LV_STATE_PRESSED)) {
      handleHomePageButton("Return", STATUS_RETURN);
    }
    // 维护按钮处理
    // if (lv_obj_has_state(objects.home_home_maintain, LV_STATE_PRESSED)) {
    //   handleHomePageButton("Maintenance", STATUS_MAINTAIN);
    // }
  }

  // 分页状态机（处理不同标签页的逻辑）
  switch (lv_tabview_get_tab_act(objects.tabview))
  {
    Serial.println("当前在其他页");
  case 1: // 身份验证页（Tab 1）
    if (lv_obj_has_state(objects.home_idcheck_ok, LV_STATE_PRESSED))
    {
      processIdentityVerification();
        // 检查是否收到物品状态更新
    }
    if (item_states_received) {
      currentWorkflowStage = 2; // 进入物品选择阶段
      Serial.println("[Action] 物品状态更新完成，进入物品选择页面");
      lv_tabview_set_act(objects.tabview, 2, LV_ANIM_ON); // 切换到选择页
      lv_obj_invalidate(lv_scr_act()); // 刷新屏幕
      item_states_received = false;
    }
    break;
    
  case 2: // 物品选择页（Tab 2）
    handle_item_buttons();
      
    if (lv_obj_has_state(objects.home_select_ok, LV_STATE_PRESSED)) {
      Serial.println("[Action] 选择确认按钮按下");
        handle_selection_confirmation();
    }

    //lv_tabview_set_act(objects.tabview, 3, LV_ANIM_ON); // 切换到完成页
    break;
    
  case 3: // 完成页（Tab 3）
    // 创建5分钟全局超时计时器（仅当不存在时创建）
    if (operationTimeoutTimer == NULL) {
      Serial.println("[Timer] Creating overall timeout timer");
      operationTimeoutTimer = lv_timer_create(timeout_callback_1, TIMEOUT_MS_OVERALL, NULL);
      // 发送操作结果
      JsonDocument doc;
      doc["operation_type"] = currentOperationType;
      JsonArray items = doc.createNestedArray("items");
      for (int i = 0; i < 4; i++) {
          items.add(itemStatusList[i]);
      }
      char buffer[256];
      serializeJson(doc, buffer);
      mqtt_publish(buffer);
      // 重置全局状态
      currentOperationType = STATUS_NULL;
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

// 超时处理回调函数
void timeout_callback_1(lv_timer_t * timer)
{
  Serial.println("[Timeout] Timer callback triggered");

  // 处理定时器自动销毁
  if(timer->user_data != NULL){
    Serial.println("[Timeout] Cleaning up timer resources");
    lv_timer_del(timer);
    operationTimeoutTimer = NULL;
    return;
  }

  // 只在非主页时处理超时
  if(lv_tabview_get_tab_act(objects.tabview) == 0) {
    return;
  }
  
  // 统一超时处理
  if (lvgl_port_lock(-1)) {
    Serial.println("[Timeout] Resetting system state");
    
    // 重置UI状态
    lv_tabview_set_act(objects.tabview, 0, LV_ANIM_ON);
    lv_textarea_set_text(objects.home_idcheck_textarea, "");
    
    // 重置业务状态
    currentOperationType = STATUS_NULL;
    for (int i = 0; i < 4; i++) {
      itemStatusList[i] = STATUS_BORROW;
    }
    
    // 更新界面
    update_select_page(itemStatusList);
    
    lvgl_port_unlock();
    Serial.println("[Timeout] System state reset completed");
  } else {
    Serial.println("[Error] Failed to acquire LVGL lock in timeout handler");
  }
}
/******************************************************************************************/
/**
 * @brief 创建进度条弹窗
 * @param title 弹窗标题
 * @param message 弹窗消息
 */
void create_progress_msgbox(const char *title, const char *message)
{
    // 如果已存在则先删除
    if(progress_msgbox) {
        lv_obj_del(progress_msgbox);
    }

    // 创建消息框
    static const char *btns[] = {"", "", ""};
    progress_msgbox = lv_msgbox_create(lv_scr_act(), title, message, btns, false);
    
    // 设置消息框样式
    lv_obj_set_size(progress_msgbox, 300, 200);
    lv_obj_center(progress_msgbox);
    lv_obj_set_style_border_width(progress_msgbox, 0, 0);
    lv_obj_set_style_shadow_width(progress_msgbox, 20, 0);
    lv_obj_set_style_shadow_color(progress_msgbox, lv_color_hex(0xa9a9a9), LV_STATE_DEFAULT);
    
    // 创建进度条滑块
    progress_slider = lv_slider_create(progress_msgbox);
    lv_obj_set_size(progress_slider, 250, 20);
    lv_obj_align(progress_slider, LV_ALIGN_CENTER, 0, 20);
    lv_slider_set_range(progress_slider, 0, 100);
    lv_slider_set_value(progress_slider, 0, LV_ANIM_OFF);
    
    // 创建进度百分比标签
    progress_label = lv_label_create(progress_msgbox);
    lv_label_set_text(progress_label, "0%");
    lv_obj_align_to(progress_label, progress_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_style_text_font(progress_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);
}

/**
 * @brief 更新进度条
 * @param value 进度值(0-100)
 */
void update_progress(int value)
{
    if(!progress_msgbox || !progress_slider || !progress_label) return;
    
    lv_slider_set_value(progress_slider, value, LV_ANIM_ON);
    lv_label_set_text_fmt(progress_label, "%d%%", value);
}

/**
 * @brief 关闭进度条弹窗
 */
void close_progress_msgbox()
{
    if(progress_msgbox) {
        lv_obj_del(progress_msgbox);
        progress_msgbox = NULL;
        progress_slider = NULL;
        progress_label = NULL;
    }
}

/**
 * @brief 显示进度条弹窗并执行操作
 * @param title 弹窗标题
 * @param message 弹窗消息
 * @param progressSteps 进度步骤数组，包含每个步骤的进度值
 * @param stepDelays 每个步骤之间的延迟时间(毫秒)
 * @param stepsCount 步骤总数
 */
void showProgressAndExecute(const char *title, const char *message, const int progressSteps[], const int stepDelays[], int stepsCount) {
  create_progress_msgbox(title, message);
  
  for (int i = 0; i < stepsCount; i++) {
      update_progress(progressSteps[i]);
  }

  close_progress_msgbox();
}