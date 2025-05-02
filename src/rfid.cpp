#include <main.h>

// const char* ssid = "306";
// const char* password = "123456789";
const char* host = "192.168.101.109";
const uint16_t port = 4001;

WiFiClient rfid_client;

uint8_t calculateEPCLength(uint8_t pc_high, uint8_t pc_low) // 计算EPC长度
{
  // 将两个字节合并为16位数值（高位在前）
  uint16_t pc = (pc_high << 8) | pc_low;
  // 提取高5位（bit15~bit11）
  // 右移11位后与0x1F（二进制11111）按位与操作
  uint8_t high5bits = (pc >> 11) & 0x1F;
  // 计算EPC长度：高5位的十进制值 × 2
  return high5bits * 2;
}

uint8_t CKSum(uint8_t *uBuff, uint8_t uBuffLen) // 校验和计算
{
  uint8_t uSum = 0;
  for(uint8_t i=0; i<uBuffLen; i++) {
    uSum = uSum + uBuff[i];
  }
  return (~uSum) + 1;
}

bool cmd_get_firmware_version() //(查询固件版本)
{
  uint8_t packet[] = {
    0xD9,               // Head
    0x04,               // Len (4 bytes after Len)
    0x01, 0x00,         // 485_Add (0x0100)
    0x10,               // Cmd (查询固件版本)
    0x12                // CK (示例值，实际需要计算)
  };

  rfid_client.clear();
  rfid_client.write(packet, sizeof(packet));
  Serial.print("查询固件版本:");
  for(size_t i=0; i<sizeof(packet); i++) {
    Serial.printf("%02X ", packet[i]);
  }
  Serial.println();

  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 5000) { // 等待5秒超时
    int available = rfid_client.available();
    // Serial.printf("available: %d\n", available);
    if(available >= 8) { // 最小响应长度
      if(rfid_client.available() >= 8 && available < 0x36) { // 预期响应8字节
        uint8_t response[8];
        rfid_client.readBytes(response, 8);
        // 检查响应格式: D9 06 0100 10 XX XX XX XX
        if(response[0] == 0xD9 && 
           response[1] == 0x06 &&
           response[2] == 0x01 && 
           response[3] == 0x00 &&
           response[4] == 0x10) {
          Serial.println("查询版本失败:");
          for(size_t i=0; i<8; i++) {
            Serial.printf("%02X ", response[i]);
          }
          Serial.println();
          return false;
        }
      }else if(available >= 0x36) {
        uint8_t response[0x36];
        rfid_client.readBytes(response, 0x36);
        // Serial.println("查询结果:");
        // for(size_t i=0; i<0x36; i++) {
        //   Serial.printf("%02X ", response[i]);
        // }
        // Serial.println();
        // 检查响应格式: D9 34 0100 10 XX XX XX XX
        if(response[0] == 0xD9 &&
           response[1] == 0x34 &&
           response[2] == 0x01 &&
           response[3] == 0x00 &&
           response[4] == 0x10) {
          Serial.print("查询版本成功:");
          for(size_t i=0; i<0x36; i++) {
            Serial.printf("%02X ", response[i]);
          }
          Serial.println();
          return true;
        }
      }
    }
    delay(10);
  }
  Serial.println("查询固件版本超时");
  return false;
}

