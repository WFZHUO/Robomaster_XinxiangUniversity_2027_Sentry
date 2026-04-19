/**
 * @file bsp_buzzer.h
 * @author WangFonzhuo
 * @brief 板载蜂鸣器
 * @version 1.0
 * @date 2026-04-18 27赛季
 */

/**
 * Cube中, 需设置定时器的一个计数周期为1MHz, 初始ARR请设置为250 - 1, 这样对应的初始频率为4000Hz
 * T = (ARR + 1) / 1MHz
 * freq = 1 / T = 1MHz / (ARR + 1)
 *
 * 此外, 该蜂鸣器有效音域为G3~ A7
 */

#ifndef BSP_BUZZER_H
#define BSP_BUZZER_H

/* Includes ------------------------------------------------------------------*/

#include "tim.h"
#include "stm32h7xx_hal.h"

/* Exported macros -----------------------------------------------------------*/

// 常见音符频率
extern const float BUZZER_FREQUENCY_A0;
extern const float BUZZER_FREQUENCY_A0_SHARP;
extern const float BUZZER_FREQUENCY_B0_FLAT;
extern const float BUZZER_FREQUENCY_B0;
extern const float BUZZER_FREQUENCY_C1;
extern const float BUZZER_FREQUENCY_C1_SHARP;
extern const float BUZZER_FREQUENCY_D1_FLAT;
extern const float BUZZER_FREQUENCY_D1;
extern const float BUZZER_FREQUENCY_D1_SHARP;
extern const float BUZZER_FREQUENCY_E1_FLAT;
extern const float BUZZER_FREQUENCY_E1;
extern const float BUZZER_FREQUENCY_F1;
extern const float BUZZER_FREQUENCY_F1_SHARP;
extern const float BUZZER_FREQUENCY_G1_FLAT;
extern const float BUZZER_FREQUENCY_G1;
extern const float BUZZER_FREQUENCY_G1_SHARP;
extern const float BUZZER_FREQUENCY_A1_FLAT;
extern const float BUZZER_FREQUENCY_A1;
extern const float BUZZER_FREQUENCY_A1_SHARP;
extern const float BUZZER_FREQUENCY_B1_FLAT;
extern const float BUZZER_FREQUENCY_B1;
extern const float BUZZER_FREQUENCY_C2;
extern const float BUZZER_FREQUENCY_C2_SHARP;
extern const float BUZZER_FREQUENCY_D2_FLAT;
extern const float BUZZER_FREQUENCY_D2;
extern const float BUZZER_FREQUENCY_D2_SHARP;
extern const float BUZZER_FREQUENCY_E2_FLAT;
extern const float BUZZER_FREQUENCY_E2;
extern const float BUZZER_FREQUENCY_F2;
extern const float BUZZER_FREQUENCY_F2_SHARP;
extern const float BUZZER_FREQUENCY_G2_FLAT;
extern const float BUZZER_FREQUENCY_G2;
extern const float BUZZER_FREQUENCY_G2_SHARP;
extern const float BUZZER_FREQUENCY_A2_FLAT;
extern const float BUZZER_FREQUENCY_A2;
extern const float BUZZER_FREQUENCY_A2_SHARP;
extern const float BUZZER_FREQUENCY_B2_FLAT;
extern const float BUZZER_FREQUENCY_B2;
extern const float BUZZER_FREQUENCY_C3;
extern const float BUZZER_FREQUENCY_C3_SHARP;
extern const float BUZZER_FREQUENCY_D3_FLAT;
extern const float BUZZER_FREQUENCY_D3;
extern const float BUZZER_FREQUENCY_D3_SHARP;
extern const float BUZZER_FREQUENCY_E3_FLAT;
extern const float BUZZER_FREQUENCY_E3;
extern const float BUZZER_FREQUENCY_F3;
extern const float BUZZER_FREQUENCY_F3_SHARP;
extern const float BUZZER_FREQUENCY_G3_FLAT;
extern const float BUZZER_FREQUENCY_G3;
extern const float BUZZER_FREQUENCY_G3_SHARP;
extern const float BUZZER_FREQUENCY_A3_FLAT;
extern const float BUZZER_FREQUENCY_A3;
extern const float BUZZER_FREQUENCY_A3_SHARP;
extern const float BUZZER_FREQUENCY_B3_FLAT;
extern const float BUZZER_FREQUENCY_B3;
extern const float BUZZER_FREQUENCY_C4;
extern const float BUZZER_FREQUENCY_C4_SHARP;
extern const float BUZZER_FREQUENCY_D4_FLAT;
extern const float BUZZER_FREQUENCY_D4;
extern const float BUZZER_FREQUENCY_D4_SHARP;
extern const float BUZZER_FREQUENCY_E4_FLAT;
extern const float BUZZER_FREQUENCY_E4;
extern const float BUZZER_FREQUENCY_F4;
extern const float BUZZER_FREQUENCY_F4_SHARP;
extern const float BUZZER_FREQUENCY_G4_FLAT;
extern const float BUZZER_FREQUENCY_G4;
extern const float BUZZER_FREQUENCY_G4_SHARP;
extern const float BUZZER_FREQUENCY_A4_FLAT;
extern const float BUZZER_FREQUENCY_A4;
extern const float BUZZER_FREQUENCY_A4_SHARP;
extern const float BUZZER_FREQUENCY_B4_FLAT;
extern const float BUZZER_FREQUENCY_B4;
extern const float BUZZER_FREQUENCY_C5;
extern const float BUZZER_FREQUENCY_C5_SHARP;
extern const float BUZZER_FREQUENCY_D5_FLAT;
extern const float BUZZER_FREQUENCY_D5;
extern const float BUZZER_FREQUENCY_D5_SHARP;
extern const float BUZZER_FREQUENCY_E5_FLAT;
extern const float BUZZER_FREQUENCY_E5;
extern const float BUZZER_FREQUENCY_F5;
extern const float BUZZER_FREQUENCY_F5_SHARP;
extern const float BUZZER_FREQUENCY_G5_FLAT;
extern const float BUZZER_FREQUENCY_G5;
extern const float BUZZER_FREQUENCY_G5_SHARP;
extern const float BUZZER_FREQUENCY_A5_FLAT;
extern const float BUZZER_FREQUENCY_A5;
extern const float BUZZER_FREQUENCY_A5_SHARP;
extern const float BUZZER_FREQUENCY_B5_FLAT;
extern const float BUZZER_FREQUENCY_B5;
extern const float BUZZER_FREQUENCY_C6;
extern const float BUZZER_FREQUENCY_C6_SHARP;
extern const float BUZZER_FREQUENCY_D6_FLAT;
extern const float BUZZER_FREQUENCY_D6;
extern const float BUZZER_FREQUENCY_D6_SHARP;
extern const float BUZZER_FREQUENCY_E6_FLAT;
extern const float BUZZER_FREQUENCY_E6;
extern const float BUZZER_FREQUENCY_F6;
extern const float BUZZER_FREQUENCY_F6_SHARP;
extern const float BUZZER_FREQUENCY_G6_FLAT;
extern const float BUZZER_FREQUENCY_G6;
extern const float BUZZER_FREQUENCY_G6_SHARP;
extern const float BUZZER_FREQUENCY_A6_FLAT;
extern const float BUZZER_FREQUENCY_A6;
extern const float BUZZER_FREQUENCY_A6_SHARP;
extern const float BUZZER_FREQUENCY_B6_FLAT;
extern const float BUZZER_FREQUENCY_B6;
extern const float BUZZER_FREQUENCY_C7;
extern const float BUZZER_FREQUENCY_C7_SHARP;
extern const float BUZZER_FREQUENCY_D7_FLAT;
extern const float BUZZER_FREQUENCY_D7;
extern const float BUZZER_FREQUENCY_D7_SHARP;
extern const float BUZZER_FREQUENCY_E7_FLAT;
extern const float BUZZER_FREQUENCY_E7;
extern const float BUZZER_FREQUENCY_F7;
extern const float BUZZER_FREQUENCY_F7_SHARP;
extern const float BUZZER_FREQUENCY_G7_FLAT;
extern const float BUZZER_FREQUENCY_G7;
extern const float BUZZER_FREQUENCY_G7_SHARP;
extern const float BUZZER_FREQUENCY_A7_FLAT;
extern const float BUZZER_FREQUENCY_A7;
extern const float BUZZER_FREQUENCY_A7_SHARP;
extern const float BUZZER_FREQUENCY_B7_FLAT;
extern const float BUZZER_FREQUENCY_B7;
extern const float BUZZER_FREQUENCY_C8;

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 板载蜂鸣器
 */
