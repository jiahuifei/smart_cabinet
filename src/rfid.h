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
  };
  

void rfid_init();
bool rfid_loop(String epc_id,uint8_t ant);
TagData read_tag_data();
bool cmd_stop_inventory();
bool cmd_inventory_epc();
bool cmd_read_tag(uint8_t ant);
bool cmd_get_firmware_version();

#endif // !RFID_H