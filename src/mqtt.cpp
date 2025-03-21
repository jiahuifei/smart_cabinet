#include <main.h>

// WiFi客户端实例
WiFiClient espClient;
// MQTT客户端实例，使用WiFi客户端
PubSubClient client(espClient);

// WiFi连接配置
const char *ssid = "306";           // WiFi名称
const char *password = "123456789"; // WiFi密码

// MQTT服务器配置
const char *mqtt_broker = "39.98.181.107"; // MQTT服务器地址
const char *topic = "smart_cabinet/01";     // MQTT主题
const char *mqtt_username = "emqx";         // MQTT用户名
const char *mqtt_password = "public";       // MQTT密码
const int mqtt_port = 1883;                 // MQTT服务器端口

// 系统状态标志
bool user_id_received = false;      // 用户ID接收状态标志
bool item_states_received = false;  // 物品状态接收标志

// 设备配置常量
#define DEVICE_ID "CP_1"      // 设备唯一标识符
#define FIRMWARE_VER "v2.1.5" // 固件版本号
#define DOOR_COUNT 50         // 柜门总数量

// 柜门状态枚举
enum DoorStatus {
    FREE,     // 空闲状态
    OCCUPIED, // 占用状态
    FAULT     // 故障状态
};

// 操作结果枚举
enum OpResult {
    SUCCESS,  // 操作成功
    FAILURE   // 操作失败
};

// 门控制回调函数类型定义
typedef void (*DoorControlCallback)(uint8_t door_num, uint8_t action, uint8_t duration);

DoorControlCallback door_control_cb = nullptr;   // 门控制回调函数指针

// MQTT消息发布函数：向指定主题发送消息
void mqtt_publish(char *message){
  client.publish(topic, message);
}

// 发送MQTT消息（需要按钮触发）
// 参数：button-LVGL按钮对象，message-要发送的消息
// 使用示例：button_check_send(my_button, "hello");
void button_check_send(lv_obj_t *button, char *message){
   // 全局或静态变量记录上一次状态
  static bool last_button_state = false;
  static bool current_state= false;
  // 获取当前状态
  current_state = lv_obj_has_state(button, LV_STATE_PRESSED);

  // 检查 mqtt_button 是否处于按下状态
  if (current_state != last_button_state && current_state) {
    last_button_state = current_state;
    printf("MQTT Button is pressed!\n");
    mqtt_publish(message);
    //delay(50);
  } else {
    printf("MQTT Button is released.\n");
    last_button_state = current_state;
  }
}

// 按钮消息框显示函数：检测按钮状态并显示消息框
// @param button: LVGL按钮对象
// @param btns: 消息框按钮文本数组
// @param msgtitle: 消息框标题
// @param msgtext: 消息框内容
void button_check_msgbox(
  lv_obj_t *button,
  const char* btns[],
  const char* msgtitle,
  const char* msgtext){
   // 全局或静态变量记录上一次状态
  static bool last_button_state = false;
  static bool current_state= false;
  // 获取当前状态
  current_state = lv_obj_has_state(button, LV_STATE_PRESSED);

  // 检查 mqtt_button 是否处于按下状态
  if (current_state != last_button_state && current_state) {
    last_button_state = current_state;
    printf("MQTT Button is pressed!\n");

    lv_obj_t* lv_msgbox = lv_msgbox_create(NULL,msgtitle,msgtext, btns,true);
    lv_obj_center(lv_msgbox);

    //delay(50);
  } else {
    printf("MQTT Button is released.\n");
    last_button_state = current_state;
  }
}

// 设备注册（自动调用，无需手动触发）
void send_device_register() {
  JsonDocument doc;
  doc["msg_id"] = "REG_" + String(millis());
  doc["device_id"] = DEVICE_ID;
  doc["firmware"] = FIRMWARE_VER;
  doc["door_count"] = DOOR_COUNT;
  
  char buffer[128];
  serializeJson(doc, buffer);
  client.publish("/device/register", buffer);
}

