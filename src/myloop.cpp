#include <main.h>


#define STATUS_NULL "NULL"
#define STATUS_BORROW "0"
#define STATUS_RETURN "1"
#define STATUS_MAINTAIN "2"

#define MAINTAIN_ACCOUNT  "123456"

/* 系统全局状态变量 */
const char *currentOperationType = STATUS_NULL;  // 当前执行的操作类型


/* 流程控制变量 */
static int currentWorkflowStage = 0;  // 当前工作流阶段
static lv_timer_t *operationTimeoutTimer = NULL;  // 操作超时计时器
static lv_timer_t *authTimeoutTimer = NULL;  // 认证超时计时器

/* 超时配置(毫秒) */
#define TIMEOUT_MS_OVERALL 30000      // 单步操作超时时间(5分钟)
#define TIMEOUT_MS_AUTH 50000           // 认证超时时间(5秒)
#define TOTAL_PROCESS_TIMEOUT_MS 60000    // 整体流程超时时间(50分钟)

/* 进度条弹窗相关变量 */
static lv_obj_t *progress_msgbox = NULL;
static lv_obj_t *progress_slider = NULL;
static lv_obj_t *progress_label = NULL;
static lv_obj_t *error_msgbox = NULL;

/* 格口信息 */
static int selectedCabinet1 = -1;  // 选中的第一个格口
static int selectedCabinet2 = -1;  // 选中的第二个格口
static uint32_t userId = 0;        // 用户ID

/*
   定义当前开门的ID列表
*/
static bool isControlIDS[48] = {false};

/**
 * 处理主页按钮点击事件
 * @param operationName 操作名称(用于日志)
 * @param operationType 操作类型(STATUS_BORROW/STATUS_RETURN/STATUS_MAINTAIN)
 */
static void handleHomePageButton(const char* operationName, const char* operationType) 
{
  Serial.printf("[系统操作] 用户选择: %s\n", operationName);

  // 更改状态变量
  currentOperationType = operationType;
  currentWorkflowStage = 1; // 进入身份验证阶段
  lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON); // 切换到身份验证页
  lv_obj_invalidate(lv_scr_act()); // 刷新屏幕
}

/**
 * 认证超时回调函数
 */
static void auth_timeout_callback(lv_timer_t * timer) {
  Serial.println("[超时] 认证超时");
  
  // 创建超时提示弹窗
  static const char* btns[] = {"close", "", ""};
  error_msgbox = lv_msgbox_create(lv_scr_act(), "认证超时", "服务器响应超时，请稍后重试", btns, false);
  lv_obj_center(error_msgbox);
  
  // 3秒后自动关闭
  lv_timer_t *close_timer = lv_timer_create([](lv_timer_t *t) {
    if (error_msgbox) {
      lv_obj_del(error_msgbox);
      error_msgbox = NULL;
    }
    lv_timer_del(t);
  }, 3000, NULL);
  
  // 添加关闭按钮事件
  lv_obj_add_event_cb(error_msgbox, [](lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_current_target(e);
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
      lv_obj_del(obj);
      error_msgbox = NULL;
    }
  }, LV_EVENT_VALUE_CHANGED, NULL);
  
  // 删除认证超时计时器
  lv_timer_del(timer);
  authTimeoutTimer = NULL;
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

  // 保存用户ID
  userId = atoi(userInputId);
  if(userInputId == "123456") {
    Serial.println("[Warning] 测试用户，跳过认证");
    currentWorkflowStage = 2; 
    lv_textarea_set_text(objects.home_idcheck_textarea, "");
    lv_tabview_set_act(objects.tabview, 2, LV_ANIM_ON); // 切换到物品选择页
    lv_obj_invalidate(lv_scr_act()); // 刷新屏幕
    return;
    
  }else {
    // 发送验证请求到服务器  Step3
    send_user_auth(userInputId, currentOperationType);
  }
  
  // 创建认证超时计时器
  if (authTimeoutTimer == NULL) {
    authTimeoutTimer = lv_timer_create(auth_timeout_callback, TIMEOUT_MS_AUTH, NULL);
  }
  
  // 清空输入框
  lv_textarea_set_text(objects.home_idcheck_textarea, "");
  lv_obj_invalidate(lv_scr_act()); // 刷新屏幕
}

/**
 * 查找可用格口
 * @param itemType 物品类型(0x01, 0x02, 0x03, 0x04)
 * @return 可用格口ID，-1表示未找到
 */
