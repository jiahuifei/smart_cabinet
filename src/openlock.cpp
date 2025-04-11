#include "main.h"

// 常量定义
#define SUCCESS   "成功"
#define FAIL      "失败"
#define OVERTIME  "超时"
#define EXCEPTION "异常"
#define VALID     "开"    // 修改前为"有效"
#define INVALID   "关"    // 修改前为"无效"

// 工具函数：将字节数组转换为十六进制字符串
void toHexString(uint8_t *data, int length, char *result, size_t buf_size) {
    if (buf_size < (size_t)length * 3) {
        strncpy(result, "BUFFER_TOO_SMALL", buf_size-1);
        return;
    }
    for (int i = 0; i < length; i++) {
        sprintf(result + i * 3, "%02X ", data[i]);
    }
    result[length*3 - 1] = '\0'; // 确保终止符
}

// 工具函数：将十六进制字符串转换为字节数组
int toByteArray(const char *hexString, uint8_t *byteArray) {
    int length = 0;
    for (int i = 0; hexString[i] != '\0'; i += 2) {
        if (hexString[i] != ' ') {
            sscanf(hexString + i, "%2hhx", &byteArray[length++]);
        }
    }
    return length;
}

// 发送数据
bool sendData(uint8_t *data, int size, char *rsMsg) {
    RS485.write(data, size); // 发送数据
    delay(50); // 等待响应

    if (RS485.available()) {
        uint8_t buffer[128];
        int len = RS485.readBytes(buffer, sizeof(buffer));
        // 添加缓冲区长度参数（根据main.cpp中result数组的32字节长度）
        toHexString(buffer, len, rsMsg, 32); // 修改调用方式
        return true;
    } else {
        strcpy(rsMsg, OVERTIME);
        return false;
    }
}

// 锁控板指令
bool lockCmd(uint8_t cmdNo, uint8_t boardNo, uint8_t lockNo, char *rsMsg) {
    // 指令帧结构：指令码 | 板地址 | 锁地址 | 固定值 | 校验码
    uint8_t sendBuffer[5];

    sendBuffer[0] = cmdNo;     // 指令类型（0x8A=开锁，0x80=查询等）
    sendBuffer[1] = boardNo;   // 板地址范围：0x01-0x20（1-32号控制板）
    sendBuffer[2] = lockNo;    // 锁地址范围：0x01-0x18（1-24号锁）
    
    // 问题1：固定值未按协议设置
    // 原代码：sendBuffer[3] = 0x11;
    // 修正后：
    if (cmdNo == 0x80) {      // 读状态指令
        sendBuffer[3] = 0x33; // 协议规定的固定值
    } else if (cmdNo >= 0x9A && cmdNo <= 0x9D) { // 特殊指令
        sendBuffer[3] = 0x10; // 协议规定的固定值
    } else {
        sendBuffer[3] = 0x11; // 普通指令固定值
    }
    
    // 异或校验计算（包含所有前4个字节）
    sendBuffer[4] = sendBuffer[0] ^ sendBuffer[1] ^ sendBuffer[2] ^ sendBuffer[3];
    
    // 发送指令并获取响应
    if (sendData(sendBuffer, 5, rsMsg)) {
        uint8_t response[128];
        int len = toByteArray(rsMsg, response); // 转换响应数据

        // 响应有效性检查（长度5字节且指令码匹配）
        if (len == 5 && response[0] == cmdNo) {
            if (cmdNo == 0x80) { // 状态查询指令处理
                /* 协议响应格式：
                   [0]指令码 [1]板地址 [2]锁地址 [3]状态码 [4]校验码
                   状态码说明：
                   - 0x11: 锁开启/有效
                   - 0x00: 锁关闭/无效 */
                if (response[3] == 0x11) {
                    strcpy(rsMsg, VALID);  // 开锁状态
                } else if (response[3] == 0x00) {
                    strcpy(rsMsg, INVALID);// 关锁状态
                }
                return true; // 只要收到响应即视为成功
            } else {// 控制类指令处理
                /* 协议响应格式：
                   [0]指令码 [1]板地址 [2]锁地址 [3]操作结果 [4]校验码
                   操作结果：
                   - 0x11: 操作成功
                   - 其他: 操作失败 */
                strcpy(rsMsg, (response[3] == 0x11) ? SUCCESS : FAIL);
                return response[3] == 0x11;
            }
        } else {
            strcpy(rsMsg, FAIL);
        }
    } else {
        strcpy(rsMsg, OVERTIME);
    }
    return false;
}


// 1. 单个开锁（协议指令0x8A）
// 参数：boardNo[IN] 板地址(0x01-0x20) 
//       lockNo[IN]  锁地址(0x01-0x25)
//       rsMsg[OUT]  返回消息缓冲区（至少32字节）
// 返回：true-操作成功 / false-操作失败
bool openLock(uint8_t boardNo, uint8_t lockNo, char *rsMsg) {
    return lockCmd(0x8A, boardNo, lockNo, rsMsg);
}

// 2. 全开测试（协议指令0x9D）
// 参数：boardNo[IN] 板地址(0x01-0x20)
//       rsMsg[OUT]  返回消息缓冲区
// 返回：true-操作成功 / false-操作失败
bool openAllLock(uint8_t boardNo, char *rsMsg) {
    
    return lockCmd(0x9D, boardNo, 0x02, rsMsg);
}

// 3. 通电保持（协议指令0x9A）
// 参数：boardNo[IN] 板地址 
//       lockNo[IN]  锁地址
//       rsMsg[OUT]  返回消息
// 返回：true-操作成功 / false-操作失败
bool openPower(uint8_t boardNo, uint8_t lockNo, char *rsMsg) {
    return lockCmd(0x9A, boardNo, lockNo, rsMsg);
}

// 4. 断电关闭（协议指令0x9B）
// 参数：boardNo[IN] 板地址
//       lockNo[IN]  锁地址
//       rsMsg[OUT]  返回消息
// 返回：true-操作成功 / false-操作失败
bool closePower(uint8_t boardNo, uint8_t lockNo, char *rsMsg) {
    return lockCmd(0x9B, boardNo, lockNo, rsMsg);
}

// 5. 查询单个状态（协议指令0x80）
// 参数：boardNo[IN] 板地址
//       lockNo[IN]  锁地址
//       rsMsg[OUT]  返回状态消息（VALID/INVALID）
// 返回：true-获取成功 / false-获取失败
bool getState(uint8_t boardNo, uint8_t lockNo, char *rsMsg) {
    return lockCmd(0x80, boardNo, lockNo, rsMsg);
}

// 6. 查询全板状态（协议指令0x80特殊地址）
// 参数：boardNo[IN] 板地址
//       rsMsg[OUT]  返回状态消息（需要解析多锁状态）
// 返回：true-获取成功 / false-获取失败
bool getAllState(uint8_t boardNo, char *rsMsg) {
    return lockCmd(0x80, boardNo, 0x00, rsMsg);
}



// 通过锁ID直接开锁
// 参数：lockId - 锁ID(0-48)
// 返回：true-操作成功 / false-操作失败
bool directOpenLockById(int lockId) {
    char rsMsg[32]; // 内部处理结果缓冲区
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
    } else if (lockId == 0) {
        // 特殊ID 0 - 全开门锁
        bool allSuccess = true;
        for (uint8_t board = 0x01; board <= 0x03; board++) {
            if (!openAllLock(board, rsMsg)) {
                allSuccess = false;
            }
        }
        return allSuccess;
    } else {
        return false;
    }
    
    return openLock(boardNo, lockNo, rsMsg);
}

