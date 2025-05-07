#ifndef DATABASE_H
#define DATABASE_H

#include <main.h>

// 定义物品状态常量
#define ITEM_STATUS_AVAILABLE 0x01    // 可用状态
#define ITEM_STATUS_BORROWED  0x00    // 已借出状态

// 定义格口类型
#define CABINET_TYPE_TRIPLE 1  // 三物品格口
#define CABINET_TYPE_SINGLE 2  // 单物品格口

// 三物品格口结构体
typedef struct {
    uint8_t cabinetId;                // 格口号
    uint8_t itemId1;                  // 物品编号1
    uint8_t itemId2;                  // 物品编号2
    uint8_t itemId3;                  // 物品编号3
    uint32_t userId;                  // 用户ID (六位数字)
    uint8_t itemReserveStatus1;       // 物品1预约状态
    uint8_t itemReserveStatus2;       // 物品2预约状态
    uint8_t itemReserveStatus3;       // 物品3预约状态
    
    uint8_t itemActualStatus1;        // 物品1实际状态
    uint8_t itemActualStatus2;        // 物品2实际状态
    uint8_t itemActualStatus3;        // 物品3实际状态

    uint8_t lockState = 0;            // 0 门锁正常  1 门锁异常
    uint8_t rfidState = 0;            // 0 RFID正常  1 RFID异常
} TripleCabinetItem;

// 单物品格口结构体
typedef struct {
    uint8_t cabinetId;                // 格口号
    uint8_t itemId;                   // 物品编号
    uint32_t userId;                  // 用户ID (六位数字)
    uint8_t itemReserveStatus;        // 物品预约状态
    uint8_t itemActualStatus;         // 物品实际状态  0 没有  1 有
} SingleCabinetItem;

// 数据库类
class ItemDatabase {
public:
    // 初始化数据库
    static void init();
    
    // 根据格口ID获取格口类型
    static uint8_t getCabinetType(uint8_t cabinetId);
    
    // 根据格口ID获取三物品格口信息
    static TripleCabinetItem* getTripleCabinetInfo(uint8_t cabinetId);
    
    // 根据格口ID获取单物品格口信息
    static SingleCabinetItem* getSingleCabinetInfo(uint8_t cabinetId);
    
    // 根据用户ID查找所有关联的格口
    static uint8_t findCabinetsByUserId(uint32_t userId, uint8_t* cabinetIds, uint8_t maxCount);
    
    // 更新物品状态
    static bool updateItemStatus(uint8_t cabinetId, uint8_t itemIndex, uint8_t newStatus);
    
    
    // 打印数据库内容（调试用）
    static void printDatabase();
};

#endif // DATABASE_H