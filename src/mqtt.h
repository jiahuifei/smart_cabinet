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
void send_user_auth(const char* phone_suffix, const char* auth_type);
// 全局变量或函数指针
extern bool user_id_received;
extern bool item_states_received;
void update_ui(const char* user_id, const char* reservation_id, uint8_t primary_door, uint8_t secondary_door, const char* valid_until);
void handle_auth_error(int error_code, const char* error_detail);

// 添加UI更新函数声明
void update_ui(uint8_t user_id, uint8_t door_num, const char* valid_until);
// 添加认证错误处理函数声明
void handle_auth_error(int error_code);

#endif // MQTT_H