/**
 * @file alg_basic.cpp
 * @author yssickjgd
 * @author WangFonzhuo
 * @brief 基础数学库
 * @version 1.0
 * @date 2026-04-27 27赛季
 */

/* Includes ------------------------------------------------------------------*/

#include "alg_basic.h"
#include "alg_arm_math.h"

#include <math.h>
#include <string.h>

/* Macros --------------------------------------------------------------------*/

/* Types ---------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/

// rpm换算到rad/s
const float BASIC_MATH_RPM_TO_RADPS = 2.0f * PI / 60.0f;
// deg换算到rad
const float BASIC_MATH_DEG_TO_RAD = PI / 180.0f;
// 摄氏度换算到开氏度
const float BASIC_MATH_CELSIUS_TO_KELVIN = 273.15f;

/* Function prototypes -------------------------------------------------------*/

/* Function definitions ------------------------------------------------------*/

/**
 * @brief 布尔值反转
 * @param Value 布尔值地址
 */
void Basic_Math_Boolean_Logical_Not(bool *Value)
{
    if (Value == nullptr)
    {
        return;
    }

    *Value = !(*Value);
}

/**
 * @brief 16位大小端原地转换
 * @param Address 地址
 */
void Basic_Math_Endian_Reverse_16(void *Address)
{
    if (Address == nullptr)
    {
        return;
    }

    uint8_t *tmp_address = static_cast<uint8_t *>(Address);

    uint8_t tmp = tmp_address[0];
    tmp_address[0] = tmp_address[1];
    tmp_address[1] = tmp;
}

/**
 * @brief 16位大小端转换
 * @param Source 源数据地址
 * @param Destination 目标存储地址, 可为nullptr
 * @return uint16_t 转换后的数值
 */
uint16_t Basic_Math_Endian_Reverse_16(void *Source, void *Destination)
{
    if (Source == nullptr)
    {
        return 0;
    }

    const uint8_t *tmp_source = static_cast<const uint8_t *>(Source);

    uint16_t tmp_value =
        (static_cast<uint16_t>(tmp_source[0]) << 8) |
        (static_cast<uint16_t>(tmp_source[1]));

    if (Destination != nullptr)
    {
        uint8_t *tmp_destination = static_cast<uint8_t *>(Destination);
        tmp_destination[0] = tmp_source[1];
        tmp_destination[1] = tmp_source[0];
    }

    return tmp_value;
}

/**
 * @brief 32位大小端原地转换
 * @param Address 地址
 */
void Basic_Math_Endian_Reverse_32(void *Address)
{
    if (Address == nullptr)
    {
        return;
    }

    uint8_t *tmp_address = static_cast<uint8_t *>(Address);

    uint8_t tmp_0 = tmp_address[0];
    uint8_t tmp_1 = tmp_address[1];

    tmp_address[0] = tmp_address[3];
    tmp_address[1] = tmp_address[2];
    tmp_address[2] = tmp_1;
    tmp_address[3] = tmp_0;
}

/**
 * @brief 32位大小端转换
 * @param Source 源数据地址
 * @param Destination 目标存储地址, 可为nullptr
 * @return uint32_t 转换后的数值
 */
uint32_t Basic_Math_Endian_Reverse_32(void *Source, void *Destination)
{
    if (Source == nullptr)
    {
        return 0;
    }

    const uint8_t *tmp_source = static_cast<const uint8_t *>(Source);

    uint32_t tmp_value =
        (static_cast<uint32_t>(tmp_source[0]) << 24) |
        (static_cast<uint32_t>(tmp_source[1]) << 16) |
        (static_cast<uint32_t>(tmp_source[2]) << 8) |
        (static_cast<uint32_t>(tmp_source[3]));

    if (Destination != nullptr)
    {
        uint8_t *tmp_destination = static_cast<uint8_t *>(Destination);
        tmp_destination[0] = tmp_source[3];
        tmp_destination[1] = tmp_source[2];
        tmp_destination[2] = tmp_source[1];
        tmp_destination[3] = tmp_source[0];
    }

    return tmp_value;
}

/**
 * @brief 8位求和
 * @param Address 起始地址
 * @param Length 被加的数据的数量, 注意不是字节数
 * @return uint8_t 结果
 */
uint8_t Basic_Math_Sum_8(const uint8_t *Address, uint32_t Length)
{
    if (Address == nullptr)
    {
        return 0;
    }

    uint8_t sum = 0;

    for (uint32_t i = 0; i < Length; i++)
    {
        sum += Address[i];
    }

    return sum;
}

