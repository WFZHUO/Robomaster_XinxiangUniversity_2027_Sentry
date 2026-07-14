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
#include "drv_tim.h"
#include "drv_uart.h"
#include "drv_usb.h"
#include "drv_can.h"
#include "drv_spi.h"
#include "drv_ospi.h"
#include "sys_timestamp.h"
#include "alg_waveform.h"
#include "bsp_ws2812.h"
#include "bsp_buzzer.h"
#include "bsp_BuzzerSongs.h"
#include "bsp_arkey.h"
#include "bsp_w25q64jv.h"
#include "dvc_serialplot.h"
#include "dvc_motor_dji.h"
#include "dvc_vofa.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 板载Key
Class_ArkKey Key;

// Serialplot
Class_Serialplot_USB Serialplot;
const char *Serialplot_Rx_List[] =
{
    "op",
    "oi",
    "od",
    "ap",
    "ai",
    "ad",
    "fre",
};
// Serialplot测试用变量
float Motor_C620_Now_Omega, Motor_C620_Now_Current, Motor_C620_Now_Encoder, Motor_C620_Now_Angle;
float Motor_C620_Target_Angle, Motor_C620_Target_Omega, Motor_C620_Target_Current;

// 波形发生器
Class_Waveform Waveform_Sine;
float Waveform_Sine_Out;

// C620
Class_Motor_DJI_C620 Motor_C620;

// 全局初始化完成标志位
bool init_finished = false;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief UART1任务回调函数
 */
void UART1_Callback(uint8_t *Buffer, uint16_t Length)
{
    //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);   
}

/**
 * @brief USB任务回调函数, 绑定Serialplot
 */
void USB0_Callback(uint8_t *Buffer, uint32_t Length)
{
    Serialplot.USB_RxCpltCallback(Buffer, static_cast<uint16_t>(Length));

    switch (Serialplot.Get_Variable_Index())
    {
        case 0:
            Motor_C620.PID_Omega.Set_K_P(Serialplot.Get_Variable_Value());
            break;

        case 1:
            Motor_C620.PID_Omega.Set_K_I(Serialplot.Get_Variable_Value());
            break;

        case 2:
            Motor_C620.PID_Omega.Set_K_D(Serialplot.Get_Variable_Value());
            break;

        case 3:
            Motor_C620.PID_Angle.Set_K_P(Serialplot.Get_Variable_Value());
            break;

        case 4:
            Motor_C620.PID_Angle.Set_K_I(Serialplot.Get_Variable_Value());
            break;

        case 5:
            Motor_C620.PID_Angle.Set_K_D(Serialplot.Get_Variable_Value());
            break;

        case 6:
            // 预留变量, 暂时没有功能
            break;

        default:
            break;
    }

    //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);   
}

/**
 * @brief CAN1任务回调函数
 *
 * @param Header 接收帧头
 * @param Buffer 接收数据缓冲区
 */
void CAN1_Callback(FDCAN_RxHeaderTypeDef &Header, uint8_t *Buffer)
{
    if (Header.Identifier == Motor_C620.Get_CAN_Rx_ID())
    {
        Motor_C620.CAN_RxCpltCallback(Buffer);
    }

    //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_15);
}

/**
 * @brief OSPI1任务回调函数
 *
 * @param Operation 完成或发生错误的操作类型
 * @param Buffer 数据缓冲区, 无数据或发生错误时为nullptr
 * @param Length 有效数据长度
 * @param Error_Code HAL错误码, 正常回调时均为HAL_OSPI_ERROR_NONE, 进入HAL_OSPI_ErrorCallback时为对应错误码
 */
void OSPI1_Callback(enum Enum_OSPI_Operation Operation, uint8_t *Buffer,
                    uint32_t Length, uint32_t Error_Code)
{
    BSP_W25Q64JV.OSPI_Callback(Operation, Buffer, Length, Error_Code);
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

    // Serialplot
    Motor_C620_Now_Omega = Motor_C620.Get_Now_Omega();
    Motor_C620_Now_Current = Motor_C620.Get_Now_Current();
    Motor_C620_Now_Encoder = Motor_C620.Get_Now_Encoder();
    Motor_C620_Now_Angle = Motor_C620.Get_Now_Angle();
    Motor_C620_Target_Angle = Motor_C620.Get_Target_Angle();
    Motor_C620_Target_Omega = Motor_C620.Get_Target_Omega();
    Motor_C620_Target_Current = Motor_C620.Get_Target_Current();

    Serialplot.TIM_1ms_Write_PeriodElapsedCallback();

    // 电机控制
    Motor_C620.Set_Target_Angle(Waveform_Sine_Out);
    Motor_C620.TIM_Calculate_PeriodElapsedCallback();
    Motor_DJI_CAN_Tx_PeriodElapsedCallback(&hfdcan1);

    // 10ms任务
    static uint16_t mod10 = 0;
    mod10++;
    if(mod10 == 10)
    {
        mod10 = 0;
        
        // WS2812定时刷新
        BSP_WS2812.Rainbow();
        BSP_WS2812.TIM_10ms_Write_PeriodElapsedCallback();
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

    // 100ms任务
    static uint16_t mod100 = 0;
    mod100++;
    if(mod100 == 100)
    {
        mod100 = 0;

        // 电机定时器回调函数
        Motor_C620.TIM_100ms_Alive_PeriodElapsedCallback();
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
    // 初始化WS2812
    BSP_WS2812.Init(&hspi6);
    // 初始化Serialplot
    Serialplot.Init(7, Serialplot_Rx_List);
    Serialplot.Set_Data(8,
                        &Waveform_Sine_Out,
                        &Motor_C620_Now_Omega,
                        &Motor_C620_Now_Current,
                        &Motor_C620_Now_Encoder,
                        &Motor_C620_Now_Angle,
                        &Motor_C620_Target_Angle,
                        &Motor_C620_Target_Omega,
                        &Motor_C620_Target_Current);

    // 初始化波形
    Waveform_Sine.Init();
    Waveform_Sine.Sine(5.0f, 0.5f);

    // 初始化USB
    USB_Init(USB0_Callback);
    // 初始化TIM
    TIM_Init(&htim7, Task1ms_Callback);
    TIM_Init(&htim5, Task3600s_Callback);
    // 初始化UART
    UART_Init(&huart1, UART1_Callback);
    // 初始化CAN
    CAN_Init(&hfdcan1, CAN1_Callback);
    // 初始化OSPI
    OSPI_Init(&hospi1, OSPI1_Callback);

    // 定时器中断初始化
    HAL_TIM_Base_Start_IT(&htim7);
    HAL_TIM_Base_Start_IT(&htim5);

    // 电机初始化
    Motor_C620.Init(&hfdcan1, Motor_DJI_ID_0x201, Motor_DJI_Control_Method_ANGLE);
    Motor_C620.PID_Omega.Init(0.0f, 0.0f, 0.0f);
    Motor_C620.PID_Angle.Init(0.0f, 0.0f, 0.0f);

    // 初始化W25Q64JV
    BSP_W25Q64JV.Init(&hospi1);

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
        BuzzerSongs_Play_Godfather(1.0f);
        played = true;
    }
}

/*----------------------------------------------------------------------------*/
