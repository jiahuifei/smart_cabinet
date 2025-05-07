#include <main.h>

// 调整日志回调函数签名匹配LVGL要求
void lvgl_log_print(const char * msg) { // 移除int level参数
    Serial.printf("[LVGL] %s\n", msg);  // 简化输出格式
}


/**
* @brief 按钮回调
* @param *e ：事件相关参数的集合，它包含了该事件的所有数据
* @return 无
*/
static void triggle_home_event_cb(lv_event_t * e)
{
  Serial.println("Update : Home button was triggle");

  // 更新们状态，到数据库
  CheckDoorState();
}

void setup()
{
  Serial.begin(115200);
  // 初始化mqtt
  mqtt_init();
  rs485_init();
  rfid_all_init();
  ItemDatabase::init();
  
  static esp_lcd_panel_handle_t panel_handle = NULL; // 声明 LCD 面板
  static esp_lcd_touch_handle_t tp_handle = NULL;    // 声明触摸面板

  // 初始化GT911触摸屏控制器
  tp_handle = touch_gt911_init();

  // 初始化 Waveshare ESP32-S3 RGB LCD 硬件
  panel_handle = waveshare_esp32_s3_rgb_lcd_init();

  // 打开背光
  wavesahre_rgb_lcd_bl_on();

  // 使用面板和触摸手柄初始化 LVGL
  ESP_ERROR_CHECK(lvgl_port_init(panel_handle, tp_handle));

  ESP_LOGI(TAG, "Display LVGL demos");

  // 锁定互斥锁，因为 LVGL API 不是线程安全的
  if (lvgl_port_lock(-1))
  {
    ui_init();

    // 注册特殊按钮的回调事件
    Serial.println("Update : add home button callback");
    /* 设置按钮事件 */
    lv_obj_add_event_cb(objects.btn_go_main, triggle_home_event_cb, LV_EVENT_CLICKED, NULL);

    // 释放互斥锁
    lvgl_port_unlock();
  }
  //lv_timer_create(timeout_callback_1, 500, NULL); // 500ms后执行超时回调函数

  lv_log_register_print_cb(lvgl_log_print);

  RS485.begin(9600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
  while (!RS485) {
    delay(10); // Wait for initialization to succeed
  }
}

void loop()
{
  // 需要包裹所有UI操作
  if (lvgl_port_lock(-1)) {
    ui_tick();
    lvgl_port_unlock();
  }
  mqtt_loop();
  super_loop();      // 可能涉及UI操作
  



}


