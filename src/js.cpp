#include <main.h>

void js_publish_id(const char *id, const char *state)
{
  // 创建一个静态 JSON 文档，容量为 300 字节
  StaticJsonDocument<300> JSONbuffer;
  // 创建一个 JSON 对象
  JsonObject JSONencoder = JSONbuffer.to<JsonObject>();

  // 添加键值对到 JSON 对象中
  JSONencoder["device"] = "CP_1";
  JSONencoder["state"] = state;
  JSONencoder["user_id"] = id;

  // 创建一个字符缓冲区来存储序列化的 JSON 消息
  char JSONmessageBuffer[100];
  // 将 JSON 对象序列化到字符缓冲区中
  serializeJson(JSONencoder, JSONmessageBuffer, sizeof(JSONmessageBuffer));
  // 打印发送消息到 MQTT 主题的日志
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);

  // 发布消息到 MQTT 主题，并检查是否成功
  if (client.publish(topic, JSONmessageBuffer))
  {
    Serial.println("Success sending message");
  }
  else
  {
    Serial.println("Error sending message");
  }
}

void js_publish_finish(char *id)
{
  // 创建一个静态 JSON 文档，容量为 300 字节
  StaticJsonDocument<300> JSONbuffer;
  // 创建一个 JSON 对象
  JsonObject JSONencoder = JSONbuffer.to<JsonObject>();

  // 添加键值对到 JSON 对象中
  JSONencoder["device"] = "CP_1";
  // 创建一个嵌套的 JSON 数组
  JsonArray user_id = JSONencoder.createNestedArray("user_id");

  // 向 JSON 数组中添加值
  user_id.add(id);

  // 创建一个字符缓冲区来存储序列化的 JSON 消息
  char JSONmessageBuffer[100];
  // 将 JSON 对象序列化到字符缓冲区中
  serializeJson(JSONencoder, JSONmessageBuffer, sizeof(JSONmessageBuffer));
  // 打印发送消息到 MQTT 主题的日志
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);

  // 发布消息到 MQTT 主题，并检查是否成功
  if (client.publish(topic, JSONmessageBuffer))
  {
    Serial.println("Success sending message");
  }
  else
  {
    Serial.println("Error sending message");
  }
}