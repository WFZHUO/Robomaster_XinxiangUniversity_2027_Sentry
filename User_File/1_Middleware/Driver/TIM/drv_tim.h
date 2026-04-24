/**
 * @file drv_tim.h
 * @author WangFonzhuo
 * @brief TIM通用接口
 * @version 1.0
 * @date 2025-12-30 26赛季定稿
 * @date 2026-04-18 27赛季
 */

#ifndef DRV_TIM_H
#define DRV_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32h7xx_hal.h"
#include <stdbool.h>

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/**
 * @brief TIM定时器回调函数数据类型
 */
typedef void (*TIM_Call_Back)(void);

/**
 * @brief TIM定时器处理结构体
 */
struct Struct_TIM_Manage_Object
{
    TIM_HandleTypeDef *TIM_Handler;
    TIM_Call_Back Callback_Function;
};

/* Exported variables --------------------------------------------------------*/

// 全局初始化完成标志位
extern bool init_finished;

// 声明TIM定时器管理对象
extern Struct_TIM_Manage_Object TIM1_Manage_Object;
extern Struct_TIM_Manage_Object TIM2_Manage_Object;
extern Struct_TIM_Manage_Object TIM3_Manage_Object;
extern Struct_TIM_Manage_Object TIM4_Manage_Object;
extern Struct_TIM_Manage_Object TIM5_Manage_Object;
extern Struct_TIM_Manage_Object TIM6_Manage_Object;
extern Struct_TIM_Manage_Object TIM7_Manage_Object;
extern Struct_TIM_Manage_Object TIM8_Manage_Object;
extern Struct_TIM_Manage_Object TIM12_Manage_Object;
extern Struct_TIM_Manage_Object TIM13_Manage_Object;
extern Struct_TIM_Manage_Object TIM14_Manage_Object;
extern Struct_TIM_Manage_Object TIM15_Manage_Object;
extern Struct_TIM_Manage_Object TIM16_Manage_Object;
extern Struct_TIM_Manage_Object TIM17_Manage_Object;
extern Struct_TIM_Manage_Object TIM23_Manage_Object;
extern Struct_TIM_Manage_Object TIM24_Manage_Object;

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 初始化TIM定时器
 *
 * @param htim 定时器编号
 * @param Callback_Function 处理回调函数
 */
void TIM_Init(TIM_HandleTypeDef *htim, TIM_Call_Back Callback_Function);


#ifdef __cplusplus
}
#endif

#endif /* DRV_TIM_H */

/*----------------------------------------------------------------------------*/
