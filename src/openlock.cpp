#include "main.h"


uint8_t calculate_xor_checksum(uint8_t cmd, uint8_t b_addr, uint8_t l_addr, uint8_t status) {
    return cmd ^ b_addr ^ l_addr ^ status;
}

// 发送开锁指令
bool sendLockCommand(uint8_t b_address, uint8_t l_address) {
  uint8_t frame[5];
  
  // 构建命令帧
  frame[0] = 0x8A;          // 命令码
  frame[1] = b_address;     // 板地址
  frame[2] = l_address;     // 锁地址
  frame[3] = 0x11;          // 状态
  frame[4] = calculate_xor_checksum(frame[0], frame[1], frame[2], frame[3]);// 校验码

  RS485.write(frame, 5);
  Serial.print("开锁指令已发送: ");
  for(int i = 0; i < 5; i++){
    Serial.printf("%02X ", frame[i]);
  }
  Serial.println();
  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 5000) { // 等待5秒超时
    uint8_t available = RS485.available();
    if(available >= 5) {
      uint8_t response[5];
      RS485.readBytes(response, 5);
      if(response[0] == 0x8A && 
         response[1] == b_address && 
         response[2] == l_address){
        Serial.println("返回值：");
        for(size_t i=0; i<5; i++) {
          Serial.printf("%02X ", response[i]);
        }
        Serial.println();
        if(response[3] == 0x11) {
          Serial.println("开锁成功");
          return true; 
        }
        else if(response[3] == 0x00){
          Serial.println("开锁失败");
          return false;
        }
        else{
          Serial.println("未知错误");
          return false;
        }
      }
    }
    delay(10);  
  }
  Serial.println("开锁指令发送失败或超时");
  return false;
}

// Step 7 : 判断是否关锁，查询锁状态
// 查询状态
uint8_t queryDoorStatus(uint8_t b_address, uint8_t l_address) {
    uint8_t frame[5];
  
    // 构建命令帧
    frame[0] = 0x80;          // 命令码
    frame[1] = b_address;     // 板地址
    frame[2] = l_address;     // 锁地址
    frame[3] = 0x33;          // 状态
    frame[4] = calculate_xor_checksum(frame[0], frame[1], frame[2], frame[3]);// 校验码
  
    RS485.write(frame, 5);
    Serial.print("查询指令已发送: ");
    for(int i = 0; i < 5; i++){
      Serial.printf("%02X ", frame[i]);
    }
    Serial.println();

    // 等待并检查响应
    unsigned long startTime = millis();
    while(millis() - startTime < 5000) { // 等待5秒超时
      uint8_t available = RS485.available();
      if(available >= 5) {
        uint8_t response[5];
        RS485.readBytes(response, 5);
        if(response[0] == 0x80 && 
           response[1] == b_address && 
           response[2] == l_address){
          Serial.println("返回值：");
          for(size_t i=0; i<5; i++) {
            Serial.printf("%02X ", response[i]);
          }
          Serial.println();
          if(response[3] == 0x11) {
            Serial.println("打开状态");
            return 1; 
          }
          else if(response[3] == 0x00){
            Serial.println("关闭状态");
            return 0;
          }
          else{
            Serial.println("未知错误");
            return -1;
          }
        }
      }
      delay(10);  
    }
    Serial.println("开锁指令发送失败或超时");
    return -1;
}

void openAllLock(uint8_t b_address) {
  uint8_t frame[5];

  // 构建命令帧
  frame[0] = 0x9D;          // 命令码
  frame[1] = b_address;     // 板地址
  frame[2] = 0x01;          // 锁地址
  frame[3] = 0x10;          // 状态
  frame[4] = calculate_xor_checksum(frame[0], frame[1], frame[2], frame[3]);// 校验码
  RS485.write(frame, 5);
  Serial.print("开锁指令已发送: ");
  for(int i = 0; i < 5; i++){
    Serial.printf("%02X ", frame[i]);
  }
  Serial.println();
  
}

// 通过锁ID直接开锁
bool directOpenLockById(int lockId) {
    uint8_t boardNo = 0;
    uint8_t lockNo = 0;
    
    // 根据锁ID映射板地址和锁地址
    if (lockId >= 1 && lockId <= 24) {
        // 0x02板的1-24号锁
        boardNo = 0x02; // 锁控板
        lockNo = lockId;
    } else if (lockId >= 25 && lockId <= 36) {
        // 0x03板的1-12号锁
        boardNo = 0x03;
        lockNo = lockId - 24;
    } else if (lockId >= 37 && lockId <= 48) { // 帽子
        // 0x01板的1-12号锁
        boardNo = 0x01;
        lockNo = lockId - 36;
    } else if (lockId == 0) {
        // 特殊ID 0 - 全开门锁
        bool allSuccess = true;
        //单独设置内存空间
        openAllLock(0x01);
        delay(5000);
        //单独设置内存空间
        openAllLock(0x02);
        delay(5000);
        //单独设置内存空间
        openAllLock(0x03);
        delay(5000);
        return allSuccess;
    } else {
        Serial.println("无效的锁ID");
        return false;
    }
    Serial.printf("映射结果: 板地址=0x%02X, 锁地址=0x%02X\n", boardNo, lockNo);
    return sendLockCommand(boardNo, lockNo);
}

// 通过锁ID直接读取状态
uint8_t isLockOpen(int lockId) {
    uint8_t boardNo = 0;
    uint8_t lockNo = 0;
    
    // 根据锁ID映射板地址和锁地址
    if (lockId >= 1 && lockId <= 24) {
        // 0x02板的1-24号锁
        boardNo = 0x02;
        lockNo = lockId;
    } else if (lockId >= 25 && lockId <= 36) {
        // 0x03板的1-12号锁
        boardNo = 0x03;
        lockNo = lockId - 24;
    } else if (lockId >= 37 && lockId <= 48) {
        // 0x01板的1-12号锁
        boardNo = 0x01;
        lockNo = lockId - 36;
    }else {
        Serial.println("无效的锁ID");
        return -1;
    }
    Serial.printf("映射结果: 板地址=0x%02X, 锁地址=0x%02X\n", boardNo, lockNo);
    return queryDoorStatus(boardNo, lockNo);
}

void rs485_init() {
    // Initialize 485 device
    RS485.begin(9600, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    while (!RS485) {
      delay(10); // Wait for initialization to succeed
    }
}


