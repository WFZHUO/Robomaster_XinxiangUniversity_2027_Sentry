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

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

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

// UART接收缓冲区, 单独定义 DMA 专用缓冲区
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

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化UART
 *
 * @param huart UART编号
 * @param Callback_Function 回调函数
 */
void UART_Init(UART_HandleTypeDef *huart, UART_Callback Callback_Function)
{
    if (huart->Instance == USART1)
    {
        UART1_Manage_Object.UART_Handler = huart;
        UART1_Manage_Object.Callback_Function = Callback_Function;

        UART1_Manage_Object.Rx_Buffer_0 = UART1_Rx_Buffer_0;
        UART1_Manage_Object.Rx_Buffer_1 = UART1_Rx_Buffer_1;

        UART1_Manage_Object.Rx_Buffer_Active = UART1_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART1_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART2)
    {
        UART2_Manage_Object.UART_Handler = huart;
        UART2_Manage_Object.Callback_Function = Callback_Function;

        UART2_Manage_Object.Rx_Buffer_0 = UART2_Rx_Buffer_0;
        UART2_Manage_Object.Rx_Buffer_1 = UART2_Rx_Buffer_1;

        UART2_Manage_Object.Rx_Buffer_Active = UART2_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART2_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART3)
    {
        UART3_Manage_Object.UART_Handler = huart;
        UART3_Manage_Object.Callback_Function = Callback_Function;

        UART3_Manage_Object.Rx_Buffer_0 = UART3_Rx_Buffer_0;
        UART3_Manage_Object.Rx_Buffer_1 = UART3_Rx_Buffer_1;
        
        UART3_Manage_Object.Rx_Buffer_Active = UART3_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART3_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART4)
    {
        UART4_Manage_Object.UART_Handler = huart;
        UART4_Manage_Object.Callback_Function = Callback_Function;

        UART4_Manage_Object.Rx_Buffer_0 = UART4_Rx_Buffer_0;
        UART4_Manage_Object.Rx_Buffer_1 = UART4_Rx_Buffer_1;

        UART4_Manage_Object.Rx_Buffer_Active = UART4_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART4_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART5)
    {
        UART5_Manage_Object.UART_Handler = huart;
        UART5_Manage_Object.Callback_Function = Callback_Function;

        UART5_Manage_Object.Rx_Buffer_0 = UART5_Rx_Buffer_0;
        UART5_Manage_Object.Rx_Buffer_1 = UART5_Rx_Buffer_1;

        UART5_Manage_Object.Rx_Buffer_Active = UART5_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART5_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART6)
    {
        UART6_Manage_Object.UART_Handler = huart;
        UART6_Manage_Object.Callback_Function = Callback_Function;

        UART6_Manage_Object.Rx_Buffer_0 = UART6_Rx_Buffer_0;
        UART6_Manage_Object.Rx_Buffer_1 = UART6_Rx_Buffer_1;

        UART6_Manage_Object.Rx_Buffer_Active = UART6_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART6_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART7)
    {
        UART7_Manage_Object.UART_Handler = huart;
        UART7_Manage_Object.Callback_Function = Callback_Function;

        UART7_Manage_Object.Rx_Buffer_0 = UART7_Rx_Buffer_0;
        UART7_Manage_Object.Rx_Buffer_1 = UART7_Rx_Buffer_1;

        UART7_Manage_Object.Rx_Buffer_Active = UART7_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART7_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART8)
    {
        UART8_Manage_Object.UART_Handler = huart;
        UART8_Manage_Object.Callback_Function = Callback_Function;

        UART8_Manage_Object.Rx_Buffer_0 = UART8_Rx_Buffer_0;
        UART8_Manage_Object.Rx_Buffer_1 = UART8_Rx_Buffer_1;

        UART8_Manage_Object.Rx_Buffer_Active = UART8_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART8_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART9)
    {
        UART9_Manage_Object.UART_Handler = huart;
        UART9_Manage_Object.Callback_Function = Callback_Function;

        UART9_Manage_Object.Rx_Buffer_0 = UART9_Rx_Buffer_0;
        UART9_Manage_Object.Rx_Buffer_1 = UART9_Rx_Buffer_1;

        UART9_Manage_Object.Rx_Buffer_Active = UART9_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART9_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART10)
    {
        UART10_Manage_Object.UART_Handler = huart;
        UART10_Manage_Object.Callback_Function = Callback_Function;

        UART10_Manage_Object.Rx_Buffer_0 = UART10_Rx_Buffer_0;
        UART10_Manage_Object.Rx_Buffer_1 = UART10_Rx_Buffer_1;

        UART10_Manage_Object.Rx_Buffer_Active = UART10_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART10_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
}

/**
 * @brief 重新初始化UART
 *
 * @param huart UART编号
 */
void UART_Reinit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        UART1_Manage_Object.Rx_Buffer_Active = UART1_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART1_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART2)
    {
        UART2_Manage_Object.Rx_Buffer_Active = UART2_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART2_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART3)
    {
        UART3_Manage_Object.Rx_Buffer_Active = UART3_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART3_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART4)
    {
        UART4_Manage_Object.Rx_Buffer_Active = UART4_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART4_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART5)
    {
        UART5_Manage_Object.Rx_Buffer_Active = UART5_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART5_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART6)
    {
        UART6_Manage_Object.Rx_Buffer_Active = UART6_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART6_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART7)
    {
        UART7_Manage_Object.Rx_Buffer_Active = UART7_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART7_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART8)
    {
        UART8_Manage_Object.Rx_Buffer_Active = UART8_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART8_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == UART9)
    {
        UART9_Manage_Object.Rx_Buffer_Active = UART9_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART9_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    else if (huart->Instance == USART10)
    {
        UART10_Manage_Object.Rx_Buffer_Active = UART10_Manage_Object.Rx_Buffer_0;
        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART10_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
}

/**
 * @brief UART发送数据
 *
 * @param huart UART编号
 * @param Data 被发送的数据指针
 * @param Length 数据长度
 * @return 发送状态
 */
uint8_t UART_Transmit_Data(UART_HandleTypeDef *huart, uint8_t *Data, uint16_t Length)
{
    return (HAL_UART_Transmit_DMA(huart, Data, Length));
}

/**
 * @brief HAL库UART接收DMA空闲中断
 *
 * @param huart UART编号
 * @param Size 长度
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // 判断程序初始化完成
    if (!init_finished)
    {
        // 重启接收
        if (huart->Instance == USART1)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART1_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == USART2)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART2_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == USART3)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART3_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == UART4)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART4_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == UART5)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART5_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == USART6)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART6_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == UART7)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART7_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == UART8)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART8_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == UART9)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART9_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        else if (huart->Instance == USART10)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(huart, UART10_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
            __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
        }
        return;
    }

    // 选择回调函数
    if (huart->Instance == USART1)
    {
        UART1_Manage_Object.Rx_Buffer_Ready = UART1_Manage_Object.Rx_Buffer_Active;
        if (UART1_Manage_Object.Rx_Buffer_Active == UART1_Manage_Object.Rx_Buffer_0)
        {
            UART1_Manage_Object.Rx_Buffer_Active = UART1_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART1_Manage_Object.Rx_Buffer_Active = UART1_Manage_Object.Rx_Buffer_0;
        }

        UART1_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART1_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART1_Manage_Object.Callback_Function != nullptr)
        {
            UART1_Manage_Object.Callback_Function(UART1_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == USART2)
    {
        UART2_Manage_Object.Rx_Buffer_Ready = UART2_Manage_Object.Rx_Buffer_Active;
        if (UART2_Manage_Object.Rx_Buffer_Active == UART2_Manage_Object.Rx_Buffer_0)
        {
            UART2_Manage_Object.Rx_Buffer_Active = UART2_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART2_Manage_Object.Rx_Buffer_Active = UART2_Manage_Object.Rx_Buffer_0;
        }

        UART2_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART2_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART2_Manage_Object.Callback_Function != nullptr)
        {
            UART2_Manage_Object.Callback_Function(UART2_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == USART3)
    {
        UART3_Manage_Object.Rx_Buffer_Ready = UART3_Manage_Object.Rx_Buffer_Active;
        if (UART3_Manage_Object.Rx_Buffer_Active == UART3_Manage_Object.Rx_Buffer_0)
        {
            UART3_Manage_Object.Rx_Buffer_Active = UART3_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART3_Manage_Object.Rx_Buffer_Active = UART3_Manage_Object.Rx_Buffer_0;
        }

        UART3_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART3_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART3_Manage_Object.Callback_Function != nullptr)
        {
            UART3_Manage_Object.Callback_Function(UART3_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == UART4)
    {
        UART4_Manage_Object.Rx_Buffer_Ready = UART4_Manage_Object.Rx_Buffer_Active;
        if (UART4_Manage_Object.Rx_Buffer_Active == UART4_Manage_Object.Rx_Buffer_0)
        {
            UART4_Manage_Object.Rx_Buffer_Active = UART4_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART4_Manage_Object.Rx_Buffer_Active = UART4_Manage_Object.Rx_Buffer_0;
        }

        UART4_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART4_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART4_Manage_Object.Callback_Function != nullptr)
        {
            UART4_Manage_Object.Callback_Function(UART4_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == UART5)
    {
        UART5_Manage_Object.Rx_Buffer_Ready = UART5_Manage_Object.Rx_Buffer_Active;
        if (UART5_Manage_Object.Rx_Buffer_Active == UART5_Manage_Object.Rx_Buffer_0)
        {
            UART5_Manage_Object.Rx_Buffer_Active = UART5_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART5_Manage_Object.Rx_Buffer_Active = UART5_Manage_Object.Rx_Buffer_0;
        }

        UART5_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART5_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART5_Manage_Object.Callback_Function != nullptr)
        {
            UART5_Manage_Object.Callback_Function(UART5_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == USART6)
    {
        UART6_Manage_Object.Rx_Buffer_Ready = UART6_Manage_Object.Rx_Buffer_Active;
        if (UART6_Manage_Object.Rx_Buffer_Active == UART6_Manage_Object.Rx_Buffer_0)
        {
            UART6_Manage_Object.Rx_Buffer_Active = UART6_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART6_Manage_Object.Rx_Buffer_Active = UART6_Manage_Object.Rx_Buffer_0;
        }

        UART6_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART6_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART6_Manage_Object.Callback_Function != nullptr)
        {
            UART6_Manage_Object.Callback_Function(UART6_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == UART7)
    {
        UART7_Manage_Object.Rx_Buffer_Ready = UART7_Manage_Object.Rx_Buffer_Active;
        if (UART7_Manage_Object.Rx_Buffer_Active == UART7_Manage_Object.Rx_Buffer_0)
        {
            UART7_Manage_Object.Rx_Buffer_Active = UART7_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART7_Manage_Object.Rx_Buffer_Active = UART7_Manage_Object.Rx_Buffer_0;
        }

        UART7_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART7_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART7_Manage_Object.Callback_Function != nullptr)
        {
            UART7_Manage_Object.Callback_Function(UART7_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == UART8)
    {
        UART8_Manage_Object.Rx_Buffer_Ready = UART8_Manage_Object.Rx_Buffer_Active;
        if (UART8_Manage_Object.Rx_Buffer_Active == UART8_Manage_Object.Rx_Buffer_0)
        {
            UART8_Manage_Object.Rx_Buffer_Active = UART8_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART8_Manage_Object.Rx_Buffer_Active = UART8_Manage_Object.Rx_Buffer_0;
        }

        UART8_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART8_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART8_Manage_Object.Callback_Function != nullptr)
        {
            UART8_Manage_Object.Callback_Function(UART8_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == UART9)
    {
        UART9_Manage_Object.Rx_Buffer_Ready = UART9_Manage_Object.Rx_Buffer_Active;
        if (UART9_Manage_Object.Rx_Buffer_Active == UART9_Manage_Object.Rx_Buffer_0)
        {
            UART9_Manage_Object.Rx_Buffer_Active = UART9_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART9_Manage_Object.Rx_Buffer_Active = UART9_Manage_Object.Rx_Buffer_0;
        }

        UART9_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART9_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART9_Manage_Object.Callback_Function != nullptr)
        {
            UART9_Manage_Object.Callback_Function(UART9_Manage_Object.Rx_Buffer_Ready, Size);
        }
    }
    else if (huart->Instance == USART10)
    {
        UART10_Manage_Object.Rx_Buffer_Ready = UART10_Manage_Object.Rx_Buffer_Active;
        if (UART10_Manage_Object.Rx_Buffer_Active == UART10_Manage_Object.Rx_Buffer_0)
        {
            UART10_Manage_Object.Rx_Buffer_Active = UART10_Manage_Object.Rx_Buffer_1;
        }
        else
        {
            UART10_Manage_Object.Rx_Buffer_Active = UART10_Manage_Object.Rx_Buffer_0;
        }

        UART10_Manage_Object.Rx_Time_Stamp = SYS_Timestamp.Get_Current_Timestamp();

        HAL_UARTEx_ReceiveToIdle_DMA(huart, UART10_Manage_Object.Rx_Buffer_Active, UART_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);

        if (UART10_Manage_Object.Callback_Function != nullptr)
        {
            UART10_Manage_Object.Callback_Function(UART10_Manage_Object.Rx_Buffer_Ready, Size);
        }
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
