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
#include "drv_usb.h"
#include "dvc_vofa.h"
#include "alg_waveform.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 板载Key
Class_ArkKey Key;

// Vofa
Class_VofaSum8_USB Vofa;
const char *Vofa_Rx_List[] =
{
    "p",
    "i",
    "d",
    "fre",
};
// Vofa测试用变量
float p,i,d,fre = 1.0f;

// 波形发生器
Class_Waveform Waveform_Sine;
float Waveform_Sine_Out;

// 全局初始化完成标志位
bool init_finished = false;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief UART1任务回调函数
 */
void UART1_Callback(uint8_t *Buffer, uint16_t Length)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);   
}

/**
 * @brief USB任务回调函数, 绑定Vofa
 */
void USB0_Callback(uint8_t *Buffer, uint32_t Length)
{
    Vofa.USB_RxCpltCallback(Buffer, static_cast<uint16_t>(Length));

    switch (Vofa.Get_Variable_Index())
    {
        case 0:
            p = Vofa.Get_Variable_Value();
            break;

        case 1:
            i = Vofa.Get_Variable_Value();
            break;

        case 2:
            d = Vofa.Get_Variable_Value();
            break;

        case 3:
            fre = Vofa.Get_Variable_Value();
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
    
    // 更新波形
    Waveform_Sine_Out = Waveform_Sine.Update();

    // Vofa
    if(fre == 1.0f) // 根据fre的值来控制是否开启Vofa的1ms周期发送, 以节省资源
    {
        Vofa.TIM_1ms_Write_PeriodElapsedCallback();
    }

    // 10ms任务
    static uint16_t mod10 = 0;
    mod10++;
    if(mod10 == 10)
    {
        mod10 = 0;
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
    // 初始化Vofa
    Vofa.Init(4, Vofa_Rx_List);
    Vofa.Set_Data(5,
                &p,
                &i,
                &d,
                &fre,
                &Waveform_Sine_Out);

    // 初始化波形
    Waveform_Sine.Init();
    Waveform_Sine.Sine(1.0f, 1.0f);

    // 初始化USB
    USB_Init(USB0_Callback);

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
