#include <main.h> // 包含主头文件，提供基础定义和对象声明

/*********************** 状态机设计 ************************/
/**
 * @brief 系统状态枚举
 * 定义了系统运行过程中的主要状态
 */
typedef enum
{
  STATE_IDLE,           // 空闲状态，等待用户操作
  STATE_AUTHENTICATING, // 身份验证中，等待用户输入ID
  STATE_SELECTING,      // 物品选择中，用户选择要操作的物品
  STATE_PROCESSING,     // 处理中（开门/RFID验证等实际操作阶段）
  STATE_COMPLETION      // 完成状态，显示操作结果
} SystemState;

/**
 * @brief 操作类型枚举
 * 定义了系统支持的主要操作类型
 */
typedef enum
{
  OP_BORROW,  // 物品领用操作
  OP_RETURN,  // 物品归还操作
  OP_MAINTAIN // 设备维护操作
} OperationType;

/**
 * @brief 系统上下文结构体
 * 保存系统运行时的所有关键状态信息
 */
SystemContext sysCtx;
static SystemContext sysCtx; // 全局系统上下文实例

/*********************** 硬件接口层 ************************/
/**
 * @brief 硬件管理类
 * 封装所有与硬件交互的操作
 */
class HardwareMgr
{
public:
  /**
   * @brief 控制指定柜门开关
   * @param lockerId 柜门ID(0-3)
   * @param open true=开门, false=关门
   * @return 操作是否成功
   */
  static bool controlLocker(uint8_t lockerId, bool open)
  {
    char msg[32]; // 存储返回消息
    // 调用底层开门函数，参数说明：(控制器ID, 柜门ID(1-4), 返回消息)
    bool ret = openLock(1, lockerId + 1, msg);
    Serial.printf("[Hardware] Locker %d %s: %s\n",
                  lockerId + 1, open ? "Open" : "Close", ret ? "Success" : "Failed");
    return ret;
  }

  /**
   * @brief 刷新所有柜门状态
   * @param states 状态数组(输出参数)
   */
  static void refreshDoorStates(bool states[4])
  {
    for (int i = 0; i < 4; i++)
    {
      // 获取单个柜门状态，柜门ID为1-4
      states[i] = directGetStateById(i + 1);
    }
  }
};

/*********************** 界面管理层 ************************/
/**
 * @brief 界面管理类
 * 处理所有与用户界面相关的操作
 */
class UIMgr
{
public:
  /**
   * @brief 更新物品选择页面的按钮状态
   * 根据sysCtx.itemStates同步UI按钮状态
   */
  static void updateSelectionUI()
  {
    // 获取LVGL硬件锁，确保线程安全
    if (lvgl_port_lock(-1))
    {
      // 获取4个选择按钮对象
      lv_obj_t *btns[] = {objects.home_select_btn0, objects.home_select_btn1,
                          objects.home_select_btn2, objects.home_select_btn3};
      // 遍历更新每个按钮状态
      for (int i = 0; i < 4; i++)
      {
        lv_imgbtn_set_state(btns[i], sysCtx.itemStates[i] ? LV_IMGBTN_STATE_PRESSED : LV_IMGBTN_STATE_RELEASED);
      }
      lvgl_port_unlock(); // 释放硬件锁
    }
  }
  /**
   * @brief 显示进度弹窗
   * @param title 弹窗标题
   * @param msg 进度消息
   * @param progress 进度值(0-100)
   */
  static void showProgress(const char *title, const char *msg, uint8_t progress)
  {
    // 实现将使用原有的create_progress_msgbox逻辑
    // 包括创建进度条、更新进度值和关闭弹窗等功能
  }
};

/*********************** 业务逻辑层 ************************/
/**
 * @brief 工作流管理类
 * 实现系统核心业务流程控制
 */
class WorkflowMgr
{
private:
  /**
   * @brief 重置系统上下文
   * 将物品状态和用户ID等重置为初始值
   */
  void resetContext()
  {
    memset(sysCtx.itemStates, 0, sizeof(sysCtx.itemStates));
    sysCtx.userId = "";
    sysCtx.currentOp = OP_BORROW; // 默认操作类型为领用
  }

