/**
 * @file drv_tim.cpp
 * @author WangFonzhuo
 * @brief TIM通用接口
 * @version 1.0
 * @date 2025-12-30 26赛季定稿
 * @date 2026-04-18 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "drv_tim.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// TIM定时器管理对象
Struct_TIM_Manage_Object TIM1_Manage_Object;
Struct_TIM_Manage_Object TIM2_Manage_Object;
Struct_TIM_Manage_Object TIM3_Manage_Object;
Struct_TIM_Manage_Object TIM4_Manage_Object;
Struct_TIM_Manage_Object TIM5_Manage_Object;
Struct_TIM_Manage_Object TIM6_Manage_Object;
Struct_TIM_Manage_Object TIM7_Manage_Object;
Struct_TIM_Manage_Object TIM8_Manage_Object;
Struct_TIM_Manage_Object TIM12_Manage_Object;
Struct_TIM_Manage_Object TIM13_Manage_Object;
Struct_TIM_Manage_Object TIM14_Manage_Object;
Struct_TIM_Manage_Object TIM15_Manage_Object;
Struct_TIM_Manage_Object TIM16_Manage_Object;
Struct_TIM_Manage_Object TIM17_Manage_Object;
Struct_TIM_Manage_Object TIM23_Manage_Object;
Struct_TIM_Manage_Object TIM24_Manage_Object;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化TIM定时器
 *
 * @param htim 定时器编号
 * @param Callback_Function 处理回调函数
 */
void TIM_Init(TIM_HandleTypeDef *htim, TIM_Call_Back Callback_Function)
{
    if (htim->Instance == TIM1)
    {
        TIM1_Manage_Object.TIM_Handler = htim;
        TIM1_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM2)
    {
        TIM2_Manage_Object.TIM_Handler = htim;
        TIM2_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM3)
    {
        TIM3_Manage_Object.TIM_Handler = htim;
        TIM3_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM4)
    {
        TIM4_Manage_Object.TIM_Handler = htim;
        TIM4_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM5)
    {
        TIM5_Manage_Object.TIM_Handler = htim;
        TIM5_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM6)
    {
        TIM6_Manage_Object.TIM_Handler = htim;
        TIM6_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM7)
    {
        TIM7_Manage_Object.TIM_Handler = htim;
        TIM7_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM8)
    {
        TIM8_Manage_Object.TIM_Handler = htim;
        TIM8_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM12)
    {
        TIM12_Manage_Object.TIM_Handler = htim;
        TIM12_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM13)
    {
        TIM13_Manage_Object.TIM_Handler = htim;
        TIM13_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM14)
    {
        TIM14_Manage_Object.TIM_Handler = htim;
        TIM14_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM15)
    {
        TIM15_Manage_Object.TIM_Handler = htim;
        TIM15_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM16)
    {
        TIM16_Manage_Object.TIM_Handler = htim;
        TIM16_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM17)
    {
        TIM17_Manage_Object.TIM_Handler = htim;
        TIM17_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM23)
    {
        TIM23_Manage_Object.TIM_Handler = htim;
        TIM23_Manage_Object.Callback_Function = Callback_Function;
    }
    else if (htim->Instance == TIM24)
    {
        TIM24_Manage_Object.TIM_Handler = htim;
        TIM24_Manage_Object.Callback_Function = Callback_Function;
    }
}

/**
 * @brief HAL库TIM定时器中断
 *
 * @param htim TIM编号
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 判断程序初始化完成
    if (init_finished == false)
    {
        return;
    }

    // 选择回调函数
    if (htim->Instance == TIM1)
    {
        if(TIM1_Manage_Object.Callback_Function != nullptr)
        {
            TIM1_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM2)
    {
        if(TIM2_Manage_Object.Callback_Function != nullptr)
        {
            TIM2_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM3)
    {
        if(TIM3_Manage_Object.Callback_Function != nullptr)
        {
            TIM3_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM4)
    {
        if(TIM4_Manage_Object.Callback_Function != nullptr)
        {
            TIM4_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM5)
    {
        if(TIM5_Manage_Object.Callback_Function != nullptr)
        {
            TIM5_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM6)
    {
        if(TIM6_Manage_Object.Callback_Function != nullptr)
        {
            TIM6_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM7)
    {
        if(TIM7_Manage_Object.Callback_Function != nullptr)
        {
            TIM7_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM8)
    {
        if(TIM8_Manage_Object.Callback_Function != nullptr)
        {
            TIM8_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM12)
    {
        if(TIM12_Manage_Object.Callback_Function != nullptr)
        {
            TIM12_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM13)
    {
        if(TIM13_Manage_Object.Callback_Function != nullptr)
        {
            TIM13_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM14)
    {
        if(TIM14_Manage_Object.Callback_Function != nullptr)
        {
            TIM14_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM15)
    {
        if(TIM15_Manage_Object.Callback_Function != nullptr)
        {
            TIM15_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM16)
    {
        if(TIM16_Manage_Object.Callback_Function != nullptr)
        {
            TIM16_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM17)
    {
        if(TIM17_Manage_Object.Callback_Function != nullptr)
        {
            TIM17_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM23)
    {
        if(TIM23_Manage_Object.Callback_Function != nullptr)
        {
            TIM23_Manage_Object.Callback_Function();
        }
    }
    else if (htim->Instance == TIM24)
    {
        if(TIM24_Manage_Object.Callback_Function != nullptr)
        {
            TIM24_Manage_Object.Callback_Function();
        }
    }
}

/*----------------------------------------------------------------------------*/
