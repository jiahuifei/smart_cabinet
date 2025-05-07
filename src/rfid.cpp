#include <main.h>

// 定义两个RFID读写器
RFIDReader rfid1("192.168.101.109", 4001);
RFIDReader rfid2("192.168.101.110", 4001); // 第二个读写器的IP和端口

// 构造函数
RFIDReader::RFIDReader(const char* host, uint16_t port) {
  this->host = host;
  this->port = port;
}

uint8_t RFIDReader::calculateEPCLength(uint8_t pc_high, uint8_t pc_low) // 计算EPC长度
{
  // 将两个字节合并为16位数值（高位在前）
  uint16_t pc = (pc_high << 8) | pc_low;
  // 提取高5位（bit15~bit11）
  // 右移11位后与0x1F（二进制11111）按位与操作
  uint8_t high5bits = (pc >> 11) & 0x1F;
  // 计算EPC长度：高5位的十进制值 × 2
  return high5bits * 2;
}

uint8_t RFIDReader::CKSum(uint8_t *uBuff, uint8_t uBuffLen) // 校验和计算
{
  uint8_t uSum = 0;
  for(uint8_t i=0; i<uBuffLen; i++) {
    uSum = uSum + uBuff[i];
  }
  return (~uSum) + 1;
}

float RFIDReader::calculateRSSI(uint8_t rssi_low, uint8_t rssi_high) // 计算RSSI值
{
  // 高低字节相反，变为高字节在前
  uint16_t rssi_value = (rssi_high << 8) | rssi_low;
  
  // 转换为有符号整数
  int16_t signed_rssi = rssi_value;
  
  // 除以10得到实际的RSSI值
  float real_rssi = signed_rssi / 10.0f;
  
  return real_rssi;
}

bool RFIDReader::connect() {
  return client.connect(host, port);
}

bool RFIDReader::init() {//初始化读写器,client.connected()
  if(client.connected()){
    Serial.println("连接读写器成功");
    while(!cmd_stop_inventory()){delay(1000);}
    delay(500);
    while(!cmd_get_firmware_version()){
      Serial.println("读写器初始化失败");
      delay(1000);
    }
    Serial.println("读写器初始化成功");
    return true;
  }
  Serial.println("连接读写器失败");
  return false;
}