  void sendAuthRequest()
  {
    JsonDocument doc;
    doc["topic"] = "/device/CP_1/user_auth";
    doc["payload"]["msg_id"] = "AUTH_" + String(millis());
    doc["payload"]["phone_suffix"] = sysCtx.userId;
    doc["payload"]["auth_type"] = (sysCtx.currentOp == OP_BORROW) ? "borrow" : "return";

    char buffer[256];
    serializeJson(doc, buffer);
    mqtt_publish(buffer);
  }

  void sendCompletionMessage()
  {
    JsonDocument doc;
    doc["topic"] = "/device/CP_1/operation_complete";
    doc["payload"]["reservation_id"] = sysCtx.reservationId;
    doc["payload"]["status"] = sysCtx.rfidVerified ? "SUCCESS" : "FAILED";

    char buffer[256];
    serializeJson(doc, buffer);
    mqtt_publish(buffer);
  }

public:
  /**
   * @brief 处理主页面操作
   * @param op 用户选择的操作类型
   */
  void handleHomeOperation(OperationType op)
  {
    sysCtx.currentOp = op;                              // 设置当前操作类型
    sysCtx.currentState = STATE_AUTHENTICATING;         // 进入身份验证状态
    lv_tabview_set_act(objects.tabview, 1, LV_ANIM_ON); // 切换到身份验证页
  }

  /**
   * @brief 处理身份验证
   * @param userId 用户输入的ID字符串
   */
  void handleAuthentication(const String &userId)
  {
    // 简单验证用户ID长度
    if (userId.length() != 6 || !userId.toInt())
    {
      UIMgr::showProgress("错误", "请输入6位数字领用码", 0);
      return;
    }

    sysCtx.userId = userId; // 保存用户ID
    // 发送验证请求到服务器
    // 将枚举类型转换为整数后传递给认证函数
    // 将操作类型转换为字符串后传递给认证函数
    sendAuthRequest();
    sysCtx.currentState = STATE_SELECTING; // 进入物品选择状态
  }

  /**
   * @brief 处理物品选择
   * @param itemIndex 物品索引(0-3)
   */
  void handleItemSelection(uint8_t itemIndex)
  {
    if (itemIndex >= 4)
      return; // 参数检查
    // 切换物品选中状态
    sysCtx.itemStates[itemIndex] = !sysCtx.itemStates[itemIndex];
    UIMgr::updateSelectionUI(); // 更新UI
  }

