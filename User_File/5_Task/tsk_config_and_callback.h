/**
 * @file tsk_config_and_callback.h
 * @author WangFonzhuo
 * @brief 当成mian.h来用
 * @version 1.0
 * @date 2025-12-30 26赛季定稿
 * @date 2026-04-18 27赛季
 */

#ifndef TSK_CONFIG_AND_CALLBACK_H
#define TSK_CONFIG_AND_CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32h7xx_hal.h"

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 主程序任务初始化函数
 */
void Task_Init();

/**
 * @brief 主程序任务循环函数
 */
void Task_Loop();

/* Exported function definitions ---------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* TSK_CONFIG_AND_CALLBACK_H */

/*----------------------------------------------------------------------------*/
