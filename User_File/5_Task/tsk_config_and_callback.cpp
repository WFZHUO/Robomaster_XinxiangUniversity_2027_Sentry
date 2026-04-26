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
#include "dvc_serialplot.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 板载Key
Class_ArkKey Key;

// Serialplot
Class_Serialplot_UART Serialplot;
const char *Serialplot_Rx_List[] =
{
    "p",
    "i",
    "d",
};

// 全局初始化完成标志位
bool init_finished = false;

float p,i,d = 100;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief UART1任务回调函数, 绑定Serialplot
 */
void UART1_Callback(uint8_t *Buffer, uint16_t Length)
{
    Serialplot.UART_RxCpltCallback(Buffer, Length);

    switch (Serialplot.Get_Variable_Index())
    {
    case 0:
        p = Serialplot.Get_Variable_Value();
        break;

    case 1:
        i = Serialplot.Get_Variable_Value();
        break;

    case 2:
        d = Serialplot.Get_Variable_Value();
        break;

    default:
        break;
    }
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);   
}

/**
 * @brief 1ms定时器回调函数
 */
void Task1ms_Callback()
{
    // 1ms任务

    // 按键状态更新
    if(Key.isPressed)
    {
        // 按键测试, 按一次LED翻转一次
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);   
    }
    
    Class_ArkKey::ClearAllFlags();

    // 10ms任务
    static uint16_t mod10 = 0;
    mod10++;
    if(mod10 == 10)
    {
        mod10 = 0;

        // Serialplot
        Serialplot.TIM_1ms_Write_PeriodElapsedCallback();
    }

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
    // 初始化Serialplot
    Serialplot.Init(&huart1,Serialplot_Checksum_8_ENABLE,3, Serialplot_Rx_List, Serialplot_Data_Type_FLOAT, 0xab);
    Serialplot.Set_Data(3, &p, &i, &d);

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
