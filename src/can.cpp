/*
* 使用前需要控制CH422G的IO5输出低电平，
* 否则CAN将无法使用
*/
#include "twai.h"  // 包含TWAI(CAN)驱动头文件
#include "main.h"

static bool driver_installed = false;
uint32_t alerts_triggered;
twai_message_t message;

// 独立的CAN初始化函数
bool can_init_bus() {
    const twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    const twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    const twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO_NUM, RX_GPIO_NUM, TWAI_MODE_NORMAL);
    
    DEV_I2C_Init();
    CH422G_init();
    CH422G_io_output(CH422G_IO_6, 1);
    
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        driver_installed = true;
        return (twai_start() == ESP_OK);
    }
    return false;
}

// 独立的CAN读取函数
bool can_read_message(twai_message_t* received_msg) {
    if (!driver_installed) return false;
    
    alerts_triggered = 0;
    if (twai_read_alerts(&alerts_triggered, pdMS_TO_TICKS(10)) == ESP_OK) {
        if (alerts_triggered & TWAI_ALERT_RX_DATA) {
            return (twai_receive(received_msg, pdMS_TO_TICKS(10)) == ESP_OK);
        }
    }
    return false;
}

// 独立的CAN写入函数
bool can_write_message(const twai_message_t* msg_to_send) {
    if (!driver_installed) return false;
    return (twai_transmit(msg_to_send, pdMS_TO_TICKS(10)) == ESP_OK);
}

// 示例用法
void can_setup() {
    if (can_init_bus()) {
        // 初始化测试消息
        message.identifier = 0x6f;
        message.data_length_code = 8;
        for (int i = 0; i < 8; i++) {
            message.data[i] = i;
        }
    }
}

void can_loop() {
    twai_message_t rx_msg;
    if (can_read_message(&rx_msg)) {
        // 处理接收到的消息
        // ...
        
        // 回环测试
        can_write_message(&rx_msg);
    }
}