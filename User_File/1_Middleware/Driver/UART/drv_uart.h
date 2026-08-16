/**
 * @file drv_uart.h
 * @author WangFangzhuo
 * @brief UART通用接口
 * @version 1.2
 * @date 2026-04-24
 */

#ifndef DRV_UART_H
#define DRV_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "usart.h"

/* Exported macros -----------------------------------------------------------*/

// 单个UART收发缓冲区的字节数
#define UART_BUFFER_SIZE 512U

/* Exported types ------------------------------------------------------------*/

/**
 * @brief UART通信接收回调函数数据类型
 */
typedef void (*UART_Callback)(uint8_t *Buffer, uint16_t Length);

/**
 * @brief UART通信处理结构体
 */
struct Struct_UART_Manage_Object
{
    UART_HandleTypeDef *UART_Handler;
    UART_Callback Callback_Function;

    // 软件双缓冲
    uint8_t *Rx_Buffer_0;
    uint8_t *Rx_Buffer_1;
    // DMA当前正在写入的缓冲区
    uint8_t *Rx_Buffer_Active;
    // 最近一次接收完成的缓冲区
    uint8_t *Rx_Buffer_Ready;

    // 最近一次接收完成的时间戳
    uint64_t Rx_Time_Stamp;
};

/* Exported variables --------------------------------------------------------*/

// UART管理对象
extern struct Struct_UART_Manage_Object UART1_Manage_Object;
extern struct Struct_UART_Manage_Object UART2_Manage_Object;
extern struct Struct_UART_Manage_Object UART3_Manage_Object;
extern struct Struct_UART_Manage_Object UART4_Manage_Object;
extern struct Struct_UART_Manage_Object UART5_Manage_Object;
extern struct Struct_UART_Manage_Object UART6_Manage_Object;
extern struct Struct_UART_Manage_Object UART7_Manage_Object;
extern struct Struct_UART_Manage_Object UART8_Manage_Object;
extern struct Struct_UART_Manage_Object UART9_Manage_Object;
extern struct Struct_UART_Manage_Object UART10_Manage_Object;

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 初始化UART
 *
 * @param huart UART编号
 * @param Callback_Function 回调函数
 * @note Callback_Function在中断上下文执行, 应保持简短且不可阻塞
 */
void UART_Init(UART_HandleTypeDef *huart, UART_Callback Callback_Function);

/**
 * @brief 重新初始化UART接收
 *
 * @param huart UART编号
 */
void UART_Reinit(UART_HandleTypeDef *huart);

/**
 * @brief UART发送数据
 *
 * @param huart UART编号
 * @param Data 被发送的数据指针
 * @param Length 数据长度
 * @return uint8_t HAL执行状态
 * @note 函数会先将数据复制到UART专属DMA发送缓冲区, 返回后Data可以立即复用
 * @note 同一UART正在发送或发送启动区被占用时返回HAL_BUSY
 */
uint8_t UART_Transmit_Data(UART_HandleTypeDef *huart, uint8_t *Data, uint16_t Length);

#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */

/*----------------------------------------------------------------------------*/