class Class_Buzzer
{
public:
    /*
     * @brief 初始化蜂鸣器
     * @param __Frequency 蜂鸣器频率, 单位Hz
     * @param __Loudness 蜂鸣器响度, 0-1
    */
    void Init(float __Frequency = 4000.0f, float __Loudness = 0.0f);

    /**
     * @brief 设置蜂鸣器频率
     * @param __Frequency 蜂鸣器频率, 单位Hz
     */
    inline void Set_Frequency(float __Frequency);

    /**
     * @brief 设置蜂鸣器响度
     * @param __Loudness 蜂鸣器响度, 0-1
     */
    inline void Set_Loudness(float __Loudness);

    /**
     * @brief 设置蜂鸣器声音
     * @param __Frequency 蜂鸣器频率, 单位Hz
     * @param __Loudness 蜂鸣器响度, 0-1
     */
    inline void Set_Sound(float __Frequency, float __Loudness);


    /**
     * @brief 播放一个音符
     * @param __Frequency 音符频率, 单位Hz
     * @param __Duration_ms 音符持续时间, 单位ms
     * @param __Loudness 音符响度, 0-1
     * @param __Gap_ms 音符间隔时间, 单位ms
     */
    void Play_Note(float __Frequency, uint32_t __Duration_ms, float __Loudness = 1.0f, uint32_t __Gap_ms = 20);

private:
    // 绑定的定时器资源
    TIM_HandleTypeDef *htim = nullptr;
    uint32_t TIM_Channel = 0;