int findAvailableCabinet(uint8_t itemType) {
  // 根据物品类型查找可用格口
  if (itemType <= 0x03) {
    // 查找三物品格口(1-36)
    for (int i = 1; i <= 36; i++) {
      TripleCabinetItem* cabinet = ItemDatabase::getTripleCabinetInfo(i);
      if (cabinet != NULL) {
        // 检查对应物品是否可用
        if ((itemType == 0x01 && cabinet->itemActualStatus1 == ITEM_STATUS_AVAILABLE) ||
            (itemType == 0x02 && cabinet->itemActualStatus2 == ITEM_STATUS_AVAILABLE) ||
            (itemType == 0x03 && cabinet->itemActualStatus3 == ITEM_STATUS_AVAILABLE)) {
          return i;
        }
      }
    }
  } else if (itemType == 0x04) {
    // 查找单物品格口(37-48)
    for (int i = 37; i <= 48; i++) {
      SingleCabinetItem* cabinet = ItemDatabase::getSingleCabinetInfo(i);
      if (cabinet != NULL && cabinet->itemActualStatus == ITEM_STATUS_AVAILABLE) {
        return i;
      }
    }
  }
  return -1; // 未找到可用格口
}

/**
 * 更新物品状态
 * @param cabinetId 格口ID
 * @param itemType 物品类型(0x01, 0x02, 0x03, 0x04)
 * @param newStatus 新状态
 * @param userId 用户ID
 */
void updateCabinetItemStatus(int cabinetId, uint8_t itemType, uint8_t newStatus, uint32_t userId) {
  if (cabinetId <= 0) return;
  
  if (itemType <= 0x03) {
    // 更新三物品格口
    TripleCabinetItem* cabinet = ItemDatabase::getTripleCabinetInfo(cabinetId);
    if (cabinet != NULL) {
      // 更新物品状态和用户ID
      cabinet->userId = userId;
      
      // 更新物品预约状态（根据itemStatusList）
      if (itemType == 0x01 && strcmp(itemStatusList[0], "1") == 0) {
        cabinet->itemReserveStatus1 = 0x01; // 已预约
      }
      else{
        cabinet->itemReserveStatus1 = 0x00; // 未预约
      }
      if (itemType == 0x02 && strcmp(itemStatusList[1], "1") == 0) {
        cabinet->itemReserveStatus2 = 0x01; // 已预约
      }
      else{
        cabinet->itemReserveStatus2 = 0x00; // 未预约
      }
      if (itemType == 0x03 && strcmp(itemStatusList[2], "1") == 0) {
        cabinet->itemReserveStatus3 = 0x01; // 已预约
      }
      else{
        cabinet->itemReserveStatus3 = 0x00; // 未预约
      }
      
      // 更新物品实际状态
      ItemDatabase::updateItemStatus(cabinetId, itemType, newStatus);
    }
  } else if (itemType == 0x04) {
    // 更新单物品格口
    SingleCabinetItem* cabinet = ItemDatabase::getSingleCabinetInfo(cabinetId);
    if (cabinet != NULL) {
      cabinet->userId = userId;
      
      // 更新物品预约状态（根据itemStatusList）
      if (strcmp(itemStatusList[3], "1") == 0) {
        cabinet->itemReserveStatus = 0x01; // 已预约
      }
      else{
        cabinet->itemReserveStatus = 0x00; // 未预约
      }
      
      // 更新物品实际状态
      ItemDatabase::updateItemStatus(cabinetId, 1, newStatus);
    }
  }
  
  // 打印更新后的数据库状态（调试用）
  Serial.println("[数据库] 更新格口信息:");
  Serial.printf("格口ID: %d, 物品类型: 0x%02X, 用户ID: %d, 状态: 0x%02X\n", 
                cabinetId, itemType, userId, newStatus);
}

/**
 * 处理物品选择页面的显示
 */
void updateItemSelectionPage() {
  // 按钮对象数组
  lv_obj_t *buttons[] = {
    objects.home_select_btn0,
    objects.home_select_btn1,
    objects.home_select_btn2,
    objects.home_select_btn3
  };
  
  // 遍历更新每个按钮状态
  for (int i = 0; i < 4; i++) {
    if (strcmp(itemStatusList[i], "1") == 0) {
      // 需要领用的物品显示为彩色(释放状态)
      lv_imgbtn_set_state(buttons[i], LV_IMGBTN_STATE_RELEASED);
      lv_obj_add_flag(buttons[i], LV_OBJ_FLAG_CLICKABLE);
    } else {
      // 不需要领用的物品显示为灰色(禁用状态)
      // 不需要领用的物品显示为灰色(禁用状态)
      lv_imgbtn_set_state(buttons[i], LV_IMGBTN_STATE_PRESSED);
      //设置改变其clickable值设置为为不可点击状态
      lv_obj_clear_flag(buttons[i], LV_OBJ_FLAG_CLICKABLE);
    }
  }
}

