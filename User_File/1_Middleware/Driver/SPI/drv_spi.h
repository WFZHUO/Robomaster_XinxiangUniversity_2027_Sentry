/**
 * @file drv_spi.h
 * @author WangFonzhuo
 * @brief SPI通用接口
 * @version 1.0
 * @date 2026-07-11 27赛季
 */

#ifndef DRV_SPI_H
#define DRV_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "spi.h"
#include "stm32h7xx_hal.h"

#include <stdbool.h>
#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

// SPI内部DMA缓冲区字节长度
#define SPI_BUFFER_SIZE (512U)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief SPI通信完成回调函数数据类型
 *
 * @param Tx_Buffer 发送缓冲区
 * @param Rx_Buffer 接收缓冲区, 仅发送时为nullptr
 * @param Tx_Length 发送数据长度
 * @param Rx_Length 接收有效数据长度
 */
typedef void (*SPI_Callback)(uint8_t *Tx_Buffer, uint8_t *Rx_Buffer, uint16_t Tx_Length, uint16_t Rx_Length);

/**
 * @brief SPI通信处理结构体
 */
struct Struct_SPI_Manage_Object
{
    SPI_HandleTypeDef *SPI_Handler;
    SPI_Callback Callback_Function;

    // 片选信号的GPIO与有效电平, 为空表示不使用片选
    GPIO_TypeDef *Activate_GPIOx;
    uint16_t Activate_GPIO_Pin;
    GPIO_PinState Activate_Level;

    // DMA专用缓冲区指针
    uint8_t *Tx_Buffer;
    uint8_t *Rx_Buffer;

    // 一次收发对应的数据长度
    uint16_t Tx_Buffer_Length;
    uint16_t Rx_Buffer_Length;

    // 发送/接收忙标志
    bool Tx_Busy;
    bool Rx_Busy;

    // 接收时间戳
    uint64_t Rx_Timestamp;
};

/* Exported variables --------------------------------------------------------*/

// SPI管理对象
extern struct Struct_SPI_Manage_Object SPI1_Manage_Object;
extern struct Struct_SPI_Manage_Object SPI2_Manage_Object;
extern struct Struct_SPI_Manage_Object SPI3_Manage_Object;
extern struct Struct_SPI_Manage_Object SPI4_Manage_Object;
extern struct Struct_SPI_Manage_Object SPI6_Manage_Object;

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 初始化SPI管理对象
 *
 * @param hspi SPI编号
 * @param Callback_Function 传输完成回调函数, 可为空
 */
void SPI_Init(SPI_HandleTypeDef *hspi, SPI_Callback Callback_Function);

/**
 * @brief SPI DMA发送数据
 *
 * @param hspi SPI编号
 * @param Data 被发送的数据指针
 * @param Length 发送长度
 * @param GPIOx 片选GPIO编组, 不使用片选则填nullptr
 * @param GPIO_Pin 片选GPIO引脚
 * @param Activate_Level 片选有效电平
 * @return uint8_t HAL执行状态
 */
uint8_t SPI_Transmit_Data(SPI_HandleTypeDef *hspi,
                          uint8_t *Data,
                          uint16_t Length,
                          GPIO_TypeDef *GPIOx,
                          uint16_t GPIO_Pin,
                          GPIO_PinState Activate_Level);

/**
 * @brief SPI DMA全双工收发数据
 *
 * @param hspi SPI编号
 * @param Tx_Data 被发送的数据指针
 * @param Length 收发长度
 * @param GPIOx 片选GPIO编组, 不使用片选则填nullptr
 * @param GPIO_Pin 片选GPIO引脚
 * @param Activate_Level 片选有效电平
 * @return uint8_t HAL执行状态
 */
uint8_t SPI_Transmit_Receive_Data(SPI_HandleTypeDef *hspi,
                                  uint8_t *Tx_Data,
                                  uint16_t Length,
                                  GPIO_TypeDef *GPIOx,
                                  uint16_t GPIO_Pin,
                                  GPIO_PinState Activate_Level);

/* Exported function definitions ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DRV_SPI_H */

/*----------------------------------------------------------------------------*/
