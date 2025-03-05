#include <main.h>

void setup() {
  Serial.begin(115200);
  //初始化mqtt
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
  if (lvgl_port_lock(-1)) {
    ui_init(); 
    
    // 演示执行完毕后释放互斥锁
    lvgl_port_unlock();
  }
}

void loop() {
  ui_tick();
  mqttloop();
  super_loop();
}