/*

*/
void CheckDoorState(){
    // 弹出对话框
    Serial.println("Update : Open model dialog");

    //  循环检查
    Serial.println("Update : loop check");

    // while (true)
    // {
    
      for (int inx  = 0 ; inx < 48; inx ++){
        bool retItem = isControlIDS[inx];

        if (!retItem) continue;

        // 发送门状态查询指令
        // send()

        break;

      }
      
    //   delay
    // }



    // 关闭对话框
    Serial.println("Update : Close model dialog");

    // 回到主页
    lv_scr_load(objects.main);

}

/**
 * @brief 等待锁状态变化，带超时功能
 * @param lockId 锁ID
 * @param timeout_ms 超时时间(毫秒)
 * @return true-锁状态已变化，false-超时
 */
bool waitForLockStateChangeWithTimeout(int lockId, unsigned long timeout_ms) {
  unsigned long startTime = millis();
  
  // 更新初始进度
  update_progress(33);
  
  while (!isLockOpen(lockId)) {
    // 检查是否超时
    if (millis() - startTime > timeout_ms) {
      Serial.println("[Warning] 等待锁状态变化超时");
      return false;
    }
    // 短暂延迟
    delay(500);
  }
  
  Serial.println("[Action] 格口已关闭");
  // 更新进度
  update_progress(66);
  
  // 锁状态已变化
  return true;
}

/**
 * 检查RFID标签状态
 * @param itemTypes 需要检查的物品类型数组
 * @param antennas 对应的天线号数组
 * @param count 物品数量
 * @return true-所有物品都已取出，false-有物品未取出
 */
bool checkRfidTags(const char* itemTypes[], const uint8_t antennas[], int count) {
  bool allItemsTaken = true;
  
  for (int i = 0; i < count; i++) {
    if (strcmp(itemTypes[i], "1") == 0) {
      // 需要领用的物品，应该读不到标签(返回false)
      String epc_id = String("0") + String(i + 1); // 构造EPC ID: "01", "02", "03", "04"
      if (rfid1.rfid_loop(epc_id, antennas[i])) {
        // 如果读到标签，说明物品未取出
        allItemsTaken = false;
        Serial.printf("[Warning] 物品 %s 未取出\n", epc_id.c_str());
      }
    }
  }
  
  return allItemsTaken;
}

/**
 * 显示领用错误弹窗
 */
void showBorrowErrorDialog() {
  static const char* btns[] = {"reuse", "neglect", ""};
  error_msgbox = lv_msgbox_create(lv_scr_act(), "wrong use", "Please check whether all items have been taken out.", btns, false);
  lv_obj_center(error_msgbox);
  
  // 添加按钮事件
  lv_obj_add_event_cb(error_msgbox, [](lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_current_target(e);
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
      uint16_t btn_id = lv_msgbox_get_active_btn(obj);
      if (btn_id == 0) {
        // 重新领取
        currentWorkflowStage = 5; // 回到第六步
      } else {
        // 忽略
        currentWorkflowStage = 8; // 跳到第九步
      }
      lv_obj_del(obj);
      error_msgbox = NULL;
    }
  }, LV_EVENT_VALUE_CHANGED, NULL);
}

/**
 * 处理物品领用流程
 */
