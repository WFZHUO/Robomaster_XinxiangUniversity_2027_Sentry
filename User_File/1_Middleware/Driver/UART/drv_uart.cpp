/**
 * @file drv_uart.cpp
 * @author WangFonzhuo
 * @brief UART通用接口
 * @version 1.0
 * @date 2026-04-24 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "drv_uart.h"
#include "sys_timestamp.h"
#include <string.h>

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/**
 * @brief UART底层缓冲区集合
 */
struct Struct_UART_Buffer_Object
{
    uint8_t *Rx_Buffer_0;
    uint8_t *Rx_Buffer_1;
    uint8_t *Tx_Buffer;
};

/* Variables -----------------------------------------------------------------*/

// UART管理对象
Struct_UART_Manage_Object UART1_Manage_Object;
Struct_UART_Manage_Object UART2_Manage_Object;
Struct_UART_Manage_Object UART3_Manage_Object;
Struct_UART_Manage_Object UART4_Manage_Object;
Struct_UART_Manage_Object UART5_Manage_Object;
Struct_UART_Manage_Object UART6_Manage_Object;
Struct_UART_Manage_Object UART7_Manage_Object;
Struct_UART_Manage_Object UART8_Manage_Object;
Struct_UART_Manage_Object UART9_Manage_Object;
Struct_UART_Manage_Object UART10_Manage_Object;

