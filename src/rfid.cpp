#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define MAX_ANTENNA 32
#define BUFFER_SIZE 512
#define DEFAULT_485_ADDR 0x0100

typedef enum {
    RFID_OK,
    RFID_ERR_CHECKSUM,
    RFID_ERR_TIMEOUT,
    RFID_ERR_INVALID_RESP,
    RFID_ERR_HARDWARE
} RFIDStatus;

typedef struct {
    uint8_t freq;
    uint8_t ant_id;
    uint16_t pc;
    uint8_t epc[64];
    int16_t rssi;
} EPCRecord;

typedef struct {
    int com_port;
    uint16_t addr;
    uint8_t ant_count;
    uint8_t current_ant;
    uint8_t tx_power[MAX_ANTENNA];
} RFIDReader;

// 校验和计算
uint8_t calculate_checksum(uint8_t *data, uint8_t len) {
    uint8_t sum = 0;
    for(int i=0; i<len; i++) sum += data[i];
    return (~sum) + 1;
}

// 构建基础指令帧
void build_command_frame(uint8_t cmd, uint16_t addr, uint8_t *data, 
                        uint8_t data_len, uint8_t *out, uint8_t *out_len) {
    out[0] = 0xD9;  // Head
    out[1] = 3 + data_len;  // Len
    out[2] = addr & 0xFF;   // 485地址低字节
    out[3] = addr >> 8;     // 485地址高字节  
    out[4] = cmd;
    
    if(data_len > 0) {
        memcpy(&out[5], data, data_len);
    }
    
    // 计算校验和
    uint8_t cksum = calculate_checksum(&out[1], 4 + data_len);
    out[5 + data_len] = cksum;
    
    *out_len = 6 + data_len;
}

// 设置天线参数
RFIDStatus set_antenna_config(RFIDReader *reader, uint8_t ant_mask[]) {
    uint8_t cmd_data[32] = {0};
    
    // 天线选择（4字节）
    memcpy(cmd_data, ant_mask, 4);  
    
    // 功率设置（每个天线1字节）
    for(int i=0; i<reader->ant_count; i++){
        cmd_data[4+i] = reader->tx_power[i];
    }
    
    // 构建完整指令
    uint8_t packet[BUFFER_SIZE];
    uint8_t pkt_len;
    build_command_frame(0x62, reader->addr, cmd_data, 4 + reader->ant_count, 
                       packet, &pkt_len);
    
    // 发送指令并验证响应...
    return RFID_OK;
}

// 单天线盘存
RFIDStatus single_antenna_inventory(RFIDReader *reader, uint8_t ant_id, 
                                   EPCRecord *records, int *record_count) {
    // 设置单天线模式
    uint8_t ant_mask[4] = {0};
    if(ant_id < reader->ant_count) {
        ant_mask[3 - (ant_id/8)] |= 1 << (ant_id%8); // Big-endian格式
    }
    
    set_antenna_config(reader, ant_mask);
    
    // 发送盘存指令
    uint8_t packet[BUFFER_SIZE];
    uint8_t pkt_len;
    build_command_frame(0x20, reader->addr, NULL, 0, packet, &pkt_len);
    
    // 发送数据...
    // 接收并解析响应...
    
    return RFID_OK;
}

// 全部门扫描
RFIDStatus full_cycle_scan(RFIDReader *reader, 
                          void (*callback)(uint8_t ant_id, EPCRecord *record)) {
    for(uint8_t ant=0; ant<reader->ant_count; ant++) {
        EPCRecord records[10];
        int count = 10;
        
        if(single_antenna_inventory(reader, ant, records, &count) == RFID_OK) {
            for(int i=0; i<count; i++) {
                callback(ant, &records[i]);
            }
        }
        
        // 添加适当延时
        //delay(50);
    }
    return RFID_OK;
}

// 初始化读写器
void init_reader(RFIDReader *reader, uint8_t ant_count, uint16_t addr) {
    reader->ant_count = ant_count;
    reader->addr = addr;
    
    // 默认功率设置（示例）
    for(int i=0; i<ant_count; i++) {
        reader->tx_power[i] = 0x1A; // 26dBm
    }
}

// 示例用法
int main() {
    RFIDReader locker32, locker16;
    
    // 初始化32通道读写器
    init_reader(&locker32, 32, 0x0100);
    // 初始化16通道读写器
    init_reader(&locker16, 16, 0x0200);
    
    // 全部门扫描示例
    full_cycle_scan(&locker32, [](uint8_t ant_id, EPCRecord *rec) {
        printf("Locker32 Door%d: EPC:", ant_id+1);
        for(int i=0; i<rec->pc/16; i++) { // 根据PC值计算EPC长度
            printf("%02X ", rec->epc[i]);
        }
        printf(" RSSI:%.1f dBm\n", rec->rssi/10.0);
    });
    
    return 0;
}