bool cmd_read_tag(uint8_t ant) //(读指定天线标签)
{
  uint8_t packet[] = {
    0xD9,               // Head
    0x0E,               // Len (13 bytes after Len)
    0x01, 0x00,         // 485_Add (0x0100)
    0x30,               // Cmd (读标签)
    0x00,               // Readmode
    ant,                // 指定天线
    0x21,               // 功率33dBm
    0x00, 0x00, 0x00, 0x00, // 访问密码
    0x01,               // 0x00：RESERVED； 0x01：EPC；0x02：TID； 0x03：USER
    0x00,               // 起始地址
    0x0C                // 读取12个字
  };
  
  uint8_t ck = CKSum(packet, sizeof(packet));

  rfid_client.clear();
  rfid_client.write(packet, sizeof(packet));
  rfid_client.write(ck);
  Serial.print("开始读标签:");
  for(size_t i=0; i<sizeof(packet); i++) {
    Serial.printf("%02X ", packet[i]);
  }
  Serial.println(ck, HEX);

  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 2000) { // 等待1秒超时
    int available = rfid_client.available();
    // Serial.printf("available: %d\n", available);
    if(available >= 8) { // 预期响应8字节
      uint8_t response[8];
      rfid_client.readBytes(response, 8);
      
      // 检查响应格式: D9 06 0100 30 00 00 CK
      if(response[0] == 0xD9 && 
         response[1] == 0x06 &&
         response[2] == 0x01 && 
         response[3] == 0x00 &&
         response[4] == 0x30 &&
         response[5] == 0x00 &&
         response[6] == 0x00 &&
         response[7] == 0xF0) {
        Serial.print("读标签指令发送成功:");
        for(size_t i=0; i<8; i++) {
          Serial.printf("%02X ", response[i]);
        }
        Serial.println();
        return true;
      }
    }
    delay(10);
  }
  Serial.println("读标签指令发送失败或超时");
  return false;
}

bool cmd_inventory_epc() //(盘存标签 EPC 数据)【最常用的读标签数据指令】
{
  uint8_t packet[] = {
    0xD9,               // Head
    0x04,               // Len (4 bytes after Len)
    0x01, 0x00,         // 485_Add (0x0100)
    0x20,               // Cmd (盘存标签)
    0x02                // Flags:0x02(盘存EPC)
  };
  rfid_client.clear();
  rfid_client.write(packet, sizeof(packet));
  Serial.print("开始盘点EPC:");
  for(size_t i=0; i<sizeof(packet); i++) {
    Serial.printf("%02X ", packet[i]);
  }
  Serial.println();

  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 5000) { // 等待1秒超时
    if(rfid_client.available() >= 7) { // 预期响应7字节
      uint8_t response[8];
      rfid_client.readBytes(response, 8);
      
      // 检查响应格式: D9 06 0100 20 00 00 CK
      if(response[0] == 0xD9 && 
         response[1] == 0x06 &&
         response[2] == 0x01 && 
         response[3] == 0x00 &&
         response[4] == 0x20||0x30 &&
         response[5] == 0x00 &&
         response[6] == 0x00 &&
         response[7] == 0x00) {
        Serial.println("盘存指令发送成功:");
        for(size_t i=0; i<8; i++) {
          Serial.printf("%02X ", response[i]);
        }
        Serial.println();
        return true;
      }
    }
    delay(10);
  }
  Serial.println("盘存指令发送失败或超时");
  return false;
}

bool cmd_stop_inventory() //(停止读标签)
{
  uint8_t packet[] = {
    0xD9,               // Head
    0x04,               // Len (4 bytes after Len)
    0x01, 0x00,         // 485_Add (0x0100)
    0x2F,               // Cmd (停止读标签)
    0xF3                // CK
  };
  
  rfid_client.clear();
  rfid_client.write(packet, sizeof(packet));
  Serial.print("停止读标签:");
  for(size_t i=0; i<sizeof(packet); i++) {
    Serial.printf("%02X ", packet[i]);
  }
  Serial.println();

  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 2000) { // 等待2秒超时
    uint8_t available = rfid_client.available();
    // Serial.printf("available: %d\n", available);
    if(available >= 8) { // 预期响应8字节
      uint8_t response[8];
      rfid_client.readBytes(response, 8);
      
      // 检查响应格式
      if(response[0] == 0xD9 && 
         response[1] == 0x06 &&
         response[2] == 0x01 && 
         response[3] == 0x00 &&
         response[4] == 0x2F) { 
        if(response[5] == 0x00 && response[6] == 0x00) { // 成功
          Serial.print("停止读标签成功:");
          for(size_t i=0; i<8; i++) {
            Serial.printf("%02X ", response[i]);
          }
          Serial.println();
          return true;
        } else { // 失败
          Serial.printf("停止读标签错误:");
          Serial.print(response[5], HEX);
          Serial.print(" ");
          Serial.print(response[6], HEX);
          Serial.println();
          for(size_t i=0; i<8; i++) {
            Serial.printf("%02X ", response[i]);
          }
          Serial.println();
          return true;
        }
      }
    }
    delay(10);
  }
  Serial.println("停止读标签超时");
  return false;
}


