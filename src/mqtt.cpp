#include <main.h>

WiFiClient espClient;
PubSubClient client(espClient);

//mqtt配置
// WiFi
const char *ssid = "306";
const char *password = "123456789";
// MQTT Broker配置
const char *mqtt_broker = "39.98.181.107";
const char *topic = "smart_cabinet/01";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

// 定义全局变量
bool user_id_received = false;
bool item_states_received = false;

//初始化mqtt
void mqtt_initialize(){
    // 连接WIFI
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to the WiFi network");

    // 连接MQTT服务器
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp8266-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("Failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
    // 发布消息
    client.publish(topic, "hello emqx");
    client.subscribe(topic);
}

// 回调函数
void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  String message;
  for (int i = 0; i < length; i++) {
      message += (char) payload[i];  // Convert *byte to string
  }
  Serial.println(message);

  // 解析JSON消息
  StaticJsonDocument<300> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      return;
  }
  // 获取JSON中的数据并解析到borrowing_status_user数组中
  JsonArray item_states = doc["user_id"];
  for (int i = 0; i < item_states.size(); i++) {
      int status = item_states[i];
      borrowing_status_user[i] = status == 0 ? "0" : "1";
  }

  if (doc.containsKey("user_id")) {
    const char* user_id = doc["user_id"];
    Serial.print("user_id: ");
    Serial.println(user_id);
    user_id_received = true;
  }

  if (doc.containsKey("item_states")) {
    JsonArray item_states = doc["item_states"];
    for (int i = 0; i < item_states.size(); i++) {
        int status = item_states[i];
        borrowing_status_user[i] = status == 0 ? "0" : "1";
    }
    item_states_received = true;
    Serial.print("item_states: ");
    for (int i = 0; i < 4; i++) {
        Serial.print(borrowing_status_user[i]);
    }
  }
  Serial.println("-----------------------");
}

//回调函数
void mqttloop(){
  client.loop();
}
//发布消息
void mqtt_publish(char *message){
  client.publish(topic, message);
}
//按键发布消息
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
//按键弹窗
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

