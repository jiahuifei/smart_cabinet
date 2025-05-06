#include "main.h"

// CRC16 校验码计算函数
uint16_t calculateCRC16(uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    
    for (uint8_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i];
        
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    
    return crc;
}

// 发送报警命令, station_id为站号(默认FF)，cmd_type为命令类型
bool send_alarm_command(uint8_t station_id = 0xFF, AlarmCommandType cmd_type = ALARM_RED_LIGHT_ON) {
    // 构建命令数组
    uint8_t cmd[8];
    
    cmd[0] = station_id;         // 站号，默认为0xFF
    cmd[1] = 0x06;               // 功能码
    cmd[2] = 0x00;               // 寄存器地址高字节
    cmd[3] = 0xC2;               // 寄存器地址低字节
    cmd[4] = 0x00;               // 数据高字节
    cmd[5] = cmd_type;           // 数据低字节（命令类型）
    
    // 计算CRC16校验码
    uint16_t crc = calculateCRC16(cmd, 6);
    cmd[6] = crc & 0xFF;         // CRC低字节
    cmd[7] = (crc >> 8) & 0xFF;  // CRC高字节
    
    // 发送命令到串口
    RS485.write(cmd, 8);

    Serial.print("报警器指令已发送: ");
    for(int i = 0; i < 8; i++){
      Serial.printf("%02X ", cmd[i]);
    }
    Serial.println();
    // 等待并检查响应
    unsigned long startTime = millis();
    while(millis() - startTime < 5000) { // 等待5秒超时
      uint8_t available = RS485.available();
      if(available >= 8) {
        uint8_t response[8];
        RS485.readBytes(response, 8);
        //对比响应值
        if(response[0] == cmd[0] && 
           response[1] == cmd[1] && 
           response[2] == cmd[2] && 
           response[3] == cmd[3] && 
           response[4] == cmd[4] && 
           response[5] == cmd[5]){
            Serial.print("报警器指令响应: ");
            for(int i = 0; i < 8; i++){
              Serial.printf("%02X ", response[i]);
            }
            Serial.println();
            return true;  
        }else{
          Serial.print("报警器指令响应错误: ");
          for(int i = 0; i < 8; i++){
            Serial.printf("%02X ", response[i]);
          }
          Serial.println();
          return false;
        }
      }
      delay(10);  
    }
    Serial.println("开锁指令发送失败或超时");
    return false;
}