void processBorrowWorkflow() {
  static unsigned long processStartTime = 0;
  
  // 根据当前工作流阶段执行不同操作
  switch (currentWorkflowStage) {
    case 0: // 初始状态
      // 等待用户操作
      break;
      
    case 1: // 身份验证阶段
      // 等待用户输入ID并点击确认
      break;
      
    case 2: // 等待服务器响应  Step5: 等待服务器响应
      if (user_id_received) {

        // 收到用户认证响应
        if (authTimeoutTimer) {
          lv_timer_del(authTimeoutTimer);
          authTimeoutTimer = NULL;
        }
        
        // 查找可用格口
        selectedCabinet1 = -1;
        selectedCabinet2 = -1;
        //Step6: 查找可用格口
        // 检查需要哪些物品
        for (int i = 0; i < 3; i++) {
          if (strcmp(itemStatusList[i], "1") == 0) {
            // 需要领用0x01-0x03物品，使用格口1-36
            if (selectedCabinet1 < 0) {

              // 查找可用格口
              selectedCabinet1 = findAvailableCabinet(i + 1);

              // 更新物品状态  --- 写入USEID
              updateCabinetItemStatus(selectedCabinet1, i + 1, ITEM_STATUS_BORROWED, userId);
            }
          }
        }
        
        // 检查是否需要0x04物品
        if (strcmp(itemStatusList[3], "1") == 0) {
          // 需要领用0x04物品，使用格口37-48
          selectedCabinet2 = findAvailableCabinet(0x04);
          updateCabinetItemStatus(selectedCabinet2, 0x04, ITEM_STATUS_BORROWED, userId);
        }
        
        // 进入物品选择页面
        lv_tabview_set_act(objects.tabview, 2, LV_ANIM_ON);
        updateItemSelectionPage();
        currentWorkflowStage = 3;
        user_id_received = false;
      }
      break;
      
    case 3: // 物品选择页面
      // 等待用户点击物品按钮
      break;
      
    case 4: // 用户点击了物品按钮
      // 记录开始时间
      processStartTime = millis();
      if (selectedCabinet1 > 0) {
        // 打开第一个格口
        currentWorkflowStage = 5; // 进入等待格口关闭阶段 
      }
      else if (selectedCabinet2 > 0) {
        // 打开第一个格口
        currentWorkflowStage = 7; // 进入等待格口关闭阶段 
      }
      else {
        // 没有需要打开的格口
        currentWorkflowStage = 9; // 进入完成阶段 
      }

      break;
      
    case 5: // 打开格口
      {
        // 创建进度条弹窗
        create_progress_msgbox("use", "Please take out your belongings and close the doors!");
        update_progress(0);
        
        // 打开需要的格口
        bool cabinet1Opened = false;
        bool cabinet2Opened = false;
        
        if (selectedCabinet1 > 0) {
          cabinet1Opened = directOpenLockById(selectedCabinet1);
          if (cabinet1Opened) {
            Serial.printf("[Action] 格口 %d 已打开\n", selectedCabinet1);
          } else {
            Serial.printf("[Error] 无法打开格口 %d\n", selectedCabinet1);
          }
        }
        
        if (selectedCabinet2 > 0) {
          cabinet2Opened = directOpenLockById(selectedCabinet2);
          if (cabinet2Opened) {
            Serial.printf("[Action] 格口 %d 已打开\n", selectedCabinet2);
          } else {
            Serial.printf("[Error] 无法打开格口 %d\n", selectedCabinet2);
          }
        }
        
        // 更新进度
        update_progress(33);
        currentWorkflowStage = 6;
      }
      break;
    
    case 6: // 等待用户关闭格口
      {
        bool allCabinetsClosed = true;
        
        // 检查格口是否已关闭
        if (selectedCabinet1 > 0 && !isLockOpen(selectedCabinet1)) {
          allCabinetsClosed = false;
        }
        
        if (selectedCabinet2 > 0 && !isLockOpen(selectedCabinet2)) {
          allCabinetsClosed = false;
        }
        
        // 检查是否超时
        if (millis() - processStartTime > 30000) { // 30秒超时
          Serial.println("[Warning] 等待关门超时");
          allCabinetsClosed = true; // 强制继续
        }
        
        // Step8:完成关锁后的进度条更新
        if (allCabinetsClosed) {
          update_progress(66);
          currentWorkflowStage = 9;
        }
      }
      break;
    // 修改流程控制部分
    // case 5: // 打开第一个格口
    // {
    //     // // 创建独立进度弹窗
    //     // create_progress_msgbox("use", "please take out items！");
    //     // update_progress(50);
        
    //     if (selectedCabinet1 > 0) {
    //         // 创建独立进度弹窗
    //         create_progress_msgbox("use", "please take out items！");
    //         update_progress(50);
    //         directOpenLockById(selectedCabinet1);
    //         currentWorkflowStage = 6;
    //     } else {
    //         currentWorkflowStage = 6; // 跳过第一个
    //     }
    // }
    // break;

    // case 6: // 等待第一个格口关闭
    // {
    //   bool allCabinetsClosed = true;
    //     // 检查格口是否已关闭
    //     if (selectedCabinet1 > 0 && !isLockOpen(selectedCabinet1)) {
    //       allCabinetsClosed = false;
    //     }
        
    //     if (selectedCabinet2 > 0 && !isLockOpen(selectedCabinet2)) {
    //       allCabinetsClosed = false;
    //     }
        
    //     // 检查是否超时
    //     if (millis() - processStartTime > 30000) { // 30秒超时
    //       Serial.println("[Warning] 等待关门超时");
    //       allCabinetsClosed = true; // 强制继续
    //     }

    //     if (allCabinetsClosed) {
    //       close_progress_msgbox();
    //       // 只禁用第一个格口相关按钮
    //       lv_imgbtn_set_state(objects.home_select_btn0, LV_IMGBTN_STATE_PRESSED);
    //       lv_obj_clear_flag(objects.home_select_btn0, LV_OBJ_FLAG_CLICKABLE);
    //       lv_imgbtn_set_state(objects.home_select_btn1, LV_IMGBTN_STATE_PRESSED);
    //       lv_obj_clear_flag(objects.home_select_btn1, LV_OBJ_FLAG_CLICKABLE);
    //       lv_imgbtn_set_state(objects.home_select_btn2, LV_IMGBTN_STATE_PRESSED);
    //       lv_obj_clear_flag(objects.home_select_btn2, LV_OBJ_FLAG_CLICKABLE);
    //       selectedCabinet1 = -1;
    //       currentWorkflowStage = 3; // 回到选择页面等待操作
    //   }
    // }
    // break;

    // case 7: // 打开第二个格口
    // {
    //     if (selectedCabinet2 > 0) {
    //         create_progress_msgbox("use", "plase take out items！");
    //         update_progress(50);
    //         directOpenLockById(selectedCabinet2);
    //         //等待用户点击
    //         currentWorkflowStage = 8;
    //     } else {
    //         currentWorkflowStage = 8; // 没有第二个格口直接完成
    //     }
    // }
    // break;

    // case 8: // 等待第二个格口关闭
    // {
    //   bool allCabinetsClosed = true;
    //   // 检查格口是否已关闭
    //   if (selectedCabinet1 > 0 && !isLockOpen(selectedCabinet1)) {
    //     allCabinetsClosed = false;
    //   }
      
    //   if (selectedCabinet2 > 0 && !isLockOpen(selectedCabinet2)) {
    //     allCabinetsClosed = false;
    //   }
      
    //   // 检查是否超时
    //   if (millis() - processStartTime > 30000) { // 30秒超时
    //     Serial.println("[Warning] 等待关门超时");
    //     allCabinetsClosed = true; // 强制继续
    //   }

    //     if (allCabinetsClosed) {
    //         close_progress_msgbox();
    //         // 更新第二个格口按钮状态
    //         lv_imgbtn_set_state(objects.home_select_btn3, LV_IMGBTN_STATE_PRESSED);
    //         lv_obj_clear_flag(objects.home_select_btn3, LV_OBJ_FLAG_CLICKABLE);
    //         selectedCabinet2 = -1;
    //         currentWorkflowStage = 9;
    //     }
    // }
    // break;
      
    case 9: // 检查RFID标签
      {
        // 准备检查的物品类型和天线
        const char* itemTypesToCheck[4] = {
          itemStatusList[0], itemStatusList[1], itemStatusList[2], itemStatusList[3]
        };
        
        uint8_t antennas[4] = {
          (uint8_t)selectedCabinet1, (uint8_t)selectedCabinet1, (uint8_t)selectedCabinet1, (uint8_t)selectedCabinet2
        };
        
        // 检查RFID标签
        bool allItemsTaken = checkRfidTags(itemTypesToCheck, antennas, 4);
        
        if (allItemsTaken) {
          // 所有物品都已取出
          update_progress(100);
          close_progress_msgbox();
          currentWorkflowStage = 10;
        } else {
          // 有物品未取出
          close_progress_msgbox();
          showBorrowErrorDialog();
          currentWorkflowStage = 10;
        }
      }
      break;
      
    case 10: // 完成流程
      // 跳转到完成页面
      lv_tabview_set_act(objects.tabview, 3, LV_ANIM_ON);
      
      // 发送操作结果到MQTT
      {
        JsonDocument doc;
        doc["operation_type"] = currentOperationType;
        doc["user_id"] = userId;
        JsonArray items = doc.createNestedArray("items");
        for (int i = 0; i < 4; i++) {
          items.add(itemStatusList[i]);
        }
        char buffer[256];
        serializeJson(doc, buffer);
        mqtt_publish(buffer);
      }
      
      // 重置状态
      currentOperationType = STATUS_NULL;
      currentWorkflowStage = 0;
      
      // 创建超时计时器返回主页
      if (operationTimeoutTimer == NULL) {
        operationTimeoutTimer = lv_timer_create(timeout_callback_1, TIMEOUT_MS_OVERALL, NULL);
      }
      break;
  }
}

