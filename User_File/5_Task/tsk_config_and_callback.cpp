/**
 * @file tsk_config_and_callback.cpp
 * @author WangFonzhuo
 * @brief 当成main.c来用
 * @version 1.0
 * @date 2025-12-30 26赛季定稿
 * @date 2026-04-18 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "tsk_config_and_callback.h"
#include "bsp_buzzer.h"
#include "bsp_BuzzerSongs.h"
#include "bsp_arkey.h"
#include "drv_tim.h"
#include "drv_uart.h"
#include "sys_timestamp.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 板载Key
Class_ArkKey Key;

// 全局初始化完成标志位
bool init_finished = false;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief UART1任务回调函数
 */
void UART1_Callback(uint8_t *Buffer, uint16_t Length)
{
    // 处理接收到的数据
    // 这里简单地回传接收到的数据
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);   
    UART_Transmit_Data(&huart1, Buffer, Length);
}

/**
 * @brief 1ms定时器回调函数
 */
void Task1ms_Callback()
{
    // 1ms任务

    if(Key.isPressed)
    {
        // 按键测试, 按一次LED翻转一次
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);   
    }
    
    Class_ArkKey::ClearAllFlags();

    // 15ms任务

    static uint16_t mod15 = 0;
    mod15++;
    if(mod15 == 15)
    {
        mod15 = 0;

        // Key扫描并更新按键状态
        Class_ArkKey::TIM_Update_PeriodElapsedCallback();
    }    
}

/**
 * @brief 3600s定时器回调函数
 */
void Task3600s_Callback()
{
    SYS_Timestamp.TIM_3600s_PeriodElapsedCallback();
}

/**
 * @brief 主程序任务初始化函数
 */
void Task_Init()
{
    // 初始化时间戳
    SYS_Timestamp.Init(&htim5);
    // 初始化蜂鸣器
    BSP_Buzzer.Init();
    // 初始化Key
    Key.Init(GPIOA, GPIO_PIN_15);

    // 初始化TIM
    TIM_Init(&htim7, Task1ms_Callback);
    TIM_Init(&htim5, Task3600s_Callback);
    // 初始化UART
    UART_Init(&huart1, UART1_Callback);

    // 定时器中断初始化
    HAL_TIM_Base_Start_IT(&htim7);
    HAL_TIM_Base_Start_IT(&htim5);

    // 设置初始化完成标志位
    init_finished = true;
}

/**
 * @brief 主程序任务循环函数
 */
void Task_Loop()
{
    static bool played = false;

    if (played == false)
    {
        // BuzzerSongs_Play_Gala_You();
        played = true;
    }
}

/*----------------------------------------------------------------------------*/
