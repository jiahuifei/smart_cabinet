#ifndef CAN_H
#define CAN_H

#include "twai.h"  // 包含TWAI(CAN)驱动头文件

bool can_init_bus();
bool can_write_message(const twai_message_t* msg_to_send);
bool can_read_message(twai_message_t* received_msg);

#endif // !CAN_H

