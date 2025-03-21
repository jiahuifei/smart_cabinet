#include <main.h>

// 操作状态定义
#define STATUS_NULL "NULL"  // 初始状态
#define STATUS_BORROW "0"   // 领用状态
#define STATUS_RETURN "1"   // 归还状态
#define STATUS_MAINTAIN "2" // 维护状态

// 全局状态变量
const char *borrowing_status = STATUS_NULL;  // 当前操作类型
const char *borrowing_status_user[4] = {STATUS_BORROW, STATUS_BORROW, STATUS_BORROW, STATUS_BORROW};  // 物品状态数组

// 流程控制
static int current_stage = 0;  // 当前操作阶段
static lv_timer_t *timeout_timer = NULL;  // 超时定时器

// 超时配置（毫秒）
#define TIMEOUT_MS 3000000         // 单步操作超时
#define TIMEOUT_MS_OVERALL 30000000  // 整体流程超时

// 全局状态说明：
// borrowing_status - 当前操作类型："0"=领用，"1"=归还，"2"=维护
// borrowing_status_user - 四个物品的当前状态（索引0~3对应不同物品）
// current_stage - 流程阶段控制：0=初始，1=身份验证，2=物品选择，3=完成
// timeout_timer - 全局超时计时器句柄

// 主循环处理函数
void super_loop()
{
  // 主页（Tab 0）按钮处理
  if (lv_tabview_get_tab_act(objects.tabview) == 0)
  {
    // 领用按钮处理：设置操作类型并跳转至身份验证页
    if (lv_obj_has_state(objects.home_home_use, LV_STATE_PRESSED)) 
    {
      Serial.println("[Action] Borrow operation selected");
      borrowing_status = STATUS_BORROW;
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
    }
    // 归还按钮处理
    if (lv_obj_has_state(objects.home_home_return, LV_STATE_PRESSED))
    {
      Serial.println("[Action] Return operation selected");
      borrowing_status = STATUS_RETURN;
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
    }
    // 维护按钮处理
    if (lv_obj_has_state(objects.home_home_maintain, LV_STATE_PRESSED))
    {
      Serial.println("[Action] Maintenance operation selected");
      borrowing_status = STATUS_MAINTAIN;
      lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON);
      lv_obj_invalidate(lv_scr_act());
    }
  }
  
  // 分页状态机（处理不同标签页的逻辑）
  switch (lv_tabview_get_tab_act(objects.tabview))
  {
  case 1: // 身份验证页（Tab 1）
    if (lv_obj_has_state(objects.home_idcheck_ok, LV_STATE_PRESSED))
    {
      // MQTT通信：发送用户ID和操作类型
      const char *idcheck_text = lv_textarea_get_text(objects.home_idcheck_textarea);
      Serial.printf("[Action] User ID: %s, Operation: %s\n", idcheck_text, borrowing_status);
      // 发送用户ID和操作类型
      send_user_auth(idcheck_text, borrowing_status);
      // 清空输入框
      lv_textarea_set_text(objects.home_idcheck_textarea, "");  
      // 等待物品状态更新（来自MQTT回调）
      if (item_states_received) {
        current_stage = 2; // 推进到物品选择阶段
        item_states_received = false;
      }
    }
    break;
    
  case 2: // 物品选择页（Tab 2）
    // 遍历处理四个物品按钮的点击状态
    for(int i=0; i<4; i++){
      lv_obj_t *btn = ((lv_obj_t*[]){objects.home_select_btn0, objects.home_select_btn1, 
                    objects.home_select_btn2, objects.home_select_btn3})[i];
      if(lv_obj_has_state(btn, LV_STATE_PRESSED)){
        // 切换物品选择状态（0=未选，1=已选）
        borrowing_status_user[i] = (strcmp(borrowing_status_user[i], STATUS_BORROW) == 0) ? STATUS_RETURN : STATUS_BORROW;
        lv_obj_clear_state(btn, LV_STATE_PRESSED); // 清除按钮按下状态
        Serial.printf("[Action] Item %d status changed to %s\n", i, borrowing_status_user[i]);
      }
    }
    
    // 确认按钮处理
    if (lv_obj_has_state(objects.home_select_ok, LV_STATE_PRESSED)) 
    {
      // 有效性检查：至少选择一个物品
      bool hasSelection = false;
      for(int i=0; i<4; i++){
        if(strcmp(borrowing_status_user[i], STATUS_RETURN) == 0){
          hasSelection = true;
          break;
        }
      }
      
      if(!hasSelection) {
        Serial.println("[Warning] No items selected");
        break; // 无选择时中止操作
      }
      
      Serial.println("[Action] Selection confirmed, proceeding to completion page");
      
      // 开启对应柜门
      for (int i = 0; i < 4; i++) {
        if (strcmp(borrowing_status_user[i], STATUS_RETURN) == 0) {
          char rsMsg[32];
          bool result = openLock(1, i + 1, rsMsg); // 假设所有物品都在第1块控制板上
          if (result) {
            Serial.printf("[Action] Door %d opened successfully\n", i + 1);
          } else {
            Serial.printf("[Error] Failed to open door %d: %s\n", i + 1, rsMsg);
          }
        }
      }
      
      // 发送操作结果到服务器
      JsonDocument doc;
      doc["operation_type"] = borrowing_status;
      JsonArray items = doc.createNestedArray("items");
      for (int i = 0; i < 4; i++) {
        items.add(borrowing_status_user[i]);
      }
      char buffer[256];
      serializeJson(doc, buffer);
      mqtt_publish(buffer);
      
      // 重置物品选择状态并更新界面
      for (int i = 0; i < 4; i++) {
        borrowing_status_user[i] = STATUS_BORROW;
      }
      update_select_page(borrowing_status_user); 
      lv_tabview_set_act(objects.tabview, 3, LV_ANIM_ON); // 跳转至完成页
    }
    break;
    
  case 3: // 完成页（Tab 3）
    // 创建5分钟全局超时计时器（仅当不存在时创建）
    if (timeout_timer == NULL) {
      Serial.println("[Timer] Creating overall timeout timer");
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

// 超时处理回调函数
void timeout_callback_1(lv_timer_t * timer)
{
  Serial.println("[Timeout] Timer callback triggered");

  // 处理定时器自动销毁
  if(timer->user_data != NULL){
    Serial.println("[Timeout] Cleaning up timer resources");
    lv_timer_del(timer);
    timeout_timer = NULL;
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
    borrowing_status = STATUS_NULL;
    for (int i = 0; i < 4; i++) {
      borrowing_status_user[i] = STATUS_BORROW;
    }
    
    // 更新界面
    update_select_page(borrowing_status_user);
    
    lvgl_port_unlock();
    Serial.println("[Timeout] System state reset completed");
  } else {
    Serial.println("[Error] Failed to acquire LVGL lock in timeout handler");
  }
}
