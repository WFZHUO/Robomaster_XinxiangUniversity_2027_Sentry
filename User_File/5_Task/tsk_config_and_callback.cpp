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

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

//全局初始化完成标志位
bool init_finished = false;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 主程序任务初始化函数
 */
void Task_Init()
{
    // 初始化蜂鸣器
    BSP_Buzzer.Init();

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
        BuzzerSongs_Play_Gala_You();
        played = true;
    }

    HAL_Delay(1000);
}

/*----------------------------------------------------------------------------*/