// UART接收缓冲区, 单独定义DMA专用缓冲区
__attribute__((section(".dma_buffer"))) uint8_t UART1_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART1_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART2_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART2_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART3_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART3_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART4_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART4_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART5_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART5_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART6_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART6_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART7_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART7_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART8_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART8_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART9_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART9_Rx_Buffer_1[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART10_Rx_Buffer_0[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART10_Rx_Buffer_1[UART_BUFFER_SIZE];

// UART发送缓冲区, 统一由底层复制后交给DMA
__attribute__((section(".dma_buffer"))) uint8_t UART1_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART2_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART3_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART4_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART5_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART6_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART7_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART8_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART9_Tx_Buffer[UART_BUFFER_SIZE];
__attribute__((section(".dma_buffer"))) uint8_t UART10_Tx_Buffer[UART_BUFFER_SIZE];

static Struct_UART_Buffer_Object UART1_Buffer_Object = {UART1_Rx_Buffer_0, UART1_Rx_Buffer_1, UART1_Tx_Buffer};
static Struct_UART_Buffer_Object UART2_Buffer_Object = {UART2_Rx_Buffer_0, UART2_Rx_Buffer_1, UART2_Tx_Buffer};
static Struct_UART_Buffer_Object UART3_Buffer_Object = {UART3_Rx_Buffer_0, UART3_Rx_Buffer_1, UART3_Tx_Buffer};
static Struct_UART_Buffer_Object UART4_Buffer_Object = {UART4_Rx_Buffer_0, UART4_Rx_Buffer_1, UART4_Tx_Buffer};
static Struct_UART_Buffer_Object UART5_Buffer_Object = {UART5_Rx_Buffer_0, UART5_Rx_Buffer_1, UART5_Tx_Buffer};
static Struct_UART_Buffer_Object UART6_Buffer_Object = {UART6_Rx_Buffer_0, UART6_Rx_Buffer_1, UART6_Tx_Buffer};
static Struct_UART_Buffer_Object UART7_Buffer_Object = {UART7_Rx_Buffer_0, UART7_Rx_Buffer_1, UART7_Tx_Buffer};
static Struct_UART_Buffer_Object UART8_Buffer_Object = {UART8_Rx_Buffer_0, UART8_Rx_Buffer_1, UART8_Tx_Buffer};
static Struct_UART_Buffer_Object UART9_Buffer_Object = {UART9_Rx_Buffer_0, UART9_Rx_Buffer_1, UART9_Tx_Buffer};
static Struct_UART_Buffer_Object UART10_Buffer_Object = {UART10_Rx_Buffer_0, UART10_Rx_Buffer_1, UART10_Tx_Buffer};

/* Function prototypes -------------------------------------------------------*/

static Struct_UART_Manage_Object *UART_Get_Manage_Object(UART_HandleTypeDef *huart);
static Struct_UART_Buffer_Object *UART_Get_Buffer_Object(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_Restart_Receive(UART_HandleTypeDef *huart,
                                              Struct_UART_Manage_Object *Manage_Object);

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 获取UART管理对象
 *
 * @param huart UART编号
 * @return Struct_UART_Manage_Object* UART管理对象
 */
static Struct_UART_Manage_Object *UART_Get_Manage_Object(UART_HandleTypeDef *huart)
{
    if (huart == nullptr)
    {
        return (nullptr);
    }

    if (huart->Instance == USART1)
    {
        return (&UART1_Manage_Object);
    }
    else if (huart->Instance == USART2)
    {
        return (&UART2_Manage_Object);
    }
    else if (huart->Instance == USART3)
    {
        return (&UART3_Manage_Object);
    }
    else if (huart->Instance == UART4)
    {
        return (&UART4_Manage_Object);
    }
    else if (huart->Instance == UART5)
    {
        return (&UART5_Manage_Object);
    }
    else if (huart->Instance == USART6)
    {
        return (&UART6_Manage_Object);
    }
    else if (huart->Instance == UART7)
    {
        return (&UART7_Manage_Object);
    }
    else if (huart->Instance == UART8)
    {
        return (&UART8_Manage_Object);
    }
    else if (huart->Instance == UART9)
    {
        return (&UART9_Manage_Object);
    }
    else if (huart->Instance == USART10)
    {
        return (&UART10_Manage_Object);
    }

    return (nullptr);
}

/**
 * @brief 获取UART底层缓冲区集合
 *
 * @param huart UART编号
 * @return Struct_UART_Buffer_Object* UART底层缓冲区集合
 */
static Struct_UART_Buffer_Object *UART_Get_Buffer_Object(UART_HandleTypeDef *huart)
{
    if (huart == nullptr)
    {
        return (nullptr);
    }

    if (huart->Instance == USART1)
    {
        return (&UART1_Buffer_Object);
    }
    else if (huart->Instance == USART2)
    {
        return (&UART2_Buffer_Object);
    }
    else if (huart->Instance == USART3)
    {
        return (&UART3_Buffer_Object);
    }
    else if (huart->Instance == UART4)
    {
        return (&UART4_Buffer_Object);
    }
    else if (huart->Instance == UART5)
    {
        return (&UART5_Buffer_Object);
    }
    else if (huart->Instance == USART6)
    {
        return (&UART6_Buffer_Object);
    }
    else if (huart->Instance == UART7)
    {
        return (&UART7_Buffer_Object);
    }
    else if (huart->Instance == UART8)
    {
        return (&UART8_Buffer_Object);
    }
    else if (huart->Instance == UART9)
    {
        return (&UART9_Buffer_Object);
    }
    else if (huart->Instance == USART10)
    {
        return (&UART10_Buffer_Object);
    }

    return (nullptr);
}

/**
 * @brief 重新启动UART空闲中断DMA接收
 *
 * @param huart UART编号
 * @param Manage_Object UART管理对象
 * @return HAL_StatusTypeDef HAL执行状态
 */
static HAL_StatusTypeDef UART_Restart_Receive(UART_HandleTypeDef *huart,
                                              Struct_UART_Manage_Object *Manage_Object)
{
    if (huart == nullptr ||
        Manage_Object == nullptr ||
        Manage_Object->Rx_Buffer_Active == nullptr ||
        huart->hdmarx == nullptr)
    {
        return (HAL_ERROR);
    }

    HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(huart,
                                                            Manage_Object->Rx_Buffer_Active,
                                                            UART_BUFFER_SIZE);

    if (status == HAL_OK)
    {
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }

    return (status);
}

/**
 * @brief 初始化UART
 *
 * @param huart UART编号
 * @param Callback_Function 回调函数
 */
void UART_Init(UART_HandleTypeDef *huart, UART_Callback Callback_Function)
{
    Struct_UART_Manage_Object *manage_object = UART_Get_Manage_Object(huart);
    Struct_UART_Buffer_Object *buffer_object = UART_Get_Buffer_Object(huart);

    if (manage_object == nullptr || buffer_object == nullptr)
    {
        return;
    }

    manage_object->UART_Handler = huart;
    manage_object->Callback_Function = Callback_Function;

    manage_object->Rx_Buffer_0 = buffer_object->Rx_Buffer_0;
    manage_object->Rx_Buffer_1 = buffer_object->Rx_Buffer_1;
    manage_object->Rx_Buffer_Active = manage_object->Rx_Buffer_0;
    manage_object->Rx_Buffer_Ready = nullptr;
    manage_object->Rx_Time_Stamp = 0U;

    UART_Restart_Receive(huart, manage_object);
}

/**
 * @brief 重新初始化UART接收
 *
 * @param huart UART编号
 */
void UART_Reinit(UART_HandleTypeDef *huart)
{
    Struct_UART_Manage_Object *manage_object = UART_Get_Manage_Object(huart);

    if (manage_object == nullptr || manage_object->Rx_Buffer_0 == nullptr)
    {
        return;
    }

    manage_object->Rx_Buffer_Active = manage_object->Rx_Buffer_0;
    manage_object->Rx_Buffer_Ready = nullptr;

    UART_Restart_Receive(huart, manage_object);
}

/**
 * @brief UART发送数据
 *
 * @param huart UART编号
 * @param Data 被发送的数据指针
 * @param Length 数据长度
 * @return uint8_t HAL执行状态
 * @note 数据会先复制到RAM_D2中的UART专属发送缓冲区
 */
uint8_t UART_Transmit_Data(UART_HandleTypeDef *huart, uint8_t *Data, uint16_t Length)
{
    Struct_UART_Buffer_Object *buffer_object = UART_Get_Buffer_Object(huart);

    if (huart == nullptr ||
        Data == nullptr ||
        Length == 0U ||
        Length > UART_BUFFER_SIZE ||
        buffer_object == nullptr ||
        buffer_object->Tx_Buffer == nullptr ||
        huart->hdmatx == nullptr)
    {
        return ((uint8_t)HAL_ERROR);
    }

    /*
     * 检查UART状态、复制数据和启动DMA必须连续完成,
     * 防止不同任务在同一UART上同时覆盖发送缓冲区。
     */
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (huart->gState != HAL_UART_STATE_READY)
    {
        __set_PRIMASK(primask);
        return ((uint8_t)HAL_BUSY);
    }

    memcpy(buffer_object->Tx_Buffer, Data, Length);

    HAL_StatusTypeDef status = HAL_UART_Transmit_DMA(huart,
                                                      buffer_object->Tx_Buffer,
                                                      Length);

    __set_PRIMASK(primask);

    return ((uint8_t)status);
}

/**
 * @brief HAL库UART接收DMA空闲中断
 *
 * @param huart UART编号
 * @param Size 长度
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    Struct_UART_Manage_Object *manage_object = UART_Get_Manage_Object(huart);

    if (manage_object == nullptr || manage_object->Rx_Buffer_Active == nullptr)
    {
        return;
    }

    // 初始化未完成时只重启接收, 不进入用户回调
    if (!init_finished)
    {
        UART_Restart_Receive(huart, manage_object);
        return;
    }

    manage_object->Rx_Buffer_Ready = manage_object->Rx_Buffer_Active;

    if (manage_object->Rx_Buffer_Active == manage_object->Rx_Buffer_0)
    {
        manage_object->Rx_Buffer_Active = manage_object->Rx_Buffer_1;
    }
    else
    {
        manage_object->Rx_Buffer_Active = manage_object->Rx_Buffer_0;
    }

    manage_object->Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

    // 先切换到另一缓冲区继续接收, 再处理刚完成的数据
    UART_Restart_Receive(huart, manage_object);

    if (manage_object->Callback_Function != nullptr)
    {
        manage_object->Callback_Function(manage_object->Rx_Buffer_Ready, Size);
    }
}

/**
 * @brief HAL库UART错误中断
 *
 * @param huart UART编号
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    UART_Reinit(huart);
}

/*----------------------------------------------------------------------------*/