//Step7: 
// 处理选择页物品按钮点击状态
static void handle_item_buttons() {
  for(int i=0; i<4; i++){
    lv_obj_t *btn = ((lv_obj_t*[]){objects.home_select_btn0, objects.home_select_btn1, 
                                   objects.home_select_btn2, objects.home_select_btn3})[i];

    // 只处理可点击的按钮(需要领用的物品)
    if(strcmp(itemStatusList[i], "1") == 0 && lv_obj_has_state(btn, LV_STATE_PRESSED)) {
      // 用户点击了需要领用的物品按钮
      lv_obj_clear_state(btn, LV_STATE_PRESSED);
      currentWorkflowStage = 4; // 进入打开格口阶段
      Serial.printf("[Action] 用户点击了物品 %d\n", i);
    }
  }
}

// 主循环处理函数
void super_loop()
{
  // 检查当前是否在管理页面
  if (lv_scr_act() == objects.manage) {
    // Serial.println("当前在管理页面");
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
            bool ret = directOpenLockById(i);  // 按钮编号直接对应锁ID

            if (ret){
              Serial.println("Update : Door open ,Record id");
              isControlIDS[i] = ret;
            }

            Serial.printf("[Action] 按钮 %d 按下，打开锁 %d\n", i, i);
            break;  // 一次只处理一个按钮
        }
    }
  }

    
  // 主页（Tab 0）按钮处理
  if (lv_tabview_get_tab_act(objects.tabview) == 0 && lv_scr_act() == objects.main)
  {
    // 领用按钮处理 --- Step 1: 处理物品选择页的按钮点击状态
    handle_item_buttons();
    if (lv_obj_has_state(objects.home_home_use, LV_STATE_PRESSED)) {
      handleHomePageButton("Borrow", STATUS_BORROW);
    }
    // 归还按钮处理
    if (lv_obj_has_state(objects.home_home_return, LV_STATE_PRESSED)) {
      handleHomePageButton("Borrow", STATUS_BORROW);
      //handleHomePageButton("Return", STATUS_RETURN);
    }

    // 维护按钮
    if (lv_obj_has_state(objects.home_home_maintain, LV_STATE_PRESSED)) {
          handleHomePageButton("MAINTAIN", STATUS_MAINTAIN);
          //handleHomePageButton("Return", STATUS_RETURN);
    }
  }

  // 分页状态机（处理不同标签页的逻辑）
  switch (lv_tabview_get_tab_act(objects.tabview))
  {
  case 1: // 身份验证页（Tab 1）  Step2: 处理身份验证页的按钮点击状态
    if (lv_obj_has_state(objects.home_idcheck_ok, LV_STATE_PRESSED))
    {
      // 如果是维护状态，则直接验证本账号
      if (currentOperationType ==STATUS_MAINTAIN ){
          Serial.println("Update : In Maintain mode");
          // MAINTAIN_ACCOUNT
          const char *userInputId = lv_textarea_get_text(objects.home_idcheck_textarea);
          if (strcmp(userInputId, MAINTAIN_ACCOUNT) == 0){
              // Modi:跳转到维护界面
              lv_scr_load(objects.manage);
          }
      }
      else{
        processIdentityVerification();
        currentWorkflowStage = 2; // 进入等待服务器响应阶段
      }

    }
    break;
    
  case 2: // 物品选择页（Tab 2）
    handle_item_buttons();
    break;
  }
  
  // 处理领用工作流
  if (strcmp(currentOperationType, STATUS_BORROW) == 0) {
    processBorrowWorkflow();
  }
  // 其他操作类型的工作流可以类似实现
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
    
    // // 更新界面
    // update_select_page(itemStatusList);
    
    lvgl_port_unlock();
    Serial.println("[Timeout] System state reset completed");
  } else {
    Serial.println("[Error] Failed to acquire LVGL lock in timeout handler");
  }
}


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