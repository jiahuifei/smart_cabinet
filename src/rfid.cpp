// #include <driver/twai.h>
// #include <esp_log.h>
// #include <main.h>

// #define RFID_CAN_ID 0x0100
// #define CMD_INVENTORY_EPC 0x20
// #define CMD_STOP_INVENTORY 0x2F
// #define CMD_RESET 0x40

// static const char* TAG = "RFID";

// // RFID指令结构体
// typedef struct {
//     uint8_t head;      // 固定头 0xD9
//     uint8_t len;       // 数据长度
//     uint16_t address;  // 485地址
//     uint8_t cmd;       // 指令码
//     uint8_t ck;        // 校验和
// } __attribute__((packed)) RfidCommand;

// // RFID响应结构体
// typedef struct {
//     uint8_t head;       // 固定头 0xD9
//     uint8_t len;       // 数据长度
//     uint16_t reserved; // 保留字段
//     uint8_t cmd;       // 指令码
//     uint8_t flags;     // 状态标志
//     uint8_t data[];    // 可变长度数据
// } __attribute__((packed)) RfidResponse;

// // 初始化RFID读写器
// bool rfid_init() {
//     // 发送复位指令
//     RfidCommand reset_cmd = {
//         .head = 0xD9,
//         .len = 0x04,
//         .address = RFID_CAN_ID,
//         .cmd = CMD_RESET,
//         .ck = 0xE2
//     };

//     if (!can_write_message((twai_message_t*)&reset_cmd, sizeof(reset_cmd))) {
//         ESP_LOGE(TAG, "Failed to send reset command");
//         return false;
//     }

//     // 等待复位响应
//     twai_message_t response;
//     if (!can_read_message(&response, pdMS_TO_TICKS(1000))) {
//         ESP_LOGE(TAG, "No response to reset command");
//         return false;
//     }

//     RfidResponse* resp = (RfidResponse*)response.data;
//     if (resp->cmd != CMD_RESET || resp->len != 0x06) {
//         ESP_LOGE(TAG, "Invalid reset response");
//         return false;
//     }

//     ESP_LOGI(TAG, "RFID reader initialized successfully");
//     return true;
// }

// // 开始盘存标签
// bool rfid_start_inventory() {
//     RfidCommand inventory_cmd = {
//         .head = 0xD9,
//         .len = 0x04,
//         .address = RFID_CAN_ID,
//         .cmd = CMD_INVENTORY_EPC,
//         .ck = 0x02
//     };

//     if (!can_write_message((twai_message_t*)&inventory_cmd, sizeof(inventory_cmd))) {
//         ESP_LOGE(TAG, "Failed to send inventory command");
//         return false;
//     }

//     // 等待开始响应
//     twai_message_t response;
//     if (!can_read_message(&response, pdMS_TO_TICKS(500))) {
//         ESP_LOGE(TAG, "No response to inventory command");
//         return false;
//     }

//     RfidResponse* resp = (RfidResponse*)response.data;
//     if (resp->cmd != CMD_INVENTORY_EPC || resp->flags != 0x00) {
//         ESP_LOGE(TAG, "Invalid inventory start response");
//         return false;
//     }

//     ESP_LOGI(TAG, "RFID inventory started");
//     return true;
// }

// // 停止盘存标签
// bool rfid_stop_inventory() {
//     RfidCommand stop_cmd = {
//         .head = 0xD9,
//         .len = 0x04,
//         .address = RFID_CAN_ID,
//         .cmd = CMD_STOP_INVENTORY,
//         .ck = 0xF3
//     };

//     if (!can_write_message((twai_message_t*)&stop_cmd, sizeof(stop_cmd))) {
//         ESP_LOGE(TAG, "Failed to send stop command");
//         return false;
//     }

//     // 等待停止响应
//     twai_message_t response;
//     if (!can_read_message(&response, pdMS_TO_TICKS(500))) {
//         ESP_LOGE(TAG, "No response to stop command");
//         return false;
//     }

//     RfidResponse* resp = (RfidResponse*)response.data;
//     if (resp->cmd != CMD_STOP_INVENTORY) {
//         ESP_LOGE(TAG, "Invalid stop response");
//         return false;
//     }

//     ESP_LOGI(TAG, "RFID inventory stopped");
//     return true;
// }

// // 处理标签数据回调
// typedef void (*RfidTagCallback)(uint8_t* epc, uint16_t epc_len, uint16_t rssi);

// // 读取标签数据
// void rfid_read_tags(RfidTagCallback callback) {
//     twai_message_t msg;
//     while (can_read_message(&msg, pdMS_TO_TICKS(100))) {
//         RfidResponse* resp = (RfidResponse*)msg.data;
        
//         if (resp->head != 0xD9 || resp->cmd != CMD_INVENTORY_EPC) {
//             continue;
//         }

//         // 处理标签数据
//         if (resp->flags == 0x01) {
//             uint8_t* data_ptr = resp->data;
//             uint8_t freq = data_ptr[0];    // 频点参数
//             uint8_t ant = data_ptr[1];    // 天线号
//             uint16_t pc = *(uint16_t*)(data_ptr + 2); // PC值
//             uint8_t* epc = data_ptr + 4;   // EPC数据
//             uint16_t epc_len = resp->len - 10; // 计算EPC长度
//             uint16_t crc = *(uint16_t*)(data_ptr + 4 + epc_len); // CRC
//             uint16_t rssi = *(uint16_t*)(data_ptr + 6 + epc_len); // RSSI
            
//             if (callback) {
//                 callback(epc, epc_len, rssi);
//             }
//         }
//         // 处理停止信号
//         else if (resp->flags == 0x02) {
//             ESP_LOGI(TAG, "RFID inventory auto stopped");
//             break;
//         }
//     }
// }

