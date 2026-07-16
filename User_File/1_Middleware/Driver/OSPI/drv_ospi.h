/**
 * @file drv_ospi.h
 * @author WangFonzhuo
 * @brief OSPI通用接口.
 *        OSPI 异步驱动. 命令、MDMA发送、MDMA接收和自动轮询的完成均通过中断回调通知；接口启动阶段 HAL 会同步检查并等待硬件 BUSY 清零，但正常状态下该等待会立即结束.
 * @version 1.0
 * @date 2026-07-13 27赛季
 */

#ifndef DRV_OSPI_H
#define DRV_OSPI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "octospi.h"
#include "stm32h7xx_hal.h"

#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

// OSPI内部MDMA缓冲区字节长度
#define OSPI_BUFFER_SIZE (512U)

/* Exported types ------------------------------------------------------------*/

/**
 * @brief OSPI操作类型
 */
enum Enum_OSPI_Operation
{
    OSPI_OPERATION_IDLE = 0,
    OSPI_OPERATION_COMMAND,
    OSPI_OPERATION_TRANSMIT,
    OSPI_OPERATION_RECEIVE,
    OSPI_OPERATION_AUTO_POLLING,
};

/**
 * @brief OSPI通信完成回调函数数据类型
 *
 * @param Operation 完成或发生错误的操作类型
 * @param Buffer 数据缓冲区, 无数据或发生错误时为nullptr
 * @param Length 有效数据长度
 * @param Error_Code HAL错误码, 正常回调时均为HAL_OSPI_ERROR_NONE, 进入HAL_OSPI_ErrorCallback时为对应错误码
 */
typedef void (*OSPI_Callback)(enum Enum_OSPI_Operation Operation,
                              uint8_t *Buffer,
                              uint32_t Length,
                              uint32_t Error_Code);

/**
 * @brief OSPI通信处理结构体
 */
struct Struct_OSPI_Manage_Object
{
    OSPI_HandleTypeDef *OSPI_Handler;
    OSPI_Callback Callback_Function;

    // MDMA专用缓冲区
    uint8_t *Tx_Buffer;
    uint8_t *Rx_Buffer;

    // 当前操作及数据长度
    enum Enum_OSPI_Operation Operation;
    uint32_t Tx_Buffer_Length;
    uint32_t Rx_Buffer_Length;

    // 最近一次操作完成时间戳与HAL错误码
    uint64_t Timestamp;
    uint32_t Error_Code;
};

/* Exported variables --------------------------------------------------------*/

// OSPI管理对象
extern struct Struct_OSPI_Manage_Object OSPI1_Manage_Object;

#if defined(OCTOSPI2)
extern struct Struct_OSPI_Manage_Object OSPI2_Manage_Object;
#endif

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 初始化OSPI管理对象
 *
 * @param hospi OSPI编号
 * @param Callback_Function 通信完成回调函数, 可为空
 */
void OSPI_Init(OSPI_HandleTypeDef *hospi, OSPI_Callback Callback_Function);

/**
 * @brief OSPI中断发送无数据阶段指令
 *
 * @param hospi OSPI编号
 * @param Command 指令配置
 * @return uint8_t HAL执行状态
 */
uint8_t OSPI_Command(OSPI_HandleTypeDef *hospi,
                     const OSPI_RegularCmdTypeDef *Command);

/**
 * @brief OSPI MDMA发送指令与数据
 *
 * @param hospi OSPI编号
 * @param Command 指令配置
 * @param Data 被发送的数据指针
 * @param Length 发送长度
 * @return uint8_t HAL执行状态
 */
uint8_t OSPI_Command_Transmit_Data(OSPI_HandleTypeDef *hospi,
                                   const OSPI_RegularCmdTypeDef *Command,
                                   const uint8_t *Data,
                                   uint32_t Length);

/**
 * @brief OSPI MDMA发送指令并接收数据
 *
 * @param hospi OSPI编号
 * @param Command 指令配置
 * @param Length 接收长度
 * @return uint8_t HAL执行状态
 */
uint8_t OSPI_Command_Receive_Data(OSPI_HandleTypeDef *hospi,
                                  const OSPI_RegularCmdTypeDef *Command,
                                  uint32_t Length);

/**
 * @brief OSPI中断自动轮询
 *
 * @param hospi OSPI编号
 * @param Command 状态寄存器读取指令配置
 * @param Config 自动轮询配置
 * @return uint8_t HAL执行状态
 */
uint8_t OSPI_Auto_Polling(OSPI_HandleTypeDef *hospi,
                          const OSPI_RegularCmdTypeDef *Command,
                          const OSPI_AutoPollingTypeDef *Config);

/* Exported function definitions ---------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* DRV_OSPI_H */

/*----------------------------------------------------------------------------*/