    // 当前蜂鸣器参数
    float Frequency = 4000.0f;
    float Loudness = 0.0f;
};

/* Exported variables --------------------------------------------------------*/

// 全局蜂鸣器对象
extern Class_Buzzer BSP_Buzzer;

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 设置蜂鸣器频率
 * @param __Frequency 蜂鸣器频率, 单位Hz
 */
inline void Class_Buzzer::Set_Frequency(float __Frequency)
{
    Frequency = (__Frequency < 0.0f) ? 0.0f : ((__Frequency > 20000.0f) ? 20000.0f : __Frequency);

    __HAL_TIM_DISABLE(htim);
    __HAL_TIM_SET_COUNTER(htim, 0);

    float arr;
    if (Frequency <= 0.0f)
    {
        arr = 250.0f - 1.0f;
        __HAL_TIM_SetAutoreload(htim, (uint32_t)arr);
        __HAL_TIM_SetCompare(htim, TIM_Channel, 0);
    }
    else
    {
        arr = (1000000.0f / Frequency) - 1.0f;
        float ccr = (arr + 1.0f) * Loudness * 0.5f;

        __HAL_TIM_SetAutoreload(htim, (uint32_t)arr);
        __HAL_TIM_SetCompare(htim, TIM_Channel, (uint32_t)ccr);
    }

    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);
    __HAL_TIM_ENABLE(htim);
}

/**
 * @brief 设置蜂鸣器响度
 *
 * @param Loudness 蜂鸣器响度, 0-1
 */
inline void Class_Buzzer::Set_Loudness(float __Loudness)
{
    Loudness = (__Loudness < 0.0f) ? 0.0f : ((__Loudness > 1.0f) ? 1.0f : __Loudness);

    __HAL_TIM_DISABLE(htim);
    __HAL_TIM_SET_COUNTER(htim, 0);

    float arr;
    if (Frequency <= 0.0f)
    {
        arr = 250.0f - 1.0f;
        __HAL_TIM_SetAutoreload(htim, (uint32_t)arr);
        __HAL_TIM_SetCompare(htim, TIM_Channel, 0);
    }
    else
    {
        arr = (1000000.0f / Frequency) - 1.0f;
        float ccr = (arr + 1.0f) * Loudness * 0.5f;

        __HAL_TIM_SetAutoreload(htim, (uint32_t)arr);
        __HAL_TIM_SetCompare(htim, TIM_Channel, (uint32_t)ccr);
    }

    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);
    __HAL_TIM_ENABLE(htim);
}

/**
 * @brief 设置蜂鸣器声音
 *
 * @param Frequency 蜂鸣器频率, 单位Hz
 * @param Loudness 蜂鸣器响度, 0-1
 */
inline void Class_Buzzer::Set_Sound(float __Frequency, float __Loudness)
{
    Frequency = (__Frequency < 0.0f) ? 0.0f : ((__Frequency > 20000.0f) ? 20000.0f : __Frequency);
    Loudness  = (__Loudness < 0.0f) ? 0.0f : ((__Loudness > 1.0f) ? 1.0f : __Loudness);

    __HAL_TIM_DISABLE(htim);
    __HAL_TIM_SET_COUNTER(htim, 0);

    float arr;
    if (Frequency <= 0.0f || Loudness <= 0.0f)
    {
        arr = 250.0f - 1.0f;
        __HAL_TIM_SetAutoreload(htim, (uint32_t)arr);
        __HAL_TIM_SetCompare(htim, TIM_Channel, 0);
    }
    else
    {
        arr = (1000000.0f / Frequency) - 1.0f;
        float ccr = (arr + 1.0f) * Loudness * 0.5f;

        __HAL_TIM_SetAutoreload(htim, (uint32_t)arr);
        __HAL_TIM_SetCompare(htim, TIM_Channel, (uint32_t)ccr);
    }

    HAL_TIM_GenerateEvent(htim, TIM_EVENTSOURCE_UPDATE);
    __HAL_TIM_ENABLE(htim);
}

#endif /* BSP_BUZZER_H */

/*----------------------------------------------------------------------------*/