// 心跳包发送函数：定期向服务器发送设备状态信息
// 心跳包发送（由mqttloop自动调用）
// 包含：时间戳、网络状态(RSSI、IP)、系统资源(CPU、内存)
void send_heartbeat() {
  static unsigned long last_send = 0;
  if (millis() - last_send < 30000) return;
  
  JsonDocument doc;
  doc["timestamp"] = millis();
  
  // 使用新的推荐语法
  auto network = doc["network"].to<JsonObject>();
  network["rssi"] = WiFi.RSSI();
  network["ip"] = WiFi.localIP().toString();
  
  auto resources = doc["resources"].to<JsonObject>();
  resources["cpu"] = 100 - xPortGetTickRateHz();
  resources["mem"] = ESP.getFreeHeap() * 100 / ESP.getHeapSize();

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(String("/device/" DEVICE_ID "/heartbeat").c_str(), buffer);
  
  last_send = millis();
}

// 门状态反馈（门操作后调用）
// 参数：door_num-门号，result-操作结果，item_type-物品类型
// 使用示例：send_door_feedback(1, SUCCESS, 2);
void send_door_feedback(uint8_t door_num, OpResult result, uint8_t item_type) {
  JsonDocument doc;
  doc["command_id"] = "CMD_" + String(millis());
  doc["result"] = result == SUCCESS ? "success" : "fail";
  doc["door_number"] = door_num;
  doc["actual_item"] = item_type;
  doc["timestamp"] = millis();

  char buffer[128];
  serializeJson(doc, buffer);
  client.publish(String("/device/" DEVICE_ID "/door_feedback").c_str(), buffer);
}

// 门状态上报（定时自动调用，可手动触发）
// 参数：door_num-门号，status-门状态，item_type-物品类型
// 使用示例：report_door_status(1, OCCUPIED, 3);
void report_door_status(uint8_t door_num, DoorStatus status, uint8_t item_type) {
  JsonDocument doc;
  doc["door"] = door_num;
  doc["status"] = status == FREE ? "free" : 
                 status == OCCUPIED ? "occupied" : "fault";
  doc["item_type"] = item_type;
  doc["timestamp"] = millis();

  auto sensors = doc["sensors"].to<JsonArray>();
  sensors.add(1); // 模拟RFID数据
  sensors.add(0);
  sensors.add(1);
  sensors.add(0);

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(String("/device/" DEVICE_ID "/status").c_str(), buffer);
}

// 实际门控制（收到服务器指令时调用）
// 参数：door-门号，action-操作类型，duration-持续时间
// 返回：操作是否成功
// 使用示例：bool success = control_door(1, "open", 5);
bool control_door(uint8_t door, String action, uint8_t duration) {
  if (door >= DOOR_COUNT) {
      Serial.printf("无效门编号: %d\n", door);
      return false;
  }

  char rsMsg[32];
  bool result = false;
  // 门控硬件寻址算法
  // 板地址计算：每块控制板管理24个锁，通过整除运算确定板号
  // 锁地址计算：取余运算确定板内具体锁编号
  // 示例：door=25 → boardNo=2, lockNo=2
  uint8_t boardNo = 1 + door / 24;
  uint8_t lockNo = 1 + door % 24;
  
  // 根据动作类型调用硬件接口
  if (action == "open") {
      result = openLock(boardNo, lockNo, rsMsg);
  } 
  else if (action == "open_all") {
      result = openAllLock(boardNo, rsMsg);
  }
  else if (action == "power_on") {
      result = openPower(boardNo, lockNo, rsMsg);
  }
  else if (action == "power_off") {
      result = closePower(boardNo, lockNo, rsMsg);
  }
  
  Serial.printf("门控制结果 [板%d 锁%d]: %s\n", boardNo, lockNo, rsMsg);
  return result;
}

