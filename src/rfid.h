#ifndef RFID_H
#define RFID_H

#include <main.h>

// 在文件顶部添加结构体定义
struct TagData {
    uint8_t head;
    uint8_t len;
    uint8_t reserved[2];  // 0x0100
    uint8_t cmd;          // 0x30
    uint8_t flags;        // 0x01
    uint8_t freq;
    uint8_t ant;
    uint8_t pc[2];
    uint8_t epc[32];      // 足够大的缓冲区存储EPC
    uint8_t epcLen;       // EPC实际长度
    uint8_t crc[2];
    uint8_t rssi[2];
    uint8_t data[64];     // 足够大的缓冲区存储数据
    uint8_t dataLen;      // 数据实际长度
    uint8_t ck;
    bool valid;           // 标记数据是否有效
    String epcStr;        // 存储EPC的十六进制字符串
    float rssiValue;      // 计算后的RSSI值
  };
  

class RFIDReader {
private:
  const char* host;     // 读写器IP地址
  uint16_t port;        // 读写器端口
  WiFiClient client;    // WiFi客户端

  // 工具函数
  uint8_t calculateEPCLength(uint8_t pc_high, uint8_t pc_low);
  uint8_t CKSum(uint8_t *uBuff, uint8_t uBuffLen);
  float calculateRSSI(uint8_t rssi_low, uint8_t rssi_high);

public:
  // 构造函数
  RFIDReader(const char* host, uint16_t port);
  
  // 连接和初始化
  bool connect();
  bool init();
  
  // 命令函数
  bool cmd_get_firmware_version();
  bool cmd_read_tag(uint8_t ant);
  bool cmd_inventory_epc();
  bool cmd_stop_inventory();
  
  // 数据处理
  TagData read_tag_data();
  
  // 高级功能
  bool rfid_loop(String epc_id, uint8_t ant);
  
  // 检查连接状态
  bool isConnected() { return client.connected(); }
};

extern RFIDReader rfid1;
extern RFIDReader rfid2;


void rfid_all_init();
void rfid_all_test();

#endif // !RFID_H