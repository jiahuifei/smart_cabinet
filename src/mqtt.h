#ifndef MQTT_H
#define MQTT_H

#include <main.h>

extern WiFiClient espClient;
extern PubSubClient client; 
extern const char *topic;

void mqtt_init();
void mqtt_loop();
void send_user_auth(const char* correlation_id, const char* auth_type);
void mqtt_publish(char *message);

// 确保声明与定义一致
extern bool item_states_received;
extern bool user_id_received;
extern const char* itemStatusList[4];

#endif // MQTT_H