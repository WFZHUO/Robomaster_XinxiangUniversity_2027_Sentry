/**
 * @file drv_can.h
 * @author WangFonzhuo
 * @brief FDCAN通信初始化与配置流程
 * @version 1.0
 * @date 2026-05-20 27赛季
 */

#ifndef DRV_CAN_H
#define DRV_CAN_H

/* Includes ------------------------------------------------------------------*/

#include "fdcan.h"
#include "stm32h7xx_hal.h"

#include <stdbool.h>
#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

#define CAN_RX_BUFFER_SIZE (64U)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief CAN通信接收回调函数数据类型
 */
typedef void (*CAN_Callback)(FDCAN_RxHeaderTypeDef &Header, uint8_t *Buffer);

/**
 * @brief CAN通信处理结构体
 */
struct Struct_CAN_Manage_Object
{
    FDCAN_HandleTypeDef *CAN_Handler;
    CAN_Callback Callback_Function;

    // 与接收相关的数据
    FDCAN_RxHeaderTypeDef Rx_Header;
    uint8_t Rx_Buffer[CAN_RX_BUFFER_SIZE];

    // 接收时间戳
    uint64_t Rx_Timestamp;
};

/* Exported variables --------------------------------------------------------*/

// CAN管理对象
extern struct Struct_CAN_Manage_Object CAN1_Manage_Object;
extern struct Struct_CAN_Manage_Object CAN2_Manage_Object;
extern struct Struct_CAN_Manage_Object CAN3_Manage_Object;

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 初始化CAN总线
 *
 * @param hfdcan CAN编号
 * @param Callback_Function 处理回调函数
 */
void CAN_Init(FDCAN_HandleTypeDef *hfdcan, CAN_Callback Callback_Function);

/**
 * @brief 发送标准数据帧
 *
 * @param hfdcan CAN编号
 * @param ID 标准帧ID
 * @param Data 被发送的数据指针
 * @param Length 长度, Classic CAN范围0~8
 * @return uint8_t HAL执行状态
 */
uint8_t CAN_Transmit_Data(FDCAN_HandleTypeDef *hfdcan, uint16_t ID, uint8_t *Data, uint16_t Length);

/**
 * @brief CAN的TIM定时器中断发送回调函数
 */
void TIM_100us_CAN_PeriodElapsedCallback();

/**
 * @brief CAN的TIM定时器中断发送回调函数
 */
void TIM_1ms_CAN_PeriodElapsedCallback();

/* Exported function definitions ---------------------------------------------*/


#endif /* DRV_CAN_H */

/*----------------------------------------------------------------------------*/
