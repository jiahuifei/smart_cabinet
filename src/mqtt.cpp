#include <main.h>
// WiFi
const char *ssid = "cb"; // Enter your Wi-Fi name
const char *password = "123456789";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "39.98.181.107";
const char *topic = "/device/CP_1/status";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient mqtt_client;
PubSubClient client(mqtt_client);



#define DEVICE_ID "CP_1"      // 设备唯一标识符
// 物品状态列表
const char* itemStatusList[4] = {"0", "0", "0", "0"};  // 初始化为0
// 系统状态标志
bool item_states_received = false;  // 物品状态接收标志
bool user_id_received = false;  // 用户ID接收标志

void callback(char *topic, byte *payload, unsigned int length);
void send_heartbeat();

void mqtt_init() {
    // Set software serial baud to 115200;
    Serial.begin(115200);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // Publish and subscribe
    client.publish(topic, "Hi, I'm ESP32 ^^");
    client.subscribe("/server/CP_1/auth_response");
}



void mqtt_loop() {
    client.loop();
    send_heartbeat();
}

// 心跳包发送函数：定期向服务器发送设备状态信息
// 心跳包发送（由mqttloop自动调用）
// 包含：时间戳、网络状态(RSSI、IP)、系统资源(CPU、内存)
void send_heartbeat() {
  static unsigned long last_send = 0;
  if (millis() - last_send < 30000) return;
  
  JsonDocument doc;
  doc["timestamp"] = millis();
  
  // 使用新语法
  auto network = doc["network"].to<JsonObject>();
  network["rssi"] = WiFi.RSSI();
  network["ip"] = WiFi.localIP().toString();
  
  auto resources = doc["resources"].to<JsonObject>();
  resources["cpu"] = 100 - xPortGetTickRateHz();
  resources["mem"] = ESP.getFreeHeap() * 100 / ESP.getHeapSize();

  char buffer[256];
  serializeJson(doc, buffer);
  client.publish(String("/device/" DEVICE_ID "/heartbeat").c_str(), buffer);
  Serial.println(String("/device/" DEVICE_ID "/heartbeat").c_str());
  Serial.println(buffer);
  
  last_send = millis();
}

void callback(char *topic, byte *payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
      message += (char) payload[i];
  }
  Serial.println("Message arrived in topic: ");
  Serial.print(topic);
  Serial.println("Message:" + message);
  Serial.println("-----------------------");
  // 创建JSON文档对象
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
  }
  if(String(topic) == String("/server/CP_1/auth_response")) {
    Serial.println("收到认证响应");
    const char* correlation_id = doc["correlation_id"]; 
    Serial.printf("用户ID: %s\n", correlation_id);
    
    // 检查状态字段
    const char* status = doc["status"];
    if(strcmp(status, "null_user") == 0){
      user_id_received = false;
      Serial.println("用户不存在");
    }
    else if(strcmp(status, "use") == 0) { // Step4
      Serial.println("领用操作");
      user_id_received = true;
      // 解析物品状态数组
      if(doc["item_states"].is<JsonArray>()) {
        JsonArray item_states = doc["item_states"].as<JsonArray>();
        for (int i = 0; i < item_states.size() && i < 4; i++) {
            itemStatusList[i] = item_states[i] == 0 ? "0" : "1";
        }
        item_states_received = true;
        Serial.println("物品状态已更新");
      } else {
          Serial.println("警告: 缺少item_states字段");
      }
    }
    else if(strcmp(status, "return") == 0) {
      Serial.println("归还操作");
      user_id_received = true;
        // 解析物品状态数组
        if(doc.containsKey("item_states")) {
          JsonArray item_states = doc["item_states"];
          for (int i = 0; i < item_states.size() && i < 4; i++) {
              itemStatusList[i] = item_states[i] == 0 ? "0" : "1";
          }
          item_states_received = true;
          Serial.println("物品状态已更新");
      } else {
          Serial.println("警告: 缺少item_states字段");
      }
    } 
    else {
        Serial.printf("状态异常: %s\n", status);
    }
  }
}

// 用户认证请求函数（应在用户输入信息后调用）
// 参数：phone_suffix-手机号后6位，auth_type-认证类型(borrow/return)
// 使用示例：send_user_auth("6789", "borrow");
void send_user_auth(const char* correlation_id, const char* auth_type) {
    JsonDocument doc;
    doc["correlation_id"] = correlation_id;
    doc["auth_type"] = auth_type;
    doc["timestamp"] = millis();
  
    char buffer[256];
    serializeJson(doc, buffer);
    client.publish("/device/" DEVICE_ID "/user_auth", buffer);
    Serial.println("/device/" DEVICE_ID "/user_auth");
    Serial.println(buffer);
}

void mqtt_publish(char *message){
    client.publish(topic, message);
}