  /**
   * @brief 处理开门操作
   * 用户确认选择后触发实际处理流程
   */
  void handleConfirmation()
  {
    sysCtx.currentState = STATE_PROCESSING; // 进入处理状态
    processOperation();                     // 开始实际处理
  }

private:
  void sendCompletion()
  {
    JsonDocument doc;
    doc["topic"] = "/device/CP_1/borrow_complete";
    doc["payload"]["reservation_id"] = sysCtx.reservationId;
    doc["payload"]["status"] = sysCtx.rfidVerified ? "SUCCESS" : "FAILED";

    JsonArray items = doc["payload"].createNestedArray("item_states");
    for (int i = 0; i < 4; i++)
    {
      items.add(sysCtx.itemStates[i] ? 1 : 0); // 1=已借出，0=未借出
    }

    char buffer[512];
    serializeJson(doc, buffer);
    mqtt_publish(buffer);

    Serial.printf("[系统状态] 领用完成：预约号%s，验证状态：%s\n",
                  sysCtx.reservationId.c_str(),
                  sysCtx.rfidVerified ? "成功" : "失败");
  }
  /**
   * @brief 等待柜门关闭（带超时检测）
   * @param timeoutMs 超时时间（毫秒）
   * @return true=所有柜门已关闭，false=超时
   */
  bool waitDoorClosed(unsigned long timeoutMs)
  {
    unsigned long startTime = millis();
    bool allClosed = false;

    while (millis() - startTime < timeoutMs)
    {
      // 刷新门状态
      HardwareMgr::refreshDoorStates(sysCtx.doorStates);

      // 检查选中的柜门
      allClosed = true;
      // if (sysCtx.itemStates[i] && sysCtx.doorStates[i])
      // {
      //   allClosed = false;
      //   break;
      // }

      if (allClosed)
      {
        Serial.println("[硬件监控] 所有柜门已关闭");
        return true;
      }

      delay(100); // 降低轮询频率
    }

    Serial.println("[超时警告] 柜门未在规定时间内关闭");
    return false;
  }
  /**
   * @brief 执行实际操作流程
   * 包括开门、等待关门、RFID验证等步骤
   */
  void processOperation()
  {
    // 开门操作
    for (int i = 0; i < 4; i++)
    {
      if (sysCtx.itemStates[i])
      {
        HardwareMgr::controlLocker(i, true);
      }
    }
    UIMgr::showProgress("操作中", "请开门取物", 33);

    // 等待关门（轮询检测）
    while (!waitDoorClosed(30000))
    {
      UIMgr::showProgress("警告", "请关闭柜门", 66);
    }

    // // RFID验证
    // if(verifyRFID()) {
    //     UIMgr::showProgress("验证中", "RFID验证通过", 100);
    //     sysCtx.rfidVerified = true;
    // } else {
    //     UIMgr::showProgress("错误", "物品不匹配", 100);
    //     // 弹出重新操作对话框
    //     createRetryDialog();
    // }

    sendCompletion();
    sysCtx.currentState = STATE_COMPLETION; // 进入完成状态
  }
};

/*********************** 主循环处理 ************************/
/**
 * @brief 主循环处理函数
 * 系统主循环，根据当前状态调用相应处理逻辑
 */
void super_loop()
{
  static WorkflowMgr workflow; // 工作流管理器实例

  // 1. 管理页面特殊处理
  if (lv_scr_act() == objects.manage)
  {
    handleManagementMode(); // 进入管理模式处理
    return;
  }

  // 2. 主状态机处理
  switch (sysCtx.currentState)
  {
  case STATE_IDLE:
    handleIdleState(); // 空闲状态处理
    break;

  case STATE_AUTHENTICATING:
    // 检测身份验证确认按钮是否按下
    if (lv_obj_has_state(objects.home_idcheck_ok, LV_STATE_PRESSED))
    {
      workflow.handleAuthentication(
          lv_textarea_get_text(objects.home_idcheck_textarea));
    }
    break;

  case STATE_SELECTING:
    handleSelectionState(); // 选择状态处理
    break;

  case STATE_PROCESSING:
    // 处理中状态由后台自动推进，无需额外处理
    break;

  case STATE_COMPLETION:
    handleCompletionState(); // 完成状态处理
    break;
  }

  checkTimeout(); // 超时检测
}

// 完善状态处理函数
void handleManagementMode()
{
  // 管理模式的硬件控制逻辑
  HardwareMgr::refreshDoorStates(sysCtx.doorStates);
  // ... 其他管理逻辑 ...
}

void handleIdleState()
{
  // 空闲状态初始化
  sysCtx.rfidVerified = false;
  sysCtx.progressValue = 0;
  UIMgr::showProgress("", "", 0);
}

void handleSelectionState()
{
  // 更新物品选择状态到UI
  UIMgr::updateSelectionUI();
}

void handleCompletionState()
{
  // 发送完成信息
  //WorkflowMgr::sendCompletionMessage();
  // 重置状态
  sysCtx.currentState = STATE_IDLE;
}

void checkTimeout()
{
  // 检查超时并重置系统
  if (sysCtx.timeoutTimer)
  {
    lv_timer_del(sysCtx.timeoutTimer);
    sysCtx.timeoutTimer = nullptr;
    sysCtx.currentState = STATE_IDLE;
  }
}
