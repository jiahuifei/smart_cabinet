#include <main.h>

class CellItem {
public:
    CellItem(int cellID,int doorBoardID, int boardInterfaceID, int rfidBoardID, int rfidInterfaceID);

public:
    // 设置用于读取rfid的实例句柄
    void SetRfHandle(RFIDReader rfid);

    // 刷新对象状态 -- 开机时，需要执行该操作--维护状态
    void Refresh();

    // 读取们的状态
    bool ReadDoor();

    // 读取RFID数据
    void ReadRFID();

    void OpenDoor();

    // 检查设备硬件是否OK
    bool CheckValid(uint8_t retryCount);

    // 检查物资是否匹配
    /*
        flowStatName : 流程状态名
    */
    bool CheckGoodValid(const char * flowStatName);

    // 02 03 04
    void AddType(const char* type);

public:
    // 优先找自己开过的柜子
    bool isMyGood(const char* goodType );


private:
    RFIDReader* rfidHandle = 0;

private:
    // 格子ID
    uint8_t cellID;
    uint8_t doorBoardID;
    uint8_t boardInterfaceID;
    uint8_t rfidBoardID;
    uint8_t rfidInterfaceID;

private:
    bool isDoorOpen = false; // false 关闭， true 开启
    bool isDoorOk = true;

    bool isComminucationOk = true;

    char* goodEPC[3]  = {0,0,0} ;

    // 归还物资时，需要清空
    char* userID;
    char* goodTypes[3] = {0,0,0};
    // 物资实际状态
    uint8_t goodStatus[3] = {0,0,0};
    // 领用状态
    uint8_t goodUsingStatus[3] = {0,0,0};

};