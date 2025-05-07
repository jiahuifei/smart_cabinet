#include "main.h"
#include "database.h"

/**
 * 三物品格口数据库 (36个格口)
 * 每个格口包含:
 * - 格口ID: 1-36
 * - 三个物品编号: 0x01, 0x02, 0x03 (默认值)
 * - 用户ID: 六位数字，0表示未分配
 * - 三个物品预约状态: 0x01表示可用，0x00表示已预约
 * - 三个物品实际状态: 0x01表示在柜，0x00表示不在柜
 */
static TripleCabinetItem tripleCabinets[36] = {
    {1,  0x01, 0x02, 0x03, 156484, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}, // 格口1，已分配给用户156484
    {2,  0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01}, // 格口2，未分配用户
    {3,  0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01}, // 格口3，未分配用户
    {4,  0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {5,  0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {6,  0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {7,  0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {8,  0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {9,  0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {10, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {11, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {12, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {13, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {14, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {15, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {16, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {17, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {18, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {19, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {20, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {21, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {22, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {23, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {24, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {25, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {26, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {27, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {28, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {29, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {30, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {31, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {32, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {33, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {34, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01}, // 格口34，未分配用户
    {35, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01}, // 格口35，未分配用户
    {36, 0x01, 0x02, 0x03, 0,      0x01, 0x01, 0x01, 0x01, 0x01, 0x01}  // 格口36，未分配用户
};

/**
 * 单物品格口数据库 (12个格口)
 * 每个格口包含:
 * - 格口ID: 37-48
 * - 物品编号: 0x04 (默认值)
 * - 用户ID: 六位数字，0表示未分配
 * - 物品预约状态: 0x01表示可用，0x00表示已预约
 * - 物品实际状态: 0x01表示在柜，0x00表示不在柜
 */
static SingleCabinetItem singleCabinets[12] = {
    {37, 0x04, 156484, 0x01, 0x01}, // 格口37，已分配给用户156484
    {38, 0x04, 0,      0x01, 0x01}, // 格口38，未分配用户
    {39, 0x04, 0,      0x01, 0x01}, // 格口39，未分配用户
    {40, 0x04, 0,      0x01, 0x01},
    {41, 0x04, 0,      0x01, 0x01},
    {42, 0x04, 0,      0x01, 0x01},
    {43, 0x04, 0,      0x01, 0x01},
    {44, 0x04, 0,      0x01, 0x01},
    {45, 0x04, 0,      0x01, 0x01},
    {46, 0x04, 0,      0x01, 0x01}, // 格口46，未分配用户
    {47, 0x04, 0,      0x01, 0x01}, // 格口47，未分配用户
    {48, 0x04, 0,      0x01, 0x01}  // 格口48，未分配用户
};

/**
 * @brief 初始化物品数据库
 * 
 * 该函数用于初始化物品数据库，但由于数据已经在静态初始化中完成，
 * 因此只输出初始化完成的日志信息。
 */
void ItemDatabase::init() {
    // 数据已经在静态初始化中完成
    Serial.println("物品数据库初始化完成");
}

/**
 * @brief 根据格口ID获取格口类型
 * 
 * @param cabinetId 格口ID (1-48)
 * @return uint8_t 格口类型:
 *         - CABINET_TYPE_TRIPLE (1): 三物品格口 (ID: 1-36)
 *         - CABINET_TYPE_SINGLE (2): 单物品格口 (ID: 37-48)
 *         - 0: 无效格口ID
 */
uint8_t ItemDatabase::getCabinetType(uint8_t cabinetId) {
    if (cabinetId >= 1 && cabinetId <= 36) {
        return CABINET_TYPE_TRIPLE; // 三物品格口
    } else if (cabinetId >= 37 && cabinetId <= 48) {
        return CABINET_TYPE_SINGLE; // 单物品格口
    } else {
        return 0; // 无效格口ID
    }
}

/**
 * @brief 根据格口ID获取三物品格口信息
 * 
 * @param cabinetId 格口ID (1-36)
 * @return TripleCabinetItem* 格口信息指针，无效ID返回NULL
 */
TripleCabinetItem* ItemDatabase::getTripleCabinetInfo(uint8_t cabinetId) {
    if (cabinetId >= 1 && cabinetId <= 36) {
        return &tripleCabinets[cabinetId - 1]; // 数组索引从0开始，格口ID从1开始
    }
    return NULL; // 无效格口ID
}

/**
 * @brief 根据格口ID获取单物品格口信息
 * 
 * @param cabinetId 格口ID (37-48)
 * @return SingleCabinetItem* 格口信息指针，无效ID返回NULL
 */
SingleCabinetItem* ItemDatabase::getSingleCabinetInfo(uint8_t cabinetId) {
    if (cabinetId >= 37 && cabinetId <= 48) {
        return &singleCabinets[cabinetId - 37]; // 数组索引从0开始，格口ID从37开始
    }
    return NULL; // 无效格口ID
}

/**
 * @brief 根据用户ID查找所有关联的格口
 * 
 * 该函数查找与指定用户ID关联的所有格口，支持一个用户关联多个格口的情况。
 * 
 * @param userId 用户ID (六位数字)
 * @param cabinetIds 输出参数，存储找到的格口ID数组
 * @param maxCount 最大可存储的格口ID数量
 * @return uint8_t 找到的格口数量
 */
uint8_t ItemDatabase::findCabinetsByUserId(uint32_t userId, uint8_t* cabinetIds, uint8_t maxCount) {
    uint8_t count = 0;
    
    // 检查参数有效性
    if (cabinetIds == NULL || maxCount == 0) {
        return 0;
    }
    
    // 在三物品格口中查找
    for (int i = 0; i < 36 && count < maxCount; i++) {
        if (tripleCabinets[i].userId == userId) {
            cabinetIds[count++] = tripleCabinets[i].cabinetId; // 添加到结果数组
        }
    }
    
    // 在单物品格口中查找
    for (int i = 0; i < 12 && count < maxCount; i++) {
        if (singleCabinets[i].userId == userId) {
            cabinetIds[count++] = singleCabinets[i].cabinetId; // 添加到结果数组
        }
    }
    
    return count; // 返回找到的格口数量
}

/**
 * @brief 更新物品状态
 * 
 * 根据格口类型和物品索引更新物品的实际状态。
 * 
 * @param cabinetId 格口ID (1-48)
 * @param itemIndex 物品索引 (三物品格口: 1-3, 单物品格口: 1)
 * @param newStatus 新状态值 (0x01: 在柜, 0x00: 不在柜)
 * @return bool 更新是否成功
 */
bool ItemDatabase::updateItemStatus(uint8_t cabinetId, uint8_t itemIndex, uint8_t newStatus) {
    uint8_t cabinetType = getCabinetType(cabinetId);
    
    if (cabinetType == CABINET_TYPE_TRIPLE) {
        // 三物品格口处理
        TripleCabinetItem* cabinet = getTripleCabinetInfo(cabinetId);
        if (cabinet == NULL) return false; // 无效格口ID
        
        switch (itemIndex) {
            case 1:
                cabinet->itemActualStatus1 = newStatus; // 更新物品1状态
                break;
            case 2:
                cabinet->itemActualStatus2 = newStatus; // 更新物品2状态
                break;
            case 3:
                cabinet->itemActualStatus3 = newStatus; // 更新物品3状态
                break;
            default:
                return false; // 无效物品索引
        }
        return true; // 更新成功
    } 
    else if (cabinetType == CABINET_TYPE_SINGLE) {
        // 单物品格口处理
        SingleCabinetItem* cabinet = getSingleCabinetInfo(cabinetId);
        if (cabinet == NULL) return false; // 无效格口ID
        
        if (itemIndex == 1) {
            cabinet->itemActualStatus = newStatus; // 更新物品状态
            return true; // 更新成功
        }
    }
    
    return false; // 更新失败
}


/**
 * @brief 打印数据库内容（调试用）
 * 
 * 该函数将数据库中的所有格口信息打印到串口，用于调试。
 * 分别打印三物品格口和单物品格口的信息。
 */
void ItemDatabase::printDatabase() {
    // 打印三物品格口数据
    Serial.println("=== 三物品格口数据 ===");
    Serial.println("格口号\t物品编号1\t物品编号2\t物品编号3\t用户ID\t物品1预约状态\t物品2预约状态\t物品3预约状态\t物品1实际状态\t物品2实际状态\t物品3实际状态");
    
    for (int i = 0; i < 36; i++) {
        TripleCabinetItem* item = &tripleCabinets[i];
        Serial.printf("%d\t0x%02X\t0x%02X\t0x%02X\t%d\t0x%02X\t0x%02X\t0x%02X\t0x%02X\t0x%02X\t0x%02X\n",
                     item->cabinetId, item->itemId1, item->itemId2, item->itemId3,
                     item->userId, item->itemReserveStatus1, item->itemReserveStatus2, item->itemReserveStatus3,
                     item->itemActualStatus1, item->itemActualStatus2, item->itemActualStatus3);
    }
    
    // 打印单物品格口数据
    Serial.println("\n=== 单物品格口数据 ===");
    Serial.println("格口号\t物品编号\t用户ID\t物品预约状态\t物品实际状态");
    
    for (int i = 0; i < 12; i++) {
        SingleCabinetItem* item = &singleCabinets[i];
        Serial.printf("%d\t0x%02X\t%d\t0x%02X\t0x%02X\n",
                     item->cabinetId, item->itemId, item->userId,
                     item->itemReserveStatus, item->itemActualStatus);
    }
}