bool RFIDReader::cmd_get_firmware_version() //(查询固件版本)
{
  uint8_t packet[] = {
    0xD9,               // Head
    0x04,               // Len (4 bytes after Len)
    0x01, 0x00,         // 485_Add (0x0100)
    0x10,               // Cmd (查询固件版本)
    0x12                // CK (示例值，实际需要计算)
  };

  client.clear();
  client.write(packet, sizeof(packet));
  Serial.print("查询固件版本:");
  for(size_t i=0; i<sizeof(packet); i++) {
    Serial.printf("%02X ", packet[i]);
  }
  Serial.println();

  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 5000) { // 等待5秒超时
    int available = client.available();
    // Serial.printf("available: %d\n", available);
    if(available >= 8) { // 最小响应长度
      if(client.available() >= 8 && available < 0x36) { // 预期响应8字节
        uint8_t response[8];
        client.readBytes(response, 8);
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
        client.readBytes(response, 0x36);
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

bool RFIDReader::cmd_read_tag(uint8_t ant) //(读指定天线标签)
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

  client.clear();
  client.write(packet, sizeof(packet));
  client.write(ck);
  Serial.print("开始读标签:");
  for(size_t i=0; i<sizeof(packet); i++) {
    Serial.printf("%02X ", packet[i]);
  }
  Serial.println(ck, HEX);

  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 2000) { // 等待1秒超时
    int available = client.available();
    if(available >= 8) { // 预期响应8字节
      uint8_t response[8];
      client.readBytes(response, 8);
      
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

bool RFIDReader::cmd_inventory_epc() //(盘存标签 EPC 数据)【最常用的读标签数据指令】
{
  uint8_t packet[] = {
    0xD9,               // Head
    0x04,               // Len (4 bytes after Len)
    0x01, 0x00,         // 485_Add (0x0100)
    0x20,               // Cmd (盘存标签)
    0x02                // Flags:0x02(盘存EPC)
  };
  client.clear();
  client.write(packet, sizeof(packet));
  Serial.print("开始盘点EPC:");
  for(size_t i=0; i<sizeof(packet); i++) {
    Serial.printf("%02X ", packet[i]);
  }
  Serial.println();

  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 5000) { // 等待1秒超时
    if(client.available() >= 7) { // 预期响应7字节
      uint8_t response[8];
      client.readBytes(response, 8);
      
      // 检查响应格式: D9 06 0100 20 00 00 CK
      if(response[0] == 0xD9 && 
         response[1] == 0x06 &&
         response[2] == 0x01 && 
         response[3] == 0x00 &&
         (response[4] == 0x20 || response[4] == 0x30) &&
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

bool RFIDReader::cmd_stop_inventory() //(停止读标签)
{
  uint8_t packet[] = {
    0xD9,               // Head
    0x04,               // Len (4 bytes after Len)
    0x01, 0x00,         // 485_Add (0x0100)
    0x2F,               // Cmd (停止读标签)
    0xF3                // CK
  };
  
  client.clear();
  client.write(packet, sizeof(packet));
  Serial.print("停止读标签:");
  for(size_t i=0; i<sizeof(packet); i++) {
    Serial.printf("%02X ", packet[i]);
  }
  Serial.println();

  // 等待并检查响应
  unsigned long startTime = millis();
  while(millis() - startTime < 2000) { // 等待2秒超时
    uint8_t available = client.available();
    if(available >= 8) { // 预期响应8字节
      uint8_t response[8];
      client.readBytes(response, 8);
      
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

TagData RFIDReader::read_tag_data() // 处理读取标签数据并返回标签结构体
{
  TagData tag = {0};  // 初始化结构体
  tag.valid = false;  // 默认数据无效
  
  Serial.println("开始读取标签数据");
  if(client.available()) {
    tag.head = client.read();
    if(tag.head == 0xD9) {  // 验证包头
      tag.len = client.read();
      
      if(client.available() >= tag.len) { 
        uint8_t response[tag.len];
        client.readBytes(response, tag.len);
        
        // 检查响应格式
        if(response[0] == 0x01 && 
           response[1] == 0x00 &&
           (response[2] == 0x20 || response[2] == 0x30) && 
           response[3] == 0x01) {
          
          Serial.print("读取到一个标签:");
          Serial.print(tag.head, HEX);
          Serial.print(" ");
          Serial.print(tag.len, HEX);
          Serial.print(" ");
          for(size_t i=0; i<tag.len; i++) {
            Serial.printf("%02X ", response[i]);
          }
          Serial.println();
          
          // 复制数据到结构体
          memcpy(tag.reserved, &response[0], 2);  // 0x0100
          tag.cmd = response[2];                  // 0x20 或 0x30
          tag.flags = response[3];                // 0x01
          tag.freq = response[4];
          tag.ant = response[5];
          
          // 复制PC值
          memcpy(tag.pc, &response[6], 2);
          
          // 计算并复制EPC
          tag.epcLen = calculateEPCLength(response[6], response[7]);
          memcpy(tag.epc, &response[8], tag.epcLen);
          
          // 复制CRC和RSSI
          int offset = 8 + tag.epcLen;//计算crc和rssi的偏移量
          if(offset + 4 <= tag.len) {
            memcpy(tag.crc, &response[offset], 2);
            memcpy(tag.rssi, &response[offset+2], 2);
            // 计算实际RSSI值
            tag.rssiValue = calculateRSSI(tag.rssi[0], tag.rssi[1]);
            
            // 如果还有数据字段
            if(offset + 4 < tag.len - 1) {  // 减1是为了排除CK
              tag.dataLen = (tag.len - (offset + 4 + 1)) / 2;  // 数据长度 = (总长 - 前面字段 - CK) / 2
              if(tag.dataLen > 0) {
                memcpy(tag.data, &response[offset+4], tag.dataLen * 2);
              }
            }
          }
          
          // 最后一个字节是CK
          tag.ck = response[tag.len-1];
          
          // 转换EPC为十六进制字符串
          tag.epcStr = "";
          for(int i=0; i<tag.epcLen; i++) {
            char buf[3];
            sprintf(buf, "%02X", tag.epc[i]);
            tag.epcStr += buf;
          }
          
          tag.valid = true;  // 标记数据有效
          client.clear();
        }
      }
    }
  }
  return tag; // 返回标签结构体
}

/*
  EPC: 数据库唯一标识码
  ANT: 天线号
*/
bool RFIDReader::rfid_loop(String epc_id, uint8_t ant) {//demo函数
  if(client.connected()){
    // 开始盘点标签
    if(cmd_read_tag(ant)){
      TagData tag; // 修改为TagData类型
      String epc; // 用于存储EPC字符串
      unsigned long startTime = millis(); // 记录开始时间
      const unsigned long timeout = 5000; // 设置超时时间为5秒

      // 持续读取直到获取到标签或超时
      while(true){
        tag = read_tag_data();
        epc = tag.epcStr; // 获取EPC字符串
        if(epc != "" && epc.startsWith(epc_id)){  // 读取到有效标签且前缀匹配
          Serial.println("读取到有效标签EPC: " + epc);
          break;  // 跳出循环
        }
        if (millis() - startTime > timeout) { // 检查是否超时
          Serial.println("读取标签超时");
          return false; // 返回失败状态
        }
        delay(500);  // 短暂延迟避免CPU占用过高
      }
      // 停止读标签
      while(!cmd_stop_inventory())
      {
        delay(1000);
      }
      return true;
    }
    return false; // 如果cmd_read_tag失败，返回false
  }
  else{
    Serial.println("连接读写器失败");
    return false;
  }
}


void rfid_all_init() {

  // 初始化第一个读写器
  if(rfid1.connect()) {
    if(rfid1.init()) {
      Serial.println("RFID读写器1初始化成功");
    } else {
      Serial.println("RFID读写器1初始化失败");
    }
  } else {
    Serial.println("RFID读写器1连接失败");
  }
  
  // 初始化第二个读写器
  if(rfid2.connect()) {
    if(rfid2.init()) {
      Serial.println("RFID读写器2初始化成功");
    } else {
      Serial.println("RFID读写器2初始化失败");
    }
  } else {
    Serial.println("RFID读写器2连接失败");
  }
}

void rfid_all_test() {
  // 使用第一个读写器读取标签
  if(rfid1.isConnected()) {
    if(rfid1.rfid_loop("01", 1)) { // 查找以"01"开头的标签，使用天线1
      Serial.println("RFID读写器1成功读取标签");
    }
  }
  
  delay(1000);
  
  // 使用第二个读写器读取标签
  if(rfid2.isConnected()) {
    if(rfid2.rfid_loop("02", 1)) { // 查找以"02"开头的标签，使用天线1
      Serial.println("RFID读写器2成功读取标签");
    }
  }
  

}