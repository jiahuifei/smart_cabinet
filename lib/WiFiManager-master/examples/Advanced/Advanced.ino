/**
 * WiFiManager高级演示，包含高级配置选项
 * 实现TRIGGEN_PIN按钮功能：短按启动按需配置门户，长按3秒重置设置
 */
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#define TRIGGER_PIN 0 // 触发引脚(GPIO0)

// WiFiManager可运行在阻塞或非阻塞模式
// 使用非阻塞模式时需确保能处理无delay()的循环
bool wm_nonblocking = false; // true=非阻塞模式，false=阻塞模式

WiFiManager wm; // WiFiManager全局实例
WiFiManagerParameter custom_field; // 全局参数(用于非阻塞模式带参数)

void setup() {
  WiFi.mode(WIFI_STA); // 显式设置为STA模式(ESP默认STA+AP)
  Serial.begin(115200); // 初始化串口通信
  Serial.setDebugOutput(true); // 启用调试输出
  delay(3000); // 延时等待串口稳定
  Serial.println("\n Starting"); // 打印启动信息

  pinMode(TRIGGER_PIN, INPUT); // 设置触发引脚为输入模式
  
  // wm.resetSettings(); // 取消注释可擦除保存的WiFi设置

  if(wm_nonblocking) wm.setConfigPortalBlocking(false); // 非阻塞模式设置

  // 添加自定义输入字段
  int customFieldLength = 40; // 自定义字段长度

  // 标准文本输入字段示例
  // new (&custom_field) WiFiManagerParameter("customfieldid", "Custom Field Label", "Custom Field Value", customFieldLength,"placeholder=\"Custom Field Placeholder\"");
  
  // 复选框输入字段示例
  // new (&custom_field) WiFiManagerParameter("customfieldid", "Custom Field Label", "Custom Field Value", customFieldLength,"placeholder=\"Custom Field Placeholder\" type=\"checkbox\""); 
  
  // 单选按钮输入字段
  const char* custom_radio_str = "<br/><label for='customfieldid'>Custom Field Label</label><input type='radio' name='customfieldid' value='1' checked> One<br><input type='radio' name='customfieldid' value='2'> Two<br><input type='radio' name='customfieldid' value='3'> Three";
  new (&custom_field) WiFiManagerParameter(custom_radio_str); // 创建自定义HTML输入参数
  
  wm.addParameter(&custom_field); // 添加自定义参数
  wm.setSaveParamsCallback(saveParamCallback); // 设置参数保存回调

  // 通过数组或向量自定义菜单
  // 菜单项："wifi","wifinoscan","info","param","close","sep","erase","restart","exit"
  // (sep是分隔符)(如果param在菜单中，参数将不会显示在wifi页面)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"}; 
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  wm.setMenu(menu); // 设置自定义菜单

  wm.setClass("invert"); // 设置暗色主题

  // 设置静态IP配置
  // wm.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0)); // 设置静态IP、网关和子网掩码
  // wm.setShowStaticFields(true); // 强制显示静态IP字段
  // wm.setShowDnsFields(true);    // 强制显示DNS字段

  // 连接超时设置
  // wm.setConnectTimeout(20); // 尝试连接的超时时间(秒)
  
  wm.setConfigPortalTimeout(30); // 配置门户自动关闭超时(30秒)
  // wm.setCaptivePortalEnable(false); // 禁用强制门户重定向
  // wm.setAPClientCheck(true); // 当客户端连接到软AP时避免超时

  // WiFi扫描设置
  // wm.setRemoveDuplicateAPs(false); // 不移除重复的AP名称(默认为true)
  // wm.setMinimumSignalQuality(20);  // 设置扫描显示的最小RSSI(百分比)，null=8%
  // wm.setShowInfoErase(false);      // 不显示信息页面的擦除按钮
  // wm.setScanDispPerc(true);       // 以百分比而非图标显示信号强度
  
  // wm.setBreakAfterConfig(true);   // 即使WiFi保存失败也退出配置门户

  bool res; // 连接结果
  // res = wm.autoConnect(); // 使用芯片ID自动生成AP名称
  // res = wm.autoConnect("AutoConnectAP"); // 使用指定AP名称(无密码)
  res = wm.autoConnect("AutoConnectAP","password"); // 使用密码保护的AP

  if(!res) {
    Serial.println("连接失败或超时");
    // ESP.restart(); // 取消注释可在失败时重启设备
  } 
  else {
    Serial.println("连接成功!");
  }
}

// 按钮检测函数
void checkButton(){
  // 检查按钮按下
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    // 简易消抖/长按检测(不适合生产环境)
    delay(50);
    if( digitalRead(TRIGGER_PIN) == LOW ){
      Serial.println("按钮按下");
      // 持续按住按钮3000ms将重置设置
      delay(3000); // 重置延迟保持
      if( digitalRead(TRIGGER_PIN) == LOW ){
        Serial.println("长按按钮");
        Serial.println("擦除配置并重启");
        wm.resetSettings(); // 重置WiFi设置
        ESP.restart(); // 重启设备
      }
      
      // 启动配置门户
      Serial.println("启动配置门户");
      wm.setConfigPortalTimeout(120); // 设置120秒超时
      
      if (!wm.startConfigPortal("OnDemandAP","password")) {
        Serial.println("连接失败或超时");
        delay(3000);
        // ESP.restart(); // 取消注释可在失败时重启设备
      } else {
        Serial.println("连接成功!");
      }
    }
  }
}

// 从服务器获取参数值(用于自定义HTML输入)
String getParam(String name){
  String value;
  if(wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

// 参数保存回调函数
void saveParamCallback(){
  Serial.println("[回调] saveParamCallback触发");
  Serial.println("参数 customfieldid = " + getParam("customfieldid"));
}

void loop() {
  if(wm_nonblocking) wm.process(); // 非阻塞模式下需要处理WiFiManager
  checkButton(); // 检测按钮状态
  // 在此处添加主循环代码
}
