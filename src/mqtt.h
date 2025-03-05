#ifndef MQTT_H
#define MQTT_H

#include <main.h>


extern WiFiClient espClient;
extern PubSubClient client; 
extern const char *topic;

void mqtt_initialize();
//void callback(char *topic, byte *payload, unsigned int length);
void callback(char *topic, byte *payload, unsigned int length);
void mqttloop();
void mqtt_publish(char *message);
void button_check_send(lv_obj_t *button, char *message);
void button_check_msgbox(lv_obj_t *button, const char* btns[] = nullptr, const  char* msgtitle = "", const char* msgtext = "");
  // static const char* btns[] = {"Apply", "Close", nullptr}; // 以 nullptr 结尾
  // button_check_msgbox(
  //     objects.idcheck_return_ok,
  //     btns,         // 直接传递静态数组
  //     "test",       // 自动转为 const char*
  //     "test"        // 自动转为 const char*
  // );

#endif /*MAIN_H*/