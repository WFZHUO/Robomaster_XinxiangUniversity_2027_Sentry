/**
 * @file bsp_BuzzerSongs.h
 * @author WangFonzhuo
 * @brief 蜂鸣器歌曲封装
 * @version 1.0
 * @date 2026-04-19 27赛季
 */

#ifndef BSP_BUZZER_SONGS_H
#define BSP_BUZZER_SONGS_H

/* Includes ------------------------------------------------------------------*/

#include "bsp_buzzer.h"
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/

/**
 * @brief 蜂鸣器音符
 */
typedef struct
{
    float frequency;      // 频率, Hz; 休止符填 0.0f
    float beat;           // 拍数, 例如 1 / 2 / 0.5 / 0.2
} Struct_BuzzerSongNote;

/* Exported function prototypes ----------------------------------------------*/

/**
 * @brief 播放一段曲谱
 * @param song 曲谱数组首地址
 * @param note_num 音符数量
 * @param unit_ms 每拍时长, 单位 ms
 * @param loudness 响度, 0~1
 */
void BuzzerSongs_Play(const Struct_BuzzerSongNote *song, uint32_t note_num, uint32_t unit_ms = 200U, float loudness = 0.90f);

/**
 * @brief 播放 GALA《你》
 */
void BuzzerSongs_Play_Gala_You(void);

/**
 * @brief 播放《Love Theme from The Godfather》
 */
void BuzzerSongs_Play_Godfather(void);

/**
 * @brief 播放《See You Again》
 */
void BuzzerSongs_Play_See_You_Again(void);

#endif /* BSP_BUZZER_SONGS_H */

/*----------------------------------------------------------------------------*/
