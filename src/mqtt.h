#ifndef MQTT_H
#define MQTT_H

#include <main.h>

extern WiFiClient espClient;
extern PubSubClient client; 
extern const char *topic;

void mqtt_initialize();
void callback(char *topic, byte *payload, unsigned int length);
void mqttloop();
void mqtt_publish(char *message);
void button_check_send(lv_obj_t *button, char *message);
void button_check_msgbox(lv_obj_t *button, const char* btns[] = nullptr, const  char* msgtitle = "", const char* msgtext = "");

// 全局变量或函数指针
extern bool user_id_received;
extern bool item_states_received;


#endif // MQTT_H