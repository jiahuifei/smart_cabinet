#include <main.h>

// 调整日志回调函数签名匹配LVGL要求
void lvgl_log_print(const char * msg) { // 移除int level参数
    Serial.printf("[LVGL] %s\n", msg);  // 简化输出格式
}

void manage_btn_event_handler(lv_event_t * e) {
  lv_obj_t * target = lv_event_get_target(e);
  
  // 检查是否是管理页面的按钮(0-48)
  for(int i = 0; i <= 48; i++) {
      char btn_name[20];
      sprintf(btn_name, "manage_btn_%d", i);
      
      if(target == objects.manage_btn_0 + i) {  // 假设按钮是按顺序排列的
          directOpenLockById(i);  // 按钮编号直接对应锁ID
          break;
      }
  }
}

// 在初始化时添加事件监听器
void setup_manage_buttons() {
  for(int i = 0; i <= 48; i++) {
      lv_obj_t * btn = objects.manage_btn_0 + i;  // 假设按钮是按顺序排列的
      lv_obj_add_event_cb(btn, manage_btn_event_handler, LV_EVENT_CLICKED, NULL);
  }
}


void setup()
{
  Serial.begin(115200);
  // 初始化mqtt
  mqtt_initialize();
  
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
    // 释放互斥锁
    lvgl_port_unlock();
  }
  //lv_timer_create(timeout_callback_1, 500, NULL); // 500ms后执行超时回调函数

  lv_log_register_print_cb(lvgl_log_print);

  RS485.begin(9600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
  while (!RS485) {
    delay(10); // Wait for initialization to succeed
  }
  setup_manage_buttons();
}



// 在loop中修改
void loop()
{
  // 需要包裹所有UI操作
  if (lvgl_port_lock(-1)) {
    ui_tick();
    lvgl_port_unlock();
  }
  mqttloop();        // 可能涉及UI操作
  super_loop();      // 可能涉及UI操作

}


