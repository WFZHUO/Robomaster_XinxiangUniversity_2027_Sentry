/**
 * @file sys_timestamp.cpp
 * @author WangFonzhuo
 * @brief 时间计算相关支持库
 * @version 1.0
 * @date 2026-04-25 27赛季
 */

/**
 * 要求: 使能并绑定一个定时器, 开外部中断, PSC分频到1MHz, ARR为3600000000
 * 保证arr计数器1us增一次, 1h触发一次外部中断
 */

/* Includes ------------------------------------------------------------------*/

#include "sys_timestamp.h"

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 全局时间戳对象
Class_Timestamp SYS_Timestamp;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化时间戳
 *
 * @param __TIM_Manage_Object 绑定的定时器, psc后为1MHz, arr要求是3 600 000 000, arr至少32位的寄存器
 */
void Class_Timestamp::Init(TIM_HandleTypeDef *htim)
{
    TIM_Handler = htim;
}

/**
 * @brief TIM定时器回调函数, 每3600s调用一次
 *
 */
void Class_Timestamp::TIM_3600s_PeriodElapsedCallback()
{
    TIM_Overflow_Count++;
}

/**
 * @brief 计算当前时间戳, 单位微秒
 *
 * @return uint64_t 当前时间戳
 */
uint64_t Class_Timestamp::Calculate_Timestamp() const
{
    if (TIM_Handler == nullptr)
    {
        return 0;
    }

    uint32_t overflow_1;
    uint32_t overflow_2;
    uint32_t cnt;

    do
    {
        // 避免刚好溢出时 overflow 和 CNT 读到不一致
        overflow_1 = TIM_Overflow_Count;
        cnt = TIM_Handler->Instance->CNT;
        overflow_2 = TIM_Overflow_Count;
    } while (overflow_1 != overflow_2);

    return (uint64_t)overflow_1 * 3600000000ULL + (uint64_t)cnt;
}

/**
 * @brief 延迟指定秒数
 *
 * @param Second 延迟秒数
 */
void Namespace_SYS_Timestamp::Delay_Second(uint32_t Second)
{
    const uint64_t start_time = SYS_Timestamp.Get_Current_Timestamp();
    const uint64_t delay_time = (uint64_t)Second * 1000000ULL;

    while ((SYS_Timestamp.Get_Current_Timestamp() - start_time) < delay_time)
    {
    }
}

/**
 * @brief 延迟指定毫秒数
 *
 * @param Millisecond 延迟毫秒数
 */
void Namespace_SYS_Timestamp::Delay_Millisecond(uint32_t Millisecond)
{
    const uint64_t start_time = SYS_Timestamp.Get_Current_Timestamp();
    const uint64_t delay_time = (uint64_t)Millisecond * 1000ULL;

    while ((SYS_Timestamp.Get_Current_Timestamp() - start_time) < delay_time)
    {
    }
}

/**
 * @brief 延迟指定微秒数
 *
 * @param Microsecond 延迟微秒数
 */
void Namespace_SYS_Timestamp::Delay_Microsecond(uint32_t Microsecond)
{
    const uint64_t start_time = SYS_Timestamp.Get_Current_Timestamp();
    const uint64_t delay_time = (uint64_t)Microsecond;

    while ((SYS_Timestamp.Get_Current_Timestamp() - start_time) < delay_time)
    {
    }
}

/*----------------------------------------------------------------------------*/
