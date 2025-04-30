#ifndef OPENLOCK_H
#define OPENLOCK_H

// 模拟 RS485 串口通信
#define RS485 Serial1

// RS485 通信引脚定义
#define RS485_RX_PIN  15
#define RS485_TX_PIN  16

bool openAllLock(uint8_t boardNo, char *rsMsg);
bool openLock(uint8_t boardNo, uint8_t lockNo, char *rsMsg);
bool sendData(uint8_t *data, int size, char *rsMsg);
bool lockCmd(uint8_t cmdNo, uint8_t boardNo, uint8_t lockNo, char *rsMsg);
bool openPower(uint8_t boardNo, uint8_t lockNo, char *rsMsg);
bool closePower(uint8_t boardNo, uint8_t lockNo, char *rsMsg);
bool getState(uint8_t boardNo, uint8_t lockNo, char *rsMsg);
bool getAllState(uint8_t boardNo, char *rsMsg);
bool directOpenLockById(int lockId);
bool directGetLockStateById(int lockId, char *rsMsg);
bool isLockOpen(int lockId);

#endif // !1