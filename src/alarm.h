#ifndef ALARM_H
#define ALARM_H

#include "main.h"

// 报警命令类型枚举
typedef enum {
    ALARM_RED_LIGHT_ON = 0x11,       // 红灯常亮、报警常开
    ALARM_YELLOW_LIGHT_ON = 0x12,    // 黄灯常亮
    ALARM_GREEN_LIGHT_ON = 0x13,     // 绿灯常亮
    ALARM_RED_LIGHT_SLOW = 0x21,     // 红灯慢闪、报警慢闪
    ALARM_YELLOW_LIGHT_SLOW = 0x22,  // 黄灯慢闪
    ALARM_GREEN_LIGHT_SLOW = 0x23,   // 绿灯慢闪
    ALARM_RED_LIGHT_FAST = 0x31,     // 红灯快闪、报警快闪
    ALARM_YELLOW_LIGHT_FAST = 0x32,  // 黄灯快闪
    ALARM_GREEN_LIGHT_FAST = 0x33,   // 绿灯快闪
    ALARM_LIGHT_OFF = 0x60,          // 报警灯关闭
    ALARM_ON = 0x40,                 // 报警开
    ALARM_OFF = 0x41,                // 报警关
    ALARM_GET_STATION = 0x42         // 查看当前站号
} AlarmCommandType;

bool send_alarm_command(uint8_t station_id = 0xFF, AlarmCommandType cmd_type = ALARM_RED_LIGHT_ON);

#endif // !ALARM_H