String read_tag_re() // 处理读取标签数据并返回EPC值
{
  Serial.println("开始读取标签数据");
  if(rfid_client.available()) {
    // Serial.println("有数据");
    uint8_t header = rfid_client.read();
    if(header == 0xD9) {  // 验证包头
      // Serial.print("有包头");
      // Serial.println(header, HEX);
      uint8_t len = rfid_client.read();
      // Serial.print("len:");
      // Serial.println(len, HEX);
      if(rfid_client.available() >= len) { 
        // Serial.println("有数据");
        uint8_t response[len];
        rfid_client.readBytes(response, len);
        // for(size_t i=0; i<len; i++) {
        //   Serial.printf("%02X ", response[i]);
        // }
        // Serial.println();
        // 检查响应格式: 0x0100 0x30 0x01 1Byte 1Byte 2Byte NByte 2Byte 2Byte N Byte 1Byte CK
        if(response[0] == 0x01 && 
           response[1] == 0x00 &&
           response[2] == 0x20||0x30 && 
           response[3] == 0x01) {
          Serial.print("读取到一个标签:");
          Serial.print(header, HEX);
          Serial.print(" ");
          Serial.print(len, HEX);
          Serial.print(" ");
          for(size_t i=0; i<len; i++) {
            Serial.printf("%02X ", response[i]);
          }
          Serial.println();
          // 解析EPC数据
          uint8_t epcLen = calculateEPCLength(response[6], response[7]); 
          // Serial.print("EPC长度:");
          // Serial.println(epcLen);
          uint8_t epc[epcLen];
          for(int i=0; i<epcLen; i++){
            epc[i] = response[8+i]; 
          }
          // Serial.print("EPC HEX:");
          // for(int i=0; i<epcLen; i++){
          //   Serial.printf("%02X ", epc[i]);
          // }
          // Serial.println();
          // 转换为16进制字符串
          String epcStr = "";
          for(int i=0; i<epcLen; i++) {
            char buf[3];
            sprintf(buf, "%02X", epc[i]);
            epcStr += buf;
          }
          //Serial.println("EPC STR: " + epcStr);
          rfid_client.clear();
          return epcStr; // 返回EPC值;
        }
      }
    }
  }
  return ""; // 没有标签数据时返回空字符串
}

void rfid_init() {
  if(rfid_client.connect(host, port)){
    Serial.println("连接读写器成功");
    while(!cmd_stop_inventory()){delay(1000);}
    delay(500);
    while(!cmd_get_firmware_version()){
      Serial.println("读写器初始化失败");
      delay(1000);
    }
    Serial.println("读写器初始化成功");
  }
}

void rfid_loop() {
  if(rfid_client.connected()){
    // 开始盘点标签
    if(cmd_inventory_epc()){
      String epc;
      // 持续读取直到获取到标签
      while(true){
        epc = read_tag_re();
        if(epc != "" && epc.startsWith("03")){  // 读取到有效标签且前两位是01
          Serial.println("读取到有效标签EPC: " + epc);
          break;  // 跳出循环
        }
        delay(500);  // 短暂延迟避免CPU占用过高
      }
      // 停止读标签
      while(!cmd_stop_inventory())
      {
        delay(1000);
      }
    }
  }
  while(true);
}
