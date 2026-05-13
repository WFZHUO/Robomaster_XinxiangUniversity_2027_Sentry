/**
 * @file drv_usb.h
 * @author WangFonzhuo
 * @brief USB通用接口
 * @version 1.0
 * @date 2026-05-09 27赛季
 */

#ifndef DRV_USB_H
#define DRV_USB_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32h7xx_hal.h"
#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

// 缓冲区字节长度
#define USB_BUFFER_SIZE 2048

/* Exported types ------------------------------------------------------------*/

/**
 * @brief USB通信接收回调函数数据类型
 *
 */
typedef void (*USB_Callback)(uint8_t *Buffer, uint32_t Length);

/**
 * @brief USB通信处理结构体
 */
struct Struct_USB_Manage_Object
{
    USB_Callback Callback_Function;

    // 双缓冲适配的缓冲区 以及 当前激活的缓冲区
    uint8_t *Rx_Buffer_0;
    uint8_t *Rx_Buffer_1;
    // 正在接收的缓冲区
    uint8_t *Rx_Buffer_Active;
    // 接收完毕的缓冲区
    uint8_t *Rx_Buffer_Ready;

    // 接收时间戳
    uint64_t Rx_Time_Stamp;
};

/* Exported variables --------------------------------------------------------*/

// 声明USB管理对象
extern struct Struct_USB_Manage_Object USB_Manage_Object;

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 初始化USB
 *
 * @param Callback_Function 回调函数
 */
void USB_Init(USB_Callback Callback_Function);

/**
 * @brief USB发送数据
 *
 * @param Data 被发送的数据指针
 * @param Length 数据长度
 * @return 发送状态
 */
uint8_t USB_Transmit_Data(uint8_t *Data, uint16_t Length);

/**
 * @brief USB接收回调转接函数
 *
 * @param Buffer 接收数据指针
 * @param Length 接收数据长度
 */
void USB_ReceiveCallback(uint8_t *Buffer, uint32_t Length);

/* Exported function definitions ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DRV_USB_H */

/*----------------------------------------------------------------------------*/