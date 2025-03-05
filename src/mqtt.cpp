#include <main.h>

WiFiClient espClient;
PubSubClient client(espClient);

//mqtt配置
// WiFi
const char *ssid = "306";
const char *password = "123456789";

// MQTT Broker
const char *mqtt_broker = "39.98.181.107";
const char *topic = "smart_cabinet/01";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

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

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  String message;
  for (int i = 0; i < length; i++) {
      message += (char) payload[i];  // Convert *byte to string
  }
  Serial.print(message);
  // if (message == "on" && !ledState) {
  //     digitalWrite(LED, HIGH);  // Turn on the LED
  //     ledState = true;
  // }
  // if (message == "off" && ledState) {
  //     digitalWrite(LED, LOW); // Turn off the LED
  //     ledState = false;
  // }
  Serial.println();
  Serial.println("-----------------------");
}

void mqttloop(){
  client.loop();
}
void mqtt_publish(char *message){
  client.publish(topic, message);
}

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