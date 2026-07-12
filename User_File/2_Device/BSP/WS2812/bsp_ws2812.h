/**
 * @file bsp_ws2812.h
 * @author WangFonzhuo
 * @brief 板载WS2812B-2020灯珠
 * @version 1.0
 * @date 2026-07-12 27赛季
 */

#ifndef BSP_WS2812_H
#define BSP_WS2812_H

/* Includes ------------------------------------------------------------------*/

#include "drv_spi.h"
#include "stm32h7xx_hal.h"

#include <stdint.h>

/* Exported macros -----------------------------------------------------------*/

// WS2812灯珠数量
#define WS2812_LED_NUM          (1U)

// 每颗WS2812为24bit, 每1bit编码为1个SPI字节
#define WS2812_BYTES_PER_LED    (24U)

// SPI 6MHz时, 240字节低电平约320us, 满足WS2812 Reset时间
#define WS2812_RESET_BYTES      (240U)

// WS2812发送缓冲区大小
#define WS2812_TX_BUFFER_SIZE   (WS2812_LED_NUM * WS2812_BYTES_PER_LED + WS2812_RESET_BYTES)

#if (WS2812_TX_BUFFER_SIZE > SPI_BUFFER_SIZE)
#error "WS2812_TX_BUFFER_SIZE is larger than SPI_BUFFER_SIZE"
#endif

/* Exported types ------------------------------------------------------------*/

/**
 * @brief WS2812颜色值
 */
struct Struct_WS2812_Color
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};

/**
 * @brief 板载WS2812B-2020灯珠
 */
class Class_WS2812
{
public:
    /**
     * @brief 初始化WS2812, 默认灯灭
     *
     * @param __hspi 绑定的SPI, 为空时默认使用hspi6
     * @param __Red 红色分量
     * @param __Green 绿色分量
     * @param __Blue 蓝色分量
     */
    void Init(SPI_HandleTypeDef *__hspi = nullptr,
              uint8_t __Red = 0,
              uint8_t __Green = 0,
              uint8_t __Blue = 0);

    /**
     * @brief 设置红色
     *
     * @param __Red 红色分量
     */
    inline void Set_Red(uint8_t __Red);

    /**
     * @brief 设置绿色
     *
     * @param __Green 绿色分量
     */
    inline void Set_Green(uint8_t __Green);

    /**
     * @brief 设置蓝色
     *
     * @param __Blue 蓝色分量
     */
    inline void Set_Blue(uint8_t __Blue);

    /**
     * @brief 设置RGB颜色
     *
     * @param __Red 红色分量
     * @param __Green 绿色分量
     * @param __Blue 蓝色分量
     */
    inline void Set_RGB(uint8_t __Red, uint8_t __Green, uint8_t __Blue);

    /**
     * @brief 设置颜色
     *
     * @param __Color 颜色值
     * @param __Brightness 亮度, 范围0~1
     */
    inline void Set_Color(const Struct_WS2812_Color &__Color, float __Brightness = 1.0f);

    /**
     * @brief 彩虹渐变效果
     */
    void Rainbow();

    /**
     * @brief 发送当前颜色到WS2812
     *
     * @return uint8_t HAL执行状态
     */
    uint8_t Write();

    /**
     * @brief 10ms定时刷新回调
     */
    void TIM_10ms_Write_PeriodElapsedCallback();

private:
    // 绑定的SPI
    SPI_HandleTypeDef *hspi = nullptr;

    // WS2812颜色顺序是GRB, 对外仍使用RGB
    Struct_WS2812_Color Color = {0, 0, 0};

    // SPI 6MHz编码:
    // 0 -> 0xC0 = 11000000, 高电平约333ns, 低电平约1000ns
    // 1 -> 0xF0 = 11110000, 高电平约667ns, 低电平约667ns
    // 整体向右偏移一位, 防止第一个边沿不稳定
    const uint8_t LEVEL_0 = 0x60;
    const uint8_t LEVEL_1 = 0x78;

    // WS2812发送缓冲区
    uint8_t Tx_Buffer[WS2812_TX_BUFFER_SIZE] = {};
};

/* Exported variables --------------------------------------------------------*/

// WS2812颜色常量
extern const Struct_WS2812_Color WS2812_COLOR_BLACK;
extern const Struct_WS2812_Color WS2812_COLOR_WHITE;
extern const Struct_WS2812_Color WS2812_COLOR_RED;
extern const Struct_WS2812_Color WS2812_COLOR_GREEN;
extern const Struct_WS2812_Color WS2812_COLOR_BLUE;
extern const Struct_WS2812_Color WS2812_COLOR_YELLOW;
extern const Struct_WS2812_Color WS2812_COLOR_CYAN;
extern const Struct_WS2812_Color WS2812_COLOR_MAGENTA;

// 全局WS2812对象
extern Class_WS2812 BSP_WS2812;

/* Exported function prototypes ----------------------------------------------*/

/* Exported function definitions ---------------------------------------------*/

/**
 * @brief 设置红色
 *
 * @param __Red 红色分量
 */
inline void Class_WS2812::Set_Red(uint8_t __Red)
{
    Color.Red = __Red;
    Color.Green = 0;
    Color.Blue = 0;
}

/**
 * @brief 设置绿色
 *
 * @param __Green 绿色分量
 */
inline void Class_WS2812::Set_Green(uint8_t __Green)
{
    Color.Red = 0;
    Color.Green = __Green;
    Color.Blue = 0;
}

/**
 * @brief 设置蓝色
 *
 * @param __Blue 蓝色分量
 */
inline void Class_WS2812::Set_Blue(uint8_t __Blue)
{
    Color.Red = 0;
    Color.Green = 0;
    Color.Blue = __Blue;
}

/**
 * @brief 设置RGB颜色
 *
 * @param __Red 红色分量
 * @param __Green 绿色分量
 * @param __Blue 蓝色分量
 */
inline void Class_WS2812::Set_RGB(uint8_t __Red, uint8_t __Green, uint8_t __Blue)
{
    Color.Red = __Red;
    Color.Green = __Green;
    Color.Blue = __Blue;
}

/**
 * @brief 设置颜色
 *
 * @param __Color 颜色值
 * @param __Brightness 亮度, 范围0~1
 */
inline void Class_WS2812::Set_Color(const Struct_WS2812_Color &__Color, float __Brightness)
{
    if (__Brightness < 0.0f)
    {
        __Brightness = 0.0f;
    }
    else if (__Brightness > 1.0f)
    {
        __Brightness = 1.0f;
    }

    Color.Red = static_cast<uint8_t>(static_cast<float>(__Color.Red) * __Brightness);
    Color.Green = static_cast<uint8_t>(static_cast<float>(__Color.Green) * __Brightness);
    Color.Blue = static_cast<uint8_t>(static_cast<float>(__Color.Blue) * __Brightness);
}

#endif /* BSP_WS2812_H */

/*----------------------------------------------------------------------------*/