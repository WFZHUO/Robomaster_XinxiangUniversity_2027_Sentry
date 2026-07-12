/**
 * @file bsp_ws2812.cpp
 * @author WangFonzhuo
 * @brief 板载WS2812B-2020灯珠
 * @version 1.0
 * @date 2026-07-12 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "bsp_ws2812.h"
#include "spi.h"

#include <string.h>

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// 纯黑
const Struct_WS2812_Color WS2812_COLOR_BLACK = {0, 0, 0};
// 纯白
const Struct_WS2812_Color WS2812_COLOR_WHITE = {255, 255, 255};

// 纯红
const Struct_WS2812_Color WS2812_COLOR_RED = {255, 0, 0};
// 纯绿
const Struct_WS2812_Color WS2812_COLOR_GREEN = {0, 255, 0};
// 纯蓝
const Struct_WS2812_Color WS2812_COLOR_BLUE = {0, 0, 255};

// 黄色
const Struct_WS2812_Color WS2812_COLOR_YELLOW = {255, 255, 0};
// 青色
const Struct_WS2812_Color WS2812_COLOR_CYAN = {0, 255, 255};
// 品红
const Struct_WS2812_Color WS2812_COLOR_MAGENTA = {255, 0, 255};

// 全局WS2812对象
Class_WS2812 BSP_WS2812;

/* Function prototypes -------------------------------------------------------*/

static void HSV_To_RGB(uint8_t hue, uint8_t &r, uint8_t &g, uint8_t &b);

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 初始化WS2812, 默认灯灭
 *
 * @param __hspi 绑定的SPI, 为空时默认使用hspi6
 * @param __Red 红色分量
 * @param __Green 绿色分量
 * @param __Blue 蓝色分量
 */
void Class_WS2812::Init(SPI_HandleTypeDef *__hspi,
                        uint8_t __Red,
                        uint8_t __Green,
                        uint8_t __Blue)
{
    if (__hspi == nullptr)
    {
        hspi = &hspi6;
    }
    else
    {
        hspi = __hspi;
    }

    SPI_Init(hspi, nullptr);

    Color.Red = __Red;
    Color.Green = __Green;
    Color.Blue = __Blue;

    Write();
}

/**
 * @brief 彩虹渐变效果
 */
void Class_WS2812::Rainbow()
{
    static uint8_t Hue = 0;

    HSV_To_RGB(
        Hue,
        Color.Red,
        Color.Green,
        Color.Blue
    );

    Hue++;
}

/**
 * @brief 发送当前颜色到WS2812
 *
 * @return uint8_t HAL执行状态
 */
uint8_t Class_WS2812::Write()
{
    if (hspi == nullptr)
    {
        return HAL_ERROR;
    }

    uint16_t tx_index = 0;

    // WS2812实际发送顺序是GRB
    uint8_t color_data[3] =
    {
        Color.Green,
        Color.Red,
        Color.Blue,
    };

    for (uint8_t color_index = 0; color_index < 3; color_index++)
    {
        for (int8_t bit_index = 7; bit_index >= 0; bit_index--)
        {
            if ((color_data[color_index] & (1U << bit_index)) != 0U)
            {
                Tx_Buffer[tx_index] = LEVEL_1;
            }
            else
            {
                Tx_Buffer[tx_index] = LEVEL_0;
            }

            tx_index++;
        }
    }

    // Reset低电平
    memset(&Tx_Buffer[tx_index], 0x00, WS2812_RESET_BYTES);
    tx_index += WS2812_RESET_BYTES;

    return SPI_Transmit_Data(hspi,
                            Tx_Buffer,
                            tx_index,
                            nullptr,
                            0U,
                            GPIO_PIN_RESET);
}

/**
 * @brief 10ms定时刷新回调
 */
void Class_WS2812::TIM_10ms_Write_PeriodElapsedCallback()
{
    Write();
}

/**
 * @brief HSV转RGB
 *
 * @param hue 色相, 范围0~255
 * @param r 红色分量指针
 * @param g 绿色分量指针
 * @param b 蓝色分量指针
 */
static void HSV_To_RGB(
        uint8_t hue,
        uint8_t &r,
        uint8_t &g,
        uint8_t &b)
{
    uint8_t region;
    uint8_t remainder;
    uint8_t p;
    uint8_t q;
    uint8_t t;

    region = hue / 43;
    remainder = (hue - (region * 43)) * 6;
    p = 0;
    q = 255 - remainder;
    t = remainder;

    switch(region)
    {
        case 0:
            r = 255;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = 255;
            b = p;
            break;
        case 2:
            r = p;
            g = 255;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = 255;
            break;
        case 4:
            r = t;
            g = p;
            b = 255;
            break;
        default:
            r = 255;
            g = p;
            b = q;
            break;
    }
}

/*----------------------------------------------------------------------------*/