// MQTT消息回调函数：处理接收到的MQTT消息
// @param topic: 消息主题
// @param payload: 消息内容
// @param length: 消息长度
// MQTT消息回调（自动触发，无需手动调用）
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i=0; i<length; i++) message += (char)payload[i];
  
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(message);

  // 创建JSON文档对象（512字节容量）
  // 容量根据MQTT消息最大长度设定，需注意：
  // 1. 容量不足会导致解析失败
  // 2. 建议定期使用doc.capacity()检查内存使用
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      return;
  }

  // 门控制指令处理
  if(String(topic) == String("/server/" DEVICE_ID "/door_control")) {
      uint8_t door = doc["door_number"];
      String action = doc["action"].as<String>();
      uint8_t duration = doc["duration"];  // 单位：秒
      
      // 添加操作延时
      if (duration > 0) {
          unsigned long start = millis();
          while (millis() - start < duration * 1000) {
              control_door(door, action, duration);
          }
      } else {
          control_door(door, action, duration);
      }
  }
  
  // 用户认证响应处理
  else if(String(topic) == String("/server/" DEVICE_ID "/auth_response")) {
      if(doc["status"] == 200) {
          // 更新用户界面状态
          update_ui(
              doc["user_info"]["user_id"],
              doc["door_assignment"]["primary"],
              doc["valid_until"]
          );
      } else {
          handle_auth_error(doc["error_code"]);
      }
  }
  
  // 保留原始JSON处理逻辑
  else {
      // 原始user_id和item_states处理逻辑
      if (doc["user_id"].is<const char*>()) {
          const char* user_id = doc["user_id"];
          Serial.print("user_id: ");
          Serial.println(user_id);
          user_id_received = true;
      }

      if (doc["item_states"].is<JsonArray>()) {
        JsonArray item_states = doc["item_states"];
        for (int i = 0; i < item_states.size() && i < 4; i++) {  // 限制为4个元素
            int status = item_states[i];
            // 修改为使用已定义的数组
            borrowing_status_user[i] = status == 0 ? "0" : "1";
        }
        item_states_received = true;
        Serial.print("item_states: ");
        for (int i = 0; i < 4; i++) {
            Serial.print(borrowing_status_user[i]);
        }
    }
  }
}

// 初始化MQTT连接（程序启动时调用一次）
// 使用示例：mqtt_initialize();
void mqtt_initialize() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  
  while (!client.connected()) {
    String client_id = "esp8266-client-" + String(WiFi.macAddress());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      client.subscribe("/server/" DEVICE_ID "/door_control");
      client.subscribe("/server/" DEVICE_ID "/auth_response");
      send_device_register();
    }
  }

// 更新后的JSON处理
JsonDocument doc;
JsonObject network = doc["network"].to<JsonObject>();
JsonArray sensors = doc["sensors"].to<JsonArray>();

// 订阅必要主题
client.subscribe(("/server/" DEVICE_ID "/door_control"));
client.subscribe(("/server/" DEVICE_ID "/auth_response"));

send_device_register(); // 发送注册信息
}

// 主循环处理（需要在Arduino loop中持续调用）
// 使用示例：在loop() { mqttloop(); }
void mqttloop() {
  client.loop();
  send_heartbeat(); // 定期发送心跳
  static unsigned long last_report = 0;
  if(millis() - last_report > 5000) { // 5秒上报
      report_door_status(0, FREE, 0); // 添加默认参数 
      last_report = millis();
  }
}

// UI更新函数（认证成功时自动调用）
// 参数：user_id-用户ID，door_num-分配柜门，valid_until-有效期
void update_ui(uint8_t user_id, uint8_t door_num, const char* valid_until) {
  // 实现具体的UI更新逻辑
  Serial.printf("更新用户界面: 用户ID=%d, 分配柜门=%d, 有效期=%s\n", 
               user_id, door_num, valid_until);
}

// 认证错误处理（认证失败时自动调用）
// 参数：error_code-服务器返回的错误码
void handle_auth_error(int error_code) {
  // 实现具体的错误处理逻辑
  Serial.printf("认证错误代码: %d\n", error_code);
  lv_obj_t* msgbox = lv_msgbox_create(NULL, "认证失败", "请检查用户权限", NULL, true);
  lv_obj_center(msgbox);
}