/**
 * @brief 16位求和
 * @param Address 起始地址
 * @param Length 被加的数据的数量, 注意不是字节数
 * @return uint16_t 结果
 */
uint16_t Basic_Math_Sum_16(const uint16_t *Address, uint32_t Length)
{
    if (Address == nullptr)
    {
        return 0;
    }

    uint16_t sum = 0;

    for (uint32_t i = 0; i < Length; i++)
    {
        sum += Address[i];
    }

    return sum;
}

/**
 * @brief 32位求和
 * @param Address 起始地址
 * @param Length 被加的数据的数量, 注意不是字节数
 * @return uint32_t 结果
 */
uint32_t Basic_Math_Sum_32(const uint32_t *Address, uint32_t Length)
{
    if (Address == nullptr)
    {
        return 0;
    }

    uint32_t sum = 0;

    for (uint32_t i = 0; i < Length; i++)
    {
        sum += Address[i];
    }

    return sum;
}

/**
 * @brief sinc函数的实现
 * @param x 输入
 * @return float 输出
 */
float Basic_Math_Sinc(float x)
{
    // 分母为0则按极限求法
    if (Basic_Math_Abs(x) <= 2.0f * FLT_EPSILON)
    {
        return 1.0f;
    }

    return arm_sin_f32(x) / x;
}

/**
 * @brief 将浮点数映射到整型
 * @param x 浮点数
 * @param Float_1 浮点数1
 * @param Float_2 浮点数2
 * @param Int_1 整型1
 * @param Int_2 整型2
 * @return int32_t 整型
 */
int32_t Basic_Math_Float_To_Int(float x, float Float_1, float Float_2, int32_t Int_1, int32_t Int_2)
{
    float tmp_denominator = Float_2 - Float_1;

    if (Basic_Math_Abs(tmp_denominator) <= FLT_EPSILON)
    {
        return Int_1;
    }

    float tmp = (x - Float_1) / tmp_denominator;
    int32_t out = static_cast<int32_t>(tmp * static_cast<float>(Int_2 - Int_1) + static_cast<float>(Int_1));

    return out;
}

/**
 * @brief 将整型映射到浮点数
 * @param x 整型
 * @param Int_1 整型1
 * @param Int_2 整型2
 * @param Float_1 浮点数1
 * @param Float_2 浮点数2
 * @return float 浮点数
 */
float Basic_Math_Int_To_Float(int32_t x, int32_t Int_1, int32_t Int_2, float Float_1, float Float_2)
{
    int32_t tmp_denominator = Int_2 - Int_1;

    if (tmp_denominator == 0)
    {
        return Float_1;
    }

    float tmp = static_cast<float>(x - Int_1) / static_cast<float>(tmp_denominator);
    float out = tmp * (Float_2 - Float_1) + Float_1;

    return out;
}

/**
 * @brief 判断浮点数是否为无效浮点数
 * @note 这里将非正规数、无穷大、NaN都视为无效浮点数
 * @param x 浮点数
 * @return 是否为无效浮点数
 */
bool Basic_Math_Is_Invalid_Float(float x)
{
    uint32_t tmp_raw = 0;
    memcpy(&tmp_raw, &x, sizeof(tmp_raw));

    uint32_t tmp_exp = (tmp_raw >> 23) & 0xff;
    uint32_t tmp_frac = tmp_raw & 0x7fffff;

    if (tmp_exp == 0x00)
    {
        if (tmp_frac == 0x00)
        {
            // 正负零
            return false;
        }

        // 次正规数和非正规数
        return true;
    }

    if (tmp_exp == 0xFF)
    {
        // 无穷大和NaN
        return true;
    }

    // 正规数
    return false;
}

/**
 * @brief 求取模归化
 * @param x 传入数据
 * @param modulus 模数
 * @return 返回的归化数, 介于 ±modulus / 2 之间
 */
float Basic_Math_Modulus_Normalization(float x, float modulus)
{
    if (Basic_Math_Abs(modulus) <= FLT_EPSILON)
    {
        return x;
    }

    float tmp = fmodf(x + modulus / 2.0f, modulus);

    if (tmp < 0.0f)
    {
        tmp += modulus;
    }

    return tmp - modulus / 2.0f;
}

/*----------------------------------------------------------------------------*/
