/**
 * @file drv_uart.h
 * @author WangFonzhuo
 * @brief UART通用接口
 * @version 1.0
 * @date 2026-04-24 27赛季
 */

#ifndef DRV_UART_H
#define DRV_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "usart.h"
#include "stm32h7xx_hal.h"

/* Exported macros -----------------------------------------------------------*/

// 缓冲区字节长度
#define UART_BUFFER_SIZE 512

/* Exported types ------------------------------------------------------------*/

/**
 * @brief UART通信接收回调函数数据类型
 *
 */
typedef void (*UART_Callback)(uint8_t *Buffer, uint16_t Length);

/**
 * @brief UART通信处理结构体
 */
struct Struct_UART_Manage_Object
{
    UART_HandleTypeDef *UART_Handler;
    UART_Callback Callback_Function;

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

// 全局初始化完成标志位
extern bool init_finished;

// 声明UART管理对象
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
 */
void UART_Init(UART_HandleTypeDef *huart, UART_Callback Callback_Function);

/**
 * @brief 重新初始化UART
 *
 * @param huart UART编号
 */
void UART_Reinit(UART_HandleTypeDef * huart);

/**
 * @brief UART发送数据
 *
 * @param huart UART编号
 * @param Data 被发送的数据指针
 * @param Length 数据长度
 * @return 发送状态
 */
uint8_t UART_Transmit_Data(UART_HandleTypeDef *huart, uint8_t *Data, uint16_t Length);

/* Exported function definitions ---------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* DRV_UART_H */

/*----------------------------------------------------------------------------*/
